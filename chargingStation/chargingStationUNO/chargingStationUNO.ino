#include <Arduino.h>
#include <SPI.h>
#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(12, 11, 10, 9, 8);

void setup(){
    u8g.begin();
    u8g.setFont(u8g_font_unifont);
}

void skjerm(){
  u8g.firstPage();
  do{
    u8g.drawStr( 1, 22, "Viva Canarias");
    }while(u8g.nextPage());
}



void loop(){
    skjerm();
}