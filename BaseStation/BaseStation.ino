//This is the sketch that will run on the base station.
//Its only purpose is to comunicate with the boat over the nrf24
//and forward the comunication via Bluetooth/Wifi/mqtt over Internet to a near by tablett 
//on which an app will be used to controll the boat

# include <BluetoothSerial.h>
# include <SPI.h>
# include <nRF24L01.h>
# include <RF24.h>

//Variables for nRF24
RF24 radio(14, 27); // CE, CSN
const byte address[6] = "00001";

// Variables for Bluetooth
BluetoothSerial SerialBT; //Create Bluetooth object

int variableA = 0; // Initialize variables for the bluetooth input
int variableB = 0;
int variableC = 0;


void setup() {
  //Set up Bluetooth
  Serial.begin(9600); //Start serial monitor
  SerialBT.begin("Arduino_bluetooth"); //Start bluetooth client

  //Set up nrf24 Radio
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

}

int number = 0; //only for testuing the nrf24!
void loop() {

  //Read Bluetooth data 
  if (SerialBT.available()) {
    String receivedData = SerialBT.readStringUntil('br');
    if (receivedData.charAt(0) == 'a') {
      // If the string starts with 'a', store it in variableA
      variableA = receivedData.substring(1).toInt();
      Serial.print("Received 'a': ");
      Serial.println(variableA);
    } else if (receivedData.charAt(0) == 'b') {
      // If the string starts with 'b', store it in variableB
      variableB = receivedData.substring(1).toInt();
      Serial.print("Received 'b': ");
      Serial.println(variableB);
    } else if (receivedData.charAt(0) == 'c') {
      // If the string starts with 'c', store it in variableB
      variableC = receivedData.substring(1).toInt();
      Serial.print("Received 'c': ");
      Serial.println(variableC);
    }
      char text[9];
  sprintf(text, "%d", variableA);
  Serial.println(text);
  radio.write(&text, sizeof(text));
  //Serial.println(variableA);
  //radio.write(&variableA, sizeof(variableA));
  delay(1000);
  }

  //Write bluetooth data over radio to the MRV


  //number += 1;


}
