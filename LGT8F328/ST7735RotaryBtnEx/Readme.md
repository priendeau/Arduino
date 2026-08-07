## Application ST7735RotaryBtnEx.ino

This application is a top template for the micro-controller LGT8F328. Offering an LCD SPI interface to show information where USB Serial with Serial.print(...) lack or make the micro-controller spontaneous rebooting of stalling, it offer a small footprint and an easy way to drive a rotary switch position with a button on top an control two other button. The choice of using an LCD SPI interface come from a bug with I2C using the communication algorithm offer with dbuezas core and Arduino core to know to be onReceive and onRequest function seems to not working using I2C interface for LCD and the communication channel too. So I design an SPI version that might be well explained and working here before generating the I2C communication function.

Table of content:

 1. What is a Rotary Switch.
 2. Description of the activity.
 3. Bill of Material.
 4. Arduino Schema to follow.
 5. Tool required.
 6. Arduino Client installation.
 7. Avrdude software installation.
 8. Compilation with arduino-compile & arrayBuildProperty.
 9. Compilation Switches example.
 10.Alternative. 

## 1. What is a Rotary Switch.

![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/rotary_encoder_ec11_pinout-small.png)

Rotary Switch are infinite roll action giving possibility to never block the rolling action. As example a volume roll does depend of the internal resistance and somewhat using variable resistance. A rotary does only report in a form of quadrature encoding if it clench between a position to another and through Pin S1/S2 it held information if the rotary is moving in clock direction or counter clock direction. Difference between S1 and S2 is S2 is 90 degree shift phase and will not report an LOW statement immediately it's S1 that perform an instant switch. While S2 take a fraction of turn it inform the micro-controller is going in the clock rotation direction or counter clock direction. 

## 2. Description of the activity.
As the schema still in drawing, the actual demonstration here imply 2 images that appear on the color LCD. An arrow apparently green show the direction of the rotary movement. The Triangle arrow point to the right when we increase the turn and show the triangle pin to the left when it's time to turn the rotary switch counter clock. The code allow to move from 0 to 99 and once at 0 you can't rewind and fall at 99. It's the same for the increasing you can't fall to 0 after reaching 99.
The second image appear when you push the rotary itself and at the left of title "ENCDR" an green circle will be drawn from small to bigger and look like to a blinking will appear once you push the rotary button. The rotary switch is also a button. Other button at the left will also blink at the screen when you push one of the 2 buttons. You can't push them all in the same it's the first that respond and at 32Mhz it's still fast enough to not report them all pushed. So by pushing the button it blink for "CONF" or "BACK".

![In the middle of the image, here the main Rotary Encoder](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/Rotary-2Btn_ST7735-final.png)
### In the middle of the image, here the main Rotary Encoder


## 3. Bill of Material.

|     Name      | Quantity |                  Component                      |
|---------------|----------|-------------------------------------------------|
|    U1         |    1     | LGT8F328 QFN32                                  |
|    U2         |    1     | ST7735 80x160 pixel 65k/282k color              |
|    U3         |    1     | 4 bridged Level voltage translator schema/chip  |
|    C1         |    1     | 470 uF, 6.3 V Polarized Capacitor               |
|    R1-R2      |    2     | 4.7 k\u2126 Resistor @3.3Volts / 10K K\u2126 @5V          |
|    RS1        |    1     | Rotary Switch 3v3/5V                            |
|    PB1-PB2    |    2     | Push Button                                     |
|               |          |                                                 |

 

## 4. Arduino Schema to follow.
In this activity we will offer a 2 kind of connection for button as it's also possible to remove an interrupt over PIN 3 which is use KEY connection from the rotary and uses attachInterrupt() from Arduino core to bind a function switchInterrupt() which can barely replaced by function buttonPressed(). This will be covered later.

In this activity we will also offer an alternative using switch compilation to transfer all the button press activity into an Watchdog offer by dbuezas core involving coding inside the ISR (WDT_vect) function. We will also see if 256 millisecond is enough for triggering button.

### This is the first method as demonstration 
Here the connection for the following activity.
Note: The LGT8F328 in the middle of the image have a top pinout totally compatible with FTDI-232 and even if the CTS pin is connect to ground it does transmit well. In general the pin of the FTDI 232 (if they are soldered ) do have to point in direction of the LGT8F328 chip not the reverse or your not going to get power for your application.

![](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/schema_ST7735RotaryBtnEx.png)Reference: https://app.cirkitdesigner.com/project/695bf364-b612-4602-a3c6-b5d8ee982562

Don't forget to put the power ! It use the default FTDI Vcc/Ground pin to allow feeding the design but it's strongly recommended to use battery, USB connector, breadboard power supply like the breadboard image on the left show a default dual 3V3 and 5V powering from AMS1117 and connection from 12volts to 3v3/5v or using the USB and source.

## 5. Tool required.
In this activity we will entirely depend of the arduino-cli and bash command from Linux. I will give the required step to install the arduino-cli. Inside this repository you do have some file used to enhance the use of arduino-cli and I invite to use them by giving all the help it's required. Flashing the hexadecimal build binary over a micro-controller do require a physical USB to TTL interface such as CH340 or FTDI-232. In this activity we will use the FTDI-232 and a software called avrdude. Avrdude software is a free tool and work in several Linux distribution as well Windows, and OSX. The website or repository offer an already build application ready to use for several platform as mentioned. Therefore if you have already install Arduino IDE you'll find an already working version of avrdude if you have installed other core.

While arduino-cli is officially used by Arduino IDE it tend to leave the IDE consuming time and beautiful environment where few widget still here inside the IDE to make it appealing. One of them is accessing to function name and declaration inside a library. Yes it offer to wound around your code and especially good to see all element declared and help your getting them. As impressive the u8g2 library offer alternative to many LCD to get connect and work with Arduino to get the declaration example is often through example but clicking to a good declaration in your code from Arduino IDE will push opening the file holding your declaration and can see different alternative. We will not give example in this way because even example can be extracted from arduino-cli. 

## 6. Arduino Client installation.

Here a famous line for the installation of Arduino client and it's accessible by opening a shell prompt from your Gnome/Cinnamon/Mate/KDE/Xwindows desktop.

**bash**

    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/usr/local/bin sh

Some of the user don't own their /usr/local/bin path and might belong to root. in this context be sure you are in your HOME installation path to call both curl and sh and it's sh that require a sudo not the curl command and will look like : 

**bash**

    curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | BINDIR=/usr/local/bin sudo sh

As it's possible your distribution is Ubuntu or Ubuntu based, they are offering "homebrew" installation (OSX friendly from shell) and snap (FreeBSD/Unix friendly known) to also install the Arduino Client application. 

from snap:

**bash**

    sudo snap install arduino-cli

from homebrew:

**bash/OSX**

    brew update && brew install arduino-cli

 And once installed, verify you can call arduino-cli:

 **bash**

    arduino-cli version

A default command is also good to perform and this one is also store inside aliasrc file left in this repository to allow you store one active configuration within all you can create. 

**bash**

    arduino-cli --config-file /home/${USER}/.arduinoIDE/arduino-cli.yaml config init

Note: Once arduino-cli is installed you can output all information in text which is by default. Outputting them into JSON and recover the content from python interpreter is a good thing to do if you want to keep the information and still be visual to seek for information. Few space in this repository does offer alternative to manage arduino-cli as visual interface using tkinter which is not demonstrated here but possible to use to enjoy and let your system suffer less from memory exhaustion because Arduino IDE is relatively huge and consume a lot of memory to perform an imitation on Intelli-Sense(1) accessing to your code with bubble and windows.

### What is holding your configuration file for arduino-cli ?
Everything inside the configuration file is written in a YAML format. It does not hold symbols in form of <element>value</element> like XML but own a specific text validation when it's time to add several's line like the core element require a '-' dash at every line. As example here the installation of dbuezas core should require your arduino-cli to work with a switch parameter if you haven't add yourself the dbuezas core with Arduino IDE you can point your Arduino IDE configuration by arduino-cli as long you know where is the path. Usually it stand around "/${HOME}/.arduinoIDE/arduino-cli.yaml" Where ${HOME} is your default Linux user home. 

### An exception
Bad news, installing only the dbuezas core does not include installing the flash tools to flash. It uses ftdi-232 and avrdude as software to enable Arduino communicating. Installing Arduino IDE can solve yourself to find a compatible avrdude configuration or start looking to installing independently avrdude version 8.0 at least. Or even installing a core that do install the avrdude. Arduino, MegaCoreX, MiniCore  fit the most in the well installed and optimized avrdude version. Installing Arduino core also offer a avrdude version 6.3 tuned for CH340 serial communication gateway, as ftdi-232 is an all compatible too. In this case there is a demonstration of avrdude by the shell because this activity export the binary and let make it available for flashing.

### Long-long way to use arduino-cli 
This long-long and heavy command line stand in using the switch from arduio-cli and everytime you want to install or update the dbuezas core will require this line to exist either inside the aliasrc or from the history (discouraged). 

**bash**

    arduino-cli --additional-urls "https://raw.githubusercontent.com/dbuezas/lgt8fx/master/package_lgt8fx_index.json" ...

### Short way to use arduino-cli
You made the config init and using the aliasrc left in this repository. User with Arduino-IDE shouldn't do that or will loose all the past configuration from Arduino IDE installation and its use. Hence having not made "config init" command open your "arduino-cli.yaml" and add under :

**editor with arduino-cli.yaml**

    board_manager:
	    additional_urls:

under "additional_urls:" you add :

    - https://raw.githubusercontent.com/dbuezas/lgt8fx/master/package_lgt8fx_index.json

Be sure the dash is align with "additional_urls" its roughly 2 space characters, and this will do the same as opening the preference and adding any other core library.

Installing dbuezas core:

With arduino-cli do this following command : 

**bash**

    arduino-cli core install "lgt8fx:avr@2.0.7"

And should install the latest dbuezas core for LGT8F328 for Wemos TTGO-IX, LGT8f328-QFN32, LGT8f328-QFN48 and LGT8f328-SSOP20 where the SSOP20 is equivalent to Arduino pro-micro and having 12 less pin than Wemos TTGO-IX, LGT8f328-QFN32, but still operate at 32Mhz. 

Once the arduino-cli finish its installation you can verify what you have installed with this command:

**bash**

    arduino-cli core list

And fall with a text list that may look like this :

|        ID          | Installed |  Latest |  Name
|--------------------|-----------|---------|--------------------------------------------------|
|arduino:avr         |  1.8.6    |  1.8.8  |   Arduino AVR Boards                             |
|arduino:mbed_nano   |  4.4.1    |  4.6.0  |   Arduino Mbed OS Nano Boards                    |
|arduino:samd        |  1.8.14   |  1.8.14 |   Arduino SAMD Boards (32-bits ARM Cortex-M0+)   |
|attiny:avr          |  1.0.2    |  1.0.2  |   attiny                                         |
|digistump:avr       |  1.7.5    |  1.7.5  |   Digistump AVR Boards                           |
|esp32:esp32         |  3.0.5    |  3.3.10 |   esp32                                          |
|esp8266:esp8266     |  3.1.2    |  3.1.2  |   esp8266                                        |
|lgt8fx:avr          |  2.0.7    |  2.0.7  |   LGT8fx Boards                                  |

And you can see at the end "**lgt8fx:avr**" is installed. 

## 7. Avrdude software installation.

Visiting Avrdude repository of the version 8.0 you won't have any problem using it for several Arduino core. This application is a top software solution to alternatively send any type of binary and fuse configuration to a micro-controller. This activity is not covering fuse configuration and chip like LGT8F328 are already "*fused*"  to work and it stand by sending the arduino-core by flashing at it if it come without. It's not covered by this activity. You can visit wolles-elektronikkiste.de at this address https://wolles-elektronikkiste.de/en/lgt8f328p-lqfp32-boards, to get any specificity including flashing an arduino-core if you wipe-it with a general overwrite command. It's a German website available in English.   

Let's visit the Avrdude website at : https://github.com/avrdudes/avrdude/releases#release-v8.0 

![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/avrdude_github_assets.png)

Go at the end of the 8.0 release list and select the arrow at the left of the "**Assets**". For all the release of avrdude you found inside the name, the version, and the platform required for you. As example, I do compile everything on an Intel Xeon-E5-2650v4 and this under linux. But I am working remotely through my local area network and access remotely and do ssh command to call arduino-cli or use it via VNC. But every build is exported with NFS onto the Raspberry Pi 4 I use. So In fact I do not need to install avrdude for Xeon which suppose to be a Linux_64bit. I do use Linux_ARM64 version because my desktop and my table for crafting is far from the Xeon and make the USB wire an 80 cm USB with mini-connector. As user of CH340 will require shorter USB cable to communicate. Unless you are not using the FTD-232 chip to send your binary to the micro-controller it's strongly recommended to use a shorter cable it's a 15 cm suggested long cable. So take your time to shop an FTDI-232 chip and 30 cm of cable will be good. But in doubt use an shorter one.  

With the downloaded version you extract avrdude and avrdude.conf they are both important. Usually you will also have to install them into accessible Linux location like /usr/local/bin. You can copy both into /usr/local/bin or keeping the avrdude.conf either in the /etc in your /home, but it's more important to know where the configuration is to call avrdude it always require the configuration file and without checking if you already have an old version of avrdude It will refer to the already installed configuration. So here the installation for Linux Arm64 as example:

**bash**

    tar xvzCf avrdude_v8.0_Linux_ARM64.tar.gz ./
    cp avrdude avrdude.conf /usr/local/bin 

If your /usr/local/bin path is in the general ${PATH} variable it's seemless you can call your avrdude always starting it like:

**bash**

    avrdude -C /usr/local/bin/avrdude.conf ...

### General form calling avrdude.

Here a general form where if you answer to all the question the avrdude command will work well.

**bash**

    __APP__ -C __APP_CONF__ -v -V -patmega328p -carduino -P __USB_DEV__ -b __BAUD__ -D -U flash:w:__BINARY__:i

where:
|        TAG          | It's meaning                                                        |
|---------------------|---------------------------------------------------------------------|
|     __APP__         |   avrdude                                                           |
|   __APP_CONF__      |   configuration file ?                                              |
|    __USB_DEV__      |   Over what you have connect it                                     |
|     __BAUD__        |   57600                                                             |
|    __BINARY__       |   The build binary in form .hex (especially the biggest of the two) |
|                     |                                                                     |
 
 As __USB_DEV__ is over where you connect. Most of Linux User using an Raspberry Pi will host and USB bar to extend the number of USB plug, but FTDI-232 user will see the TTL to USB device under /dev/ttyACM0, CH340 User will see the device under /dev/USB0, /dev/USB1. Windows user to have COM port and will sometime refer to install the FTDI-232 driver or CH340-SER driver which is not cover here actually. OSX user will vary but look like /dev/cua... 

As __BINARY__ represent the binary name file it must include the whole path location and the file. As user using arduino-compile and arrayBuildProperty in this activity  using view_BuildProperty will show you a key name "--output-dir" based on .BuildProperty file from this repository the key "--output-dir" show a value of "build/lgt8fx.avr.328" where from your git-clone seance you might have directory name build/lgt8fx.avr.328 where two hexadecimal are present if arduino-compile was used for this activity you should see :

**bash**

    ls build/lgt8fx.avr.328/

**output**

    -rw-rw-r-- 1 maxiste maxiste     13 Aug  4 18:16 ST7735RotaryBtnEx.ino.eep
	-rw-rw-r-- 1 maxiste maxiste   2812 Aug  4 18:16 ST7735RotaryBtnEx.ino.with_bootloader.hex
	-rw-r--r-- 1 maxiste maxiste  30720 Aug  4 18:16 ST7735RotaryBtnEx.ino.with_bootloader.bin
	-rw-rw-r-- 1 maxiste maxiste  46642 Aug  4 18:16 ST7735RotaryBtnEx.ino.hex
	-rwxrwxr-x 1 maxiste maxiste 124736 Aug  4 18:16 ST7735RotaryBtnEx.ino.elf

 You see "ST7735RotaryBtnEx.ino.with_bootloader.hex" which is a small file and ST7735RotaryBtnEx.ino.hex which is bigger, you should take this one not the first. Everything inside is converted from binary to Intel Hexadecimal sheet which is small line with a checksum and only avrdude can read or any Intel Hexadecimal format. 

Remark. The file is bigger than 16K or so show while compiling with arduino-compile is it normal ? Absolutely as big the file is in text mode and is filled from the binary segment and a checksum at every segment. Avrdude is pretty checking all the line to ensure they have no error transfer through the Intel Hexadecimal protocol and send it to the micro-controller. The file "ST7735RotaryBtnEx.ino.elf" left in is Unix like format is available to do objdump and linking it to your code letting you see the assembly code with your Arduino file. It's not covered up but it's genuine programmatic tool reserved to inspect the code and it's opacity.  

Future effort will introduce a bash function to help flashing. 

## 8. Compilation with arduino-compile & arrayBuildProperty.
First let do a shell operation because the main arrayBuildProperty read hidden file and this repository does not support to store hidden file:

**bash**

    mv aliasrc .aliasrc 
    mv BuildProperty .BuildProperty

Be sure to load the script file before : 

**bash**

    . ./aliasrc
    . ./Arduino.sh
    load_BuildProperty 

from this repository the .aliasrc file is a shortcut allowing you to gain time at the prompt it's simply better than using the up and down arrow to search in the history your last command. History is good for short term browsing into your own command wisdom but going to be cleared if you haven't verify if your log got wipe by the number of entry or upon time period you're going to be shy in front of your terminal the day it's wiped off, as alias  stay and can be stored and simply claimed back by calling ". ./.aliasrc" it's time shorter. 

### Alias of *_BuildProperty and its uses 
The example is already here and in few command you can add or remove option you made inside your code and including changing variable value which are define with #define are those yield here because it's changing the behavior of the application. One of the famous is removing the Serial.print(...) command and inside the ST7735RotaryBtnEx.ino all Serial command are brace with a define which is present inside the "arduino-cli compile --build-property" of two common recipe for Arduino called  compiler.c.extra_flags and compiler.cpp.extra_flags, which is arrayBuilderProperty that own such information and arduino-compile function that read the declarative Array and transform all the switch into recipe declaration which is pretty long. As declared inside ST7735RotaryBtnEx.ino DEBUG_USB_SERIAL should have to be inside the arduino-cli compile directive to let the output being show over your serial terminal. Actually it's not because we don't have too and especially good for short action and might not stay here forever. Why ? the code start working by removing the Serial.print and might mean it taking more than 5000 ms and may falling into a timeout every time it use the terminal serial and not having time to proceed all the LCD operation and look like being stall into operation. One good also is how fast it should be the terminal, here I have used TERMINAL_BAUD as define and let it work at 9600 to minimize the work and the load to not throw itself all into the 115200 baud speed, where almost everything stop to respond but you can show information from the terminal... It's a micro-controller not a computer and having a lot to manage all micro-controller does not have a super timer-controller and memory manager embedded and might fall into you code trap that slow the application to perform what you want . Its a good condition to develop a bash-function for this type of reality.

Loading is done while doing the previous bash operation let see inside the Array to see which option are present :

**bash**

    view_BuildProperty

**output**

    View BuildProperty information.
    Key --export-binaries	        Value:
    Key compiler.c.extra_flags	    Value:-DTERMINAL_BAUD=9600 -DLED_CTRL=12 -DANALOG_RESOLUTION=10 -DGEN_SLEEP_WAIT=1000 -DI2C_SLAVE_ADDR_MSG=0x55 -DI2C_CLOCK_FREQ=400000 -DFLEX_WIRE_PORT_SCL=A0 -DFLEX_WIRE_PORT_SDA=A1 -DWATCHDOG_ISR_WAIT_TIME=WTOH_256MS -DWITH_WDT_LGT8F
    Key compiler.cpp.extra_flags	Value:-DTERMINAL_BAUD=9600 -DLED_CTRL=12 -DANALOG_RESOLUTION=10 -DGEN_SLEEP_WAIT=1000 -DI2C_SLAVE_ADDR_MSG=0x55 -DI2C_CLOCK_FREQ=400000 -DFLEX_WIRE_PORT_SCL=A0 -DFLEX_WIRE_PORT_SDA=A1 -DWATCHDOG_ISR_WAIT_TIME=WTOH_256MS -DWITH_WDT_LGT8F
    Key --output-dir	            Value:build/lgt8fx.avr.328

The function is quite simple to show the Key and theirs value. In this Array declaration there is 2 kind of key. And as associative Array are possible to put anything as key, beware they can't be enumerated with value as BuildProperty['--output-dir'] is not equivalent to BuildProperty[3].

As '--export-binaries' and '--output-dir' are arduino-cli compile switches can be added simply and since '--export-binaries' are lonely option it;s true you store them without value and deleting them require only to call them like ACTION=DEL KEY="--export-binaries" arrayBuildProperty, remove the key existence inside the compile life cycle. key like compiler.c.extra_flags and compiler.cpp.extra_flags are storing the content and can manage by #define directive to be stored inside the BuildProperty Array but once arduino-compile function is acting it decompose any of the -DTERMINAL_BAUD=9600 -DLED_CTRL=12 into :

**bash**

    arduino-cli compile --build-property compiler.c.extra_flags="-DTERMINAL_BAUD=9600 -DLED_CTRL=12" --build-property compiler.cpp.extra_flags="-DTERMINAL_BAUD=9600 -DLED_CTRL=12" ... 

And start to be heavy for only compilation and debugging, the arduino-compile was designed in that way.  

### Number of jobs
As arduino-compile own its own number of job for compilation, having too much parallel compilation can sometimes stop your system to work I do suggest to create an alias for arduino-compile with your number of available core for compiling. Designed on a Xeon E5 2650v4 it own 12 core let use some shell command to help you designing a not to aggressive workload:

**bash**

How many core you have :

    grep "cpu cores" /proc/cpuinfo | head -n1 | cut -d: -f 2

Following the number you have let spilt in 2 : 

**bash**

    alias arduino-compile='ACJobs=$(( $( grep "cpu cores" /proc/cpuinfo | head -n1 | cut -d: -f 2 ) / 2 )) arduino-compile'
or simply edit Arduino.sh and inside the function arduino-compile change the line:

**editor**

    local IntJobP=${ACJobs:=12} ;

By the number the grep give you. Exceptionally for Pentium III and IV you may have between number 1 and 2 for Pentium III if it's in dual. Same thing for Amd x86_64 it start at 2 with Atlhon, Rizen up to 10, using Arm like raspberry pi 4 or 5, between 4 and 8, and so on.

If you change the Arduino.sh, don't forget to load it back it's the same as chapter "Compilation with arduino-compile & arrayBuildProperty". 

Is there an helper with arduino-compile ? 
Not really it's a straight forward resource reader and command line composer. But since the management of option is an hard task using arrayBuildProperty, this one store and save into an Array and might own characteristic of a database and own its ADD, DELETE, CHANGE and some other operation just start with the help:

**bash**

    ACTION=HELP arrayBuildProperty

**output:**

    This Help.
    In this help it show major action allowed by using from prompt
	POSIX-Variable ACTION and KEY used to perform action on associative
	array named BuildProperty which is used inside function arduino-compile
	to enhance uses of arduino-cli command in case of compilation of Arduino
	Sketches.

	Available command through ACTION:

	HELP     This help.
	VIEW     List all key and value to be use during compilation.
	ADD      Add a new key considered during a compilation pass.
             Adding a know key does not erase the content, if VALUE
             is not empty is added at the end of the key name.
	DEL      Delete a key at the time.
             Deleting while having VALUE with non empty value search.
             inside the key and remove the content inside the key name.
	CHANGE   Change the VALUE ONLY of a key.
	SHOW     Show key from arduino-cli and it's value.
	SAVE     Save to file the BuildProperty key/value.
	LOAD     Restore BuildProperty from saved key/value.
	PARSE    Parse a file inserted as key=FILENAME and return a
             parsed file. Each tag do have to be inserted inside
             the Array and must have the form __TAG__ and a value
             should be added for parsing. 

         As in 3 action to create a file text.txt with __TAG__ inside
         the file.

         In action number 1 you tell __TAG__ will become 'Hello'
         In action 2 you add the File name text.txt inside the
         arrayBuildProperty
         In 3 step you start the parsing .
         Ex:1. ACTION=ADD KEY=__TAG__ VALUE=Hello arrayBuildProperty
            2. ACTION=ADD KEY=FILENAME VALUE=text.txt arrayBuildProperty
            3. ACTION=PARSE arrayBuildProperty
         Option for Parsing:
         - Adding BPExtension=extension will create your parsed file under
         text.txt.extension
         - Adding BPIsOvExt=True overwrite the current extension text to
         text.extension . While an extension is starting at the end of
         the file and start with a dot.
	Notice:  To LOAD command you do require to call and evaluated and execution\n
	eval \$(  ACTION=LOAD arrayBuildProperty ) to make it loading.
	Or call the Filename . ./itself .

As activity, adding the compilation switch DEBUG_USB_SERIAL to see an ouput.

**bash**

    declare -a ArrayKey=( compiler.c.extra_flags compiler.cpp.extra_flags ) 
    for item in ${ArrayKey[@]} ; do 
      ACTION=ADD KEY=${item} VALUE="-DDEBUG_USB_SERIAL" arrayBuildProperty 
    done 
Or:

    ACTION=ADD KEY=compiler.c.extra_flags VALUE="-DDEBUG_USB_SERIAL" arrayBuildProperty
    ACTION=ADD KEY=compiler.cpp.extra_flags VALUE="-DDEBUG_USB_SERIAL" arrayBuildProperty

Deleting the DEBUG_USB_SERIAL:

**bash**

    declare -a ArrayKey=( compiler.c.extra_flags compiler.cpp.extra_flags ) 
    for item in ${ArrayKey[@]} ; do 
      ACTION=DEL KEY=${item} VALUE="-DDEBUG_USB_SERIAL" arrayBuildProperty 
    done
This will remove DEBUG_USB_SERIAL and leave the Array with all the switch. Beware if you only put :

    ACTION=DEL KEY=${item} arrayBuildProperty
   This will erase the whole ${item} as key Defined into a enumeration of the key by using an inline array declaration it will suppress both key compiler.c.extra_flags and compiler.cpp.extra_flags, so beware, or load it again and only save when the test are done.


## 9. Compilation Switches example.
In this activity the micro-controller and the rotary switch and buttons including the rotary switch button may run in severals way and #define clause were developed to show characteristic of the dbuezas core and the original coding for arduino-core and how to change some elements. The definition as changing some elements can be the KEY of the rotary switch and where on the micro-controller it going to be associated. The other characteristic of the activity are moving where the button answer are treated, as speed response from changing the Watchdog respond time. And finally comparaison of using some switch. Using the exclusion of the Watchdog does weight in the balance but personally it's a good deal as alternative to attachInterrupt() to use the Watchdog function vector interrupt seems to be seamless.  

As you can use an alias to view the concurrent Array with arrayBuildProperty, be sure you start with the repository version or .BuildProperty :

**bash**

    load_BuildProperty
    ACTION=view arrayBuildProperty
 
 **output**
		
	View BuildProperty information.
    Key --export-binaries	        Value:
    Key compiler.c.extra_flags	    Value:-DTERMINAL_BAUD=9600 -DLED_CTRL=12 -DANALOG_RESOLUTION=10 -DGEN_SLEEP_WAIT=1000 -DI2C_SLAVE_ADDR_MSG=0x55 -DI2C_CLOCK_FREQ=400000 -DFLEX_WIRE_PORT_SCL=A0 -DFLEX_WIRE_PORT_SDA=A1 -DWATCHDOG_ISR_WAIT_TIME=WTOH_256MS -DWITH_WDT_LGT8F
    Key compiler.cpp.extra_flags	Value:-DTERMINAL_BAUD=9600 -DLED_CTRL=12 -DANALOG_RESOLUTION=10 -DGEN_SLEEP_WAIT=1000 -DI2C_SLAVE_ADDR_MSG=0x55 -DI2C_CLOCK_FREQ=400000 -DFLEX_WIRE_PORT_SCL=A0 -DFLEX_WIRE_PORT_SDA=A1 -DWATCHDOG_ISR_WAIT_TIME=WTOH_256MS -DWITH_WDT_LGT8F
    Key --output-dir	            Value:build/lgt8fx.avr.328

At the key compiler.c.extra_flags and compiler.cpp.extra_flags we have the option "-DWATCHDOG_ISR_WAIT_TIME=WTOH_256MS -DWITH_WDT_LGT8F". This is one of the default #define clause developed. It use use the Watchdog, but without other #define clause and belong to the code, there is inside 2 attachInterrupt(), one for rotaryInterrupt() and one for the switchInterrupt(). If you look inside the Watchdog ISR function there is only isrflag=true to determine if it pass inside the Watchdog function and belong to the loop() function in less than 5000 ms as expected before it yield an timeout the isrflag can be reset around 9 times in 5000ms and offering you 18 possible trigger to survey for touching something and react. But using the attachInterrupt() function add a possible check up behaving out of the consideration and not passing by the interrupt leave the micro-controller falling in the timeout if it hit the attachInterrupt() for too long, like you can't use switchInterrupt() function with define DEBOUNCING_SW as option and changing the code to spend 10000 cycle checking if the rotary switch have been pulled it will exhaust and might delay the pushing reaction of button to be late or not recorded. But it stand in the definition to add #define . 

### Fully using the Watchdog ?
This activity was conceived to fully change the button interrupt to stiff the rotary switch button to be part of the watch dog. But I also make it out of the Interrupt and being out of the watchdog too. Using those two switch will render the button over the rotary untreated and no reaction will occur. But using the Watchdog only for all the action including the rotary roll, not at this moment. This activity was also designed to test another rotary switch as replacement witch different qualification and here I show it does not change the code itself. And assuming the next activity was also to impair a rotary from a key not treated with attachInterrupt() of the function switchInterrupt() into a Watchdog function and adding the second rotary switch to also change the value show at the screen. 

So to fully use the Watchdog ISR function and detach the button from the rotary switch and make all the other button being handled by the Watchdog ISR function, add this following with arrayBuilder:

**bash**

    declare -a ArrayKey=( compiler.c.extra_flags compiler.cpp.extra_flags ) 
    for item in ${ArrayKey[@]} ; do 
      ACTION=ADD KEY=${item} VALUE="-DBTN_IN_WDT -DNO_BTN_IN_ISR" arrayBuildProperty 
    done

and recompile:

**bash**

    arduino-compile
**output**

    Sketch uses 16888 bytes (56%) of program storage space. Maximum is 29696 bytes.
	Global variables use 583 bytes (28%) of dynamic memory, leaving 1465 bytes for local variables. Maximum is 2048 bytes.

	Used library                       Version Path
	WDT                                        ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/WDT
	SPI                                1.0     ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/SPI
	Adafruit GFX Library               1.12.4  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_GFX_Library
	Adafruit BusIO                     1.17.4  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_BusIO
	Wire                               1.0     ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/Wire
	Adafruit ST7735 and ST7789 Library 1.11.0  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_ST7735_and_ST7789_Library
	
	Used platform Version Path
	lgt8fx:avr    2.0.7   ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7

### Looking at the initial definition:
**bash**

    declare -a ArrayKey=( compiler.c.extra_flags compiler.cpp.extra_flags ) 
    for item in ${ArrayKey[@]} ; do 
      ACTION=DEL KEY=${item} VALUE="-DBTN_IN_WDT -DNO_BTN_IN_ISR" arrayBuildProperty 
    done

recompile:

**bash**

    arduino-compile
**output**

    Sketch uses 16864 bytes (56%) of program storage space. Maximum is 29696 bytes.
	Global variables use 583 bytes (28%) of dynamic memory, leaving 1465 bytes for local variables. Maximum is 2048 bytes.

	Used library                       Version Path
	WDT                                        ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/WDT
	SPI                                1.0     ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/SPI
	Adafruit GFX Library               1.12.4  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_GFX_Library
	Adafruit BusIO                     1.17.4  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_BusIO
	Wire                               1.0     ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/Wire
	Adafruit ST7735 and ST7789 Library 1.11.0  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_ST7735_and_ST7789_Library

	Used platform Version Path
	lgt8fx:avr    2.0.7   ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7

Ok it remove 24 bytes but merely rewritten few line and trowed out an attachInterrupt() call. Let's updating your micro-controller with avrdude ( explain at the end ). Those can test will see no difference at this moment and you save in Arduino core call. By calling WDT.h library which hold code in assembly for managing the watchdog. Now removing all trace of the watchdog for pretending porting the code in full Arduino and having no idea if the assembly code will work for an atmega328p ?

**bash**

    declare -a ArrayKey=( compiler.c.extra_flags compiler.cpp.extra_flags ) 
    for item in ${ArrayKey[@]} do 
        ACTION=DEL KEY=${item} VALUE="-DWATCHDOG_ISR_WAIT_TIME=WTOH_256MS -DWITH_WDT_LGT8F -DBTN_IN_WDT -DNO_BTN_IN_ISR" arrayBuildProperty ; 
    done
recompile:

**bash**

    arduino-compile
**output**

    Global variables use 581 bytes (28%) of dynamic memory, leaving 1467 bytes for 	local variables. Maximum is 2048 bytes.

	Used library                       Version Path
	SPI                                1.0     ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/SPI
	Adafruit GFX Library               1.12.4  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_GFX_Library
	Adafruit BusIO                     1.17.4  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_BusIO
	Wire                               1.0     ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7/libraries/Wire
	Adafruit ST7735 and ST7789 Library 1.11.0  ${HOME}/Documents/Arduino/Sketches/libraries/Adafruit_ST7735_and_ST7789_Library

	Used platform Version Path
	lgt8fx:avr    2.0.7   ${HOME}/.arduino15/packages/lgt8fx/hardware/avr/2.0.7

Inside used library, it confirm the WDT is scrapped of the list so the total cost of fully externalize the 2 buttons including another button on the rotary switch through the WDT and make only one attachInterrupt() call it cost 234 bytes. It's not the end of the world but personally another ISR function while most atmega328p and LGT8F328 can only own 2 attachInterrupt() one on the pin2 and one on the pin 3 it's like an extra function working at interleave N offering another possibility to manage the changing in pin state. 

### The one that does not work from ISR. 
Using #define from excluding Button inside the ISR in prediction to use the WDT by having both "-DWITH_WDT_LGT8F" and "-DNO_BTN_IN_ISR" which all button being handled by a non ISR function which is apparently slower if processed inside function UpdatePressAction(). It's up to you to test. 

### Missmatching with #WITH_WDT_LGT8F
by telling you, your addition with ACTION=ADD arrayBuilderProperty, you forget to add "-DWITH_WDT_LGT8F" but add "-DBTN_IN_WDT", and "-DNO_BTN_IN_ISR". Button are officially removed from attachInterrupt() and nothing belong to the rotary switch button is handled by no attachInterrupt() and no WDT() ? Your are going to get not reaction from the rotary switch button because the UpdatePressAction() does react to NO_BTN_IN_ISR and the button in the WDT while the code for WDT is not present. It work, for rotary action to roll and react to the direction, button from "CONF" and "BACK" will respond but only them. 

### Different Pin configuration
As we had totally get rid of the button algorithm using attachInterrupt(), we can make it free for something else. This schema suggest now the rotary button switch will move from previous schema from Pin 3 to Pin 7 and with uses of arrayBuilderProperty and default value issue inside the code belong to the SW_ENCDR Pin name inside the Arduino code show some #define directive possible to change the pin name. Let see the code:

**editor** file: ST7735RotaryBtnEx.ino

    #if !defined( SW_ENCDR_PIN )
	#define SW_ENCDR                    3
	#else
	#define SW_ENCDR                    SW_ENCDR_PIN
	#endif

  Found at the beginning of the Arduino Code, This show a default value of '3', the Pin reference name. Using the #define SW_ENCDR_PIN=7 to change the compilation information using arrayBuilderProperty .

From the original version of .BuildProperty from this repository do this command line.

**bash**

	declare -a ArrayKey=( compiler.c.extra_flags compiler.cpp.extra_flags ) 
    for item in ${ArrayKey[@]} do 
      ACTION=ADD KEY=${item} VALUE="-DBTN_IN_WDT -DNO_BTN_IN_ISR -DSW_ENCDR_PIN=7" arrayBuildProperty ; 
    done
recompile:

**bash**

    arduino-compile

And flash the build version inside the LGT8F328 chip and this will work too. But warning, to only change the Pin assignation and not telling to remove the attachInterrupt() with the define compilation "-DNO_BTN_IN_ISR" let try to attach PC INT interrupt to a different Pin than 2 or 3 which is not possible it only check these two Pin. The define option "-DBTN_IN_WDT" might be not used an will work too assigning to Pin 7. In extra you can try yourself Using Pin 12, Pin 22 SW or Pin 23 SWD they should work too. Apparently the Analog pin can support the INPUT_PULLUP situation and might work, but exclude pin A4 and A5 they are used for I2C and may not work directly as Analog Pin and either as digital pin running to several frequency. 

![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/schema_ST7735RotaryBtnEx_Pin7.png)

## 10. Alternative.
Here the alternative section. using the default model from this repository, flashing this chip with the file ST7735RotaryBtnEx.ino.hex and instead of using the normal encoder, using the KY-040 or the Keyes rotary encoder does require to get a level voltage translator. Here the cheap and good one should be enough. The 4 transistors with 4 x 10 kilo Ohm resistors will be enough.  If you canget an 8 bridged level voltage translator you can put the 2 button on the level bridge number 4 and 5 and connect the button to 5V instead and Put LV4 and LV5 to Pin 5 and Pin 6 and change the resistor for 4.7 Kilo Ohms by 10 Kilo Ohms and all your entry will be on 5V. Here only the KY-040 rotary encoder and the internal button switch is 5V based.  


![enter image description here](https://raw.githubusercontent.com/priendeau/Arduino/refs/heads/main/LGT8F328/ST7735RotaryBtnEx/ST7735BtnEx_2ndTypeRotr.png)
