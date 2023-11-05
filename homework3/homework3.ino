// Declare all the joystick pins
const unsigned int pinSW = 2;
const unsigned int pinX = A0;
const unsigned int pinY = A1;

unsigned int xValue, yValue;

byte swValue = HIGH;
byte lastSwValue = HIGH;
byte swState = LOW;

// Declare all the segments pins
const unsigned int pinA = 12;
const unsigned int pinB = 10;
const unsigned int pinC = 9;
const unsigned int pinD = 8;
const unsigned int pinE = 7;
const unsigned int pinF = 6;
const unsigned int pinG = 5;
const unsigned int pinDP = 4;

const unsigned int segSize = 8;

const unsigned int rightNeighbour[segSize] = {
  1, 1, 7, 2, 2, 1, 6, 7
};
const unsigned int leftNeighbour[segSize] = {
  5, 5, 4, 4, 4, 5, 6, 2
};
const unsigned int upperNeighbour[segSize] = {
  0, 0, 6, 6, 6, 0, 0, 7
};
const unsigned int lowerNeighbour[segSize] = {
  6, 6, 3, 3, 3, 6, 3, 7
};

const unsigned int segments[segSize] = { 
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

byte segmentState[segSize] = {
  LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW
};

const unsigned int blinkingDelay = 1000;

unsigned int currentSegmentIndex = 7;

unsigned long lastDebounceTime;

const unsigned long debounceDelay = 50;

bool commonAnode;
bool modifiedSegment;

const unsigned int buttonHoldTime = 3000;

unsigned long buttonHoldStartTime;

void reset(){
  for(unsigned int i = 0; i < segSize; i++)
    segmentState[i] = LOW;
  currentSegmentIndex = segSize - 1;
}

void checkForReset(){
  if(!swValue && !buttonHoldStartTime)
    buttonHoldStartTime = millis();
  else if(!swValue && buttonHoldStartTime && millis() - buttonHoldStartTime >= buttonHoldTime){
    reset();
    buttonHoldStartTime = 0;
  }
  else if(swValue)
    buttonHoldStartTime = 0;
}

void checkForButtonPush(){
  if(swValue != lastSwValue)
    lastDebounceTime = millis();
  if(millis() - lastDebounceTime > debounceDelay){
    if(swValue != swState){
      swState = swValue;
      if(swState == LOW)
        segmentState[currentSegmentIndex] = !segmentState[currentSegmentIndex];   
    }
  }
  lastSwValue = swValue;
}

void blink(unsigned int pin){
  digitalWrite(pin, millis()/blinkingDelay%2);
}

void setup() {
  Serial.begin(9600);

  pinMode(pinSW, INPUT_PULLUP);
  for (unsigned int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  
  if (commonAnode == true) {
  	for(unsigned int i = 0; i < segSize; i++)
      segmentState[i] = !segmentState[i];
  }
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);
  swValue = digitalRead(pinSW);

  checkForReset();

  checkForButtonPush();

  if(xValue >= 600){
    if(!modifiedSegment){
      modifiedSegment = true;
      currentSegmentIndex = rightNeighbour[currentSegmentIndex];
    }
  }
  else if(xValue <= 400){
    if(!modifiedSegment){
      modifiedSegment = true;
      currentSegmentIndex = leftNeighbour[currentSegmentIndex];
    }
  }
  else if(yValue >= 600){
    if(!modifiedSegment){
      modifiedSegment = true;
      currentSegmentIndex = upperNeighbour[currentSegmentIndex];
    }
  }
  else if(yValue <= 400){
    if(!modifiedSegment){
      modifiedSegment = true;
      currentSegmentIndex = lowerNeighbour[currentSegmentIndex];
    }
  }
  else
    modifiedSegment = false;
  
  for(unsigned int i = 0; i < segSize; i++)
    digitalWrite(segments[i], segmentState[i]);

  blink(segments[currentSegmentIndex]);
}
