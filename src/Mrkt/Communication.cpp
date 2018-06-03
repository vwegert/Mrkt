/*
 *  This file is part of Mrkt, a hardware frontend for Grbl.
 *
 *  Mrkt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Foobar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Mrkt.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */ 
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#include "Configuration.h"
#include "Communication.h"

/**
 * The "singleton" instance of the Communication class.
 */
Communication MrktCommunication;

Communication::Communication() : 
  grblSerial(GRBL_RX, GRBL_TX) {
  Serial.begin(HOST_SERIAL_SPEED);
  this->grblSerial.begin(GRBL_SERIAL_SPEED);
  this->grblSerial.listen();
  memset(this->grblResponseBuffer, '\0', COMMUNICATION_GRBL_RESPONSE_BUFFER_SIZE + 1);
  this->grblResponseBufferPosition = 0;
  this->state = Idle;
}

void Communication::loop() {
  switch(this->state) {
    case Idle:
      // do nothing
      break;
    case GrblCommand:
      loopGrblCommand();
      break;
  }
}

void Communication::sendGrblCommand(String command, uint16_t timeout, CommandResponseHandler handler) {
  if (this->state == Idle) {
    // clear the buffer
    while (grblSerial.available()) grblSerial.read();
    this->state = GrblCommand;
    this->grblResponseHandler = handler;
    this->grblResponseTimeout = millis() + timeout;    
    grblSerial.print(command);
    grblSerial.print("\r");
    grblSerial.listen();
  }
}

void Communication::loopGrblCommand() {
  bool cleanup = false;

  // handle serial communication if we have incoming data
  if (grblSerial.available()) {
    // check whether the next byte would make the buffer overflow
    if (this->grblResponseBufferPosition >= COMMUNICATION_GRBL_RESPONSE_BUFFER_SIZE - 1) {
      // yes it would - signal the overflow to the listener and clean up
      this->grblResponseHandler(COMMUNICATION_STATUS_BUFFER_OVERFLOW, this->grblResponseBuffer);
      cleanup = true;
    } else {
      // store the next byte read      
      char nextChar = grblSerial.read();
      this->grblResponseBuffer[this->grblResponseBufferPosition] = nextChar;
      if ((nextChar == '\r') || (nextChar == '\n')) {
        // check whether the line that was just completed was an 'ok' or an 'error:X' response line
        if ((this->grblResponseBuffer[this->grblResponseLineStart]     == 'o') &&
            (this->grblResponseBuffer[this->grblResponseLineStart + 1] == 'k')) {
          this->grblResponseHandler(COMMUNICATION_STATUS_OK, this->grblResponseBuffer);
          cleanup = true;
        } else if ((this->grblResponseBuffer[this->grblResponseLineStart]     == 'e') &&
                   (this->grblResponseBuffer[this->grblResponseLineStart + 1] == 'r') && 
                   (this->grblResponseBuffer[this->grblResponseLineStart + 2] == 'r') && 
                   (this->grblResponseBuffer[this->grblResponseLineStart + 3] == 'o') && 
                   (this->grblResponseBuffer[this->grblResponseLineStart + 4] == 'r') && 
                   (this->grblResponseBuffer[this->grblResponseLineStart + 5] == ':')) {
          int errorCode = atoi(&this->grblResponseBuffer[this->grblResponseLineStart + 6]);            
          this->grblResponseHandler(errorCode, this->grblResponseBuffer);
          cleanup = true;
        } else {
          // none of the above: new line starts with the next character after the current one
          this->grblResponseLineStart = this->grblResponseBufferPosition + 1;
        }
      } 
      this->grblResponseBufferPosition++;
    }
  }

  // check for a timeout
  if (!cleanup && (millis() > this->grblResponseTimeout)) {
    this->grblResponseHandler(COMMUNICATION_STATUS_TIMEOUT, this->grblResponseBuffer);
    cleanup = true;   
  }
  
  if (cleanup) {
    // swallow all of the remaining buffer contents
    while (grblSerial.available()) grblSerial.read();
    memset(this->grblResponseBuffer, '\0', COMMUNICATION_GRBL_RESPONSE_BUFFER_SIZE + 1);
    this->grblResponseBufferPosition = 0;
    this->grblResponseLineStart = 0;
    this->grblResponseHandler = 0;
    this->grblResponseTimeout = 0;
    this->state = Idle;
  } 
}

