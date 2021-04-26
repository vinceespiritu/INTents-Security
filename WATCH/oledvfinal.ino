#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include <RFM69.h>
#include <SPI.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>

#define PIN_RESET 9  // Connect RST to pin 9 (req. for SPI and I2C)
#define PIN_DC    8  // Connect DC to pin 8 (required for SPI)
#define PIN_CS    7 // Connect CS to pin 7 (required for SPI)

MicroOLED oled(PIN_RESET, PIN_DC, PIN_CS);

#define NETWORKID     0   // Must be the same for all nodes
#define MYNODEID      1   // My node ID
#define TONODEID      2   // Destination node ID

#define FREQUENCY     RF69_915MHZ

#define ENCRYPT       true // Set to "true" to use encryption
#define ENCRYPTKEY    "TOPSECRETPASSWRD" // Use the same 16-byte key on all nodes

#define USEACK        false // Request ACKs or not

int count = 0;
int countsleep = 0;
int timer = 0;
int flag = 0;
int flag2 = 1;
int flag3 = 1;
int flag4 = 1;
int alertf = 0;
int lowbatf = 0;
int button = 0;
int x1 = 0;
int x2 = oled.getLCDHeight()/6;
int x3 = x2 + oled.getLCDHeight()/3;
int y1 = oled.getLCDWidth()/6;
int y2 = y1 + oled.getLCDWidth()/3;

char false_alarm[10] = {'f','a','l','s','e','a','l','a','r','m'};
char msgln[7] = {'l','i','g','h','t','o','n'};
char msglf[8] = {'l','i','g','h','t','o','f','f'};
char msgbn[6] = {'b','u','z','z','o','n'};
char msgbf[7] = {'b','u','z','z','o','f','f'};
char msgs[5] = {'s','l','e','e','p'};

volatile uint8_t portbhistory = 0xFF; 
volatile uint8_t changedbits;

RFM69 radio;

void setup()
{
  delay(100);
  Serial.begin(9600);
  Serial.print("Node ");
  Serial.print(MYNODEID,DEC);
  Serial.println(" ready");  

  DDRD &= ~(1<<PD4); //button
  PORTD |= (1<<PD4);  //pullup resistor    

  //Initializes interrupt
  PCICR|=(1<<PCIE2); //pin change interrupt for port D
  PCMSK2|=(1<<PCINT20); //D0 i/o line interrupt enabled
  sei();  //global interrupt 
  
  // Initialize the RFM69HCW:
  // radio.setCS(10);  //uncomment this if using Pro Micro
  radio.initialize(FREQUENCY, MYNODEID, NETWORKID);
  radio.setHighPower(); // Always use this for RFM69HCW

  if (ENCRYPT)
    radio.encrypt(ENCRYPTKEY);
    
  oled.clear(ALL);
  oled.display();
  oled.begin();
}

void loop()
{
  static char sendbuffer[62];
  static int sendlength = 0;
  String str;

  // SENDING
  if (Serial.available() > 0)
  {
    char input = Serial.read();

    if (input != '\r') // not a carriage return
    {
      sendbuffer[sendlength] = input;
      sendlength++;
    }

    if ((input == '\r') || (sendlength == 61)) // CR or buffer full
    {
      Serial.print("sending to node ");
      Serial.print(TONODEID, DEC);
      Serial.print(", message [");
      for (byte i = 0; i < sendlength; i++){
        Serial.print(sendbuffer[i]);
      }
      Serial.println("]");

      if (USEACK)
      {
        if (radio.sendWithRetry(TONODEID, sendbuffer, sendlength))
          Serial.println("ACK received!");
        else
          Serial.println("no ACK received");
      }

      // If you don't need acknowledgements, just use send():

      else // don't use ACK
      {
        radio.send(TONODEID, sendbuffer, sendlength);
      }

      sendlength = 0; // reset the packet
    }
  }

  // RECEIVING

  if (radio.receiveDone()) // Got one!
  {
    // Print out the information:

    Serial.print("received from node ");
    Serial.print(radio.SENDERID, DEC);
    Serial.print(", message [");

    for (byte i = 0; i < radio.DATALEN; i++){
      Serial.print((char)radio.DATA[i]);
      str += (char)radio.DATA[i];
        }
      Serial.print("HELLO");  

    Serial.print("], RSSI ");
    Serial.println(radio.RSSI);
    Serial.print("string: ");
    Serial.print(str);

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.println("ACK sent");
    }
  }

    if((str == "alert") && (alertf == 0)){
      alertf = 1;
      oled.clear(ALL);
      //oled.circle(x2,y1,x2,WHITE,NORM);
      //oled.circle(x3+x2-1,y1,x2,WHITE,NORM);
      oled.rectFill(0,0,oled.getLCDWidth(),oled.getLCDHeight(),WHITE,NORM);

      oled.setFontType(1); 
      oled.setColor(BLACK); 
      oled.setCursor(x2-2, 3);
      oled.print("OBJECT");
      oled.setCursor(x2-2, 16);
      oled.print("AROUND");
      oled.rectFill(0,30,64,19,BLACK,NORM);
      oled.setColor(WHITE);
      oled.setCursor(x2+6, 33);
      oled.print("OKAY");
      oled.display(); 
      
      delay(150);
    }

    else if((str == "lowbat") && (lowbatf == 0)){
      lowbatf = 1;
      oled.clear(PAGE);
      //oled.circle(x2,y1,x2,WHITE,NORM);
      //oled.circle(x3+x2-1,y1,x2,WHITE,NORM);
      oled.rectFill(0,0,oled.getLCDWidth(),oled.getLCDHeight(),WHITE,NORM);

      oled.setFontType(1); 
      oled.setColor(BLACK); 
      oled.setCursor(x2+10, 3);
      oled.print("LOW");
      oled.setCursor(x2-2, 16);
      oled.print("CHARGE");
      oled.rectFill(0,30,64,19,BLACK,NORM);
      oled.setColor(WHITE);
      oled.setCursor(x2+6, 33);
      oled.print("OKAY");
      oled.display();  
      delay(150);
    }

    if ((button == 1)&&(alertf == 1)){
      button = 0;
      count = 0;
      alertf = 0;
      radio.send(TONODEID, false_alarm, 10);
      oled.clear(ALL);
      count = 0;
      //oled.circle(x2,y1,x2,WHITE,NORM);
      //oled.circle(x3+x2-1,y1,x2,WHITE,NORM);
      oled.rectFill(0,0,oled.getLCDWidth(),oled.getLCDHeight(),WHITE,NORM);

      oled.setFontType(1); 
      oled.setColor(BLACK); 
      oled.setCursor(x2-2, 3);
      oled.print("OBJECT");
      oled.setCursor(x2-2, 16);
      oled.print("AROUND");
      oled.rect(0,30,64,19,BLACK,NORM);
      oled.setCursor(x2+6, 33);
      oled.print("OKAY");
      oled.display(); 
      oled.setColor(WHITE);
      delay(150);     
    }

    if ((button == 1)&&(lowbatf == 1)){
      button = 0;
      count = 0;
      lowbatf = 0;
      oled.clear(ALL);
      count = 0;
      //oled.circle(x2,y1,x2,WHITE,NORM);
      //oled.circle(x3+x2-1,y1,x2,WHITE,NORM);
      oled.rectFill(0,0,oled.getLCDWidth(),oled.getLCDHeight(),WHITE,NORM);

      oled.setFontType(1); 
      oled.setColor(BLACK); 
      oled.setCursor(x2+10, 3);
      oled.print("LOW");
      oled.setCursor(x2-2, 16);
      oled.print("CHARGE");
      oled.rect(0,30,64,19,BLACK,NORM);
      oled.setCursor(x2+6, 33);
      oled.print("OKAY");
      oled.display(); 
      oled.setColor(WHITE);
      delay(150);     
    }

    if ((button == 1) && (count == 0)&&(flag == 0)){ //home
      button = 0;
      count = count +1;

          oled.clear(PAGE);

          if(flag2 == 1){
          oled.circle(x2+2,y1+6,x2+2,WHITE,NORM);
          print_l();
          }
          else{
          oled.circleFill(x2+2,y1+6,x2+2,WHITE,NORM);
          oled.setColor(BLACK); 
          print_l();
          oled.setColor(WHITE); 
          }

          if(flag3 == 1){
          oled.circle(x3+x2-1,y1+6,x2+2,WHITE,NORM);
          print_b();
          }
          else{
          oled.circleFill(x3+x2-1,y1+6,x2+2,WHITE,NORM);
          oled.setColor(BLACK); 
          print_b();
          oled.setColor(WHITE); 
          }
      
          if(flag4 == 1){
          oled.circle(2*x3+x2-4,y1+6,x2+2,WHITE,NORM);
          print_s();
          }
          else{
          oled.circleFill(2*x3+x2-4,y1+6,x2+2,WHITE,NORM);
          oled.setColor(BLACK); 
          print_s();
          oled.setColor(WHITE); 
          }
          
          oled.setFontType(1);  
          oled.setCursor(x2+6, y2);
          oled.print("HOME");
          oled.rectFill(0,y2-2,64,1);
          oled.rectFill(0,y2+12,64,1);
          oled.display(); 
          delay(150); 
      }

      else if ((button == 1) && (count == 1)&&(flag == 1)){
        count = 0;
        flag= 0;
      //Serial.print("here");
      }

      else if ((button == 1) && (count == 1)&&(flag == 0)){ //LIGHTS
        button = 0;
        count = count +1;
//        Serial.print(count);
//        Serial.print("\t");
//        Serial.print(flag);
//        Serial.print("\n");

          oled.clear(PAGE);
          oled.circle(x2+2,y1+6,x2+2,WHITE,NORM);
          //oled.circle(x3+x2-1,y1,x2,WHITE,NORM);
          //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
          print_l();
          print_b();
          print_s();
          print_lights();
          oled.display(); 
          delay(150); 

       }

       else if ((button == 1) && (count == 2) &&(flag == 1)) {//LIGHTS OFF/ON
        button = 0;
//        Serial.print(count);
//        Serial.print("\t");
//        Serial.print(flag);
//        Serial.print("\n");
        count = 0;
        flag = 0;
        
        if(flag2 == 0){
        flag2 = 1;
        oled.clear(PAGE);
        //oled.circle(x2,y1,x2,WHITE,NORM);
        oled.circle(x3+x2-1,y1+6,x2+2,WHITE,NORM);
        oled.setFontType(1);  
        oled.setCursor(x3+x2-4,y1);
        oled.print("L");
        //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
        print_off();
        oled.display();
        radio.send(TONODEID, msglf, 8);
        delay(150);
        }

        else{
        flag2 = 0;
        oled.clear(PAGE);
        //oled.circle(x2,y1,x2,WHITE,NORM);
        oled.circleFill(x3+x2-1,y1+6,x2+2,WHITE,NORM);
        oled.setFontType(1); 
        oled.setCursor(x3+x2-4,y1);
        oled.setColor(BLACK); 
        oled.print("L");
        oled.setColor(WHITE); 
        //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
        print_on();
        oled.display();
        radio.send(TONODEID, msgln, 7);
        //
        delay(150);
        
        }
       }

       else if ((button == 1) && (count == 2) &&(flag == 0)){ //BUZZER
     //while((PIND & (1<<PD0)) == 0){  }
      button = 0;
//      Serial.print(count);
//        Serial.print("\t");
//        Serial.print(flag);
//        Serial.print("\n");
          count = count +1;

          oled.clear(PAGE);
          //oled.circle(x2,y1,x2,WHITE,NORM);
          oled.circle(x3+x2-1,y1+6,x2+2,WHITE,NORM);
          //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
          print_l();
          print_b();
          print_s();
          print_buzz();
          oled.display();
          delay(150); 
          
    }

    else if ((button == 1) && (count == 3)&&(flag == 1)){ //BUZZER OFF/ON
     //while((PIND & (1<<PD0)) == 0){  }
        button = 0;
        flag = 0;
//        Serial.print(count);
//        Serial.print("\t");
//        Serial.print(flag);
//        Serial.print("\n");
        count = 0;
        //flag = 0;
        if(flag3 == 0){
        flag3 = 1;
        oled.clear(PAGE);
        //oled.circle(x2,y1,x2,WHITE,NORM);
        oled.circle(x3+x2-1,y1+6,x2+2,WHITE,NORM);
        oled.setFontType(1);  
        oled.setCursor(x3+x2-4,y1);
        oled.print("B");
        //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
        print_off();
        oled.display();
        radio.send(TONODEID, msgbf, 7);
        delay(150); 
        }
        else{
        flag3 = 0;
        oled.clear(PAGE);
        //oled.circle(x2,y1,x2,WHITE,NORM);
        oled.circleFill(x3+x2-1,y1+6,x2+2,WHITE,NORM);
        oled.setFontType(1); 
        oled.setColor(BLACK); 
        oled.setCursor(x3+x2-4,y1);
        oled.print("B");
        oled.setColor(WHITE);
        //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
        print_on();
        oled.display();
        radio.send(TONODEID, msgbn, 6);
        //
        delay(150);
        }
        
       }
    
    else if ((button == 1) && (count == 3)&&(flag == 0)){ //SLEEP
     //while((PIND & (1<<PD0)) == 0){  }
      button = 0;
//      Serial.print(count);
//        Serial.print("\t");
//        Serial.print(flag);
//        Serial.print("\n");
          count = count+1;
          //countsleep = 1;

          oled.clear(PAGE);
          //oled.circle(x2,y1,x2,WHITE,NORM);
          //oled.circle(x3+x2-1,y1+6,x2+2,WHITE,NORM);
          oled.circle(2*x3+x2-4,y1+6,x2+2,WHITE,NORM);
          print_l();
          print_b();
          print_s();
          print_sleep();
          oled.display();
          delay(150);
         
      }

      
     else if ((button == 1) && (count == 4)&&(flag==1)){ //SLEEP OFF/ON
     //while((PIND & (1<<PD0)) == 0){  }
        button = 0;
//        Serial.print("3");
//        Serial.print("\t");
//        Serial.print(flag);
//        Serial.print("\n");
        count = 0;
        countsleep = 0;
        flag = 0;
        if(flag4==0){
        flag4 = 1;
        oled.clear(PAGE);
        //oled.circle(x2,y1,x2,WHITE,NORM);
        oled.circle(x3+x2-1,y1+6,x2+2,WHITE,NORM);
        oled.setFontType(1);  
        oled.setCursor(x3+x2-4,y1);
        oled.print("S");
        //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
        print_off();
        oled.display();
        delay(150); 
        }
        else{
        flag4 = 0;
        oled.clear(PAGE);
        //oled.circle(x2,y1,x2,WHITE,NORM);
        oled.circleFill(x3+x2-1,y1+6,x2+2,WHITE,NORM);
        oled.setFontType(1);  
        oled.setColor(BLACK);
        oled.setCursor(x3+x2-4,y1);  
        oled.print("S");
        oled.setColor(WHITE);
        //oled.circle(2*x3+x2-1,y1,x2,WHITE,NORM);
        print_on();
        oled.display();
        delay(150);
        }
       }
       
       else if ((button == 1) && (count == 4)&&(flag==0)){
        count = 0;
       }
  
}

void print_off(){
     oled.setFontType(1);  
     oled.setCursor(x2*2+3, y2);
     oled.print("OFF");
}

void print_on(){
     oled.setFontType(1);  
     oled.setCursor(x2*2+8, y2);
     oled.print("ON");
}

void print_lights(){
    oled.setFontType(1);  
    oled.setCursor(x2-1, y2);
    oled.print("LIGHTS"); // Print a const string
    //oled.print(analogRead(0));  // Print an integer
    //oled.print(42.07);  // Print a float
    //oled.display(); // Draw to the screen
}

void print_buzz(){
    oled.setFontType(1);  
    oled.setCursor(x2-1, y2);
    oled.print("BUZZER"); // Print a const string
    //oled.print(analogRead(0));  // Print an integer
    //oled.print(42.07);  // Print a float
    //oled.display(); // Draw to the screen
}

void print_sleep(){
    oled.setFontType(1);  
    oled.setCursor(x2+1, y2);
    oled.print("SLEEP"); // Print a const string
    //oled.print(analogRead(0));  // Print an integer
    //oled.print(42.07);  // Print a float
    //oled.display(); // Draw to the screen
}

void print_l(){
    oled.setFontType(1);  
    oled.setCursor(x2-1,y1);
    oled.print("L"); // Print a const string
}

void print_b(){
    oled.setFontType(1);  
    oled.setCursor(x3+x2-4,y1);
    oled.print("B"); // Print a const string
}

void print_s(){
    oled.setFontType(1);  
    oled.setCursor(2*x3+x2-7,y1);
    oled.print("S"); // Print a const string
}

//Interrupt for button
ISR(PCINT2_vect){

  changedbits = (PIND ^ portbhistory);
  portbhistory = PIND;
  if((changedbits && (1 << PD4)) == 1)
    {
    /* PCINT16 changed */
    timer = 0;
    button = 1; 
    while((PIND & (1<<PD4)) == 0){
    timer = timer +1;
    //flag = 0;
    delay(1);
    //Serial.print(timer);
    //Serial.print(timer);
    //Serial.print("\n");
    if (timer > 1000){
      flag = 1;
      delay(1);
      Serial.print("pressed");
      Serial.print("\t");
      Serial.print(timer);
      Serial.print("\t");
      Serial.print(flag);
      Serial.print("\n");
      break;
    }
    }
    }
}
