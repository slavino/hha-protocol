/**
    Copyright (c) 2013 Slavomir Hustaty <slavomir.hustaty@gmail.com>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id$
*/

/**
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
|RecAddr|SndAddr|TTL| Unused                                          | AES 128 encrypted data                                                        |
|-----------------------------------------------------------------------------------------------------------------------------------------------------|

* Recipient address [bytes on position 0,1]
	allows addressing to 255^2 devices [65025]

* Sender address [bytes on position 0,1]
	allows addressing to 255^2 devices [65025]

* TTL [byte on position 4]

* AES 128 Encrypted data [bytes on position 16-31]
	allows 16byte messages

AES 128 Encrypter - online check tool -- http://testprotect.com/appendix/AEScalc

*/

#ifndef _HHAPROTOCOL_H_
#define _HHAPROTOCOL_H_

#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include "AES128.h"

class HHAProtocol {
	
	public:
		HHAProtocol(byte recipientAddress[], byte senderAddress[], byte information[]);

		void setRecipientAddr(byte addr[]);
		void setSenderAddr(byte addr[]);
		void setTTL(byte ttl);
		void setDebug(boolean debugState);
		
		boolean checkSenderAddr(byte addr[]);
		boolean checkRecipientAddr(byte addr[]);

		void decreaseTTL();
		byte getTTL();
		void setInformation(byte information[]);

		void parse(byte *data);
		
		byte* getPacket();
		
		void encrypt();
		void decrypt();
	private:
		byte _packet[32];
		boolean hhaProtocol_DEBUG;
	
};

#endif /* _HHAPROTOCOL_H_ */
