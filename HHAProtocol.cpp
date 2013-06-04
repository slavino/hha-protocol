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

byte key[] = { 
  0xa1, 0xc1, 0x0a, 0x77, 0xa1, 0xfa, 0xac, 0xa7, 
  0xa5, 0xb9, 0xac, 0xb1, 0x11, 0x07, 0xea, 0xfa 
};
AES128 aes(key);

const uint8_t TTL = 4;
const uint8_t PACKET_SIZE = 32;
 
HHAProtocol::HHAProtocol(byte recipientAddress[], byte senderAddress[], byte information[]) {
	byte sample[32] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
					   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	memcpy(_packet, &sample, PACKET_SIZE);
	this->setSenderAddr(senderAddress);
	this->setRecipientAddr(recipientAddress);
	this->setInformation(information);
	this->setTTL(0x03);
	
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
		Serial.print("HHAProtocol::setRecipientAddr(): [");
		Serial.print(this->_packet[0], HEX);
		Serial.print(",");
		Serial.print(this->_packet[1], HEX);
		Serial.println("]");
	}
}

void HHAProtocol::setSenderAddr(byte addr[]) {
	this->_packet[2] = addr[0];
	this->_packet[3] = addr[1];
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setSenderAddr(): [");
		Serial.print(this->_packet[2], HEX);
		Serial.print(",");
		Serial.print(this->_packet[3], HEX);
		Serial.println("]");
	}
}

void HHAProtocol::setDebug(boolean debugState) {
	this->hhaProtocol_DEBUG = debugState;
}

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
	return (this->_packet[0] == addr[0]) && (this->_packet[1] == addr[1]);
}

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
		for(int i = 0 ; i < 16 ; i++) {
			Serial.print(information[i]>>4, HEX);
            Serial.print(information[i]&0x0f, HEX);
			Serial.print(",");
		}
		Serial.println("]");
	}

	//aes.encrypt(information);
	for(int i = 16 ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = information[i-16];
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

	//memcpy((void *)this->_packet, (const void *)data, PACKET_SIZE);
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
	byte plainData[16];
	
	for(int i = 16 ; i < PACKET_SIZE ; i++ ) {
		plainData[i-16] = this->_packet[i];
	}
	
	aes.encrypt(plainData);
	
	for(int i = 16 ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = plainData[i-16];
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
	byte encryptedData[16];
	
	for(int i = 16 ; i < PACKET_SIZE ; i++ ) {
		encryptedData[i-16] = this->_packet[i];
	}
	
	aes.decrypt(encryptedData);
	
	for(int i = 16 ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = encryptedData[i-16];
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