
#define SSD1305_128_64
/*=========================================================================*/

#define SSD1305_LCDWIDTH                  128
#define SSD1305_LCDHEIGHT                 64

#define SSD1305_RST_GPIO                  21

#define SSD1305_I2C_ADDRESS   0x3C	// 011110+SA0+RW - 0x3C or 0x3D

#define SSD1305_SETLOWCOLUMN 0x00
#define SSD1305_SETHIGHCOLUMN 0x10
#define SSD1305_MEMORYMODE 0x20
#define SSD1305_SETCOLADDR 0x21
#define SSD1305_SETPAGEADDR 0x22
#define SSD1305_SETSTARTLINE 0x40

#define SSD1305_SETCONTRAST 0x81
#define SSD1305_SETBRIGHTNESS 0x82

#define SSD1305_SETLUT 0x91

#define SSD1305_SEGREMAP 0xA0
#define SSD1305_DISPLAYALLON_RESUME 0xA4
#define SSD1305_DISPLAYALLON 0xA5
#define SSD1305_NORMALDISPLAY 0xA6
#define SSD1305_INVERTDISPLAY 0xA7
#define SSD1305_SETMULTIPLEX 0xA8
#define SSD1305_DISPLAYDIM 0xAC
#define SSD1305_MASTERCONFIG 0xAD
#define SSD1305_DISPLAYOFF 0xAE
#define SSD1305_DISPLAYON 0xAF

#define SSD1305_SETPAGESTART 0xB0

#define SSD1305_COMSCANINC 0xC0
#define SSD1305_COMSCANDEC 0xC8
#define SSD1305_SETDISPLAYOFFSET 0xD3
#define SSD1305_SETDISPLAYCLOCKDIV 0xD5
#define SSD1305_SETAREACOLOR 0xD8
#define SSD1305_SETPRECHARGE 0xD9
#define SSD1305_SETCOMPINS 0xDA
#define SSD1305_SETVCOMLEVEL 0xDB

/*

class Adafruit_SSD1305 : public Adafruit_GFX {
 public:

 Adafruit_SSD1305(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS) :sid(SID), sclk(SCLK), dc(DC), rst(RST), cs(CS), Adafruit_GFX(SSD1305_LCDWIDTH, SSD1305_LCDHEIGHT) {}

 Adafruit_SSD1305(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST) :sid(SID), sclk(SCLK), dc(DC), rst(RST), cs(-1), Adafruit_GFX(SSD1305_LCDWIDTH, SSD1305_LCDHEIGHT) {}

 Adafruit_SSD1305(int8_t DC, int8_t RST, int8_t CS) :sid(-1), sclk(-1), dc(DC), rst(RST), cs(CS), Adafruit_GFX(SSD1305_LCDWIDTH, SSD1305_LCDHEIGHT) {}
  
 Adafruit_SSD1305(int8_t RST) :sid(-1), sclk(-1), dc(-1), rst(RST), cs(-1), Adafruit_GFX(SSD1305_LCDWIDTH, SSD1305_LCDHEIGHT) {}
  
  
  void begin(uint8_t i2caddr = SSD1305_I2C_ADDRESS);
  void command(uint8_t c);
  void data(uint8_t c);

  void clearDisplay(void);
  void invertDisplay(uint8_t i);
  void setBrightness(uint8_t i);
  void display();

  void drawPixel(int16_t x, int16_t y, uint16_t color);

private:
  uint8_t _i2caddr;
  int8_t sid, sclk, dc, rst, cs;
  void spixfer(uint8_t x);

};
*/