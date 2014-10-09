hha-protocol
============
WORK IN PROGRESS

Hustaty Home Automation protocol - Arduino library for Wireless NRF24L01 and AES128 (utilizing 3rd party libs)

32 byte protocol definition

```cpp
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
|RecAddr|SndAddr|TTL|Enc| Unused (Considered For Unencrypted Metadata)| AES 128 encrypted or plain data                                                |
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
```

AESLib re-used from
https://github.com/DavyLandman/AESLib

XTEA re-used from
https://github.com/franksmicro/Arduino/tree/master/libraries/Xtea

Mirf
https://github.com/aaronds/arduino-nrf24l01.git

Hardware
NRF24L01+ 2.4GHz (U$D 1.7 - 2.0 on ebay.com)
Arduino compatible board (in my case Arduino Nano v3 - max. U$D 10 on ebay.com)

Use Fritzing for *.fzz (http://fritzing.org/) 
