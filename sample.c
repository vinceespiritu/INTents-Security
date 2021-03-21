#include"alert.h"

int main(void)
{
    alert_initialize();
    //Serial.begin(9600);
    int count=0; 

    while (1) {

        if ( count == 100 ){  // Check counter

          //Lights_ON();
          Buzz_ON();
          //Alert_ON();
          //Serial.println("ON\n");
          _delay_ms(100);
          //Lights_OFF();
          Buzz_OFF();
          //Alert_OFF();
          //Serial.println("OFF\n");
          _delay_ms(100);
          count = 0;
        }

    count++;
    _delay_ms(10);

    }


    return 0;   /* never reached */
}