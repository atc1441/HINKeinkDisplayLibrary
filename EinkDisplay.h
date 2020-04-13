#ifndef _EinkDisplay_H_
#define _EinkDisplay_H_

#include <SPI.h>
#include <Adafruit_GFX.h>

#define WHITE                   0
#define BLACK                   1
#define RED                 2

class EinkDisplay : public Adafruit_GFX {
  public:
    EinkDisplay(int einkheight, int einkwidth, SPIClass& spi, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin, int8_t busy_pin);
    boolean      begin();
    void         display(void);
    void         prepare();
    void         clearDisplay(void);
    void         setWhiteBorder(void);
    void         setBlackBorder(void);
    void         setRedBorder(void);
    void         drawPixel(int16_t x, int16_t y, uint16_t color);
    virtual void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    virtual void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    int eink_height, eink_width;
  private:
    void _writeCommand(uint8_t command);
    void _writeData(uint8_t data);
    uint8_t _readPixel(int16_t x, int16_t y, uint16_t color);
    void _waitWhileBusy();
    void _beginSPI(void);
    void _endSPI(void);
    SPIClass& spi;
    int8_t       dcPin    ,  csPin,  busyPin, rstPin;
    uint8_t border = 1;
  protected:
    SPISettings  spiSettings;
};

#endif // _EinkDisplay_
