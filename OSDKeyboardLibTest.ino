#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREY  0x5AEB


#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library
XPT2046 touch(/*cs=*/ D2, /*irq=*/ D1);


int16_t h = 320;
int16_t w = 240;

int nisse=3 ;
const int buflen = 63; 
 char keyboardArray[255]; // create for virtual keyboard  
 char newSSID[buflen];
 char newPassword[buflen];

#include <OSDKeyBoard.h>



void setup()
{



  tft.init();
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  //tft.setTextColor(WHITE, BLACK);
  //tft.setTextColor(WHITE,GREY);
  //tft.drawString("AGGE", w/2, h/2, 4);
  delay(1000);
  Serial.begin(9600);
}

void loop()
{
  KeyBoard(240, 320, /*(t,T=top b,B=bottom */ 'T');


  delay(300);
}
