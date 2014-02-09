/**
 * Pins:
 * Hardware SPI:
 * MISO -> 12
 * MOSI -> 11
 * SCK -> 13
 *
 * Configurable:
 * CE -> 9
 * CSN -> 10
 */

#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>
#include "AESLib.h"
#include "HHAProtocol.h"

const int PACKET_LENGTH = 32;

/**
 * local device Address
 */
byte hhaAddress[2] = {0x00, 0x01};

/**
 * Byte array reserved for reading inputs.
 */
byte serialInput[PACKET_LENGTH] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                                    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                                    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
                                    0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
                        
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
boolean serialInputComplete = false;

/**
 * perform board RESET
 */
void(* resetFunc) (void) = 0; //declare reset function @ address 0 

void setup(){
  Serial.begin(115200);
  
  Mirf.cePin = 9;
  Mirf.csnPin = 10;

  Mirf.spi = &MirfHardwareSpi;
  Mirf.init();
  
  Mirf.setRADDR((byte *)"clie1");
   
  Mirf.payload = 32;
  Mirf.channel = 10;

  Mirf.config();
  
  randomSeed(analogRead(0));
  
  //Serial.println("Listening..."); 
}

void sendData() {
  byte recipientHHAAddress[2] = {serialInput[0], serialInput[1]};
  byte hhaInformation[16] = {serialInput[16], serialInput[17], serialInput[18], serialInput[19], serialInput[20], serialInput[21], serialInput[22], serialInput[23], 
                             serialInput[24], serialInput[25], serialInput[26], serialInput[27], serialInput[28], serialInput[29], serialInput[30], serialInput[31]};
  HHAProtocol *hhaProtocol = new HHAProtocol(recipientHHAAddress, hhaAddress, hhaInformation);
  hhaProtocol->setDebug(false);
  hhaProtocol->encrypt();
  
  Mirf.send(hhaProtocol->getPacket());
  
  //perform cleanup
  delete(hhaProtocol);
  hhaProtocol = NULL;

  //Clearing serial input
  clearSerialInput();                
  serialInputComplete = false;
}

void loop(){

  //sendData or Reset triggered by Serial event
  if (serialInputComplete) {

    printSerialInput();

    //if remote reset command recieved via SERIAL
    if(serialInput[0] == 0xAA
        && serialInput[1] == 0xBB 
        && serialInput[2] == 0xCC 
        && serialInput[3] == 0xEE 
        && serialInput[4] == 0xDD 
        ) {
      resetFunc();
    }
    
    sendData();
  
    // clear the string:
    clearSerialInput();                
    serialInputComplete = false;
  }

  if(!Mirf.isSending() && Mirf.dataReady()){
    byte data[32];
    
    Mirf.getData(data);

    /*
    Serial.println("RECEIVED PACKET: [");
    for(int i = 0 ; i < 32 ; i ++) {
      Serial.print(data[i]>>4, HEX);
      Serial.print(data[i]&0x0f, HEX);
      Serial.print(",");
    }
    Serial.println("]");  */

    byte hhaInformation[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                               0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
                               
    byte senderAddr[2] = {data[2], data[3]};
    HHAProtocol *hhaProtocol = new HHAProtocol(hhaAddress, senderAddr, hhaInformation);
    hhaProtocol->setDebug(false);
    
    hhaProtocol->parse(data);

    if(hhaProtocol->checkRecipientAddr(hhaAddress)) {
		
		//YES the message is for this client
		hhaProtocol->decrypt();

		byte* dataX;      
		dataX = hhaProtocol->getPacket();
		//Serial.print("### DECRYPTED PACKET:");
		for(int i = 0 ; i < 32 ; i ++) {
  			Serial.print(dataX[i]>>4, HEX);
			Serial.print(dataX[i]&0x0f, HEX);
			//Serial.print(",");
		}
		Serial.println();  

		delayMicroseconds(random(10,40));

		hhaProtocol->setRecipientAddr((byte *)hhaProtocol->getSenderAddr());
		hhaProtocol->setSenderAddr(hhaAddress);
		hhaProtocol->setInformation((byte *)"ACK1234567890123");
		hhaProtocol->encrypt();

		Mirf.setTADDR((byte *)"clie1");
		Mirf.send(hhaProtocol->getPacket());

		while(Mirf.isSending()){
		}

		//CLEANUP
		delete(dataX);
		dataX = NULL;

    } else {
      //NO the message is for another client
      //Serial.println("### NOT FOR ME RESENDING ###");  
      Mirf.setTADDR((byte *)"clie1");
    
      hhaProtocol->decreaseTTL();

      delayMicroseconds(random(10,40));

      Mirf.send(hhaProtocol->getPacket());

      while(Mirf.isSending()){
      }

    }

    //perform cleanup
    delete(hhaProtocol);
    hhaProtocol = NULL;
  }
}

void serialEvent() {
  int i = 0;
  while(Serial.available()) {
    // get the new byte:
    if(i < PACKET_LENGTH) {
      serialInput[i++] = (byte) Serial.read();
    } else {
      Serial.read();
    }
    // add it to the inputString:
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if ((char)serialInput[i] == '\n') {
      serialInputComplete = true;
    }
  }
}

void clearSerialInput() {
  for(int i = 0 ; i < PACKET_LENGTH ; i++) {
    serialInput[i] = 0x00;
  }
}

void printSerialInput() {
  for(int i = 0 ; i < PACKET_LENGTH ; i++) {
    Serial.print(serialInput[i]);
  }
  Serial.println();
}
