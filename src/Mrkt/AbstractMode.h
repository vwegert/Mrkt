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
#ifndef MRKT_AbstractMode_h
#define MRKT_AbstractMode_h

#include "Configuration.h"

/**
 * The abstract superclass of the internal operating modes of the 
 * Mrkt system.
 */
class AbstractMode {
  
  public:
    /**
     * The default constructor.
     */
    AbstractMode();

    /**
     * This method is called when the mode is activated. It is called before
     * the first call to loop().
     */
    virtual void activate() = 0;

    /**
     * This method is called as part of the main loop iteration.
     */
    virtual void loop() = 0;

    /**
     * This method is called before the mode is deactivated and another mode
     * is activated.
     */
    virtual void deactivate() = 0;
    
};

#endif
