#include <SPI.h>
#include <Adafruit_GFX.h>
#include <EinkDisplay.h>

/*1,54inch WIDTH 152 HEIGHT 152*/
/*2,9inch WIDTH 128 HEIGHT 296*/
/*4.2inch WIDTH 400 HEIGHT 300*/


/*Arduino UNO SPI: MOSI = 11 MISO = 12 CLK = 13*/
/*To enable reading from the display you need a 10k Resistor between MISO and MOSI, connect the Data pin from the display to MISO*/
EinkDisplay display(152/*Height*/, 152/*Width*/, SPI, 9/*DC*/, 8/*Reset*/, 10/*CS*/, 7/*Busy*/);

void setup() {
  Serial.begin(115200);
  Serial.println("Eink Display Demo");
  display.begin();
  display.setRotation(0);
  display.setTextSize(2);
  display.setTextColor(BLACK);
  
  display.setWhiteBorder();
  //display.setBlackBorder();
  //display.setRedBorder();
}

void loop() {
  display.prepare();// always call this to wake the display
  display.clearDisplay();
  display.fillCircle(display.eink_width / 2, display.eink_height / 2, 14, RED);
  display.setCursor(0, 0);
  display.println("Hello World");
  display.println(millis());
  display.display();

  delay(10000);
}

void print2digits(int number) {
  if (number < 10)display.print("0");
  display.print(number);
}