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

/**
 * local device Address
 */
byte hhaAddress[2] = {0x00, 0x01};

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
  
  Serial.println("Listening..."); 
}

void loop(){

  if(!Mirf.isSending() && Mirf.dataReady()){
    byte data[32];
    
    Mirf.getData(data);

    Serial.println("RECEIVED PACKET: [");
    for(int i = 0 ; i < 32 ; i ++) {
      Serial.print(data[i]>>4, HEX);
      Serial.print(data[i]&0x0f, HEX);
      Serial.print(",");
    }
    Serial.println("]");  

    byte hhaInformation[16] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                               0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
                               
    HHAProtocol *hhaProtocol = new HHAProtocol(hhaAddress, hhaAddress, hhaInformation);
    hhaProtocol->setDebug(true);
    
    hhaProtocol->parse(data);

    if(hhaProtocol->checkRecipientAddr(hhaAddress)) {
		
		//YES the message is for this client
		hhaProtocol->decrypt();

		byte* dataX;      
		dataX = hhaProtocol->getPacket();
		Serial.print("### DECRYPTED PACKET:");
		for(int i = 0 ; i < 32 ; i ++) {
			Serial.print(dataX[i], HEX);
			Serial.print(",");
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
      //NO them message is for another client
      Serial.println("### NOT FOR ME RESENDING ###");  
      Mirf.setTADDR((byte *)"clie1");
    
      hhaProtocol->decreaseTTL();

      delayMicroseconds(random(10,40));

      Mirf.send(hhaProtocol->getPacket());

      while(Mirf.isSending()){
      }

      Serial.println("Reply sent.");
      
    }

    //perform cleanup
    delete(hhaProtocol);
    hhaProtocol = NULL;
  }
}
