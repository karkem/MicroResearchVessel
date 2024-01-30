# include <ESP32Servo.h>

Servo esc;

void setup() {
  Serial.begin(115200);  // Starte die serielle Kommunikation
  esc.attach(26);
  esc.writeMicroseconds(1000);
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
