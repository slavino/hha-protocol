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
#include "AES128.h"
#include "HHAProtocol.h"

/**
 * local device Address
 */
byte hhaAddress[2] = {0x01, 0x0a};

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
  
  Serial.println("Beginning ... "); 
}

void loop(){

  unsigned long time = millis();
  
  Mirf.setTADDR((byte *)"clie1");
  
  byte hhaInformation[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                             0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  byte hhaRecipientAddress[2] = {0x00, 0x01};
  
  HHAProtocol *hhaProtocol = new HHAProtocol(hhaRecipientAddress, hhaAddress, hhaInformation);
  hhaProtocol->setDebug(true);
  hhaProtocol->encrypt();
  
  Mirf.send(hhaProtocol->getPacket());
  
  while(Mirf.isSending()){
  }
  
  delay(10);
  while(!Mirf.dataReady()) {
    if ( ( millis() - time ) > 1000 ) {
      Serial.println("### Timeout on response from server!");
      delete(hhaProtocol);
      hhaProtocol = NULL;
      return;
    }
  }
  
  Serial.println("### RECEIVED DATA ###");

  byte data[32];
  Mirf.getData((byte *)&data);

  hhaProtocol->parse(data);
  hhaProtocol->decreaseTTL();
  hhaProtocol->decrypt();
  
  byte* data2 = (byte *)hhaProtocol->getPacket();

  Serial.println("DECRYPTED PACKET:");
  for(int i = 0 ; i < 32 ; i ++) {
    Serial.print(data2[i], HEX);
    Serial.print(",");
  }
  Serial.println(); 
  
  delete(hhaProtocol);
  delete(data2);
  hhaProtocol = NULL;
  data2 = NULL;
  
  delay(1000);
} 
  
  
  
