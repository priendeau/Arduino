## application DrnBtShiftRegPISO.ino

## What is a Shift-Register.

This application allow you to test Shift Register based on chip 74HC165 as 8-bit Shift Register in PISO mode. The PISO mode or Parallel-In Serial-Out is a chip device that accept to read from 8 individuals port an HIGH state or LOW state able to be coded into signal where it's throw back into lined-up serial data call a binary information where the serial line always repeat and inform the micro-controller if the state had changed. 

Here it's the final montage, and share the common breadboard, we can found the Arduino equivalent call Wemos TTGO-XI no longer available, instead Logic Green issues itself through the marketplace Temu, aliexpress and even ebay a equivalent near similar micro-controller found in green or purple board. They are exactly the same. As Arduino work at 16 Mhz, this one rule at 32Mhz and does have few more function not discussed here and not used. So similar code will also work for Arduino Uno R3, Arduino Nano too and even generic one available on aliexpres. 

![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/DrnBtShiftRegPISO/LGT8F328_SR-74HC165-8state-switch.JPG)

## Bill of Material:

|        Name        |      Quantity      |                  Component                 |
|--------------------|--------------------|--------------------------------------------|
|       U1           |         1          | 74HC165 8-Bit Shift Register TSSOP or DIP  |
|       U2           |         1          | Arduino Uno R3 or Wemos TTGO-XI (LGT8F328) |
|       D1           |         2          | Diode 1n4001                               |
|       C1           |         1          | 220 uF, 6.3 V Polarized Capacitor          |
|       C2           |         1          | 1 uF, 16 V Polarized Capacitor             |
|       R1           |         11         | 10 k\u2126 Resistor @5Volts / 4.7K K\u2126 @3.3V     |
|       S1-S8        |         8          | Slideswitch 3 pin                          |
|                    |                    |                                            |

 

## Arduino Schema to follow:![enter image description here](https://github.com/priendeau/Arduino/blob/main/LGT8F328/DrnBtShiftRegPISO/Arduino-schematic_shift_registerPISO_74hc165.png?raw=true)

The code is inside repository.

While the code and Hexadecimal file in intel format is available on this github repository I include some shoot of the step-by-step action of the micro-controller and up to was is possible to do is.

 1. Pressing the reset if you want to start it again. There is a wait-time not because the micro-controller is slow but let you flash the chip and have the time to open a terminal an connect a terminal application to acces to micro-contrroller communication. It use USB device from the flashing USB port. Using Linux it's usually the /dev/ttyACM0, everywhere else it's a COMX on windows. Mac user have something similar to /dev/cua... 
 2. You can look to the light. It turning blue during the operation and turning off during the reboot. It's simplist action but the micro-controller is coded to loop 100 times give you a topo of what you done and start-again.
 3. Switching one of the 8 switch installed to correspond to a state of the register will record. Upon each trial there is a line-up grid that tell the number of the trial and what it see.
 
 

## Boot Process
It should look like this:
![enter image description here](https://github.com/priendeau/Arduino/blob/main/LGT8F328/DrnBtShiftRegPISO/DrnBtShiftRegPISO-1.png?raw=true)
 
While you do trial over the switch to discover the 0 and 1 under the trial number you have few time and sometime if you haven't touch the switch it stay like. If you do  see the last line of each trial it's the decimal value corresponding to the value. Here the program is complete and does pading 0 after tthe number to complete a 8 digit sequence where switch set to off mean 0.

## Plenty trial during few moment.
![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/DrnBtShiftRegPISO/DrnBtShiftRegPISO-2.png)

## Final passes the result you did.
It won't tell you every trial you made from the first one, but unfortunately Sam Simon the Answer is coded alike this widget and a different program made from this original essay you can use another Shift-Register the 74HC595 which this one is getting the binary or serial answer to 8 port like  light where switches once paired will reflect the one you made. In the case of Sam Simon the Answer your going to look at light and press the corresponding switch to inform the program your about to set all the switch to light it display. 
![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/DrnBtShiftRegPISO/DrnBtShiftRegPISO-3.png)  
Here the 49 trials recorded from my personal try. If you are not touching it it stay at the unique switch it was if all was set to OFF your going to get 1 value or 00000000. 

As this breadboard was easy to craft, it get an original law to make it work. Shift Register are precise piece of hardware considered like TTL level all TTL level even if they say it's working between 2 and 6 Volts, if you are stating at 5 volts your micro-controller, this piece was also having multiple pass of voltage analyzes and does conclude it should remain at 4.5 volts like original TTL chip. Working at different voltage it will respond over a 3.3 Volts without diode, but unfortunately all Arduino UNO are 5 volts and are tolerant only if there is no voltage higer than 3.3 Volts, or become non-sensitive to variation and usually 5 volts is too much for a TTL. 

## A little trick that made it work

 
A way to skip the diode where you do see the board, one over the 5 volts from the micro-controller and the rail, and one from the rail to the Vcc of the Shift Register chip, this lower the voltage to 4.4Volts or (2 * 0.3Volts ) of voltage forwarding reduce well the requirement. Another way is to use 3 resistors, one 470 \u03a9 and 2 X 100 \u03a9 where you connect the 470\u03a9 and the 100\u03a9 in the following way. The 100\u03a9 will be in parallel with the ground and 2 ends of the 100\u03a9 will be plugged with the 470\u03a9 connected to the 5 Volts and you start your wire to feed the Vcc pin of a TTL chip from junction of 470\u03a9 and ( 2x 100 \u03a9), belong to voltage divider law it fit in 4.5 volts.
