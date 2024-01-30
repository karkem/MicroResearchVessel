// Sensor is powert with 5v over an external powersupply
// The RX Cable from the Sensor (white) is put in pin 16 at ESP 
// The TX Cable from the Sensor (Yellow) is put in pin 17
// RX and TX must not be confused 
// The Sensor only gives data other than 0 when it is put in water!


#include <SoftwareSerial.h>
unsigned char buffer_RTT[4] = {0};
uint8_t CS;
#define COM 0x55
SoftwareSerial mySerial(16, 17); 
void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);
}
void loop() {
  mySerial.write(COM);
  delay(100);
  if(mySerial.available() > 0){
    delay(4);
    if(mySerial.read() == 0xff){    
      buffer_RTT[0] = 0xff;
      for (int i=1; i<4; i++){
        buffer_RTT[i] = mySerial.read();   
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