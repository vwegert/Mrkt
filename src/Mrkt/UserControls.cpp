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
#include "UserControls.h"

/**
 * The bit mask values used for the button state processing.
 */
#define USER_CONTROLS_BUTTON_UP       1
#define USER_CONTROLS_BUTTON_DOWN     2
#define USER_CONTROLS_BUTTON_LEFT     4
#define USER_CONTROLS_BUTTON_RIGHT    8
#define USER_CONTROLS_BUTTON_SELECT  16
#define USER_CONTROLS_BUTTON_ENCODER 32
#define USER_CONTROLS_BUTTON_MODE    64

/**
 * The "singleton" instance of the UserControls class.
 */
UserControls MrktUserControls;

UserControls::UserControls() {
  // configure the hardware 
  pinMode(KEYPAD_BUTTONS, INPUT);
  pinMode(RE_MODE_ENC_BUTTONS, INPUT);

  // initialize the rotary encoder instance
  this->encoder = new Encoder(RE_CLOCK, RE_DATA);

  // initialize the event queue
  clearEvents(); 
}

void UserControls::loop() {

  // this variable will contain a bit mask of the buttons that are currently pressed
  uint8_t currentButtonState = 0;

  // check the keypad buttons
  int keypadValue = analogRead(KEYPAD_BUTTONS);
  if (keypadValue <= 50) {
    currentButtonState |= USER_CONTROLS_BUTTON_RIGHT;
  } else if (keypadValue <= 250) {
    currentButtonState |= USER_CONTROLS_BUTTON_UP;
  } else if (keypadValue <= 450) {
    currentButtonState |= USER_CONTROLS_BUTTON_DOWN;
  } else if (keypadValue <= 650) {
    currentButtonState |= USER_CONTROLS_BUTTON_LEFT;
  } else if (keypadValue <= 850) {
    currentButtonState |= USER_CONTROLS_BUTTON_SELECT;
  }

  // check the mode and encoder buttons
  int buttonValue = analogRead(RE_MODE_ENC_BUTTONS);
  if (buttonValue <= 250) {
      currentButtonState |= USER_CONTROLS_BUTTON_MODE;
  } else if (buttonValue <= 750) {
    currentButtonState |= USER_CONTROLS_BUTTON_ENCODER;
  }

  // if the button state has changed, update the event queue 
  if (currentButtonState != this->prevButtonState) {

    // we want to trigger on the button presses, so we create a
    // bit mask of the newly pressed buttons by masking out the ones that
    // were pressed during the previous pass
    uint8_t pressedButtons = currentButtonState & ~this->prevButtonState;
    if ((pressedButtons & USER_CONTROLS_BUTTON_UP) > 0) {
      queueEvent(UserControls::KeyUp, 1);
    } 
    if ((pressedButtons & USER_CONTROLS_BUTTON_DOWN) > 0) {
      queueEvent(UserControls::KeyDown, 1);
    } 
    if ((pressedButtons & USER_CONTROLS_BUTTON_LEFT) > 0) {
      queueEvent(UserControls::KeyLeft, 1);
    } 
    if ((pressedButtons & USER_CONTROLS_BUTTON_RIGHT) > 0) {
      queueEvent(UserControls::KeyRight, 1);
    } 
    if ((pressedButtons & USER_CONTROLS_BUTTON_SELECT) > 0) {
      queueEvent(UserControls::KeySelect, 1);
    } 
    if ((pressedButtons & USER_CONTROLS_BUTTON_ENCODER) > 0) {
      queueEvent(UserControls::EncButton, 1);
    } 
    if ((pressedButtons & USER_CONTROLS_BUTTON_MODE) > 0) {
      queueEvent(UserControls::ModeButton, 1);
    } 

    this->prevButtonState = currentButtonState;
  } 
  
  // check the encoder wheel
  int32_t currentEncoderPosition = this->encoder->read() / RE_STEP_SIZE;
  if (currentEncoderPosition != this->prevEncoderPosition) {
#if RE_INVERT_DIRECTION == 1
    int8_t encoderData = - (currentEncoderPosition - this->prevEncoderPosition);
#else
    int8_t encoderData =   (currentEncoderPosition - this->prevEncoderPosition);
#endif
    queueEvent(UserControls::EncChanged, encoderData);
    this->prevEncoderPosition = currentEncoderPosition; 
  }
}

void UserControls::clearEvents() {
  for(int i = 0; i < USER_CONTROLS_EVENT_BUFFER_SIZE; i++) {
    this->eventQueue[i].type = UserControls::None;
    this->eventQueue[i].data  = 0;
  }
}

bool UserControls::isEventAvailable() {
  return (this->eventQueue[this->eventQueueStart].type != None);
}

UserControls::Event UserControls::getEvent() {
  // get the event at the first position
  Event result = this->eventQueue[this->eventQueueStart];
  if (result.type != UserControls::None) {
    // remove the current event
    this->eventQueue[this->eventQueueStart].type = UserControls::None;
    this->eventQueue[this->eventQueueStart].data = 0;
    // move the start pointer on if the queue contains more than one event
    if (this->eventQueueStart != this->eventQueueEnd) {
      if (this->eventQueueStart < USER_CONTROLS_EVENT_BUFFER_SIZE - 1) {
        this->eventQueueStart += 1;
      } else {
        this->eventQueueStart = 0;
      }
    }
  }
  return result;
}

void UserControls::queueEvent(EventType type, int8_t data) {
  // check whether the queue is full - if that is the case, we have to drop the event
  if (!(this->eventQueueStart == 0 && this->eventQueueEnd == USER_CONTROLS_EVENT_BUFFER_SIZE - 1) &&
      !(this->eventQueueEnd == this->eventQueueStart - 1)) {
    // if this is a repetition of the last event in the queue, increment the counter in the data field
    if (this->eventQueue[this->eventQueueEnd].type == type) {
      this->eventQueue[this->eventQueueEnd].data += data;
    } else {
      // queue a new event
      if (this->eventQueue[this->eventQueueEnd].type != UserControls::None) {
        // queue is not empty - move end pointer to new empty element
        if (this->eventQueueEnd < USER_CONTROLS_EVENT_BUFFER_SIZE - 1) {
          this->eventQueueEnd += 1;
        } else {
          this->eventQueueEnd = 0;
        }
      }
      // insert event at the current end position
      this->eventQueue[this->eventQueueEnd].type = type;
      this->eventQueue[this->eventQueueEnd].data = data;
    }
  }
}

