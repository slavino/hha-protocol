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
#include "nRF24L01.h"
#include "RF24.h"
#include "AESLib.h"
#include "HHAProtocol.h"

int relay = 8;

//
// Hardware conf
//

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10 

RF24 radio(9,10);

//
// Topology
//

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

/**
 * local device Address
 */
byte hhaAddress[2] = {0x01, 0x0a};

void setup(void) {
  Serial.begin(57600);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);

  //
  // Setup and configure rf radio
  //
  radio.begin();
  radio.setRetries(15,15);

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  radio.startListening();
  radio.printDetails();
}

void loop(void) {

    // if there is data ready
    if ( radio.available() ) {
      // Dump the payloads until we've gotten everything
      byte message[32];
      bool done = false;
      while (!done) {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &message, sizeof(message) );

        // Spew it
        //printf("Got message %lu...",message);
        if (message[0] == 'r') {
          digitalWrite(relay, LOW);
          Serial.println("relay LOW");
        } else {
          digitalWrite(relay, HIGH);
          Serial.println("relay HIGH");
        }

		// Delay just a little bit to let the other unit
		// make the transition to receiver
		delay(5);
      }

      // First, stop listening so we can talk
      radio.stopListening();
      
      byte hhaInformation[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 
                                 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
      byte hhaRecipientAddress[2] = {0x00, 0x01};
      
      HHAProtocol *hhaProtocol = new HHAProtocol(hhaRecipientAddress, hhaAddress, hhaInformation);
      hhaProtocol->setDebug(false);
      //hhaProtocol->encrypt();
    
      byte *dataPacket;
      memcpy(dataPacket,hhaProtocol->getPacket(),32);
      // Send the final one back.
      radio.write( &dataPacket, sizeof(dataPacket) );
      //printf("Sent response.\n\r");
      delete(hhaProtocol);
      hhaProtocol = NULL;

      // Now, resume listening so we catch the next packets.
      radio.startListening();
    }

}

