
#include"alert.h"
#define Lights_SW PD4
#define Buzz_SW PD3


void alert_initialize(){

   DDRD |= 1 << DDD4; // Set PORTD bit 4 for output (LIGHT SWITCH)
   DDRD |= 1 << DDD3; // Set PORTD bit 3 for output (BUZZ SWITCH)

}

void Lights_ON(){

  PORTD |= (1<< Lights_SW); //LIGHTS ON
  _delay_ms(100);

}

void Lights_OFF(){

  PORTD &= ~(1 << Lights_SW); //LIGHTS OFF
  _delay_ms(100);

}

void Buzz_ON(){

  PORTD |= (1<< Buzz_SW); //BUZZER ON
  _delay_ms(100);

}

void Buzz_OFF(){

  PORTD &= ~(1 << Buzz_SW); //BUZZER OFF
  _delay_ms(100);

}

void Alert_ON(){

  //Turns ON both Lights and Buzzer
  Lights_ON();
  Buzz_ON();
   _delay_ms(100);

}

void Alert_OFF(){

  //Turns OFF both Lights and Buzzer
  Lights_OFF();
  Buzz_OFF();
  _delay_ms(100);

}