
// ***   for the TFT_eSPI
/***************************************************
  Arduino TFT graphics library targetted at ESP8266
  based boards. (ESP32 support is planned!)

  This library has been derived from the Adafruit_GFX
  library and the associated driver library. See text
  at the end of this file.

  This is a standalone library that contains the
  hardware driver, the graphics funtions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce
  their FLASH footprint.

 ****************************************************/
 /***************************************************

  ORIGINAL LIBRARY HEADER

  This is our library for the Adafruit  ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

  Updated with new functions by Bodmer 14/4/15
 ****************************************************/
 // ***   for the XPT2046
/*
 * Copyright (c) 2015-2016  Spiros Papadimitriou
 *
 * This file is part of github.com/spapadim/XPT2046 and is released
 * under the MIT License: https://opensource.org/licenses/MIT
 *
 * This software is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied.
 */

 // *** For OSD keyboard
 /***************************************************
 Copyleft (cl) 2017  Agge the grumpy bastard

    OSD keyboard was written for a easy way to
    configure my ESP8266 module using a TFT ILI9341
    and a XPT2046 touch controller.
    The "lib2 is just a framework for adding buttons
    and text for each use.

    The Code is crappy, uses more memory and
    resources than what a decent C++ programmer
    would wright, But what the heck I'm a novis.

    Use at own risk. Tweak and improve.
    But your improvements would be nice to
    implement so send it over

 ****************************************************/

#include "Arduino.h"
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
#include <XPT2046.h>

#ifndef OSDKeyboard_h
#define OSDKeyboard_h

extern char keyboardArray[255];// create for virtual keyboard

//extern char keyboardArray[63];


/*You have to put extern before your prototyped function you want to use in your library*/
extern void KeyBoard(long widht, long hight, char place);
void IRQTest();
void EXTest();
int scroll_line();
void setupScrollArea(uint16_t tfa, uint16_t bfa);
void scrollAddress(uint16_t vsp);
void DrawKeyBoard(long widht, long hight, char place);
#endif



