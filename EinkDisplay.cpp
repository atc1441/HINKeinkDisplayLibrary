#include <Adafruit_GFX.h>
#include "EinkDisplay.h"

#define xy_swap(a, b) \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b))) ///< No-temp-var swap operation

EinkDisplay::EinkDisplay(int einkheight, int einkwidth, SPIClass& spi, int8_t dc_pin, int8_t rst_pin, int8_t cs_pin, int8_t busy_pin) :
  Adafruit_GFX(einkheight, einkwidth), spi(spi), dcPin(dc_pin), csPin(cs_pin), busyPin(busy_pin), rstPin(rst_pin),
  eink_height(einkheight), eink_width(einkwidth) {
  spiSettings = SPISettings(4000000, MSBFIRST, SPI_MODE0);
}

boolean EinkDisplay::begin() {
  clearDisplay();
  pinMode(busyPin, INPUT);
  digitalWrite(csPin, HIGH);
  pinMode(csPin, OUTPUT);
  digitalWrite(dcPin, HIGH);
  pinMode(dcPin, OUTPUT);
  digitalWrite(rstPin, HIGH);
  pinMode(rstPin, OUTPUT);
  spi.begin();
  return true;
}

void EinkDisplay::prepare(void) {
  uint16_t eink_x = ((eink_width - 1) / 8);
  uint16_t eink_y = (eink_height - 1);

  delay(20);
  digitalWrite(rstPin, LOW);
  delay(20);
  digitalWrite(rstPin, HIGH);
  delay(30);

  //Registers Like https://download.mikroe.com/documents/datasheets/e-paper-display-2%2C9-296x128-n.pdf
  _beginSPI();

  _writeCommand(0x12);//Software reset
  _waitWhileBusy();
  delay(5);

  _writeCommand(0x74);//Set Analog Block Control
  _writeData(0x54);

  _writeCommand(0x7E);//Set Digital Block Control
  _writeData(0x3B);

  _writeCommand(0x2B);//ACVCOM setting
  _writeData(0x04);
  _writeData(0x63);

  _writeCommand(0x0C); // Softstart Control
  _writeData(0x8F);
  _writeData(0x8F);
  _writeData(0x8F);
  _writeData(0x3F);

  _writeCommand(0x01);//Driver Output control
  _writeData(eink_y);
  _writeData((eink_y >> 8));
  _writeData(0x00);

  _writeCommand(0x11);//Data Entry mode setting
  _writeData(0x01);

  _writeCommand(0x44);//Set RAM X - address Start/End position
  _writeData(0x00);//RAM X Start
  _writeData(eink_x);//RAM X End

  _writeCommand(0x45);//Set RAM Y - address Start/End position
  _writeData(eink_y);//RAM Y Start
  _writeData((eink_y >> 8));//RAM Y Start
  _writeData(0x00);//RAM Y End
  _writeData(0x00);//RAM Y End

  _writeCommand(0x3C);//Border Waveform Control
  _writeData(border);//0 = black,1 = white,2 = Red

  _writeCommand(0x18);// Temperature sensor control
  _writeData(0x80);// 0x48 = External,0x80 = Internal

  _writeCommand(0x21);//Display Update Control 1
  _writeData(B00001000);//inverse or ignore ram content

  _writeCommand(0x22);//Display Update Control 2
  _writeData(0xB1);

  _writeCommand(0x20);//Master Activation
  _waitWhileBusy();
  delay(5);
  _endSPI();
}


void EinkDisplay::display(void) {
  _beginSPI();
  _writeCommand(0x22);//Display Update Control 2
  _writeData(0xC7);

  _writeCommand(0x20);//Master Activation
  _waitWhileBusy();

  _writeCommand (0x10);//Deep Sleep mode
  _writeData (0x01);
  _endSPI();
}

void EinkDisplay::_beginSPI(void)
{
  spi.beginTransaction(spiSettings);
}

void EinkDisplay::_endSPI(void)
{
  spi.endTransaction();
}

void EinkDisplay::_writeCommand(uint8_t command)
{
  digitalWrite(dcPin, LOW);
  digitalWrite(csPin, LOW);
  spi.transfer(command);
  digitalWrite(csPin, HIGH);
}

void EinkDisplay::_writeData(uint8_t data)
{
  digitalWrite(dcPin, HIGH);
  digitalWrite(csPin, LOW);
  spi.transfer(data);
  digitalWrite(csPin, HIGH);
}

uint8_t EinkDisplay::_readPixel(int16_t x, int16_t y, uint16_t color)
{
  _writeCommand (0x41);//Read RAM option
  _writeData(color);//0 = BW, 1 = RED RAM

  _writeCommand(0x4E);//Set RAM X address counter
  _writeData((x / 8));
  _writeCommand(0x4F);//Set RAM Y address counter
  _writeData(y);
  _writeData((y >> 8));

  digitalWrite(csPin, LOW);
  digitalWrite(dcPin, LOW);
  spi.transfer(0x27);
  digitalWrite(dcPin, HIGH);
  uint8_t result = spi.transfer(0x27);
  result = spi.transfer(0x27);
  digitalWrite(csPin, HIGH);
  return result;
}

void EinkDisplay::_waitWhileBusy()
{
  while (digitalRead(busyPin)) {}
}

void EinkDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
    switch (getRotation()) {
      case 1:
        xy_swap(x, y);
        x = WIDTH - x - 1;
        break;
      case 2:
        x = WIDTH  - x - 1;
        y = HEIGHT - y - 1;
        break;
      case 3:
        xy_swap(x, y);
        y = HEIGHT - y - 1;
        break;
    }

    uint8_t eink_x = (x / 8);
    uint16_t eink_y = y;
    uint8_t curr_black;
    uint8_t curr_red;

    _beginSPI();
    curr_black = _readPixel(x, y, 0); //read black pixel
    curr_red = _readPixel(x, y, 1); //read red pixel

    curr_black = (curr_black & (0xFF ^ (1 << (7 - x % 8))));
    curr_red = (curr_red & (0xFF ^ (1 << (7 - x % 8))));

    if (color == BLACK) curr_black = (curr_black | (1 << (7 - x % 8)));
    if (color == RED) curr_red = (curr_red | (1 << (7 - x % 8)));

    //Writing Black/Wide pixel
    _writeCommand(0x4E);//Set RAM X address counter
    _writeData(eink_x);
    _writeCommand(0x4F);//Set RAM Y address counter
    _writeData(eink_y);
    _writeData((eink_y >> 8));

    _writeCommand(0x24);//WRITE RAM BW
    _writeData(curr_black);//write new BW pixel

    //Writing RED Pixel
    _writeCommand(0x4E);//Set RAM X address counter
    _writeData(eink_x);
    _writeCommand(0x4F);//Set RAM Y address counter
    _writeData(eink_y);
    _writeData((eink_y >> 8));

    _writeCommand(0x26);//WRITE RAM RED
    _writeData(curr_red);//write new RED pixel

    _endSPI();
  }
}

void EinkDisplay::clearDisplay(void) {
  uint16_t eink_x = ((eink_width - 1) / 8);
  uint16_t eink_y = (eink_height - 1);

  _beginSPI();
  _writeCommand(0x4E);//Set RAM X address counter
  _writeData(0x00);
  _writeCommand(0x4F);//Set RAM Y address counter
  _writeData(eink_y);
  _writeData((eink_y >> 8));

  _writeCommand(0x24);//WRITE RAM BW

  for (uint32_t i = 0; i < (eink_width * eink_height / 8); i++)
  {
    _writeData(0x00);
  }
  _writeCommand(0x4E);//Set RAM X address counter
  _writeData(0x00);
  _writeCommand(0x4F);//Set RAM Y address counter
  _writeData(eink_y);
  _writeData((eink_y >> 8));

  _writeCommand(0x26);//Write RAM RED

  for (uint32_t i = 0; i < (eink_width * eink_height / 8); i++)
  {
    _writeData(0x00);
  }
  _endSPI();
}

void EinkDisplay::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
  for (uint16_t i = 0; i < w; i++)
  {
    drawPixel(x + i, y, color);
  }
}

void EinkDisplay::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
  for (uint16_t i = 0; i < h; i++)
  {
    drawPixel(x, y + i, color);
  }
}

void EinkDisplay::setWhiteBorder(void) {
  border = 1;
}
void EinkDisplay::setBlackBorder(void) {
  border = 0;
}
void EinkDisplay::setRedBorder(void) {
  border = 2;
}
