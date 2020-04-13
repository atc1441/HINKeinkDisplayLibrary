# HINKeinkDisplayLibrary
Driver for HINK E-Ink E-Paper Displays Arduino


This library dont uses any Buffer, it will directly write to the E-Paper Display and dont uses much RAM, it is not as fast as with buffer.

One 10k Ohm Resistore is needed to use the library.
you also need the Adafruit GFX Library installed.

Tested with:

1.54" HINKE0154A35

2.9" HINKE029A14

4.2" HINKE042A11


### Some interesting feature is the Border:
use it via:

display.setWhiteBorder();

display.setBlackBorder();

display.setRedBorder();

![Image of E-Paper Display](https://raw.githubusercontent.com/atc1441/HINKeinkDisplayLibrary/master/20200413_135931.jpg)

### Here are more infos on the Displays compatible with it:
Youtube Video(click on the picture)
[![YoutubeVideo](https://img.youtube.com/vi/p28IE1oAKGQ/0.jpg)](https://www.youtube.com/watch?v=p28IE1oAKGQ)
