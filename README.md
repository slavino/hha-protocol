hha-protocol
============
WORK IN PROGRESS

Hustaty Home Automation protocol - Arduino library for Wireless NRF24L01 and AES128 (utilizing 3rd party libs)

32 byte protocol definition

```cpp
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
|RecAddr|SndAddr|TTL| Unused                                          | AES 128 encrypted data                                                        |
|-----------------------------------------------------------------------------------------------------------------------------------------------------|
```

AES128 re-used from
https://github.com/una1veritas/Arduino.git \libraries\AES128

Mirf
https://github.com/aaronds/arduino-nrf24l01.git