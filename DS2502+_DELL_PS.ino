/*

Programmer for DS2502+ device used in  DELL 90Watt power supply to identify the power adapter as a 90W, 19.5V 4.6A DELL AC adapter

This sketch needs a DS2502+ programmer interface board attached to arduino digital pins 6 and 7, fed with a 12V PS capable of 100ma
This sketch is running on a Pro-mini running at 16mhz, 5V from the on board 12 to 5V regulator and has been verified under Arduino 1.6.1 IDE, 

Eagle CAD files for programmer board can be downloaded from Github here: https://github.com/garyStofer/DS2502_DELL_PS
PCB boards can be ordered from OSH PCB here: https://oshpark.com/shared_projects/l4VC80Px

Sketch requires library  PJRC OneWire 2.0 library http://www.pjrc.com/teensy/td_libs_OneWire.html

Sketch is roughly based on the One-Wire example code provided  by Guillermo Lovato <glovato@gmail.com>

 The DS250x is a 512/1024bit add-only PROM(you can add data but cannot change the old one) that's used mainly for device identification purposes
 like serial number, mfgr data, unique identifiers, etc. It uses the Maxim 1-wire bus.

Use: Upon power-up of the Arduino the code is looking for an attached DS2502 device in a 3 second loop. if the device is found it will be programmed with the 
text in progStr and after wards the programmed block will be read back, bot in hex and as a string. The program then stops executing until reset is pressed.
Programming a device multiple times will not hurt or corrupt the device unless a different programming string or address is chosen.

Progress can be monitored via the Serial terminal in the IDE

 
 Sept. 2015 , Gary Stofer, GaryStofer@gmail.com
 This file archived at https://github.com/garyStofer/DS2502_DELL_PS

 August 30, 2025, Chad Bittner, cbittner56@msn.com
 - Replace R1 4.7K resistor with 2.2K resistor for programming the DS2502+ version of the DS2502 IC
 - https://www.digikey.com/en/products/detail/analog-devices-inc-maxim-integrated/DS2502/1197440

*/

#include <OneWire.h>

#define DOPROGRAM

#define LED_PIN 10
#define PROG_PIN 7
#define ONE_WIRE_IO_PIN 6
#define DS2502DevID 0x0B

// DS250x commands
#define READ_ROM   0x33
#define READ_MEMORY  0xF0
#define WRITE_MEMORY 0x0F

#define ProgLocation 0x00
#define ProgPulseUs 600

OneWire ds(ONE_WIRE_IO_PIN);

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT); 
    digitalWrite(PROG_PIN, HIGH);
    pinMode(PROG_PIN, OUTPUT);
    Serial.println("DS2502+ Programmer Ready");
}

const char *progStr = "DELL00AC090195046CN09T";

void lockup(void) {
    Serial.println("Stopped!!");
    while (1) { delay(10); }
}

void ProgPulse(void) {    
    digitalWrite(PROG_PIN, LOW);  
    delayMicroseconds(ProgPulseUs);
    digitalWrite(PROG_PIN, HIGH); 
    delayMicroseconds(100); 
}

void loop() {
    byte data[32];
    byte crc, crc_calc;

    digitalWrite(LED_PIN, LOW);
    
    if (ds.reset()) {
        digitalWrite(LED_PIN, HIGH);
        Serial.println("Device present");
        
        // Read ROM (but don't require specific family code)
        ds.write(READ_ROM, 1);
        Serial.print("ROM Data: ");
        for (byte i = 0; i < 8; i++) {
            data[i] = ds.read();
            Serial.print(data[i] < 0x10 ? "0" : "");
            Serial.print(data[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
        
        // Continue even if ROM CRC fails (blank chip)
        crc_calc = OneWire::crc8(data, 7);
        if (crc_calc == data[7]) {
            Serial.println("ROM CRC matches");
        } else {
            Serial.println("ROM CRC failed - continuing anyway (blank chip?)");
        }

#ifdef DOPROGRAM        
        Serial.println("Starting Programming...");
        
        // Program each byte with better timing
        for (byte i = 0; i < strlen(progStr); i++) {
            Serial.print("Programming byte ");
            Serial.print(i);
            Serial.print(" at address 0x");
            Serial.print(ProgLocation + i, HEX);
            Serial.print(": 0x");
            Serial.print(progStr[i], HEX);
            Serial.print(" ('");
            Serial.print((char)progStr[i]);
            Serial.println("')");
            
            ds.reset();
            ds.skip();
            
            // Send write command
            ds.write(WRITE_MEMORY, 1);
            ds.write(ProgLocation + i, 1);
            ds.write(0, 1);
            ds.write(progStr[i], 1);
            
            // Read CRC response
            crc = ds.read();
            
            // Calculate expected CRC
            byte crc_data[4] = {WRITE_MEMORY, (byte)(ProgLocation + i), 0, progStr[i]};
            crc_calc = OneWire::crc8(crc_data, 4);
            
            if (crc_calc != crc) {
                Serial.print("CRC mismatch on write! Expected: 0x");
                Serial.print(crc_calc, HEX);
                Serial.print(" Got: 0x");
                Serial.println(crc, HEX);
                Serial.println("Attempting programming anyway...");
            }
            
            // Send programming pulse
            ProgPulse();
            
            // Read back verification
            byte read_back = ds.read();
            
            // Immediate verification read
            ds.reset();
            ds.skip();
            ds.write(READ_MEMORY, 1);
            ds.write(ProgLocation + i, 1);
            ds.write(0, 1);
            byte verify_byte = ds.read();
            
            Serial.print("Write: 0x");
            Serial.print(progStr[i], HEX);
            Serial.print(" Read back: 0x");
            Serial.print(read_back, HEX);
            Serial.print(" Verify: 0x");
            Serial.print(verify_byte, HEX);
            
            if (verify_byte == progStr[i]) {
                Serial.println(" ✓");
            } else {
                Serial.println(" ✗");
                // Don't lock up, just continue
            }
            
            delay(10); // Short delay between bytes
        }
#endif

        // Final verification read
        Serial.println("Final Verification:");
        ds.reset();
        ds.skip();
        ds.write(READ_MEMORY, 1);
        ds.write(ProgLocation, 1);
        ds.write(0, 1);
        
        Serial.print("Memory: ");
        for (byte i = 0; i < 32; i++) {
            data[i] = ds.read();
            Serial.print(data[i] < 0x10 ? "0" : "");
            Serial.print(data[i], HEX);
            Serial.print(" ");
            if ((i+1) % 8 == 0) Serial.print(" ");
        }
        Serial.println();
        
        // Display as string
        data[strlen(progStr)] = 0;
        Serial.print("String: ");
        Serial.println((char *)data);
        
        Serial.println("Programming sequence completed");
        lockup();
    } else {
        Serial.println("Nothing connected");
        delay(3000);
    }
}