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
#ifndef MRKT_ModeController_h
#define MRKT_ModeController_h

#include "Configuration.h"
#include "AbstractMode.h"

/**
 * This is the main controller object that handles the modes that the 
 * Mrkt system can be in. The actual logic is encapsulated in the various
 * mode implementations that are derived from the AbstractMode class.
 */
class ModeController {

  public:
    /**
     * This enum represents the various modes that the system can be in.
     */
#if SDCARD_AVAILABLE == 1
    enum Mode { Initialization, Command, Passthrough, Reader };
#else
    enum Mode { Initialization, Command, Passthrough };
#endif

    /**
     * The default constructor.
     */
    ModeController();

    /**
     * This method has to be called from the main loop. It delegates the 
     * call to the subsystem controllers.
     */
    void loop();

    /**
     * This method is used to switch the Mrkt system into the initial working mode
     * after the system initialization is complete.
     */
    void switchToInitialWorkingMode();

    /**
     * Switches the system to a new mode. The actual switch is done during the next 
     * main loop iteration to allow the current mode to complete its operations gracefully.
     */
    void switchToMode(Mode newMode);

  private:
    /**
     * The mode the system is currently in.
     */
    Mode currentMode;

    /**
     * The mode to switch to during the next main loop iteration.
     */
    Mode targetMode;

    /**
     * A pointer to the current mode to make the method calls a bit faster.
     */
    AbstractMode * currentModeInstance;

};

/**
 * Access to the "singleton" instance of the ModeController class.
 */
extern ModeController MrktModeController;

#endif
