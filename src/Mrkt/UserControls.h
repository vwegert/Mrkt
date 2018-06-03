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
#ifndef MRKT_UserControls_h
#define MRKT_UserControls_h

#include <Encoder.h> // see https://www.pjrc.com/teensy/td_libs_Encoder.html

#include "Configuration.h"

/**
 * The size of the event queue. Due to the deduplication of event repetitions,
 * this queue does not need to be overly large - it is rarely ever used at all.
 */
#define USER_CONTROLS_EVENT_BUFFER_SIZE 3

/**
 * This class handles the user interactiouns through the various buttons and the
 * rotary encoder. It is integrated into the main loop and provides an event queue
 * for convenient access.
 */
class UserControls {

  public:
  
    /**
     * The events that can be triggered by the user.
     */
    enum EventType { None, KeyLeft, KeyRight, KeyUp, KeyDown, KeySelect, EncChanged, EncButton, ModeButton };

    /**
     * The data of an entry in the event queue. An event consists of the event type and
     * an integer that represents the number of steps turned for EncChanged events and 
     * the number of repetitions for all other event types.
     */
    struct Event {
      EventType type;
      int8_t data; 
    };

    /**
     * The default constructor.
     */
    UserControls();

    /**
     * This method has to be called from the main loop.
     */
    void loop();

    /**
     * Removes all pending events from the event queue.
     */
    void clearEvents();

    /**
     * Checks whether an event is available in the queue.
     */
    bool isEventAvailable();

    /**
     * Removes and returns the next event from the queue.
     */
    Event getEvent();

  private:

    /**
     * The event queue buffer. The queue is organized as a ring buffer.
     */
    Event eventQueue[USER_CONTROLS_EVENT_BUFFER_SIZE];

    /**
     * The start and end position of the current queue inside the ring buffer.
     */
    uint8_t eventQueueStart = 0;
    uint8_t eventQueueEnd = 0;

    /**
     * This bit mask stores the state of the buttons between loop iterations 
     * to detect newly pressed buttons.
     */
    uint8_t prevButtonState = 0;

    /**
     * The encoder position encountered during the last loop iteration.
     */
    int32_t prevEncoderPosition = 0;

    /**
     * This object is provided by an external library to handle the rotary 
     * encoder input.
     */
    Encoder * encoder;

    /**
     * Stores an event in the event queue. This method checks whether the last event
     * is of the same type and adds the data in this case. If the event queue is full,
     * the event is discarded silently.
     */
    void queueEvent(EventType type, int8_t data);
    
};

/**
 * Access to the "singleton" instance of the UserControls class.
 */
extern UserControls MrktUserControls;

#endif
