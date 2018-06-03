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
#ifndef MRKT_InitializationMode_h
#define MRKT_InitializationMode_h

#include "Configuration.h"
#include "AbstractMode.h"

/**
 * The number of characters to reserve for the Grbl version number.
 */
#define GRBL_VERSION_SIZE 6

/**
 * This class implements the initialization phase of Mrkt. It is started 
 * automatically when Mrkt is switched on, initializes the hardware and 
 * checks whether the Grbl system is accessible and has the right version.
 * It also provides a simple test mode for the user controls to check 
 * the wiring.
 * 
 * The initialization mode follows the internal states described by the following diagram.
 * The lines beneath the status name show an example of the display contents.
 * 
 *                                                    .───────────────.                             
 *                                                   (    power up     )                            
 *                                                    `───────────────'                             
 *                                                            │                                     
 *                                                            │                                     
 *                                                            ▼                                     
 *                                                   ┌─────────────────┐                            
 *                                                   │Initial          │                            
 *                                                   │Mrkt_1.0a_______ │                            
 *                                                   │________________ │                            
 *                                                   └─────────────────┘                            
 *                                                            │                                     
 *                                                            │                                     
 *                                                            ▼                                     
 *                                                   ┌─────────────────┐                            
 *                                           $I      │GrblSearchStart  │                            
 *       ┌───────┐◀──────────────────────────────────│Mrkt_1.0a_______ │◀─────────────┐             
 *       │░░░░░░░│                                   │Grbl_…__________ │              │             
 *       │░░░░░░░│                                   └─────────────────┘              │             
 *       │░░░░░░░│                                            │                       │             
 *       │░░░░░░░│            INIT_MODE_EVENT_DISPLAY_TIME────┤                       │             
 *       │░░░░░░░│              │                             │                       │             
 *       │░░░░░░░│              │                             ▼                       │             
 *       │░░░░░░░│     ┌─────────────────┐           ┌─────────────────┐ INIT_MODE_COMM_RETRY_DELAY 
 *       │░░░░░░░│     │EventDisplay     │           │GrblWaiting      │              │             
 *       │░Grbl░░│     │Mrkt_1.0a_______ │◀──────────│Mrkt_1.0a_______ │              │             
 *       │░░░░░░░│     │E_Wheel___-5____ │           │Grbl_…__________ │              │             
 *       │░░░░░░░│     └─────────────────┘           └─────────────────┘              │             
 *       │░░░░░░░│                                            │                       │             
 *       │░░░░░░░│                                            │                       │             
 *       │░░░░░░░│                                ┌───────────┴───────────┐           │             
 *       │░░░░░░░│                                │                       │           │             
 *       │░░░░░░░│                                ▼                       ▼           │             
 *       │░░░░░░░│                       ┌─────────────────┐     ┌─────────────────┐  │             
 *       │░░░░░░░│  [VER:1.1d.20161014:] │GrblVersionFound │     │GrblCommError    │  │             
 *       └───────┘──[OPT:,15,128]───────▶│Mrkt_1.0a_______ │     │Mrkt_1.0a_______ │──┘             
 *                  ok                   │Grbl_1.1d_______ │     │Grbl_Com_Err_-2_ │                
 *                                       └─────────────────┘     └─────────────────┘                
 *                                                │                                                 
 *                                                │                                                 
 *                                    ┌───────────┴────────────┐                                    
 *                                    │                        │                                    
 *                                    ▼                        ▼                                    
 *                           ┌─────────────────┐      ┌─────────────────┐                           
 *                           │GrblVersionOK    │      │GrblVersionError │                           
 *                           │Mrkt_1.0a_______ │      │Mrkt_1.0a_______ │                           
 *                           │Grbl_1.1d___OK__ │      │Grbl_0.9f___ERR_ │                           
 *                           └─────────────────┘      └─────────────────┘                           
 *                                    │                                                             
 *                                    │                                                             
 *                       INIT_MODE_GRBL_DISPLAY_TIME                                                
 *                                    │                                                             
 *                                    ▼                                                             
 *                           ┌─────────────────┐                                                    
 *                           │Final            │                                                    
 *                           │Mrkt_1.0a_______ │                                                    
 *                           │Grbl_1.1d___OK__ │                                                    
 *                           └─────────────────┘                                                    
 *                                    │                                                             
 *                                    │                                                             
 *                                    ▼                                                             
 *                   .─────────────────────────────────.                                            
 *                  ( hand over to initial working mode )                                           
 *                   `─────────────────────────────────'                                            
 */
class InitializationMode : public AbstractMode {
  
  public:
    /**
     * The default constructor.
     */
    InitializationMode();

    /**
     * See AbstractMode for an explanation of these methods.
     */
    virtual void activate();
    virtual void loop();
    virtual void deactivate();

  private:
    /**
     * The enumeration to represent the internal state of the mode implementation.                                         
     */ 
    enum InternalState { 
      Initial, 
      EventDisplay, 
      GrblSearchStart, 
      GrblWaiting, 
      GrblCommError, 
      GrblVersionFound, 
      GrblVersionError, 
      GrblVersionOK, 
      Final 
    };
    InternalState state;

    /**
     * The system time to wait for before performing the next state check.
     */
    uint32_t stateChangeTime;

    /**
     * The value of the mode LED (used for blinking).
     */
    uint8_t mainLEDStatus;

    /**
     * When the state of the mode LED was last changed.
     */
    uint32_t prevBlinkTime;

    /**
     * The interval at which the mode LED is blinking.
     */
    uint16_t blinkInterval;

    /**
     * The Grbl version as extracted from the $I response.
     */
    char grblVersion[GRBL_VERSION_SIZE];

    /**
     * The status of the Grbl communication (see Communication.h, COMMUNICATION_STATUS_*).
     */
    int grblCommStatus;

    /** 
     * The handler method for the $I command.
     */
    static void handleVersionQueryResponse(int status, char * response);

    /**
     * The implementations called during the loop() processing for each internal state.
     */
    void loopInitial();
    void loopEventDisplay();
    void loopGrblSearchStart();
    void loopGrblWaiting();
    void loopGrblCommError();
    void loopGrblVersionFound();
    void loopGrblVersionError();
    void loopGrblVersionOK();
    void loopFinal();
};

/**
 * Access to the "singleton" instance of the InitializationMode class.
 */
extern InitializationMode MrktInitializationMode;

#endif
