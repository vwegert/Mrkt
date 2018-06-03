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

// This is the main sketch file of the project.
// In order to customize Mrkt, check the file Configuration.h. No changes
// to any other file should be necessary.

#include "ModeController.h"

void setup() {
  MrktModeController = ModeController();
}

void loop() {
  MrktModeController.loop();
}
