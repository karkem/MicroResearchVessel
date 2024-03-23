// Define the pin for the cable connected to pin 35
const int cablePin = 35;

void setup() {
  // Set the cablePin as input
  pinMode(cablePin, INPUT);
  
  // Initialize serial communication
  Serial.begin(9600);
}

void loop() {
  // Read the state of the cablePin
  Serial.println(calibrate_stepper());
  // Delay for stability
  delay(1000);
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
