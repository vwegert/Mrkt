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
#ifndef MRKT_Display_h
#define MRKT_Display_h

#include <LiquidCrystal.h> // see https://www.arduino.cc/en/Reference/LiquidCrystal

#include "Configuration.h"

/**
 * This class represents the display options used to communicate with the user. It 
 * handles both the 16x2 LCD as well as the main mode LED.
 */
class Display : public LiquidCrystal {
  
  public:

    /**
     * The default constructor.
     */
    Display();

    /**
     * see LiquidCrystal::begin();
     */
    void begin();

    /**
     * Writes the Feedrate symbol (FR) to the current position or the position specified.
     */
    void writeFeedrate();
    void writeFeedrate(uint8_t col, uint8_t row);

    /**
     * Writes the plus/minus symbol to the current position or the position specified
     */
    void writePlusMinus();
    void writePlusMinus(uint8_t col, uint8_t row);
    
    /**
     * Writes the ellipsis symbol (...) to the current position or the position specified
     */
    void writeEllipsis();
    void writeEllipsis(uint8_t col, uint8_t row);

    /**
     * Sets the level of the main mode LED.
     */
    void setMainLED(uint8_t level);

  private:

};

/**
 * Access to the "singleton" instance of the Display class.
 */
extern Display MrktDisplay;

#endif
