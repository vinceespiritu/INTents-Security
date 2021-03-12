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

int main(void)
{
    DDRC |= 1 << DDC0;          // Set PORTC bit 0 for output
    PORTC |= 1 << PC1;          // Enable pull-up for switch on PORTC bit 1

    while (1) {
        if (PINC & (1 << PC1))  // Read the button
            PORTC &= ~(1 << PC0);  // Not pressed, LED off
        else
            PORTC |= 1 << PC0;  // Pressed, LED on
    }

    return 0;   /* never reached */
}
