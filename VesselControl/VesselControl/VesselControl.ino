//This is the sketsch, that is running on the esp32 on the Boat. (WROOM 32)
//the components are:
//The ESP32
//GPS module: 1,3 (rx,tx)
//Sonar Pin: 16,17(rx,tx) + 5V external power
//2x ESC with DC Motor - Pin:25,26,gnd + 4V external power
//Stepper 12, 14, 27, 26
//Contact to calibrate stepper
//nrf24 Pin: 25,33, + 3V external power + 23,19,18
//Thermometer
//SD card reader 

# include <ESP32Servo.h> //for ESCs
# include <SoftwareSerial.h> //for Sonar
# include <SPI.h> //For nrf24
# include <nRF24L01.h> //For nrf24
# include <RF24.h> //For nrf24
# include <TinyStepper_28BYJ_48.h> //for stepper
# include <TinyStepper.h> //alternative to above!
# include "neo6mGPS.h" //For the GPS Module



//variables for sonar
unsigned char buffer_RTT[4] = {0};
uint8_t CS;
#define COM 0x55
SoftwareSerial SonarSerial(16, 17); //RX (white) TX (yellow)

//variables for ESCs
Servo escL; //Left motor
Servo escR; //Right Motor

//variables for nrf24
char text[32] = "";
RF24 radio(25, 33); // CE, CSN
const byte address[6] = "00001";

//variables for stepper
const int MOTOR_IN1_PIN = 11;
const int MOTOR_IN2_PIN = 10;
const int MOTOR_IN3_PIN = 6;
const int MOTOR_IN4_PIN = 5;

const int STEPS_PER_REVOLUTION = 2048;
//TinyStepper_28BYJ_48 stepper; // create the stepper motor object

//OR
int location = 0;
int go = 0;
int destination = 0;

// Define Arduino Pin Outputs
#define IN1 12
#define IN2 14
#define IN3 27
#define IN4 26
#define HALFSTEPS 4096  // Number of half-steps for a full rotation

// Init the TinyStepper Class
TinyStepper stepper(HALFSTEPS, IN1, IN2, IN3, IN4);

//variables for GPS
neo6mGPS GPS;



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
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  //Setup for stepper
  stepper.Enable();

  //Setup GPS
  GPS.begin(Serial1);


  delay(2000);

}




void loop() {
  //read radio command
  read_radio();

  controle_stepper();

  read_sonar();

  read_GPS();

  delay(2000);
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

void read_sonar(){
  SonarSerial.write(COM);
  delay(100);
  if(SonarSerial.available() > 0){
    delay(4);
    if(SonarSerial.read() == 0xff){    
      buffer_RTT[0] = 0xff;
      for (int i=1; i<4; i++){
        buffer_RTT[i] = SonarSerial.read();   
      }
      CS = buffer_RTT[0] + buffer_RTT[1]+ buffer_RTT[2];  
      if(buffer_RTT[3] == CS) {
        int Distance = (buffer_RTT[1] << 8) + buffer_RTT[2];
        Serial.print("Distance:");
        Serial.print(Distance);
        Serial.println("mm");
      }
    }
  }
}

void read_GPS(){
    if(GPS.available())
  {
    //Serial.print(GPS.utc_year);  Serial.print(" | ");
    //erial.print(GPS.utc_month); Serial.print(" | ");
    //Serial.print(GPS.utc_day);   Serial.print(" | ");
    //Serial.print(GPS.utc_hour);  Serial.print(" | ");
    //Serial.print(GPS.utc_min);   Serial.print(" | ");
    //Serial.print(GPS.utc_sec);   Serial.print(" | ");
    //Serial.print(GPS.lat_dd);    Serial.print(" | ");
    //Serial.print(GPS.lon_dd);    Serial.print(" | ");
    //Serial.print(GPS.sog_knots); Serial.print(" | ");
    //Serial.print(GPS.cog_true);  Serial.print(" | ");
    //Serial.print(GPS.navStatus);
    //Serial.println();
  }
}

void controle_stepper(){
  go = destination - location;
  Serial.print(" go: ");
  Serial.print(go);
  Serial.print(" destination: ");
  Serial.print(destination);
  Serial.print(" location: ");
  Serial.println(location);
  stepper.Move(go);
  location = destination;
}

void read_radio(){
    if (radio.available()) {
    radio.read(&text, sizeof(text));
    Serial.println("recieved:");
    Serial.println(text);
    destination = atoi(text);
  }
}