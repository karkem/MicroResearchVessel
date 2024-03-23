//This is the sketsch, that is running on the esp32 on the Boat. (WROOM 32)
//the components are:
//The ESP32
//GPS module: 1,3 (rx,tx)
//Sonar Pin: 16,17(rx,tx) + 5V external power
//2x ESC with DC Motor - Pin:32,26,gnd + 4V external power
//Stepper 12, 14, 27, 26
//Contact to calibrate stepper
//nrf24 Pin: 25,33, + 3V external power + 23,19,18
//Thermometer Pin 4
//SD card reader PIN (SPI like nrf24 but with one ce pin)

# include <ESP32Servo.h> //for ESCs
# include <SoftwareSerial.h> //for Sonar
# include <SPI.h> //For nrf24
# include <nRF24L01.h> //For nrf24
# include <RF24.h> //For nrf24
# include <TinyStepper_28BYJ_48.h> //for stepper
# include <TinyStepper.h> //alternative to above!
//# include "neo6mGPS.h" //For the GPS Module
# include <OneWire.h> //for temp sensor 
# include <DallasTemperature.h> //for temp sensor


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
const byte addresses[][6] = {"0"};

struct package {
  int id = 1;
  float temperature = 0;
  int depth = 0;
  int engineL = 1500;
  int engineR = 1500;
  char  text[300] = "Text to be transmit";
};

typedef struct package Package;
Package dataRecieve;
Package dataTransmit;

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
//neo6mGPS GPS;

//variables for Temperature sensor
// GPIO where the DS18B20 is connected to
const int oneWireBus = 4; 
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

//Variables for stepper calibration
const int cablePin = 35;

void setup() {
  Serial.begin(115200);  // Starte serial communication
  
  //Set up ESCs for DC Motors
  escL.attach(32); //The Pin the ESC is connected to
  escR.attach(25); //The Pin the ESC is connected to
  escL.writeMicroseconds(1000); //The arming signal
  escR.writeMicroseconds(1000); //The arming signal

  //Set up sonar
  //SonarSerial.begin(115200);

  //Set up for nrf24
  radio.begin();
  radio.setChannel(115); 
  //radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN); //MAX?
  radio.setDataRate(RF24_250KBPS); //is this important for range?
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();

  //Setup for stepper
  stepper.Enable();

  //Setup GPS
  //GPS.begin(Serial1);

  //set up temp sensor
  sensors.begin();

  //set up stepper calibrater
  pinMode(cablePin, INPUT);

  delay(2000);

}




void loop() {
  read_radio();

  escL.writeMicroseconds(dataRecieve.engineL);

  controle_stepper();

  read_sonar();

  read_temp();

  Serial.print("calibrate stepper: ");
  Serial.println(calibrate_stepper());
  //read_GPS();

  //delay(2000); //For some reason the sonar has a problem with the delay
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
void read_temp(){
  // Call sensors.requestTemperatures() to issue a global temperature and Requests to all devices on the bus
  sensors.requestTemperatures(); 
  // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
  dataTransmit.temperature = sensors.getTempCByIndex(0);
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
        dataTransmit.depth = Distance;
      }
    }
  }
}

//void read_GPS(){
  //  if(GPS.available())
  //{
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
  //}
//}

void controle_stepper(){
  if (calibrate_stepper()){
    location = 0;
  }
  destination = atoi(dataRecieve.text);
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

bool calibrate_stepper() {
  // Read the state of the cablePin
  int cableState = digitalRead(cablePin);
  if (cableState == HIGH) {
    return true;
  } else {
    return false;
  }
}


void read_radio(){
    //Listen
    //if (radio.available()) {
    //radio.read(&text, sizeof(text));
    //Serial.println("recieved:");
    //Serial.println(text);
    //destination = atoi(text);
    //Listen
    if ( radio.available()) {
      while (radio.available()){ //While might be dangerous!!!
        radio.read( &dataRecieve, sizeof(dataRecieve) );
      }
      Serial.println("Recieve: ");
      Serial.print("Package:");
      Serial.print(dataRecieve.id);
      Serial.print("\n");
      Serial.println(dataRecieve.temperature);
      Serial.println(dataRecieve.text);
      Serial.print("\n");
    }
    delay(200);

  radio.stopListening();
  dataTransmit.id = dataTransmit.id + 1;
  dataTransmit.temperature = dataTransmit.temperature+0.1;
  Serial.println("Transmit: ");
  Serial.print("Package:");
  Serial.print(dataTransmit.id);
  Serial.print("\n");
  Serial.println(dataTransmit.temperature);
  Serial.println(dataTransmit.text);
  Serial.print("\n");
  radio.openWritingPipe(addresses[0]);
  radio.write(&dataTransmit, sizeof(dataTransmit));
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();
}