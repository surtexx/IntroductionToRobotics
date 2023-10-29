#include "Queue.h"

const unsigned int floorButtonPin[3] = {2,4,7};
const unsigned int floorLedPin[3] = {8,12,13};
const unsigned int blinkingLedPin = 3;
const unsigned int buzzerPin = 5;
unsigned int floorButtonState[3], lastButtonState[3], debounceButtonState[3];

const unsigned int debounceDelay = 50;
const unsigned long scheduleDelay = 5000;
const unsigned long scheduleFloorDelay = 1000;

Queue<unsigned int> scheduledQueries;
unsigned int currentFloor, destinationFloor, lastDebounceTime;
byte finishedQuery;
int currentQuery = -1;
unsigned long timeDelayScheduling = -1;
unsigned int floorCount;

void finishQuery(){
  digitalWrite(floorLedPin[currentFloor], LOW);
  digitalWrite(blinkingLedPin, LOW);
  scheduledQueries.pop();
  currentQuery = -1;
  finishedQuery = LOW;
  timeDelayScheduling = -1;
  floorCount = 0;
}

void nextFloor(){
  digitalWrite(floorLedPin[currentFloor], LOW);
  if(currentFloor < destinationFloor)
    currentFloor++;
  else
    currentFloor--;
  timeDelayScheduling = -1;
  floorCount++;
}

void schedule(){
  if(scheduledQueries.count() > 0)
  {
    if(currentQuery == -1){
      currentQuery = scheduledQueries.peek();
      destinationFloor = currentQuery;
    }
    if(currentFloor != destinationFloor)
    {
      digitalWrite(blinkingLedPin, millis()/scheduleFloorDelay%2);
      tone(buzzerPin, 100);
      if(timeDelayScheduling == -1)
        timeDelayScheduling = millis();
      if(millis() - timeDelayScheduling > scheduleFloorDelay){
        digitalWrite(floorLedPin[currentFloor], HIGH);
        if(millis() - timeDelayScheduling - scheduleFloorDelay > scheduleDelay)
          nextFloor();
      }
    }
    else{
      if(!floorCount){
        finishQuery();
        return;
      }
      if(!finishedQuery){
        finishedQuery = HIGH;
        timeDelayScheduling = millis();
      }
      if(millis() - timeDelayScheduling > scheduleFloorDelay){
        if(millis() - timeDelayScheduling - scheduleFloorDelay <= scheduleDelay){
          digitalWrite(floorLedPin[currentFloor], HIGH);
          digitalWrite(blinkingLedPin, HIGH);
          if(millis() - timeDelayScheduling - scheduleFloorDelay < scheduleDelay/5)
            tone(buzzerPin, 1000);
          else
            noTone(buzzerPin);
        }
        else
          finishQuery();
      }
    }
  }
}

void setup() {
  for(unsigned int pin:floorButtonPin)
    pinMode(pin, INPUT_PULLUP);
  
  for(unsigned int pin:floorLedPin)
    pinMode(pin, OUTPUT);
  
  pinMode(blinkingLedPin, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  for(unsigned int i=0;i<sizeof(floorButtonPin) / sizeof(floorButtonPin[0]);i++)
    floorButtonState[i] = digitalRead(floorButtonPin[i]);
  for(unsigned int i=0;i<sizeof(floorButtonPin) / sizeof(floorButtonPin[0]);i++){
    if(floorButtonState[i] && !lastButtonState[i]){
      lastDebounceTime = millis();
    }
    if(millis() - lastDebounceTime > debounceDelay)
      if(floorButtonState[i] != debounceButtonState[i]){
        debounceButtonState[i] = floorButtonState[i];
        if(debounceButtonState[i] == LOW)
          scheduledQueries.push(i);
      }
    lastButtonState[i] = floorButtonState[i];
  }
  schedule();
}
