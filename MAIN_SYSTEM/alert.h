#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>

//Initialize DDR
void alert_initialize();

//Lights ONLY SWITCH
void Lights_ON();
void Lights_OFF();

//Buzzer ONLY SWITCH
void Buzz_ON();
void Buzz_OFF();

//Alert SWITCH
void Alert_ON();
void Alert_OFF();
