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
  volatile int rfm_flag = 0; //no message sent
  volatile int alert_flag =0; //alert 
  int lights_flag = 1;
  int buzzer_flag = 1;
  volatile int pir_flag = 0;     //no motion detected
  volatile int motion = 0;	
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

int main(void)
{
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
  
  DDRD|=(1<<PD2); //motion led
  PORTD&=~(1<<PD2);
  //DDRB|=(1<<PD2); //alert led
  //PORTB&=~(1<<PD2);
  //DDRD|=(1<<PD3); //rfm led
  //PORTD&=~(1<<PD3); 
  
  DDRD&=~(1<<PD6);        //PIR input signal
  //PORTD|=(1<<PD6);    //enable pullup resistors
  
  DDRB|=(1<<PB1);       //servo output pwm
  PORTB&=~(1<<PB1); //initialize to zero
  
  PCICR|=(1<<PCIE2); //pin change interrupt for port D
  PCMSK2|=(1<<PCINT22); //D6 i/o line interrupt enabled
  sei();  //global interrupt enabled
  
  //pwm timer
  //https://embedds.com/controlling-servo-motor-with-avr/
  TCCR1A|=(1<<COM1A1)|(1<<WGM11);        //NON Inverted PWM
    TCCR1B|=(1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10); //PRESCALER=64 MODE 14(FAST PWM)
    ICR1=4999;  //fPWM=50Hz 

  // Initialize the LCD
  //  lcd_init();
  
  //initialize alert
  alert_initialize();
  
  while(1){
    //RFM Code for receiving
    if (radio.receiveDone()){ // Got one!
	rfm_receiver() //will update message string rfm_receieve    
    }
    //servo code 
    if (pir_flag == 0){
      OCR1A=370;  //turn clockwise 325
      while (pir_flag==1){  //all these if statements are so it doesn't have to finish the entire clockwise/counterclockwise sequence (which takes 8 seconds if the interrupt occurs at the very beginning) before checking the flags; instead checks for pir_flag every 1 second
        TCCR1B&=~(1<<CS11)|(1<<CS10); //reset prescalar to 0 to stop motor
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user 
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}*/

        
        _delay_ms(1000);    
      }
	  TCCR1B|=(1<<CS11)|(1<<CS10); //start motor again since pir_flag has lowered
      _delay_ms(1000);
      
      while (pir_flag==1){         
        TCCR1B&=~(1<<CS11)|(1<<CS10); 
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}
	      
        _delay_ms(1000);    
      }
	  TCCR1B|=(1<<CS11)|(1<<CS10);
      _delay_ms(1000);
      
      while (pir_flag==1){     
        TCCR1B&=~(1<<CS11)|(1<<CS10);
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}
	      
        _delay_ms(1000);    
      }
      TCCR1B|=(1<<CS11)|(1<<CS10);
      _delay_ms(1000);

      while (pir_flag==1){     
        TCCR1B&=~(1<<CS11)|(1<<CS10);
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}
	      
        _delay_ms(1000);    
      }
      TCCR1B|=(1<<CS11)|(1<<CS10);
      _delay_ms(1000);
      
      
    //  OCR1A=376; //stop 
    //  _delay_ms(5000);
      OCR1A=384; //turn counter clockwise 425
      while (pir_flag==1){      
        TCCR1B&=~(1<<CS11)|(1<<CS10);
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}
	      
        _delay_ms(1000);    
      }
	  TCCR1B|=(1<<CS11)|(1<<CS10);
      _delay_ms(1000);
      
      while (pir_flag==1){         
        TCCR1B&=~(1<<CS11)|(1<<CS10);
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}
	      
        _delay_ms(1000);    
      }
	  TCCR1B|=(1<<CS11)|(1<<CS10);
      _delay_ms(1000);
      
      while (pir_flag==1){     
        TCCR1B&=~(1<<CS11)|(1<<CS10);
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}
	      
        _delay_ms(1000);    
      }
	  TCCR1B|=(1<<CS11)|(1<<CS10);
      _delay_ms(1000);

      while (pir_flag==1){     
        TCCR1B&=~(1<<CS11)|(1<<CS10);
        if (alert_flag == 0){
            alert_flag = 1;
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
		
		if(rfm_flag = 0){
			//send rfm message notifying user
			radio.sendWithRetry(TONODEID, msga, 5);
			//rfm_send("alert");
			rfm_flag = 1; //or rfm_flag = rfm_confirm but we want to make sure not to spam the user with messages if they don't respond
		}
		
		//check if the user wants to turn off the alarm
		if (rfm_receive == false_alarm){
			alert_OFF();
		}
	      
        _delay_ms(1000);    
      }
	  TCCR1B|=(1<<CS11)|(1<<CS10);
      _delay_ms(1000);
    //  OCR1A=375; //stop 
    //  _delay_ms(5000);
    }

	//if rfm receives setting about only lights and no buzzer
	if (rfm_receive == msgln){
		lights_flag = 1;
	}
	  
	if (rfm_receive == msglf){
		lights_flag = 0;
	}
	
	//if rfm receives buzzer only no lights
	if (rfm_receive == msgbn){
		buzzer_flag = 1;
	}
	  
	if (rfm_receive == msgbf){
		buzzer_flag = 0;
	}
	
	
	//low battery option for rfm?
	if (rfm_receive = msglb){
		//lowbat setting for RFM NOT DONE
	}
	
	
	//turn off option?
	if (rfm_receive == "sleep"){
		//turn on some kind of atmeg sleep mode, will have to do more research later but I think there are options available
	}
	
	
  }
  
  return 0;
  
}

ISR(PCINT2_vect){
  //PORTD|=(1<<PD2);
  //Serial.println("INTERRUPT");
  //Serial.println(PIND & (1<<PD6));
  //motion = (PIND & (1<<PD6)) ;  // read input value
    if((PIND & (1<<PD6))) {            // check if the input is HIGH
      PORTD|=(1<<PD2);  // turn LED ON
      if (pir_flag == 0) {
        // we have just turned on
       // Serial.println("Motion detected!");
        // We only want to print on the output change, not state
        pir_flag= 1;
      }
      } 
    else {
   //   Serial.println("INTERRUPT2");
      _delay_ms(500);
      PORTD&=~(1<<PD2); // turn LED OFF
      if (pir_flag == 1){
        // we have just turned of
        //Serial.println("Motion ended!");
        // We only want to print on the output change, not state
        pir_flag = 0;
        
        alert_flag = 0;
        Alert_OFF();
        
        rfm_flag = 0;
      }
    }

}
