//This is the sketsch, that is running on the esp32 on the Boat.
//the components are:
//The ESP32
//GPS module
//Sonar Pin: 16,17(rx,tx) + 5V external power
//2x ESC with DC Motor - Pin:25,26,gnd + 4V external power
//Stepper
//Contact to calibrate stepper
//nrf24 Pin: 14,27 + 3V external power
//Thermometer
//SD card reader 

# include <ESP32Servo.h> //for ESCs
# include <SoftwareSerial.h> //for Sonar
# include <SPI.h> //For nrf24
# include <nRF24L01.h> //For nrf24
# include <RF24.h> //For nrf24
# include <TinyStepper_28BYJ_48.h> //for stepper


//variables for sonar
unsigned char buffer_RTT[4] = {0};
uint8_t CS;
#define COM 0x55
SoftwareSerial SonarSerial(16, 17); //RX (white) TX (yellow)

//variables for ESCs
Servo escL; //Left motor
Servo escR; //Right Motor

//variables for nrf24
RF24 radio(14, 27); // CE, CSN
const byte address[6] = "00001";

//variables for stepper
const int MOTOR_IN1_PIN = 11;
const int MOTOR_IN2_PIN = 10;
const int MOTOR_IN3_PIN = 6;
const int MOTOR_IN4_PIN = 5;

const int STEPS_PER_REVOLUTION = 2048;
TinyStepper_28BYJ_48 stepper; // create the stepper motor object


void setup() {
  Serial.begin(115200);  // Starte serial communication
  
  //Set up ESCs for DC Motors
  escL.attach(26); //The Pin the ESC is connected to
  escR.attach(25); //The Pin the ESC is connected to
  escL.writeMicroseconds(1000); //The arming signal
  escR.writeMicroseconds(1000); //The arming signal

  //Set up sonar
  SonarSerial.begin(115200);

  //Set up for nrf24
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();


  delay(2000);

}

void loop() {
  //read radio command
  //if there is a manual mode flag:
    //switch to manual mode:
    //direct controll off the two engins and the winch

  //If there is a new mission with waypoints:
    //for every waypoint:
      //Go to waypoint 
      //If waypoint reached:
        //hold position
        //lower winch to depth 
        //get winch back up 
        //write temperature data and position onto the sd card
        //when winch is back up: go to next waypoint 
      


  

}
