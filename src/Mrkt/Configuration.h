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

// -----------------------------------------------------------------------------
//   GENERAL CONFIGURATION
// -----------------------------------------------------------------------------

// Set this to 0 if you do not have a SD card reader installed. 
#define SDCARD_AVAILABLE 1 // 1 = yes, 0 = no

// The baud rates to use to connect to the host and the Grbl system. Note that 
// it is hard to get a reliable connection using the Grbl default speed of 
// 115.200 baud with an Arduino Uno - hence the lower default speed. 
// It is advisable to use the same baud rate for both connections!
#define HOST_SERIAL_SPEED 57600
#define GRBL_SERIAL_SPEED 57600

// -----------------------------------------------------------------------------
//   HARDWARE CONFIGURATION
// -----------------------------------------------------------------------------

// Adapt these settings if your hardware differs from the standard layout.
// All remarks regarding pin compatibility and usage relate to an Arduino Uno R3.

// Pins claimed by the LCD Keypad Shield - you probably won't be able to change
// these unless you choose not to use that particular shield (which is perfectly
// possible, just a tad more work).
#define LCD_DB4              4
#define LCD_DB5              5
#define LCD_DB6              6
#define LCD_DB7              7
#define LCD_RS               8
#define LCD_EN               9 // enable
#define LCD_BL              10 // backlight PWM
#define KEYPAD_BUTTONS      A0

// Pins used to connect to the SD card reader - if you intend to add an SD card
// reader, you will have to use the pins 11 to 13 because they provide the SPI 
// interface required.
#define SDCARD_MOSI         11
#define SDCARD_MISO         12
#define SDCARD_SCK          13
#define SDCARD_CS           A3

// Pins used to connect the user interface elements: the rotary encoder and the
// main mode switch and LED. It is adivsable to keep the rotary encoder connected
// to pins 2 and 3 because the interrupt-driven handling routine is much more 
// precise.
#define RE_CLOCK             2
#define RE_DATA              3
#define RE_MODE_ENC_BUTTONS A5
#define MAIN_LED            A4

// Some encoders do strange things: They record steps in increments of four, and 
// and some will register clockwise turns as negative increments. Use these settings
// to adjust...
#define RE_STEP_SIZE         4
#define RE_INVERT_DIRECTION  1 // 1 = yes, 0 = no

// Pins used to connect to the grbl system we are controlling.
#define GRBL_RX             A1 // connect this to Grbl Pin D1 (Tx)
#define GRBL_TX             A2 // connect this to Grbl Pin D0 (Rx)

// Pins to connect to the host system. These are the serial pins - we have to keep 
// these pins unoccupied even if we're connected via USB.
#define HOST_RX              0
#define HOST_TX              1

// -----------------------------------------------------------------------------

// There should be no need to change anything beyond this line.

#define MRKT_VERSION     "0.1a" // max. 11 chars, rest will be cut off

