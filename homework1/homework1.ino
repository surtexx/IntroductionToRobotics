// Modify RGB LED intensity using three potentiometers
 
const int redPinInput = A0;
const int greenPinInput = A1;
const int bluePinInput = A2;

int redPotValue = 0;
int greenPotValue = 0;
int bluePotValue = 0;

const int redPinLed = 11;
const int greenPinLed = 10;
const int bluePinLed = 9;

float intensity;
 
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}
 
void loop() {
  redPotValue = analogRead(redPinInput);
  greenPotValue = analogRead(greenPinInput);
  bluePotValue = analogRead(bluePinInput);

  intensity = redPotValue/4;
  analogWrite(redPinLed, intensity);

  intensity = bluePotValue/4;
  analogWrite(bluePinLed, intensity);

  intensity = greenPotValue/4;
  analogWrite(greenPinLed, intensity);
}