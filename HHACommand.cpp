/**
 * HHACommand
 *
 * By: Slavomir Hustaty <slavomir.hustaty@gmail.com>
 * $Id: HHACommand.cpp 67 2013-05-28 13:25:53Z slavo $
 *
 * Slavomir Hustaty <slavomir.hustaty@gmail.com>
 *
 */

#include "HHACommand.h"

const byte CMD_GET_SWVERSION    = 0x00; //#1 get - version of command set
const byte CMD_GET_TEMPERATURE  = 0x01; //#2 get - get temperature(s)
const byte CMD_GET_SWITCH       = 0x02; //#3 get - digital pins on relays
const byte CMD_SET_SWITCH       = 0x03; //#4 set - digital pins on relays
const byte CMD_GET_TIME         = 0x04; //#5 get - get time on RTC
const byte CMD_SET_TIME         = 0x05; //#6 set - set time on RTC
const byte CMD_GET_UPTIME       = 0x06; //#7 get - CPU millis, useful if RTC not present
const byte CMD_RESET            = 0xFF; //#8 set - force reboot

const uint_8 CMD_COUNT = 8;

const byte SUPPORTED_CMD_SET[CMD_COUNT] = {
            CMD_GET_SWVERSION,
            CMD_GET_TEMPERATURE,
            CMD_GET_SWITCH,
            CMD_SET_SWITCH,
            CMD_GET_TIME,
            CMD_SET_TIME,
            CMD_GET_UPTIME,
            CMD_RESET
            };

/**
* Default constructor.
*/
HHACommand::HHACommand(byte cmdType) {

    //set DBUG to false by default
    if(this->hhaCommand_DEBUG == NULL) {
        this->hhaCommand_DEBUG = false;
    }

    this->setCommandType(cmdType);

}

/**
* Setter for command type with business logic inside.
*/
void HHACommand::setCommandType(byte cmdType) {
    for(int i = 0 ; i < CMD_COUNT ; i++) {
        if(SUPPORTED_CMD_SET[i] == cmdType) {
            this->commandType = cmdType;
            return;
        }
    }
    //default behavior
    this->commandType = CMD_GET_UPTIME;
}


