#!/usr/bin/python3.10

#! -*-*-filename: arduinoLibExport.py-*-*-


import sys
import os
import time
from time import sleep
from os import environ
from os import unlink
import subprocess
import json
import tempfile
import argparse

strFilename="/home/__USER__/Documents/Arduino/Sketches/lib-list.json"
strYamlArduino="/home/__USER__/.arduinoIDE/arduino-cli.yaml"
strAppArduinoClient="/usr/local/bin/arduino-cli"
try:
  if environ['FILE_NAME'] is None :
        ### The if-test itself will trigger an KeyError
        ### because environ[Key] is not possible to test.
        pass
  else:
    strFilename=environ['FILE_NAME']
except KeyError:
      print( "Environmenet variable %s not present, you can specified the downloaded lib-list.json with FILE_NAME=%s" % ('FILE_NAME','FILE_NAME') )

try:
  if environ['YAML_ARDUINO'] is None :
        ### The if-test itself will trigger an KeyError
        ### because environ[Key] is not possible to test.
        pass
  else:
    strYamlArduino=environ['YAML_ARDUINO']
except KeyError:
      print( "Environmenet variable %s not present, you can specified arduino-cli or Arduino IDE YAML configuration with %s=..." % ('YAML_ARDUINO','YAML_ARDUINO') )

try:
  if environ['ARDUINO_CLI'] is None :
        ### The if-test itself will trigger an KeyError
        ### because environ[Key] is not possible to test.
        pass
  else:
    strAppArduinoClient=environ['ARDUINO_CLI']
except KeyError:
      print( "Environmenet variable %s not present, you can specify arduino-cli application with %s=..." % ('ARDUINO_CLI','ARDUINO_CLI') )


strCmdArduinoCli="%s --config-file %s" % (strAppArduinoClient,strYamlArduino)
strCmdLibList="%s --config-file %s --json lib list" % (strAppArduinoClient,strYamlArduino)

DictInstalled={ 'installed_libraries':{ 'name':[ ] } }

def ReadJson( strFile, strHeader ):
  strFile=strFile.replace( '__USER__', environ['USER'] )
  Areader=open(strFile,"r")
  Aobj=json.load(Areader)[strHeader]
  return Aobj

def GenerateListExport( objJson ):
  listLib=list()
  intCount=0
  for item in objJson:
    StrName="%s@%s" % ( item['library']['name'],item['library']['version'] )
    if StrName not in listLib:
      listLib.append( StrName )
  return listLib

def GenerateExportInstalled( ):
  listRead=list()
  try:
    ObjJsonExported = ReadJson( "exported_lib.json" , 'installed_libraries' )
    for itemExport in ObjJsonExported['name']:
      listRead.append( itemExport )
  except FileNotFoundError:
    ### The ExportInstalled library is
    ### simply not existing, normal at first try.
    print("No exported_lib.json present, this one will be created upon first incompatible library detected.")
  return listRead

def PostInstallVerif( StrCmd, StrModule ):
  IsFound=False
  proc = subprocess.Popen('/bin/bash', stdin = subprocess.PIPE, stdout = subprocess.PIPE)
  StrCmd=StrCmd.replace( "__USER__", environ['USER'] )
  stdout, stderr = proc.communicate( bytes( StrCmd, 'utf-8') )
  AobjVerif=json.loads(stdout)
  for item in AobjVerif["installed_libraries"]:
    StrName="%s@%s" % ( item['library']['name'],item['library']['version'] )
    if StrName == StrModule:
      IsFound=True
  return IsFound
  
def InstallFromExport( StrCmd, StrModule ):
  proc = subprocess.Popen('/bin/bash', stdin = subprocess.PIPE, stdout = subprocess.PIPE)
  StrCmd=StrCmd.replace( "__USER__", environ['USER'] )
  StrCmdLaunch=("%s lib install \"%s\"" % ( StrCmd , StrModule))

  IsInstall=False
  if environ['IsDebug'] != "True" :
    IsInstall=PostInstallVerif( strCmdLibList , StrModule )
    if IsInstall is False:
      print("Command to launch:[%s]" % StrCmdLaunch)
      stdout, stderr = proc.communicate( bytes( StrCmdLaunch, 'utf-8') )
      if environ['NoStdOut'] == "False" and environ['NoStdErr'] == "False":
        print("%s\n%s" % ( stdout, stderr ) )
      if environ['NoStdOut'] == "True" and environ['NoStdErr'] == "False":
        print("%s\n" % ( stderr ) )
      if environ['NoStdOut'] == "False" and environ['NoStdErr'] == "True":
        print("%s\n" % ( stdout ) )
    else:
      print( "Module library: %s already installed" % ( StrModule ) )
  else:
    print("DEBUG: Cmd:[%s]" % StrCmdLaunch)

def CreateExcludeList( StrVarName, cSeparator=';' ):
  listContain=list()
  StrExclude=environ[StrVarName]
  listContain=StrExclude.split(sep=cSeparator)
  return listContain

def InstallExportList( strExportJsonLib ):
  listExport=list()
  ListLib=list()
  listExclude=list()

  listExclude=CreateExcludeList( 'SkipList', ';' )  
  print("Reading Exclusion list")
  for item in listExclude:
    print("Will exclude %s" % ( item ) )
  ObjJsonInst = ReadJson( strExportJsonLib , 'installed_libraries' )
  
  ListLib=GenerateListExport( ObjJsonInst )
  listExport=GenerateExportInstalled()
  IsProcessError=False
  StrPackage=""
  for item in ListLib:
    if item not in listExclude:
      if item not in listExport:
        print("Processing installation of %s" % ( item ) )
        if IsProcessError is False:
          InstallFromExport( strCmdArduinoCli, item )
          print("Verification of installation.")
          if PostInstallVerif( strCmdLibList, item ) is False:
            IsProcessError=True
            StrPackage=item
          else:
            ### Library is correctly installed. Adding it to installed_library dict.
            DictInstalled['installed_libraries']['name'].append( item )
      else:
        print("Skipping installation %s, already found in exported-list of installed library." % ( item ) )
        ### We do have to provide inside DictInstalled or it will be dumped into
        ### the already installed library.
        DictInstalled['installed_libraries']['name'].append( item )
    else:
      print("Skipping library %s, specified in exclusion-list." % ( item ) )
  if IsProcessError:
    print( "An error occur at installation of package: %s\nThis package went in problem and probably require\nto add this package inside Variable SkipList with\na semi-colon and start the python script again.\n" % ( StrPackage ) )
    print( "Dumping installed library into json format.")
    with open("exported_lib.json", "w") as file:
       json.dump(DictInstalled, file, indent=2)
  else:
    print("Updating the installed-list into json format.")
    with open("exported_lib.json", "w") as file:
       json.dump(DictInstalled, file, indent=2)
    print("All exported list has been installed. You can try arduino-cli or Arduino IDE.")

def EnvironKeyStart( listElement ):
  for ItemVar in listElement:
    try:
      if environ[ItemVar] is None :
        ### The if-test itself will trigger an KeyError
        ### because environ[Key] is not possible to test.
        pass
    except KeyError:
      print( "Environmenet variable %s not present, defaulty assigned to False " % ItemVar )
      environ[ItemVar]="False"


if __name__ == '__main__':
  print( "System Argument from command-line: %s" % ( sys.argv ) ) 
  if '--help' not in sys.argv:
    listVarEnviron=['NoStdOut','NoStdErr','IsDebug']
    EnvironKeyStart( listVarEnviron )
    InstallExportList( strFilename )
  else:
    parser=argparse.ArgumentParser(
      description='''Arduino py-script to install library based on json export from arduino lib list --json command.''',
      epilog="""Allow you to install on other computer the original library list developped internally without doing an installation from Arduino IDE.""")
    print("\nAllow you to install on other computer the original library list\ndevelopped internally without doing an installation from Arduino IDE.\n\nThis application run from shell and require several VARIABLE-NAME to\nbe use to enhance your transfert of your Arduino Library:\n\nA variable name is a POSIX SHELL variable being declared before the\napplication:\n\nas example using arduinoLibExport.py with python application, all\nvariable will be before:\n\nexample:\n[ VARIABLE-NAME ] python3 arduinoLibExport.py\nWith SkipList:\n\nSkipList=\"DS323x_Generic@1.3.1;PU2REO Si5351ArduinoLite@1.0.1\" \\ \npython3 arduinoLibExport.py\n\nwhere VARIABLE-NAME is supposed to be added.\n\nList of VARIABLE-NAME:\n\nNoStdOut    Disable output from standard output being show during\n            installation of library.\n\nNoStdErr    Disable error output from stderr to be sho during\n            library installation. Some error are not belong to \n             standard installation. Us application do a verification \n            after every installation and certify its there.\nIsDebug     Only display which command are launched but not execute \n            them.\nSkipList    Some Old library from Arduino 1.8.19 are not existing\n            anymore or some American deny access to them by removing\n            the dns map, since you can not install them, but many \n            library where removed to not be available yet, you \n            specified them here with a semi-colon, and will be \n            excluded during installation see the example.\nFILE_NAME   The lib-list.json from github or your own arduino-cli lib \n            list --json.\nYAML_ARDUINO Where your new arduino-cli/IDE does store the configuration\n             and it's filename (usually arduino-cli.yaml)\nARDUINO_CLI Required application to install the library,usually it's\n            /usr/local/bin/arduino-cli\n\nValue for NoStdOut,NoStdErr,IsDebug are [True/False] and sensitive case.\n" )
    args=parser.parse_args()

###
### The provided script with library list also include dead link where it's easier to 
### supply a SkipList with here the example:
### 
### SkipList="DS323x_Generic@1.3.1;PU2REO Si5351ArduinoLite@1.0.1;RF24Audio@1.0;ATCommands@0.0.1;TwiBus@1.2.0;NEO-6M_GPS@1.0.2;TimonelTwiM@1.2.0;NbMicro@1.2.0;esp8266-framework@1.1.5;MD_MAXPanel@1.4.0;Timezone_Generic@1.10.1;WiFiManager_Generic_Lite@1.8.0;iarduino_GprsClient_A9@1.0.2" NoStdOut=True NoStdErr=True IsDebug=False python3 arduinoLibExport.py
###
### This example enhance your experience of installing a list of library suggested with this github example of 
###
      
