// Define connections to the shift register
const int latchPin = 11; // Connects to STCP (latch pin) on the shift register
const int clockPin = 10; // Connects to SHCP (clock pin) on the shift register
const int dataPin = 12;  // Connects to DS (data pin) on the shift register

// Define connections to the digit control pins for a 4-digit display
const int segD1 = 4;
const int segD2 = 5;
const int segD3 = 6;
const int segD4 = 7;

// Store the digits in an array for easy access
int displayDigits[] = {segD1, segD2, segD3, segD4};
const int displayCount = 4; // Number of digits in the display

// Define the number of unique encodings (0-9, A-F for hexadecimal)
const int encodingsNumber = 10;
// Define byte encodings for the hexadecimal characters 0-F
int byteEncodings[encodingsNumber] =
    {
        // A B C D E F G DP
        B11111100, // 0
        B01100000, // 1
        B11011010, // 2
        B11110010, // 3
        B01100110, // 4
        B10110110, // 5
        B10111110, // 6
        B11100000, // 7
        B11111110, // 8
        B11110110, // 9
};

// Variables for controlling the display update timing
unsigned long lastIncrement = 0;
const unsigned int delayCount = 100; // Delay between updates (milliseconds)
unsigned int number = 0;       // The number being displayed

// Define the pins for the buttons and joysticks
unsigned int startPauseButtonPin = 8;
unsigned int joystickSwPin = 2;
unsigned int resetButtonPin = 13;
unsigned int joystickXPin = A0;
unsigned int joystickYPin = A1;

// Variables for storing the buttons states
byte startPauseButtonState = HIGH;
byte startPauseButtonLastState = HIGH;
byte debounceStartPauseButtonState = HIGH;
byte joystickSwState = HIGH;
byte joystickSwLastState = HIGH;
byte joystickSwDebounceState = HIGH;
byte resetButtonState = HIGH;
byte resetButtonLastState = HIGH;
byte resetButtonDebounceState = HIGH;

// Variables for timekeeping and flow control
unsigned long lastDebounceTime;
unsigned long debounceDelay = 50;
unsigned int xValue;
unsigned int milliseconds = 0;
unsigned int seconds = 0;
unsigned int currentLapIndex = 1;
const unsigned int lapCount = 4;
bool started = false;
bool modifiedLap = false;
int lapTimes[lapCount + 1];


void setup()
{
  // Initialize the pins connected to the shift register as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  // Initialize the pins connected to the buttons and joystick as inputs
  pinMode(startPauseButtonPin, INPUT_PULLUP);
  pinMode(joystickSwPin, INPUT_PULLUP);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);

  // Initialize digit control pins and set them to LOW (off)
  for (int i = 0; i < displayCount; i++)
  {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }
  // initialize the lap times with -1
  for (int i = 0; i < lapCount + 1; i++)
    lapTimes[i] = -1;
  // Begin serial communication for debugging purposes
  Serial.begin(9600);
}

void loop()
{
  debounceStartPauseTimer();
  if (!started)
  {
    cycleThroughLaps();
    debounceResetTimer();
    debounceResetLaps();
  }
  else
  {
    saveLapTime();
    countUp();
  }
  writeNumber(number);
}


// Read the state of the start/pause button using a debouncing algorithm
void debounceStartPauseTimer()
{
  startPauseButtonState = digitalRead(startPauseButtonPin);
  if (startPauseButtonState && !startPauseButtonLastState)
    lastDebounceTime = millis();
  if (millis() - lastDebounceTime > debounceDelay)
    if (startPauseButtonState != debounceStartPauseButtonState)
    {
      debounceStartPauseButtonState = startPauseButtonState;
      if (debounceStartPauseButtonState == LOW)
      {
        started = !started;
        if (!started)
        {
          currentLapIndex = 0;
          lapTimes[currentLapIndex] = number;
        }
      }
    }
}

void cycleThroughLaps()
{
  xValue = analogRead(joystickXPin);
  if (xValue >= 600)
  {
    if (!modifiedLap)
    {
      modifiedLap = true;
      currentLapIndex = (currentLapIndex + 1) % (lapCount + 1);
      if (lapTimes[currentLapIndex] == -1)
        currentLapIndex = 0;
      number = lapTimes[currentLapIndex];
      milliseconds = number % 10;
      seconds = number / 10;
    }
  }
  else if (xValue <= 400)
  {
    if (!modifiedLap)
    {
      modifiedLap = true;
      currentLapIndex = (currentLapIndex + lapCount) % (lapCount + 1);
      while (lapTimes[currentLapIndex] == -1 && currentLapIndex != 0)
        currentLapIndex = (currentLapIndex + lapCount) % (lapCount + 1);
      if (currentLapIndex != 0)
      {
        number = lapTimes[currentLapIndex];
        milliseconds = number % 10;
        seconds = number / 10;
      }
      else{
        currentLapIndex = 0;
        number = lapTimes[currentLapIndex];
        milliseconds = number % 10;
        seconds = number / 10;
      }
    }
  }
  else
    modifiedLap = false;
}

// Read the state of the reset button using a debouncing algorithm
void debounceResetTimer()
{
  resetButtonState = digitalRead(resetButtonPin);
  if (resetButtonState && !resetButtonLastState)
    lastDebounceTime = millis();
  if (millis() - lastDebounceTime > debounceDelay)
    if (resetButtonState != resetButtonDebounceState)
    {
      resetButtonDebounceState = resetButtonState;
      if (resetButtonDebounceState == LOW)
      {
        milliseconds = 0;
        seconds = 0;
        number = 0;
        for (int i = 0; i < lapCount + 1; i++)
          lapTimes[i] = -1;
        currentLapIndex = 1;
      }
    }
}

// Read the state of the joystick button using a debouncing algorithm
void debounceResetLaps()
{
  joystickSwState = digitalRead(joystickSwPin);
  if (joystickSwState && !joystickSwLastState)
    lastDebounceTime = millis();
  if (millis() - lastDebounceTime > debounceDelay)
    if (joystickSwState != joystickSwDebounceState)
    {
      joystickSwDebounceState = joystickSwState;
      if (joystickSwDebounceState == LOW)
      {
        number = lapTimes[0];
        milliseconds = number % 10;
        seconds = number / 10;
        for (int i = 1; i < lapCount + 1; i++)
          lapTimes[i] = -1;
        currentLapIndex = 1;
      }
    }
}

// Read the state of the joystick button using a debouncing algorithm
void saveLapTime()
{
  joystickSwState = digitalRead(joystickSwPin);
  if (joystickSwState && !joystickSwLastState)
    lastDebounceTime = millis();
  if (millis() - lastDebounceTime > debounceDelay)
    if (joystickSwState != joystickSwDebounceState)
    {
      joystickSwDebounceState = joystickSwState;
      if (joystickSwDebounceState == LOW)
      {
        lapTimes[currentLapIndex] = number;
        currentLapIndex = currentLapIndex % lapCount + 1;
      }
    }
}
void countUp()
{
  if (millis() - lastIncrement > delayCount)
  {
    milliseconds++;
    if (milliseconds == 10)
    {
      milliseconds = 0;
      seconds++;
    }
    number = seconds * 10 + milliseconds;
    lastIncrement = millis();
  }
}

void writeReg(int digit)
{
  // Prepare to shift data by setting the latch pin low
  digitalWrite(latchPin, LOW);
  // Shift out the byte representing the current digit to the shift register
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  // Latch the data onto the output pins by setting the latch pin high
  digitalWrite(latchPin, HIGH);
}

void activateDisplay(int displayNumber)
{
  // Turn off all digit control pins to avoid ghosting
  for (int i = 0; i < displayCount; i++)
    digitalWrite(displayDigits[i], HIGH);
  // Turn on the current digit control pin
  digitalWrite(displayDigits[displayNumber], LOW);
}

void writeNumber(int number)
{
  int currentNumber = number;
  int displayDigit = displayCount - 1;

  while (displayDigit >= 0)
  {
    activateDisplay(displayDigit);
    if (currentNumber > 0)
    {
      if (displayDigit == displayCount - 2)
        writeReg(byteEncodings[currentNumber % 10] | B00000001);
      else
        writeReg(byteEncodings[currentNumber % 10]);
    }
    else
    {
      if (displayDigit == displayCount - 2)
        writeReg(byteEncodings[0] | B00000001);
      else
        writeReg(byteEncodings[0]);
    }
    displayDigit--;
    currentNumber = currentNumber / 10;
    writeReg(B00000000);
  }
}
