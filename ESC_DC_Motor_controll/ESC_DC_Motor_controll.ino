//Wiring:
//The ESC is connected to at least 4V input over the female battery plug
//The male plug is connacted to the dc motor - The motor is made for 3 - 6V (6V is decreasing the lifetime drastically, I hope 4V is okay)
//the black  wire of the three wire plug is connected to ground of the Arduino 
//the white wire to Pin26 (or any other PWM Pin) 
//the red wire can be used to power the arduino (?)

//Explanation
//The ESC will only work after it got an "arming signal" of 1000 /(1500?) PWM - a repeating beeping sound is "I am okay, but need the arming signal"
//1500 PWM is the neutral position
//above is one direction - what is max?
//below is the other direction -what is max speed?

# include <ESP32Servo.h>

Servo esc;

void setup() {
  Serial.begin(115200);  // Starte serial communication
  esc.attach(26); //The Pin the ESC is connected to
  esc.writeMicroseconds(1000); //The arming signal
  delay(2000);
}

void loop() {
  // Überprüfen, ob Daten im Serial Monitor verfügbar sind
  if (Serial.available() > 0) {
    // Lesen Sie den PWM-Wert von der seriellen Schnittstelle
    int pwmValue = Serial.parseInt();

    // Setze den PWM-Wert
    esc.writeMicroseconds(pwmValue);

    // Zeige den aktuellen PWM-Wert im Serial Monitor an
    Serial.print("PWM-Wert gesetzt auf: ");
    Serial.println(pwmValue);

    // Warte, bis ein neuer Wert eingegeben wird
    while (Serial.available() > 0) {
      Serial.read();  // Leere den Serial Buffer
    }
  }

  delay(100);  // Kleine Verzögerung, um die serielle Schnittstelle nicht zu überlasten
}
