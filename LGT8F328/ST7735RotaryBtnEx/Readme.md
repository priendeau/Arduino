## Application ST7735RotaryBtnEx.ino

## What is a Rotary Switch.

Rotary Switch are infnite roll action giving posibility to never block the rolling action. As example a volume roll does depend of the internal resistance and somewhat using variable resistance. A rotary does only report in a form of quadrature encoding if it clench between a position to another and throught Pin S1/S2 it held information if the rotary is moving in clock direction or counter clock direction. Difference between S1 and S2 is S2 is 90 degree shift phase and will not report an LOW statement immediately it's S1 that perform an instant switch. While S2 take a fraction of turn it inform the micro-controller is going in the clock rotation direction or counter clock direction. 

## Description of the activity.
As the schema still in drawing, the actual demonstration here imply 2 images that appear on the color LCD. An arrow apparently green show the direction of the rotary movement. The Triangle arrow point to the right when we increase the turn and show the triangle pin to the left when it's time to turn the rotary switch counter clock. The code allow to move from 0 to 99 and once at 0 you can't rewind and fall at 99. It's the same for the increasing you can't fall to 0 after reaching 99.
The second image appear when you push the rotary itself and at the left of title "ENCDR" an green circle will be drawn from small to bigger and look like to a blinking will appear once you push the rotary button. The rotary switch is also a button. Other button at the left will also blink at the screen when you push one of the 2 buttons. You can't push them all in the same it's the first that respond and at 32Mhz it's still fast enough to not report them all pushed. So by pushing the button it blink for "CONF" or "BACK".

![In the middle of the image, here the main Rotary Encoder](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/Rotary-2Btn_ST7735-final.png)
### In the middle of the image, here the main Rotary Encoder


## Bill of Material:

|        Name        |      Quantity      |                  Component                 |
|--------------------|--------------------|--------------------------------------------|
|       U1           |         1          | LGT8F328 QFN32                             |
|       U2           |         1          | ST7735 80x160 pixel 65k/282k color         |
|       C1           |         1          | 470 uF, 6.3 V Polarized Capacitor          |
|       R1-R2        |         2          | 4.7 k Ohms Resistor @3.3Volts / 10K K Ohms @5V     |
|       RS1          |         1          | Rotary Switch 3v3/5V                       |
|       PB1-PB2      |         2          | Push Button                                |
|                    |                    |                                            |

 

## Arduino Schema to follow:
In this activity we will offer a 2 kind of connection for button as it's also possible to remove an interrupt over PIN 3 which is use KEY connection from the rotary and uses attachInterrupt() from arduino core to bind a function switchInterrupt() which can barely replaced by function buttonPressed(). This will be covered later.

In this activity we will also offer an alternative using switch compilation to transfer all the button press activity into an Watchdog offer by DBUZA core involving coding insede the ISR (WDT_vect) function. We will also see if 256 millisecond is enough for triggering button.

### This is the first method as demonstration 
Here the connection for the following activity.
Note: The LGT8F328 in the middle of the image have a top pinout totally compatible with FTDI-232 and even if the CTS pin is connect to ground it does transmit well. The Application to draw does not offer an FTDI front facing the LGT8F328 chip but belong to configuration, the top FTDI chip should face  the LGT8F328 chip to flash the chip correctly.

![](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/schema_ST7735RotaryBtnEx.png)Reference: https://app.cirkitdesigner.com/project/695bf364-b612-4602-a3c6-b5d8ee982562

Don't forget to put the power ! It use the default FTDI Vcc/Ground pin to allow feeding the design but it's strongly recommended to use battery, USB connector, breadboard power supply like the breadboard image on the left show a default dual 3V3 and 5V powering from AMS1117 and connection from 12volts to 3v3/5v or using the USB and source.

