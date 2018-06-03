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
#include "ModeController.h"

#include "Communication.h"
#include "Display.h"
#include "InitializationMode.h"
#include "UserControls.h"

/**
 * The "singleton" instance of the ModeController class.
 */
ModeController MrktModeController;

ModeController::ModeController() {

  // initialize the LCD screen
  MrktDisplay = Display();
  MrktDisplay.begin();

  // initialize the communication subcontroller
  MrktCommunication = Communication();
  
  // initialize the user control interface
  MrktUserControls = UserControls();

  // initialize the individual modes
  MrktInitializationMode = InitializationMode();

  // TODO initialize other modes

  // set the initialization mode on system startup
  MrktInitializationMode.activate();
  this->currentMode = Initialization;
  this->currentModeInstance = & MrktInitializationMode;
  this->targetMode = currentMode;
}

void ModeController::loop() {
  // delegate to the various sub-controllers and the current mode implementation
  MrktCommunication.loop();
  MrktUserControls.loop();
  this->currentModeInstance->loop();

  // handle a mode switch if requested
  if (this->targetMode != this->currentMode) {
    this->currentModeInstance->deactivate();
    this->currentMode = targetMode;
    switch(currentMode) {
      case Initialization:
        this->currentModeInstance = & MrktInitializationMode;
        break;
      case Command:
// TODO       this->currentModeInstance = & MrktCommandMode;
        break;
      case Passthrough:
// TODO       this->currentModeInstance = & MrktPassthroughMode;
        break;
    }
    this->currentModeInstance->activate();
  }
}

void ModeController::switchToInitialWorkingMode() {
  // TODO make this configurable
  switchToMode(Command);
}

void ModeController::switchToMode(Mode newMode) {
  this->targetMode = newMode;
}


