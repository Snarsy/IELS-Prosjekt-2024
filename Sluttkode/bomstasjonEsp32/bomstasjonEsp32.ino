/*#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SPITFT.h>
#include <gfxfont.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup(){
  Serial.begin(115200);
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.drawPixel(10, 10, WHITE);

  //display.setLayout21x8();
}

void loop(){
  /*display.clear();
  display.gotoXY(3,3);
  display.print("Hei");*/
  //display.drawPixel(10, 10, WHITE);

//}
//const int hexForIR_howMuchGas2 = 0x11111122;

/*const uint16_t IRPin = 32;  // ESP32 pin GPIO 32

IRsend irsend(IRPin);

void setup() {
  
  Serial.begin(115200);

  //Start ir sending
  irsend.begin();
  Serial.println("IR LED enabled");
}

void loop() {
  //uint16_t address = 0x45678912;
  //uint16_t command = 32;
  //irsend.sendNEC(address, command); //Send til addresse og command. 
  irsend.sendNEC(0x22222222, 32);

  //Serial.println("Sent NEC IR signal");

  
  delay(100);
}*/



#include <Arduino.h>
#include <SPI.h>
#include "U8glib.h" // include the universal graphcs library

//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI display(U8G2_R0, /* clock=/ 13, / data=/ 11, / cs=/ 10, / dc=/ 9, / reset=*/ 8);

 
U8GLIB_SSD1306_128X64 u8g(12, 11, 10, 9, 8);    // initialise a u8g display object
//clk = 12, mosi = 11, cs = 10, dc = 9, res = 8.  på arduino

//U8GLIB_SSD1306_128X64 u8g(19, 23, 18, 17, 27);    // initialise a u8g display object
//clk = 19, mosi = 23, cs = 18, dc = 17, res = 27. på esp32

void setup() {
  u8g.begin();
  u8g.setFont(u8g_font_6x10);  //unifont
}

void skjerm(){
  u8g.firstPage();
  do{
    u8g.drawStr( 35, 10, "Ladeprosent");
    u8g.drawStr( 60, 30, "20%");
    u8g.drawStr(7, 60, "Avbryt");
    u8g.drawStr(100, 60, "Kjop");

    }while(u8g.nextPage());
  
}

void loop() {
  skjerm();
}
//Fjern neopixel