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


//#include <MUIU8g2.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <U8x8lib.h>
#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     16
#define OLED_CS     5
#define OLED_RESET  17

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);

void setup() {
  u8g2.begin();
  Serial.begin(115200);
}

void first() {
  /*u8g2.clearBuffer();           // Fjern innholdet i bufferet
  u8g2.setFont(u8g2_font_ncenB08_tr);  // Sett font
  u8g2.drawStr(0,10,"Hello, world!");  // Tegn tekst
  u8g2.sendBuffer();            // Send bufferet til skjermen
  delay(1000);*/
  display.drawPixel(10, 10, WHITE);

}


/*void second(){
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH1106_WHITE);
  display.setCursor(5,28);
  display.println("Electronics Hub");
  display.display();
  delay(3000);
}*/


void loop(){
  first();
}

