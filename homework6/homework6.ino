#include "LedControl.h"

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int xPin = A0;
const int yPin = A1;
const int swPin = A2;
const int seedPin = A3;

byte swState = HIGH, swLastState = HIGH, swDebounceState = HIGH;

// Create an LedControl object to manage the LED matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); // DIN, CLK, LOAD, No. DRIVER

byte matrixBrightness = 2; // Variable to set the brightness level of the matrix
byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

// Thresholds
const int minThreshold = 200;
const int maxThreshold = 600;
const unsigned int debounceDelay = 50;
const unsigned int blinkingDelay = 500;
const unsigned int fastBlinkingDelay = 100;
const unsigned int buttonHoldTime = 3000;
const unsigned int resultDisplayTime = 5000;

const byte moveInterval = 125; // Timing variable to control the speed of LED movement
unsigned long lastClicked = 0, lastDebounceTime = 0;

const byte matrixSize = 8; // Size of the LED matrix
bool matrixChanged = true; // Flag to track if the matrix display needs updating
bool started = false;
bool finished = false;

byte matrix[matrixSize][matrixSize] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

byte currentDrawing[matrixSize][matrixSize] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

byte hiddenDrawing[matrixSize][matrixSize] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}};

unsigned long lastUpdateTime = 0; // Tracks the last time the LED position was updated
unsigned long lastBlinkTime = 0;  // Tracks the last time the LED blinke
unsigned long buttonHoldStartTime = 0;
unsigned long finishedTime = 0;

void setup()
{
    Serial.begin(9600);

    pinMode(swPin, INPUT_PULLUP);

    // the zero refers to the MAX7219 number, it is zero for 1 chip
    lc.shutdown(0, false);                // turn off power saving, enables display

    lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
    lc.clearDisplay(0);                   // Clear the matrix display
    randomSeed(analogRead(seedPin));
}

void loop()
{
    if (!started)
    {
        generateDrawing();
        delay(5000);
        started = true;
        clearDrawing();
    }
    else
    {
        if (!finished)
        {
            click();
            move();
            submitDrawing();
        }
        else
        {
            showResult();
            if (millis() - finishedTime >= resultDisplayTime)
            {
                started = false;
                finished = false;
                clearDrawing();
            }
        }
    }
}

// Function to generate a random drawing
void generateDrawing()
{
    for (int i = 0; i < matrixSize; i++)
        for (int j = 0; j < matrixSize; j++)
        {
            matrix[i][j] = (byte)(0 == random(10)); // 10% chance of being on
            hiddenDrawing[i][j] = matrix[i][j];
        }
    updateMatrix();
}

// Function to clear the LED matrix
void clearDrawing()
{
    for (int i = 0; i < matrixSize; i++)
        for (int j = 0; j < matrixSize; j++)
        {
            matrix[i][j] = 0;
            currentDrawing[i][j] = 0;
        }
    matrix[xPos][yPos] = 1;
    updateMatrix();
}

// Function to move the LED cursor around the matrix
void move()
{
    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= moveInterval)
    {
        lastUpdateTime = currentTime;

        if (xPos != xLastPos || yPos != yLastPos)
        {
            matrix[xLastPos][yLastPos] = currentDrawing[xLastPos][yLastPos];
            matrixChanged = true;
            xLastPos = xPos;
            yLastPos = yPos;
            matrix[xLastPos][yLastPos] = 1;
        }
        blink();
        updatePositions();
    }
}

// Function to modify LED state when button is pressed
void click()
{
    swState = digitalRead(swPin);
    if (swState && !swLastState)
        lastDebounceTime = millis();
    if (millis() - lastDebounceTime > debounceDelay)
        if (swState != swDebounceState)
        {
            swDebounceState = swState;
            if (swDebounceState == LOW)
            {
                currentDrawing[xPos][yPos] = !currentDrawing[xPos][yPos];
                updateMatrix();
            }
        }
}

// Function to mark the game as finished when button is held for 3 seconds
void submitDrawing()
{
    swState = digitalRead(swPin);
    if (!swState && !buttonHoldStartTime)
        buttonHoldStartTime = millis();
    else if (!swState && buttonHoldStartTime && millis() - buttonHoldStartTime >= buttonHoldTime)
    {
        buttonHoldStartTime = 0;
        finished = true;
        currentDrawing[xPos][yPos] = !currentDrawing[xPos][yPos];
        finishedTime = millis();
    }
    else if (swState)
        buttonHoldStartTime = 0;
}

// Function to display the result of the game
void showResult()
{
    for (int i = 0; i < matrixSize; i++)
        for (int j = 0; j < matrixSize; j++)
            if (currentDrawing[i][j] != hiddenDrawing[i][j])
                lc.setLed(0, i, j, millis() / fastBlinkingDelay % 2); // blink fast
            else
                lc.setLed(0, i, j, currentDrawing[i][j]);
}

// Function to blink the LED cursor
void blink()
{
    lc.setLed(0, xPos, yPos, millis() / blinkingDelay % 2);
}

// Function to update the LED matrix
void updateMatrix()
{
    for (int row = 0; row < matrixSize; row++)
    {
        for (int col = 0; col < matrixSize; col++)
        {
            lc.setLed(0, row, col, matrix[row][col] || currentDrawing[row][col]); // set each led individually
        }
    }
}

// Function to read joystick input and update the position of the LED
void updatePositions()
{
    unsigned int xRead = analogRead(xPin);
    unsigned int yRead = analogRead(yPin);

    if (xRead < minThreshold)
    {
        xPos++;
        xPos %= matrixSize;
    }
    else if (xRead > maxThreshold)
    {
        xPos--;
        xPos = (xPos + matrixSize) % matrixSize;
    }
    if (yRead < minThreshold)
    {
        yPos--;
        yPos = (yPos + matrixSize) % matrixSize;
    }
    else if (yRead > maxThreshold)
    {
        yPos++;
        yPos %= matrixSize;
    }

    if (matrixChanged)
    {
        updateMatrix();
        matrixChanged = false;
    }
}