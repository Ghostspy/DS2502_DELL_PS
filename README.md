## Programmer for DS2502/DS2502+ device used in  DELL 90Watt power supply to identify the power adapter as a 90W, 19.5V 4.6A DELL AC adapter

This sketch needs a DS2502/DS2502+ programmer interface board attached to arduino digital pins 6 and 7, fed with a 12V PS capable of 100ma

This sketch is running on a Pro-mini running at 16mhz, 5V from the on board 12 to 5V regulator and has been verified under Arduino 1.6.1 IDE.

This sketch has also been verified to work with an Arduino Uno R3 and Arduino 2.3.5 IDE.

Eagle CAD files for programmer board can be downloaded from Github here: https://github.com/garyStofer/DS2502_DELL_PS

PCB boards can be ordered from OSH PCB here: https://oshpark.com/shared_projects/l4VC80Px

Sketch requires library  PJRC OneWire 2.0 library http://www.pjrc.com/teensy/td_libs_OneWire.html

Sketch is roughly based on the One-Wire example code provided  by Guillermo Lovato <glovato@gmail.com>

 The DS250x is a 512/1024bit add-only PROM(you can add data but cannot change the old one) that's used mainly for device identification purposes
 like serial number, mfgr data, unique identifiers, etc. It uses the Maxim 1-wire bus.

Use: Upon power-up of the Arduino the code is looking for an attached DS2502/DS2502+ device in a 3 second loop. if the device is found it will be programmed with the 
text in progStr and after wards the programmed block will be read back, bot in hex and as a string. The program then stops executing until reset is pressed.

Programming a device multiple times will not hurt or corrupt the device unless a different programming string or address is chosen.

Progress can be monitored via the Serial terminal in the IDE
---
 Sept. 2015 , Gary Stofer, GaryStofer@gmail.com
  - This file archived at https://github.com/garyStofer/DS2502_DELL_PS

 August 30, 2025, Chad Bittner, cbittner56@msn.com
 - Added DS2502_DELL_PS.ino for programming the DS2502+ from [DigiKey](https://www.digikey.com/en/products/detail/analog-devices-inc-maxim-integrated/DS2502/1197440) 
 - Replace R1 4.7K resistor with 2.2K resistor for programming the DS2502+ version of the DS2502 IC
