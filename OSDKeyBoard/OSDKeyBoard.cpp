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
#include "OSDKeyboard.h"

/// The scrolling area must be a integral multiple of TEXT_HEIGHT
#define TEXT_HEIGHT 16 // Height of text to be printed and scrolled
#define BOT_FIXED_AREA 92// Number of lines in bottom fixed area (lines counted from bottom of screen) 0
#define TOP_FIXED_AREA 148 // Number of lines in top fixed area (lines counted from top of screen)
#define YMAX 320 // Bottom of screen area
///

// The initial y coordinate of the top of the scrolling area
uint16_t yStart = TOP_FIXED_AREA;
// yArea must be a integral multiple of TEXT_HEIGHT
uint16_t yArea = YMAX - TOP_FIXED_AREA - BOT_FIXED_AREA;
// The initial y coordinate of the top of the bottom text line
uint16_t yDraw = TOP_FIXED_AREA - TEXT_HEIGHT;
uint16_t old_yDraw = TOP_FIXED_AREA - TEXT_HEIGHT;
//uint16_t yDraw = YMAX - BOT_FIXED_AREA - TEXT_HEIGHT;

// Keep track of the drawing x coordinate
uint16_t xPos = 0;
uint16_t old_xPos = 0;
//in library file:
// We have to blank the top line each time the display is scrolled, but this takes up to 13 milliseconds
// for a full width line, meanwhile the serial buffer may be filling... and overflowing
// We can speed up scrolling of short text lines by just blanking the character we drew
int blank[19]; // We keep all the strings pixel lengths to optimise the speed of the top line blanking

int scrolling = 0;
///


extern char keyboardArray[255];

void KeyBoard(long widht, long hight, char place)
{

// For the byte we read from the keyboard
byte data = 0;

// A few test variables used during debugging
boolean change_colour = 1;
boolean selected = 1;

// Setup scroll area
// setupScrollArea(TOP_FIXED_AREA, BOT_FIXED_AREA);
  // Zero the array
  for (int i = 0; i<18; i++) {
    blank[i]=0;
}

/// set up a coordinates matrix for delete and put cursor at right place
long int termMatrix[254][2];
for (int i =0; i < 254; i++) {
  for (int j =0; j < 2; j++) {
    termMatrix[i][j] = 0;
  }
}
/// will make it an terminal later on
    Serial.println("");
    Serial.println("OSD Keyboard started");

 long placeMent = 0;
 int charCounter = 0 ; // cont and fill the keyboardArray
 long colSpacing = widht/10 -1 ; /// 10 columns gives width/10 -1 = 23 (240 pixles)
 long rowSpacing = hight/12 ;  /// 27; (240 pixles)
 int charColOffset = 8;
 int charRowOffset = 4;
 int rowNumber = 0;
 int colNumber = 0;
 bool isToched = 0; // Software IRQ
 int tochRow = 0;
 int tochCol = 0 ;
 bool shiftOn = 0;
 bool cursorTOGGLE  = 0 ;
 unsigned long cursorHz = 500 ; /// in millisecond gives around 2 HZ blinking cursor
 unsigned long cursorTimer = millis();
 unsigned long cursorPreviusTimer = 0 ;
 bool debuncerTOGGLE  = 0 ;
 unsigned long debuncer = 200 ; /// in millisecond gives around 2 HZ blinking cursor
 unsigned long debuncerTimer = millis();
 unsigned long debuncerPreviusTimer = 0 ;
 String TerminalString =  String("   AGGE OSD keyboard Ver 1.0");

/// BROKEN since use of hardware scroll !!!!!
/// place keyboard on top or bottom
if (place == 't' || place == 'T'){
    placeMent = 0;
}
else if (place == 'b' || place == 'B'){
   placeMent = hight - rowSpacing * 5 -2 ;
}

/// the keyboard mapping
/// will ad triple shift for special chars and for terminal
char RowColChar [2][5][10] =    /// RowColChar [shiftOn][row][column]
         {
            {
            {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
            {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
            {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'z'},
            {' ', ' ', 'x', 'c', 'v', 'b', 'n', 'm', ' ', ' '},
            {'+', '?', '[', '{', ' ', ' ', '}', ']', '$', '-'}
            },
            {
            {'!', '@', '#', '$', '%', '^', '~', '`', '(', ')'},
            {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P'},
            {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z'},
            {' ', ' ', 'X', 'C', 'V', 'B', 'N', 'M', ' ', ' '},
            {'=', '.', ':', '/', ' ', ' ', '|', ';', ',', '_'}
            },

        };

/// init the TFT_eSPI lib
  TFT_eSPI tft = TFT_eSPI();
  tft.init();
  tft.setRotation(0); /// must be set when using hardware scroll on tft.
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
/// init the XPT2046 touch lib
  XPT2046 touch( D2, D1);
  touch.begin(widht, hight);
  touch.setCalibration(296, 1739, 1665, 334);
  pinMode(LED_BUILTIN, OUTPUT);

/// Start up text TFT
    tft.drawString("   AGGE OSD keyboard Ver 1.0", charColOffset,charRowOffset + hight /2 - rowSpacing, 2);
    tft.drawRect(0, hight /2 - rowSpacing , widht-1, rowSpacing * 2,  TFT_GREEN);
    delay(500);
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 287, 240, 32,  TFT_GREEN);

/// Lets make some start up character buttons
     while (rowNumber < 5 ){
        /// Do some font line up offsets here !!!
        for (colNumber=0; colNumber <= 10; colNumber++){
            /// the Shift,Del and Space buttons are different so catch em'
            if (rowNumber == 3 && colNumber == 0 ){
                tft.drawRoundRect(colNumber + colNumber*colSpacing, placeMent + rowNumber + rowNumber*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                    ///drawString(const char *string, int poX, int poY, int font),
                tft.drawString("Shift",charColOffset + colNumber + colNumber*colSpacing,placeMent + charRowOffset + rowNumber + rowNumber*rowSpacing, 2);
                colNumber++; // jump one
            }
            else if (rowNumber == 3 && colNumber == 8 ){
                tft.drawRoundRect(colNumber + colNumber*colSpacing,placeMent + rowNumber + rowNumber*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                    ///drawString(const char *string, int poX, int poY, int font),
                tft.drawString("Del",charColOffset+ 7 + colNumber + colNumber*colSpacing,placeMent + charRowOffset + rowNumber + rowNumber*rowSpacing, 2);
                colNumber++; // jump one
            }
            else if (rowNumber == 4 && colNumber == 4 ){
                tft.drawRoundRect(colNumber + colNumber*colSpacing,placeMent + rowNumber + rowNumber*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                    ///drawString(const char *string, int poX, int poY, int font),
                tft.drawString("Space",(charColOffset - 3) + colNumber + colNumber*colSpacing,placeMent + charRowOffset + rowNumber + rowNumber*rowSpacing, 2);
                colNumber++; // jump one
            }
            else {
            tft.drawRoundRect(colNumber + colNumber*colSpacing,placeMent + rowNumber + rowNumber*rowSpacing, colSpacing-2, rowSpacing-2, 3,  TFT_BLUE);
                ///drawRoundRect(int32_t x0, int32_t y0, int32_t w, int32_t h, int32_t radius, uint32_t color),
            tft.drawChar(RowColChar[shiftOn][rowNumber][colNumber],charColOffset + colNumber + colNumber*colSpacing,placeMent + charRowOffset + rowNumber + rowNumber*rowSpacing,2);
            }

         }
        rowNumber++;
     }
     rowNumber = 0; // reset for reuse
     digitalWrite(LED_BUILTIN, HIGH);
/// The "terminal" window
//tft.drawRect(0, hight /2 - rowSpacing , widht-1, rowSpacing * 2,  TFT_GREEN);


/// The initial graphics are drawn let us start the toch screen fetching and drawing
 // Are we typing ?

for( ; ; ) /// Loop forever until break
    { // stay in the keyboard typing loop until break
    ESP.wdtFeed();
    delay(1); /// Feed the esp8266 soft watch dog

/// Terminal Line breaking for three rows

    switch (isToched) { /// Highlight the tapped button





        case 0: /// The terminal window

                    ESP.wdtFeed();


                    digitalWrite(LED_BUILTIN, HIGH);
                    tft.setTextColor(TFT_WHITE, TFT_BLACK);

                /// Blinking cursor is nice

                 cursorTimer = millis();

                  if (cursorTimer - cursorPreviusTimer >= cursorHz) {

                    cursorPreviusTimer = cursorTimer;

                    if (cursorTOGGLE ==  0) {

                        /// cursor on
                        tft.fillRect(xPos ,yDraw +10 ,8,3,TFT_DARKGREY);
                        if (scrolling > 5){
                            if (scrolling == 6){ // remove old line
                                tft.fillRect(0, TOP_FIXED_AREA - TEXT_HEIGHT, widht , TEXT_HEIGHT , TFT_BLACK);
                            }
                            tft.setTextColor(TFT_BLACK, TFT_DARKGREY);
                            tft.drawString(" ------- Scrolling ------- ", 40, TOP_FIXED_AREA - TEXT_HEIGHT , 2);
                            tft.setTextColor(TFT_WHITE, TFT_BLACK);
                        }

                        cursorTOGGLE =  1;

                    } else {

                        /// cursor off
                        tft.fillRect(xPos ,yDraw +10 ,20,3,TFT_BLACK);
                        if (scrolling > 5){
                            tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
                            tft.drawString(" ------- Scrolling ------- ", 40, TOP_FIXED_AREA - TEXT_HEIGHT , 2);
                            tft.setTextColor(TFT_WHITE, TFT_BLACK);
                        }

                        cursorTOGGLE =  0;

                        }
                    }

            break;
        case 1:
            ESP.wdtFeed();
         /// debuncer
/*
            debuncerTimer = millis();
            if (debuncerTimer - debuncerPreviusTimer >= debuncer) {

                debuncerPreviusTimer = debuncerTimer;

                if (debuncerTOGGLE ==  0) {
*/
                /// run code here
            if (tochRow == 3 && tochCol <= 1 ){
                        int colCounter = 0 ; // temp counter
                        int rowCounter = 0 ; // temp counter

/// Switch shiftOn starts here
                switch (shiftOn) {

                    case 0:
                        ESP.wdtFeed();
                        while (rowCounter < 5 ){
                        /// Redraw every character "m" has to have extra cleaned !!!!
                            for (colCounter=0; colCounter <= 10; colCounter++){
                                /// Change all characters to unshifted ones
                                tft.drawChar(RowColChar[shiftOn][rowCounter][colCounter],charColOffset + colCounter + colCounter*colSpacing,placeMent + charRowOffset + rowCounter + rowCounter*rowSpacing,2);
                            }
                            rowCounter++;
                        }
                        rowCounter = 0; // reset for reuse

                        /// Retype Del and Space
                        tft.drawString("Del",charColOffset+ 7 + 8 + 8*colSpacing,placeMent + charRowOffset + 3 + 3*rowSpacing, 2);
                        tft.drawString("Space",(charColOffset - 3) + 4 + 4*colSpacing,placeMent + charRowOffset + 4 + 4*rowSpacing, 2);

                        /// unhighlight Shift
                        tft.fillRoundRect(0, placeMent + 3 + 3*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLACK);
                        tft.drawRoundRect(0, placeMent + 3 + 3*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                        tft.setTextColor(TFT_WHITE, TFT_BLACK);
                        tft.drawString("Shift",charColOffset,placeMent + charRowOffset + 3 + 3*rowSpacing, 2);
                        break;

                   case 1:
                        ESP.wdtFeed();
                        /// Redraw every character "M and W" has to have an smaller offset !!!!
                        while (rowCounter < 5){
                            for (colCounter=0; colCounter <= 10; colCounter++){
                                tft.drawChar(RowColChar[shiftOn][rowCounter][colCounter],charColOffset + colCounter + colCounter*colSpacing,placeMent + charRowOffset + rowCounter + rowCounter*rowSpacing,2);
                            }
                            rowCounter++;
                        }
                        rowCounter = 0; // reset for reuse

                        /// Retype Del and Space
                        tft.drawString("Del",charColOffset+ 7 + 8 + 8*colSpacing,placeMent + charRowOffset + 3 + 3*rowSpacing, 2);
                        tft.drawString("Space",(charColOffset - 3) + 4 + 4*colSpacing,placeMent + charRowOffset + 4 + 4*rowSpacing, 2);

                        /// highlight Shift
                        tft.fillRoundRect(0, placeMent + 3 + 3*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                        tft.setTextColor(TFT_WHITE, TFT_BLUE);
                        tft.drawString("Shift",charColOffset,placeMent + charRowOffset + 3 + 3*rowSpacing, 2);
                        tft.setTextColor(TFT_WHITE, TFT_BLACK);
                        break;
                }
            }
            else if (tochRow == 3 && tochCol >= 8 ){
                tft.fillRoundRect(8 + 8*colSpacing,placeMent + tochRow + tochRow*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                tft.setTextColor(TFT_WHITE, TFT_BLUE);
                tft.drawString("Del",charColOffset+ 7 + 8 + 8*colSpacing,placeMent + charRowOffset + tochRow + tochRow*rowSpacing, 2);
                delay(30);
                tft.fillRoundRect(8 + 8*colSpacing,placeMent + tochRow + tochRow*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLACK);
                tft.drawRoundRect(8 + 8*colSpacing,placeMent + tochRow + tochRow*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString("Del",charColOffset+ 7 + 8 + 8*colSpacing,placeMent + charRowOffset + tochRow + tochRow*rowSpacing, 2);

            }
            else if (tochRow == 4 && tochCol >= 4 && tochCol <=5){
                tft.fillRoundRect(4+ 4*colSpacing,placeMent + 4 + 4*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                tft.setTextColor(TFT_WHITE, TFT_BLUE);
                tft.drawString("Space",(charColOffset - 3) + 4 + 4*colSpacing,placeMent + charRowOffset + tochRow + tochRow*rowSpacing, 2);
                delay(30);
                tft.fillRoundRect(4+ 4*colSpacing,placeMent + 4 + 4*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLACK);
                tft.drawRoundRect(4+ 4*colSpacing,placeMent + 4 + 4*rowSpacing, 2*colSpacing -2, rowSpacing-2, 3,  TFT_BLUE);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawString("Space",(charColOffset - 3) + 4 + 4*colSpacing,placeMent + charRowOffset + tochRow + tochRow*rowSpacing, 2);
            }
            else {
                tft.fillRoundRect(tochCol + tochCol*colSpacing,placeMent + tochRow + tochRow*rowSpacing, colSpacing-2, rowSpacing-2, 3,  TFT_BLUE);
                tft.setTextColor(TFT_WHITE, TFT_BLUE);
                tft.drawChar(RowColChar[shiftOn][tochRow][tochCol],charColOffset + tochCol + tochCol*colSpacing,placeMent + charRowOffset + tochRow + tochRow*rowSpacing,2);
                delay(30);
                tft.fillRoundRect(tochCol + tochCol*colSpacing,placeMent + tochRow + tochRow*rowSpacing, colSpacing-2, rowSpacing-2, 3,  TFT_BLACK);
                tft.drawRoundRect(tochCol + tochCol*colSpacing,placeMent + tochRow + tochRow*rowSpacing, colSpacing-2, rowSpacing-2, 3,  TFT_BLUE);
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.drawChar(RowColChar[shiftOn][tochRow][tochCol],charColOffset + tochCol + tochCol*colSpacing,placeMent + charRowOffset + tochRow + tochRow*rowSpacing,2);
            }
            digitalWrite(LED_BUILTIN, LOW);
            isToched = 0;
            break;


        digitalWrite(LED_BUILTIN, HIGH);
        isToched = 0;
 //   } // debounce


                    debuncerTOGGLE =  1;

  //              } // debounce
               // else {

                 //   debuncerTOGGLE =  0;

               //} // debounce
            }


/// Switch isToched ends here
    while (touch.isTouching()) {
        ESP.wdtFeed();
        debuncerTimer = millis();
        digitalWrite(LED_BUILTIN, LOW);
        isToched = 1 ;
        uint16_t x, y;
        touch.getPosition(x, y);  // x col y row


        while(debuncer + debuncerTimer > millis() ){delay(1);} /// touch de bounce and assure that SPI from touch controller is transfered

    if (x > 0 && x < widht && y > 0 && y < placeMent + rowSpacing * 5 ) { /// Map touch to row and colomn and some ADC toch filtering

        tochCol = map(x, 1, widht, 0, 10);                                  /// width
        tochRow = map(y, placeMent+1, placeMent + rowSpacing * 5, 0, 5);    /// hight


        if (charCounter > 254 ) { /// buffer overrun safety, fault state reset last char

            charCounter = 254;

            }

            if (tochCol <=1 && tochRow == 3) { /// Is Shift hit ?

                // Toggle, ad a triple toggle for a more qwerty look a like keyboard ?
                if (shiftOn == 0){

                    shiftOn = 1;
                    }

                else { shiftOn = 0; }

                }

            else if (tochCol >=8 && tochRow == 3) { /// is Del hit ?
                if (charCounter > 0 ){

                    --charCounter; // remove one char from the charCounter
                    if (charCounter <= 0){charCounter = 0;}
                    keyboardArray[charCounter] = '\0';  // shorten the char array
                    /// remove the char from the terminal window
                    tft.fillRect(termMatrix[charCounter][1],termMatrix[charCounter][0],10,TEXT_HEIGHT, TFT_BLACK);
                    yDraw = termMatrix[charCounter][0] ;
                    xPos = termMatrix[charCounter][1]  ;
                    }
                }
                else { /// Put the selected char in the array
                    keyboardArray[charCounter] = RowColChar [shiftOn] [tochRow] [tochCol];

                        data = byte(RowColChar [shiftOn] [tochRow] [tochCol]);
                        // If it is a CR or we are near end of line then scroll one line
                        if (data == '\r' || xPos>231) {
                            termMatrix[charCounter][0] = yDraw;
                            termMatrix[charCounter][1] = xPos ;
                            xPos = 0;
                            tft.fillRect(termMatrix[charCounter][1],termMatrix[charCounter][0],10,TEXT_HEIGHT, TFT_BLACK);
                            yDraw = scroll_line(); // It can take 13ms to scroll and blank 16 pixel lines

                        }
                        if (data > 31 && data < 128) {
                            termMatrix[charCounter][0] = yDraw;
                            termMatrix[charCounter][1] = xPos ;
                            xPos += tft.drawChar(data,xPos,yDraw,2);
                            //tft.fillRect(xPos ,yDraw ,8,3,TFT_DARKGREY);
                            blank[(18+(yStart-TOP_FIXED_AREA)/TEXT_HEIGHT)%19]=xPos; // Keep a record of line lengths
                            //Serial.print(char(data));
                        }
                        /*
                    Serial.print (" x:");
                    Serial.print (termMatrix[charCounter][1]);
                    Serial.print (" y:");
                    Serial.print (termMatrix[charCounter][0]);
                    Serial.print (" data:");
                    Serial.print(char(data));
                    Serial.print (" char:");
                    Serial.println(keyboardArray[charCounter]);
                    */
                    delay(10); /// feed the esp soft watch dog!
                    charCounter++ ;
                    break ;

                }
            }
        }
    }
}

void IRQTest(){

Serial.print("Toch IRQ !!!!!!"); // Debugg
}

void EXTest(){

Serial.print("EX test OK !!!!!!"); // Debugg
}

int scroll_line() {
TFT_eSPI tft = TFT_eSPI();
ESP.wdtFeed();
  int yTemp = yStart; // Store the old yStart, this is where we draw the next line
  // Use the record of line lengths to optimise the rectangle size we need to erase the top line
  //tft.fillRect(0,yStart,blank[(yStart-TOP_FIXED_AREA)/TEXT_HEIGHT],TEXT_HEIGHT, TFT_BLACK);

  // Change the top of the scroll area
  yStart+=TEXT_HEIGHT;
  // The value must wrap around as the screen memory is a circular buffer
  if (yStart >= YMAX - BOT_FIXED_AREA) yStart = TOP_FIXED_AREA + (yStart - YMAX + BOT_FIXED_AREA);
  // Now we can scroll the display
  scrolling++ ;
  scrollAddress(yStart);

  return  yTemp;
}

// ##############################################################################################
// Setup a portion of the screen for vertical scrolling
// ##############################################################################################
// We are using a hardware feature of the display, so we can only scroll in portrait orientation
void setupScrollArea(uint16_t tfa, uint16_t bfa) {
TFT_eSPI tft = TFT_eSPI();
ESP.wdtFeed();
  tft.writecommand(ILI9341_VSCRDEF); // Vertical scroll definition
  tft.writedata(tfa >> 8);           // Top Fixed Area line count
  tft.writedata(tfa);
  tft.writedata((YMAX-tfa-bfa)>>8);  // Vertical Scrolling Area line count
  tft.writedata(YMAX-tfa-bfa);
  tft.writedata(bfa >> 8);           // Bottom Fixed Area line count
  tft.writedata(bfa);
}

// ##############################################################################################
// Setup the vertical scrolling start address pointer
// ##############################################################################################
void scrollAddress(uint16_t vsp) {
TFT_eSPI tft = TFT_eSPI();
ESP.wdtFeed();
  tft.writecommand(ILI9341_VSCRSADD); // Vertical scrolling pointer
  tft.writedata(vsp>>8);
  tft.writedata(vsp);
}
