//This is the sketch that will run on the base station.
//Its only purpose is to comunicate with the boat over the nrf24
//and forward the comunication via Bluetooth/Wifi/mqtt over Internet to a near by tablett 
//on which an app will be used to controll the boat

# include <BluetoothSerial.h>
# include <SPI.h>
//# include <nRF24L01.h>
# include <RF24.h>

//Variables for nRF24
RF24 radio(14, 27); // CE, CSN
byte addresses[][6] = {"0"};

struct package {
  int id = 1;
  float temperature = 18.3;
  int depth = 0;
  int engineL = 1500;
  int engineR = 1500;
  char  text[300] = "Text to be transmit";
};

typedef struct package Package;
Package dataRecieve;
Package dataTransmit;

// Variables for Bluetooth
BluetoothSerial SerialBT; //Create Bluetooth object

int variableA = 0; // Initialize variables for the bluetooth input
int variableB = 0;
int variableC = 0;


void setup() {
  //Set up Bluetooth
  Serial.begin(115200); //Start serial monitor
  delay(1000);
  SerialBT.begin("Arduino_bluetooth"); //Start bluetooth client

  //Set up nrf24 Radio
  radio.begin();  
  radio.setChannel(115); 
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate( RF24_250KBPS );
  
  radio.openReadingPipe(1, addresses[0]);
  radio.startListening();

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
    //char text[300];
  sprintf(dataTransmit.text, "%d", variableA);
  dataTransmit.engineL = variableB;

  //dataTransmit.text = text
  //Serial.println(text);
  //radio.write(&text, sizeof(text));
  //Serial.println(variableA);
  //radio.write(&variableA, sizeof(variableA));
  //delay(1000);
  }
  //Write bluetooth data over radio to the MRV
  //number += 1;
  radio_listen_and_transmit();

  SerialBT.println(dataRecieve.depth);

  delay(200);
}

void radio_listen_and_transmit(){
  if ( radio.available()) {
    while (radio.available()){
      radio.read( &dataRecieve, sizeof(dataRecieve) );
    }
    Serial.println("Recieve: ");
    Serial.print("Package:");
    Serial.print(dataRecieve.id);
    Serial.print("\n");
    Serial.println(dataRecieve.temperature);
    Serial.println(dataRecieve.depth);
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
