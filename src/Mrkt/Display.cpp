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
#include "Display.h"

/** 
 * The numbers of the custom characters. 
 */
#define DISPLAY_CH_FEEDRATE  0 // FR
#define DISPLAY_CH_PLUSMINUS 1 // +-
#define DISPLAY_CH_ELLIPSIS  3 // ...

/**
 * The size of the LCD panel. Note that if you use anything else than a 
 * 1602 panel, you will have to adapt a lot of code...
 */
#define DISPLAY_LCD_LINES        2
#define DISPLAY_LCD_COLUMNS     16

/**
 * The bit patterns of the custom characters. For symbol generation, 
 * see https://omerk.github.io/lcdchargen/
 */
const byte Display_CustomCharFR[8] PROGMEM = {
  0b11100,
  0b10000,
  0b11000,
  0b10110,
  0b10101,
  0b00110,
  0b00101,
  0b00101
};  
const byte Display_CustomCharPM[8] PROGMEM= {
  0b00100,
  0b00100,
  0b11111,
  0b00100,
  0b00100,
  0b00000,
  0b11111,
  0b00000
}; 
const byte Display_CustomCharE[8] PROGMEM = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10101,
  0b00000
}; 

/**
 * The "singleton" instance of the Display class.
 */
Display MrktDisplay;

Display::Display() : LiquidCrystal(LCD_RS, LCD_EN, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7) {
  // setup the custom characters
  createChar(DISPLAY_CH_FEEDRATE,  (uint8_t*)Display_CustomCharFR); // feedrate symbol
  createChar(DISPLAY_CH_PLUSMINUS, (uint8_t*)Display_CustomCharPM); // plus-minus symbol
  createChar(DISPLAY_CH_ELLIPSIS,  (uint8_t*)Display_CustomCharE);  // ellipsis (three dots)
  
  // setup the backlight and main mode LED 
  pinMode(MAIN_LED, OUTPUT); 
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);  
};

void Display::begin() {
  LiquidCrystal::begin(DISPLAY_LCD_LINES, DISPLAY_LCD_COLUMNS);
}

void Display::writeFeedrate() {
  write(byte(DISPLAY_CH_FEEDRATE));
}

void Display::writeFeedrate(uint8_t col, uint8_t row) {
  setCursor(col, row);
  writeFeedrate();
}

void Display::writePlusMinus() {
  write(byte(DISPLAY_CH_PLUSMINUS));  
}

void Display::writePlusMinus(uint8_t col, uint8_t row) {
  setCursor(col, row);
  writePlusMinus();
}

void Display::writeEllipsis() {
  write(byte(DISPLAY_CH_ELLIPSIS));  
}

void Display::writeEllipsis(uint8_t col, uint8_t row) {
  setCursor(col, row);
  writeEllipsis();
}

void Display::setMainLED(uint8_t level) {
  digitalWrite(MAIN_LED, level);
}

