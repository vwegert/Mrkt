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
#include "InitializationMode.h"

#include "Communication.h"
#include "Display.h"
#include "ModeController.h"
#include "UserControls.h"

/**
 * The "slow" blink rate used while establishing the communication.
 */
#define INIT_MODE_BLINK_INTERVAL_INIT  500

/**
 * The "fast" blink rate used to signal that an unrecoverable error
 * condition was encountered.
 */
#define INIT_MODE_BLINK_INTERVAL_ERROR 100

/**
 * The time in ms to display event messages.
 */
#define INIT_MODE_EVENT_DISPLAY_TIME   750

/**
 * The time in ms to display the Grbl version before handing over to 
 * the initial working mode.
 */
#define INIT_MODE_GRBL_DISPLAY_TIME   1000

/**
 * The time in ms to wait for a Grbl response.
 */
#define INIT_MODE_COMM_TIMEOUT        1500

/**
 * The time in ms to wait before sending the $I command again.
 */
#define INIT_MODE_COMM_RETRY_DELAY     250

/**
 * The "singleton" instance of the InitializationMode class.
 */
InitializationMode MrktInitializationMode;

InitializationMode::InitializationMode() : 
  AbstractMode() {
    // clear the version buffer
    memset(this->grblVersion, '\0', GRBL_VERSION_SIZE);
}

void InitializationMode::activate() {
  this->state = Initial;
  this->stateChangeTime = 0;
  this->prevBlinkTime = 0;
  this->mainLEDStatus = LOW; 
}

void InitializationMode::deactivate() {
  // disable the main LED
  MrktDisplay.setMainLED(LOW);
}

void InitializationMode::loop() {
  unsigned long currentTime = millis();
  
  // handle the blinking main status LED
  if (currentTime - this->prevBlinkTime > this->blinkInterval) {
    this->mainLEDStatus = (this->mainLEDStatus == HIGH) ? LOW : HIGH;
    MrktDisplay.setMainLED(this->mainLEDStatus);
    this->prevBlinkTime = currentTime;
  }

  // check whether we can re-evaluate the current state
  if (currentTime >= this->stateChangeTime) {
    switch(this->state) {
      case Initial:
        loopInitial();
        break;
      case EventDisplay:
        loopEventDisplay();
        break;
      case GrblSearchStart:
        loopGrblSearchStart();
        break;
      case GrblWaiting:
        loopGrblWaiting();
        break;
      case GrblCommError:
        loopGrblCommError();
        break;
      case GrblVersionFound:
        loopGrblVersionFound();
        break;
      case GrblVersionError:
        loopGrblVersionError();
        break;
      case GrblVersionOK:
        loopGrblVersionOK();
        break;
      case Final:
        loopFinal();
        break;     
    }
  }
}

void InitializationMode::loopInitial() {
  // set the main status LED to slow blinking mode
  this->blinkInterval = INIT_MODE_BLINK_INTERVAL_INIT;
        
  // set the display contents - first line displays banner with version info
  MrktDisplay.clear();
  MrktDisplay.print(F("Mrkt "));
  MrktDisplay.print(MRKT_VERSION);
      
  // next state: start GrblSearch without any delay
  this->state = GrblSearchStart;
  this->stateChangeTime = 0;
}

void InitializationMode::loopEventDisplay() {
  // display the user control event
  UserControls::Event event = MrktUserControls.getEvent();
  MrktDisplay.setCursor(0, 1);
  switch(event.type) {
    case UserControls::None:
      // should never happen
      break;        
    case UserControls::KeyLeft:
      MrktDisplay.print(F("KP Left         "));
      break;
    case UserControls::KeyRight:
      MrktDisplay.print(F("KP Right        "));
      break;
    case UserControls::KeyUp:
      MrktDisplay.print(F("KP Up           "));
      break;
    case UserControls::KeyDown:
      MrktDisplay.print(F("KP Down         "));
      break;
    case UserControls::KeySelect:
      MrktDisplay.print(F("KP Select       "));
      break;
    case UserControls::EncChanged:
      MrktDisplay.print(F("E Wheel         "));
      break;
    case UserControls::EncButton:
      MrktDisplay.print(F("E Button        "));
      break;
    case UserControls::ModeButton:
      MrktDisplay.print(F("M Button        "));
      break;
  }
  MrktDisplay.setCursor(10, 1);
  MrktDisplay.print(event.data);
  
  // next state: waiting mode after the message display delay
  this->state = GrblWaiting;
  this->stateChangeTime = millis() + INIT_MODE_EVENT_DISPLAY_TIME;
}

void InitializationMode::loopGrblSearchStart() {
  // show a message that we're attempting to contact the Grbl system
  MrktDisplay.setCursor(0, 1);
  MrktDisplay.print(F("Grbl            "));
  MrktDisplay.writeEllipsis(5, 1);

  // send the $I command to query the version identification 
  MrktCommunication.sendGrblCommand("$I", INIT_MODE_COMM_TIMEOUT, &InitializationMode::handleVersionQueryResponse);

  // next state: waiting mode without any delay
  this->state = GrblWaiting;
  this->stateChangeTime = 0;
}

void InitializationMode::loopGrblWaiting() {
  // if a user control event is encountered while waiting for a communication reply, 
  // switch to event display mode immediately
  if (MrktUserControls.isEventAvailable()) {
    this->state = EventDisplay;
    this->stateChangeTime = 0;
  }
  // this state is only left through the communication response handler 
}

void InitializationMode::loopGrblCommError() {
  // show the communication status received
  MrktDisplay.setCursor(0, 1);
  if (this->grblCommStatus < 0) {
    MrktDisplay.print(F("Grbl Com Err    "));
  } else {
    MrktDisplay.print(F("Grbl Cmd Err    "));
  }
  MrktDisplay.setCursor(13, 1);
  MrktDisplay.print(this->grblCommStatus);

  // re-start seach after a brief delay
  this->state = GrblSearchStart;
  this->stateChangeTime = millis() + INIT_MODE_COMM_RETRY_DELAY;
}

void InitializationMode::loopGrblVersionFound() {
  // show the version information extracted
  MrktDisplay.setCursor(0, 1);
  MrktDisplay.print(F("Grbl            "));
  MrktDisplay.setCursor(5, 1);
  MrktDisplay.print(this->grblVersion);

  // check the version and set the subsequent status
  if (this->grblVersion[0] == '1') {
    this->state = GrblVersionOK;
  } else {
    this->state = GrblVersionError;
  }
  this->stateChangeTime = 0;  
}

void InitializationMode::loopGrblVersionError() {
  // set the main status LED to fast blinking mode
  this->blinkInterval = INIT_MODE_BLINK_INTERVAL_ERROR;

  // show the error status
  MrktDisplay.setCursor(12, 1);
  MrktDisplay.print(F("ERR"));

  // this state can only be left through a system reset
  // set the state time high enough to prevent the display from flickering
  this->stateChangeTime = millis() + INIT_MODE_GRBL_DISPLAY_TIME;
}

void InitializationMode::loopGrblVersionOK() {
  // show the status
  MrktDisplay.setCursor(12, 1);
  MrktDisplay.print(F("OK"));

  // after a certain delay, hand over to the mode controller to swith to another mode
  this->state = Final;
  this->stateChangeTime = millis() + INIT_MODE_GRBL_DISPLAY_TIME;  
}

void InitializationMode::loopFinal() {
  // hand over to the actual working mode
  MrktModeController.switchToInitialWorkingMode();
}

void InitializationMode::handleVersionQueryResponse(int status, char * response) {
  if (status == 0) {
    // OK - extract the version string from the message
    // The version string looks like this: [VER:1.1d.20161014:]
    //                  We need this part:      ^^^^
    char *version = strstr(response, "VER:") + 4;
    char *firstDot = strstr(version, ".");
    char *secondDot = strstr(firstDot + 1, ".");
    strncpy(MrktInitializationMode.grblVersion, version, (secondDot - version)); 

    // immediately switch state to evaluate version
    MrktInitializationMode.state = GrblVersionFound;
    MrktInitializationMode.stateChangeTime = 0;
  } else {
    // store error and switch state to display code
    MrktInitializationMode.grblCommStatus = status;
    MrktInitializationMode.state = GrblCommError;
    MrktInitializationMode.stateChangeTime = 0;
  }
}

