#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
//13, 12, 3, 4, 11
U8X8_SSD1306_128X64_NONAME_4W_SW_SPI u8x8(23, 25, 31, 29, 27 );
//U8X8_SSD1306_128X32_UNIVISION_HW_I2C u8x8(U8X8_PIN_NONE);
void initOLED()
{
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}


void displayOLED(String str)
{
  str = str + "       ";
  u8x8.draw1x2String(0,0,str.c_str());
}
