#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include <SPI.h>
#include <Wire.h>

#include <SFE_MicroOLED.h>

#define PIN_RESET 7  // Connect RST to pin 9 (req. for SPI and I2C)
#define PIN_DC    8  // Connect DC to pin 8 (required for SPI)
#define PIN_CS    9 // Connect CS to pin 10 (required for SPI)

#define DC_JUMPER 0 // Set to either 0 (default) or 1 based on jumper, matching the value of the DC Jumper
// Also connect pin 13 to SCK and pin 11 to MOSI

    
// Declare a MicroOLED object. The parameters include:
// 1 - Reset pin: Any digital pin
// 2 - D/C pin: Any digital pin (SPI mode only)
// 3 - CS pin: Any digital pin (SPI mode only, 10 recommended)
MicroOLED oled(PIN_RESET, PIN_DC, PIN_CS); //Example SPI declaration, comment out if using I2C
//MicroOLED oled(PIN_RESET, DC_JUMPER); //Example I2C declaration, uncomment if using I2C 



void print_message(){
    oled.setFontType(1);  
    oled.setCursor(0, 0);
    oled.print("Hello, world"); // Print a const string
    oled.print(analogRead(0));  // Print an integer
    oled.print(42.07);  // Print a float
    oled.display(); // Draw to the screen
}

void setup()
{
    delay(100);
    //Wire.begin(); //set up I2C bus, uncomment if you are using I2C
    // Before you can start using the OLED, call begin() to init
    // all of the pins and configure the OLED.
    oled.begin();
     oled.clear(ALL);
     oled.display();   

  DDRD|=(1<<PD2); //motion led
  PORTD&=~(1<<PD2);
    
    DDRD &= ~(1<<PD0); //button
    PORTD |= (1<<PD0);  //pullup resistor
}

void loop() {
  // put your main code here, to run repeatedly:
 if ((PIND & (1<<PD0)) == 0){ //chcek if button is pressed
     while((PIND & (1<<PD0)) == 0){  }
      PORTD|=(1<<PD2);
          print_message();  //oled message     
       }
  
}
