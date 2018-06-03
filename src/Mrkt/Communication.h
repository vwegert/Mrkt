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
#ifndef MRKT_Communication_h
#define MRKT_Communication_h

#include <SoftwareSerial.h> // see https://www.arduino.cc/en/Reference/SoftwareSerial

#include "Configuration.h"

/**
 * The size of the buffer to store responses to Grbl commands. At the moment, it 
 * is sized to hold the response to the $I command which contains the version 
 * information.
 */
#define COMMUNICATION_GRBL_RESPONSE_BUFFER_SIZE  50

/**
 * The communication status reported to the callback methods can be 
 * - zero, which designates a successful execution,
 * - negative, which designates a communication error or
 * - positive, which designates a Grbl error code.
 */
#define COMMUNICATION_STATUS_OK               0
#define COMMUNICATION_STATUS_TIMEOUT         -1
#define COMMUNICATION_STATUS_BUFFER_OVERFLOW -2

/**
 * This class encapsulates the serial communication to both the host system and the 
 * Grbl installation.
 */
class Communication {
  
  public:
    /**
     * The signature of a result handler for the sendGrblCommand method.
     */
    typedef void (*CommandResponseHandler) (int status, char * response);

    /**
     * The default constructor.
     */
    Communication();

    /**
     * This method has to be called from the main loop.
     */
    void loop();

    /**
     * Sends a command to the Grbl system and waits for a response that ends in an
     * "ok" or "error" message, which is then passed to a result handler method.
     */
    void sendGrblCommand(String command, uint16_t timeout, CommandResponseHandler handler);

  private:
    /**
     * The representation of the state of the communication system.
     */
    enum InternalState { Idle, GrblCommand };
    InternalState state;

    /**
     * The serial connection to the Grbl system. The built-in serial connection is used to
     * connect to the host system.
     */
    SoftwareSerial grblSerial;

    /**
     * The buffer to store the responses to commands sent using sendGrblCommand.
     * The buffer is one char larger than the constant defined above. This allows us to 
     * always have a \0 character at the end.
     */
    char grblResponseBuffer[COMMUNICATION_GRBL_RESPONSE_BUFFER_SIZE + 1];

    /**
     * The position of the next char to write into the buffer.
     */
    uint8_t grblResponseBufferPosition;

    /**
     * The position of the start of the last line encountered - either the start of the 
     * buffer or the character after the last newline encountered.
     */
    uint8_t grblResponseLineStart;

    /**
     * The timeout while waiting for a response.
     */
    uint32_t grblResponseTimeout;

    /**
     * The method to call when the response has been received completely.
     */
    CommandResponseHandler grblResponseHandler;

    /**
     * The implementations called during the loop() processing for each internal state.
     */
    void loopGrblCommand();
      
};

/**
 * Access to the "singleton" instance of the Communication class.
 */
extern Communication MrktCommunication;

#endif
