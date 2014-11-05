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

// position of TTL byte in the packet
const uint8_t TTL = 4;

// position of ENCRYPTION byte in the packet
const uint8_t ENCRYPTION_TYPE = 5;

const uint8_t PACKET_SIZE = 32;

const uint8_t DATA_SIZE = 16;

const uint8_t KEY_SIZE = 16;

//encryption types enumeration
const byte ENCRYPTION_AES = 0x01;
const byte ENCRYPTION_NONE = 0x00;

HHAProtocol::HHAProtocol(byte recipientAddress[], byte senderAddress[], byte information[]) {
	byte sample[32] = {0xff, 0xff, 0xff, 0xff, 0xff, ENCRYPTION_NONE, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
					   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if(this->hhaProtocol_DEBUG == NULL) {
		this->hhaProtocol_DEBUG = false;
	}
 
    memcpy(_packet, &sample, PACKET_SIZE);
	delete(sample);
	this->setSenderAddr(senderAddress);
	this->setRecipientAddr(recipientAddress);
	this->setInformation(information); //and encrypt it
	
	this->setTTL(0x03);
	
	this->calculateKey();
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::HHAProtocol(recipientAddress, senderAddress, information): Initializing HHAProtocol: [");
		this->printByteArrayToSerial(this->_packet);
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
	if(this->_packet[TTL] > 0x00) {
		this->_packet[TTL]--;
	}
}

void HHAProtocol::setInformation(byte information[]) {

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setInformation(information): information: [");
		this->printByteArrayToSerial(information);
		Serial.println("]");
	}
		
	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = information[i-DATA_SIZE];
	}

	if(ENCRYPTION_NONE != this->getEncryptionMethod()) {
		return;
	}

	this->encrypt();
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::setInformation(information): setting: [");
		this->printByteArrayToSerial(this->_packet);
		Serial.println("]");
	}
}


byte HHAProtocol::getEncryptionMethod() {
	return this->_packet[ENCRYPTION_TYPE];
}

void HHAProtocol::parse(byte *data) {

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::parse(data): DATA[");
		this->printByteArrayToSerial(this->_packet);
		Serial.println("]");
	}

	for(int i = 0 ; i < PACKET_SIZE ; i++) {
		this->_packet[i] = data[i];
	}
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::parse(data): [");
		this->printByteArrayToSerial(this->_packet);
		Serial.println("]");
	}
}

byte* HHAProtocol::getPacket() {

	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::getPacket(): [");
		this->printByteArrayToSerial(this->_packet);
		Serial.println("]");
	}

	this->decrypt();
	
	return (byte *)&this->_packet;
}

void HHAProtocol::setEncryption(byte encryption) {
	this->_packet[ENCRYPTION_TYPE] = encryption;
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
	
	this->setEncryption(ENCRYPTION_AES);
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::encrypt(): setting: [");
		this->printByteArrayToSerial(this->_packet);
		Serial.println("]");
	}

}

void HHAProtocol::decrypt() {
	byte encryptedData[DATA_SIZE];
	
	if(ENCRYPTION_AES != this->getEncryptionMethod()) {
		return;
	}
	
	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		encryptedData[i-DATA_SIZE] = this->_packet[i];
	}
	
	aes128_dec_single(this->_key, encryptedData);
	
	for(int i = DATA_SIZE ; i < PACKET_SIZE ; i++ ) {
		this->_packet[i] = encryptedData[i-DATA_SIZE];
	}

	this->setEncryption(ENCRYPTION_NONE);
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::decrypt(): setting: [");
		this->printByteArrayToSerial(this->_packet);
		Serial.println("]");
	}
}

void HHAProtocol::calculateKey() {

	byte key[16] = {0xDA, 0xBB, 0xA1, 0x34, 0x11, 0x23, 0xD1, 0x1F,
					0xFA, 0xCB, 0x10, 0x4E, 0x04, 0x5E, 0x2F, 0x2A};
	
	for(int i = 0 ; i < KEY_SIZE ; i++ ) {
		if(i % 4 == 0) {
			this->_key[i] = this->_packet[i%4] & key[i];
		} else if(i % 4 == 1) {
			this->_key[i] = this->_packet[i%4] | key[i];
		} else if(i % 4 == 2) {
			this->_key[i] = this->_packet[i%4] ^ key[i];
		} else {
			this->_key[i] = this->_packet[i%4];
		}
	}
	
	if(this->hhaProtocol_DEBUG) {
		Serial.print("HHAProtocol::calculateKey(): Key is: [");
		this->printByteArrayToSerial(this->_key);
		Serial.println("]");
	}
}

void HHAProtocol::printByteArrayToSerial(byte bytearray[]) {
	int length = sizeof(bytearray);
	for(int i = 0 ; i < length ; i++) {
		Serial.print(this->_key[i]>>4, HEX);
		Serial.print(this->_key[i]&0x0f, HEX);
		if(i < length - 1) {
			Serial.print(",");
		}
	}
}
