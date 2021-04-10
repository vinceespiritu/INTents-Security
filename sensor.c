#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "alert.h"

//for RFM
#include <RFM69.h>
#include <SPI.h> 

#define NETWORKID     0   
#define MYNODEID      1   // Main rfm node ID
#define TONODEID      2   // Watch rfm node ID
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "INTentsSecurity"
#define USEACK        true // Request ACKs

RFM69 radio;

#define F_CPU 16000000
  volatile int rfm_flag = 0; //rfm_flag =1 - message sent
  volatile int alert_flag =0; //alert_flag=1 - alert just turned on
  volatile int pir_flag = 0;   //pir_flag=0 - no motion detected
  volatile bool detected = 0; // flag for handling the alert and RFM notification
  volatile string rfm_recieve; //RFM

//RFM for receiving
void rfm_receiver(){
   char message[8] = "        ";
   int x = 0;
   for (byte i = 0; i < radio.DATALEN; i++){
     message[x] = (char)radio.DATA[i];
     x++;
   }
   
  rfm_recieve = "";
    for (int y = 0; y < x; y++) {
        rfm_recieve = rfm_recieve + message[i];
    }
}

int main(void) {
  // Initialize the RFM69HCW:
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower();
  radio.encrypt(ENCRYPTKEY);

  // Messages
  char msga[5] = "alert";
  string false_alarm = "falsealarm";
  string msgln = "lighton";
  string msglf = "lightoff";
  string msgbn = "buzzon";
  string msgbf = "buzzoff";
  string msglb = "lowbat";
  string msgs = "sleep";


//  Serial.begin(9600);
  
  //low battery detector
  DDRD|=(1<<PD2);

  //PIR input signal
  DDRD&=~(1<<PD6);       
  
  //Servo motor pwm signal
  DDRB|=(1<<PB1);       //servo output pwm
  PORTB&=~(1<<PB1); 	//initialize to zero
  
  //Initializes interrupt
  PCICR|=(1<<PCIE2); //pin change interrupt for port D
  PCMSK2|=(1<<PCINT22); //D6 i/o line interrupt enabled
  sei();  //global interrupt enabled
  
  //Initializes pwm timer
  //https://embedds.com/controlling-servo-motor-with-avr/

  TCCR1A|=(1<<COM1A1)|(1<<WGM11);        			 //NON Inverted PWM
  TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)
  ICR1=4999;  //fPWM=50Hz 

  // Initialize the LCD
  //  lcd_init();
  
  //initialize alert
  alert_initialize();

  //local counter
  int pause_count = 0; //counter for pause time
  int motor_count = 0;	// counter for rotation
  int lights_flag = 1;
  int buzzer_flag = 1;

  //initialize to rotate the motor cw
  OCR1A = 370;

  while(1){

   	//rotates the motor (4s/250ms=16)
    if(motor_count == 16 && OCR1A == 370 ){ //after 4s rotate ccw
    	OCRIA = 384;
    	motor_count = 0;
    }
    else if(motor_count == 16 && OCR1A ==384){ //after 4s rotate cw
    	OCRIA = 370;
    	motor_count = 0;
    }

     //RFM Code for receiving
    if ( radio.receiveDone() ){ // Got one!

	rfm_receiver(); //will update message string rfm_receieve

		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
				alert_OFF();
		}

		//if rfm receives setting about only lights and no buzzer
		else if (rfm_receive == msgln){
			lights_flag = 1;
		}
		  
		else if (rfm_receive == msglf){
			lights_flag = 0;
		}
		
		//if rfm receives buzzer only no lights
		else if (rfm_receive == msgbn){
			buzzer_flag = 1;
		}
		  
		else if (rfm_receive == msgbf){
			buzzer_flag = 0;
		}
		
		//low battery option for rfm?
		else if (rfm_receive = msglb){
			//lowbat setting for RFM NOT DONE
		}
		
		//turn off option?
		else if (rfm_receive == "sleep"){
			//turn on some kind of atmeg sleep mode, will have to do more research later but I think there are options available
		}


    }


    //handles alert and motor stopping - only once after an interrupt occurs
    if(pir_flag==1 && detected == 1){

    	TCCR1B&=~(1<<CS11)|(1<<CS10); //reset prescalar to 0 to stop motor

    	pause_count = 0;	//makes sure start counting from 0

        //turn on alert
        if (alert_flag == 0){

			if (lights_flag == 1 && buzzer_flag == 0){
				Lights_ON();
			}
			else if (lights_flag == 0 && buzzer_flag == 1){
				Buzzer_ON();
			}
			else if (lights_flag = 0 && buzzer_flag == 0){ //scenario in which user only wants to be notified but no alarm 
				Alert_OFF();
			}
			else{
				Alert_ON(); //default lights and buzzer = 1
			}
			alert_flag = 1;
        }
		
		//sends a message to watch
		if(rfm_flag = 0){
			//send rfm message notifying user 
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
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
