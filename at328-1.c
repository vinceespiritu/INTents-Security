/*************************************************************
*       at328-1.c - Demonstrate simple I/O functions of ATmega328
*
*       Port C, bit 1 - input from switch (0 = pressed, 1 = not pressed)
*       Port C, bit 0 - output to LED anode (1 = LED on, 0 = LED off)
*
*       When the switch is pressed, the LED comes on
*
* Revision History
* Date     Author      Description
*          A. Weber    Initial Release
* 09/05/05 A. Weber    Modified for JL8 processor
* 01/13/06 A. Weber    Modified for CodeWarrior 5.0
* 08/25/06 A. Weber    Modified for JL16 processor
* 05/08/07 A. Weber    Some editing changes for clarification
* 04/22/08 A. Weber    Added "one" variable to make warning go away
* 04/02/11 A. Weber    Adapted for ATmega168
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#define switch PD0

int main(void)
{
    DDRD |= 1 << DDD0;          // Set PORTD bit 0 for output

    int counter = 0;

    while (1) {
        
        if (counter==10)  // Check counter
            PORTD |= (1 << switch);  // Lights ON
        
        else if (counter ==20)
            PORTC &= ~(1 << switch);  // LIGHTS OFF
    		counter = 0;

    	counter++;
    }

    return 0;   /* never reached */
}
