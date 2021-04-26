Authors: Caroline Jin, Arunrangfa Bou, Vince Espiritu
Description: This code is created for INTents Security. It handles the motion detection, alert, and communication system of the product. Some of the codes and libraries are referenced to other sources properly. The authors make use of all the available libraries for the RFM and OLED to complete the functionality of this program.

How to compile and run the program:
+ sensor.c is the main program of the INTents Security's main system. It uses both C and C++ libraries. Arduino IDE is used to successfully compile and upload sensor.c to ATMega328P
+ oledvfinal.ino is the program of the INTents Security's watch system. This code used the SparkFun_Micro_OLED_Arduino_Library-master libary. Arduino IDE is used to successfully compile and upload oledvfinal.io to watch's processor (ATMega328)
+ Both oledvfinal.ino and sensor.c used the RFM library to handle radio communication between the two systems.

Credits: 
+ RFM69 library By Felix Rusu, LowPowerLab.com
+ SparkFun_Micro_OLED_Arduino_Library-master

