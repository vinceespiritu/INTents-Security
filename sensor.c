
#include "alert.h"

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define F_CPU 16000000
volatile int rfm_flag = 0; //rfm_flag =1 - message sent
volatile int alert_flag =0; //alert_flag=1 - alert just turned on
volatile int pir_flag = 0;   //pir_flag=0 - no motion detected
volatile int detected = 0; // flag for handling the alert and RFM notification


int main(void) {

//  Serial.begin(9600);
  
  //low battery detector
  DDRD|=(1<<PD2);

  //PIR input signal
  DDRD&=~(1<<PD6);       
  
  //Servo motor pwm signal
  DDRB|=(1<<PB1);       //servo output pwm
  PORTB&=~(1<<PB1);   //initialize to zero
  
  //Initializes interrupt
  PCICR|=(1<<PCIE2); //pin change interrupt for port D
  PCMSK2|=(1<<PCINT22); //D6 i/o line interrupt enabled
  sei();  //global interrupt enabled
  
  //Initializes pwm timer
  //https://embedds.com/controlling-servo-motor-with-avr/

  TCCR1A|=(1<<COM1A1)|(1<<WGM11);              //NON Inverted PWM
  TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)
  ICR1=4999;  //fPWM=50Hz 

  // Initialize the LCD
  //  lcd_init();
  
  //initialize alert
  alert_initialize();

  //local counter
  int pause_count = 0; //counter for pause time
  int motor_count = 0;  // counter for rotation
  int lights_flag = 1;
  int buzzer_flag = 1;

  //initialize to rotate the motor cw
  OCR1A = 370;

  while(1){

    //rotates the motor (4s/250ms=16)
    if(motor_count == 16 && OCR1A == 370 ){ //after 4s rotate ccw
      OCR1A = 384;
      motor_count = 0;
    }
    else if(motor_count == 16 && OCR1A ==384){ //after 4s rotate cw
      OCR1A = 370;
      motor_count = 0;
    }

    //handles alert and motor stopping - only once after an interrupt occurs
    if(pir_flag==1 && detected == 1){

      TCCR1B&=~(1<<CS11)|(1<<CS10); //reset prescalar to 0 to stop motor

      pause_count = 0;  //makes sure start counting from 0

        //turn on alert
        if (alert_flag == 0){

      if (lights_flag == 1 && buzzer_flag == 0){
        Lights_ON();
      }
      else if (lights_flag == 0 && buzzer_flag == 1){
        Buzz_ON();
      }
      else if (lights_flag == 0 && buzzer_flag == 0){ //scenario in which user only wants to be notified but no alarm 
        Alert_OFF();
      }
      else{
        Alert_ON(); //default lights and buzzer = 1
      }
      alert_flag = 1;
        }


    detected = 0;

    }


    //leave the alert ON when human/animal is present
    if( PIND & (1<<PD6) && pir_flag == 1){
      pause_count++;
    }

    else if( !(PIND & (1<<PD6)) && pir_flag == 1){ //returns to normal operation

      //clear flags
      pir_flag = 0;
        alert_flag = 0;
        rfm_flag = 0;

        //turns off alert
        Alert_OFF();

        //restart motor
        TCCR1B|=(1<<CS11)|(1<<CS10); //start motor again since pir_flag has lowered

        //reset pause count duration
        pause_count = 0;

    }

  // only increment motor count under normal operation
  if(pir_flag == 0){
    motor_count++;
  }

  _delay_ms(250);

  }
  
  return 0;
  
}


//Interrupt for motion detector
ISR(PCINT2_vect){

    if((PIND & (1<<PD6))) {  // check if the input is HIGH
     // PORTD|=(1<<PD2);  // turn LED ON for debugging
        pir_flag = 1;
        detected = 1;
    }

}
