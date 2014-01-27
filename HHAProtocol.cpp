/**
 * HHAProtocol
 *
 * By: Slavomir Hustaty <slavomir.hustaty@gmail.com>
 * $Id: HHAProtocol.cpp 67 2013-05-28 13:25:53Z slavo $
 *
 * Slavomir Hustaty <slavomir.hustaty@gmail.com>
 *
 */

#include "HHAProtocol.h"

byte BROADCAST_ADDR[] = {0xff, 0xff};

/*byte key[] = { 
  0xa1, 0xc1, 0x0a, 0x77, 0xa1, 0xfa, 0xac, 0xa7, 
  0xa5, 0xb9, 0xac, 0xb1, 0x11, 0x07, 0xea, 0xfa 
};*/

const uint8_t TTL = 4;
const uint8_t PACKET_SIZE = 32;
const uint8_t DATA_SIZE = 16;
const uint8_t KEY_SIZE = 16;

HHAProtocol::HHAProtocol(byte recipientAddress[], byte senderAddress[], byte information[]) {
	byte sample[32] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
					   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	memcpy(_packet, &sample, PACKET_SIZE);
	this->setSenderAddr(senderAddress);
	this->setRecipientAddr(recipientAddress);
	this->setInformation(information);
	this->setTTL(0x03);
	
	this->calculateKey();

	if(this->hhaProtocol_DEBUG == NULL) {
		this->hhaProtocol_DEBUG = false;
	}

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::HHAProtocol(address,information): Initializing HHAProtocol: [");
		for(int i = 0 ; i < PACKET_SIZE ; i++) {
			Serial.print(this->_packet[i]>>4, HEX);
            Serial.print(this->_packet[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}
}

void HHAProtocol::setRecipientAddr(byte addr[]) {
	this->_packet[0] = addr[0];
	this->_packet[1] = addr[1];
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setRecipientAddr(addr): [");
		Serial.print(this->_packet[0], HEX);
		Serial.print(",");
		Serial.print(this->_packet[1], HEX);
		Serial.println("]");
	}
}

byte* HHAProtocol::getRecipientAddr() {
	byte recipient[2] = {0x00, 0x00};
	recipient[0] = this->_packet[0];
	recipient[1] = this->_packet[1];

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::getRecipientAddr(): [");
		Serial.print(recipient[0], HEX);
		Serial.print(",");
		Serial.print(recipient[1], HEX);
		Serial.println("]");
	}
	return (byte *)&recipient;
}

void HHAProtocol::setSenderAddr(byte addr[]) {
	this->_packet[2] = addr[0];
	this->_packet[3] = addr[1];
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setSenderAddr(addr): [");
		Serial.print(this->_packet[2], HEX);
		Serial.print(",");
		Serial.print(this->_packet[3], HEX);
		Serial.println("]");
	}
}

byte* HHAProtocol::getSenderAddr() {
	byte sender[2] = {0x00, 0x00};
	sender[0] = this->_packet[2];
	sender[1] = this->_packet[3];

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::getSenderAddr(): [");
		Serial.print(sender[0], HEX);
		Serial.print(",");
		Serial.print(sender[1], HEX);
		Serial.println("]");
	}

	return (byte *)&sender;
}

void HHAProtocol::setDebug(boolean debugState) {
	this->hhaProtocol_DEBUG = debugState;
}

/**
 * Checks if this module is the recipient.
 *
 * @param	addr	byte[2] address space
 * @return	TRUE if the device has same address OR is package was broadcasted
 */
boolean HHAProtocol::checkRecipientAddr(byte addr[]) {
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::checkRecipientAddr(addr): [own ");
		Serial.print(this->_packet[0], HEX);
		Serial.print(",");
		Serial.print(this->_packet[1], HEX);
		Serial.print(" - checked ");
		Serial.print(addr[0], HEX);
		Serial.print(",");
		Serial.print(addr[1], HEX);
		Serial.println("]");
	}
	return (this->_packet[0] == addr[0]) && (this->_packet[1] == addr[1])
		|| (this->_packet[0] == BROADCAST_ADDR[0]) && (this->_packet[1] == BROADCAST_ADDR[1]);
}

/**
 * Checks if this module is the original sender.
 *
 * @param	addr	byte[2] address space
 * @return	TRUE if the device has same address
 */
boolean HHAProtocol::checkSenderAddr(byte addr[]) {
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::checkSenderAddr(addr): [own ");
		Serial.print(this->_packet[2], HEX);
		Serial.print(",");
		Serial.print(this->_packet[3], HEX);
		Serial.print(" - checked ");
		Serial.print(addr[0], HEX);
		Serial.print(",");
		Serial.print(addr[1], HEX);
		Serial.println("]");
	}
	return (this->_packet[2] == addr[0]) && (this->_packet[3] == addr[1]);
}

void HHAProtocol::setTTL(byte ttl) {
	this->_packet[TTL] = ttl;
}

byte HHAProtocol::getTTL() {
	return this->_packet[TTL];
}

void HHAProtocol::decreaseTTL() {
	if(this->_packet[TTL] != 0x00) {
		this->_packet[TTL]--;
	}
}

void HHAProtocol::setInformation(byte information[]) {

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setInformation(information): information: [");
		for(int i = 0 ; i < DATA_SIZE ; i++) {
			Serial.print(information[i]>>4, HEX);
            Serial.print(information[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}

	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = information[i-DATA_SIZE];
	}
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setInformation(information): setting: [");
		for(int i = 0 ; i < PACKET_SIZE ; i++) {
			Serial.print(this->_packet[i]>>4, HEX);
            Serial.print(this->_packet[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}
}

void HHAProtocol::parse(byte *data) {

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::parse(data): DATA[");
		for(int i = 0 ; i < PACKET_SIZE ; i++) {
			Serial.print(data[i]>>4, HEX);
            Serial.print(data[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}

	for(int i = 0 ; i < PACKET_SIZE ; i++) {
		this->_packet[i] = data[i];
	}
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::parse(data): [");
		for(int i = 0 ; i < PACKET_SIZE ; i++) {
			Serial.print(this->_packet[i]>>4, HEX);
            Serial.print(this->_packet[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}
}

byte* HHAProtocol::getPacket() {

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::getPacket(): [");
		for(int i = 0 ; i < PACKET_SIZE ; i++) {
			Serial.print(this->_packet[i]>>4, HEX);
            Serial.print(this->_packet[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}

	return (byte *)&this->_packet;
}

void HHAProtocol::encrypt() {
	byte plainData[DATA_SIZE];
	
	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		plainData[i-DATA_SIZE] = this->_packet[i];
	}
	
	aes128_enc_single(this->_key, plainData);
	
	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = plainData[i-DATA_SIZE];
	}
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setInformation(information): setting: [");
		for(int i = 0 ; i < PACKET_SIZE ; i++) {
			Serial.print(this->_packet[i]>>4, HEX);
            Serial.print(this->_packet[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}

}

void HHAProtocol::decrypt() {
	byte encryptedData[DATA_SIZE];
	
	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		encryptedData[i-DATA_SIZE] = this->_packet[i];
	}
	
	aes128_dec_single(this->_key, encryptedData);
	
	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = encryptedData[i-DATA_SIZE];
	}
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setInformation(information): setting: [");
		for(int i = 0 ; i < PACKET_SIZE ; i++) {
			Serial.print(this->_packet[i]>>4, HEX);
            Serial.print(this->_packet[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}
}

void HHAProtocol::calculateKey() {

	for(int i = 0 ; i < KEY_SIZE ; i++ ) {
		this->_key[i] = this->_packet[i%4];
	}
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::calculateKey(): Key is: [");
		for(int i = 0 ; i < KEY_SIZE ; i++) {
			Serial.print(this->_key[i]>>4, HEX);
            Serial.print(this->_key[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}

}
	
