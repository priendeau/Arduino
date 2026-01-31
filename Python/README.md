## application arduinoLibExport.py

This application allow you to install on other computer the original library list developed internally without doing an installation from Arduino IDE.

This application run from shell and require several VARIABLE-NAME to be use to enhance your transfer of your Arduino Library:

A variable name is a **POSIX SHELL** variable being declared before the
application:

as example using arduinoLibExport.py with python application, all variable will be before:

### example:
*[ VARIABLE-NAME ]* python3 arduinoLibExport.py

With SkipList:
SkipList="DS323x_Generic@1.3.1;PU2REO Si5351ArduinoLite@1.0.1"  python3 arduinoLibExport.py

where VARIABLE-NAME is supposed to be added.


### List of VARIABLE-NAME:
Here all the Variable name you can use for this application. As some are requiring to follow a logic, some are [True/False] and require to respect the case. This is the case for :

 - NoStdOut	is a *[True/False]* statement.
 - NoStdErr	is a *[True/False]* statement.
 - IsDebug	is a *[True/False]* statement.
 - FILE_NAME	is a *[String]* where filename is a Linux compliant and path are slash based.
 - YAML_ARDUINO is is a *[String]* where filename is a Linux compliant and path are slash based.
 -  ARDUINO_CLI	is a *[String]* where filename is a Linux compliant and path are slash based.

Here the definition:

|  Variable Name | Definition                                                  |
|----------------|-------------------------------------------------------------|
|NoStdOut        |Disable output from standard output being show during        |
|                |installation of library.                                     |
|                |                                                             |
|NoStdErr        |Disable error output from stderr to be sho during            |
|                |library installation. Some error are not belong to           |
|                |standard installation. Us application do a verification      |
|                |after every installation and certify its there.              |
|IsDebug         |Only display which command are launched but not execute      |
|                |them.                                                        |
|SkipList        |Some Old library from Arduino 1.8.19 are not existing        |
|                |anymore or some American deny access to them by removing     |
|                |the dns map, since you can not install them, but many        |
|                |library where removed to not be available yet, you           |
|                |specified them here with a semi-colon, and will be           |
|                |excluded during installation see the example.                |
|FILE_NAME       |The lib-list.json from github or your own arduino-cli lib    |
|                |list --json.                                                 |
|YAML_ARDUINO    |Where your new arduino-cli/IDE does store the configuration  |
|                |and it's filename (usually arduino-cli.yaml)                 |
|ARDUINO_CLI     |Required application to install the library,some are in      |
|                |/usr/local/bin/arduino-cli                                   |
|                |                                                             |

### Switch support
--help Show you this help.
