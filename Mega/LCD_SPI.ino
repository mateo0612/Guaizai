#include <Arduino_GFX_Library.h>
#include "image1.h"
/* More dev device declaration: https://github.com/moononournation/Arduino_GFX/wiki/Dev-Device-Declaration */

/* More data bus class: https://github.com/moononournation/Arduino_GFX/wiki/Data-Bus-Class */
Arduino_DataBus *bus = new Arduino_HWSPI(26 /* DC */, 28 /* CS */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 24 /* RST */, 3 /* rotation */);

uint16_t backColor = WHITE;
void initLCD()
{
    gfx->begin();
    gfx->fillScreen(BLACK);

    gfx->draw16bitRGBBitmap(35, 30, gImage, 128, 87, 2);
    //gfx->setCursor(40, 70);
    //gfx->setTextColor(BLACK);
    //gfx->setTextSize(3 /* x scale */, 3 /* y scale */, 0 /* pixel_margin */);
    //gfx->println("Hello GuaiZai!");

}

void displayLCD(int cursorW, int cursorH, int textColor, int backColor, int fontSizex, int fontSizey, int fontSizem, String str)
{
    gfx->setCursor(cursorW, cursorH);
    gfx->setTextColor(textColor, backColor);
    gfx->setTextSize(fontSizex /* x scale */, fontSizey /* y scale */, fontSizem /* pixel_margin */);
    gfx->println(str);
}

void displayLCD(int cursorW, int cursorH, int textColor, int fontSize, String str)
{
    displayLCD(cursorW, cursorH, textColor, backColor, fontSize, fontSize, 1, str);
}
