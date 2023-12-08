#include "LedControl.h"
#include <LiquidCrystal.h>

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int xPin = A0;
const int yPin = A1;
const int swPin = A2;
const int seedPin = A3;

byte swState = HIGH, swLastState = HIGH, swDebounceState = HIGH;

// LCD pins
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte a = 3;

// LCD object
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

String menuOptions[] = {"1. Start", "2. Settings", "3. About"};
String settingsMenuOptions[] = {"1. Display", "2. Game", "3. Back"};

unsigned int menuOptionsLength = 3;
unsigned int menuOptionIndex = 0;
unsigned int settingsMenuOptionsLength = 3;
unsigned int settingsMenuOptionIndex = 0;

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
const unsigned int fastBlinkingDelay = 200;
const unsigned int veryFastBlinkingDelay = 30;
const unsigned int buttonHoldTime = 3000;
const unsigned int resultDisplayTime = 5000;
const unsigned int startDelay = 5000;

const unsigned int moveInterval = 125;   // Timing variable to control the speed of LED movement
const unsigned int scrollInterval = 250; // Timing variable to control the speed of menu scrolling
unsigned long lastClicked = 0, lastDebounceTime = 0;

const byte matrixSize = 8; // Size of the LED matrix
bool matrixChanged = true; // Flag to track if the matrix display needs updating
bool started = false;
bool finished = false;
bool showMenu = true;
bool settingsMenu = false;

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

unsigned long lastUpdateTime = 0;       // Tracks the last time the LED position was updated
unsigned long lastBlinkTime = 0;        // Tracks the last time the LED blinke
unsigned long lastScrollUpdateTime = 0; // Tracks the last time the menu was scrolled
unsigned long buttonHoldStartTime = 0;
unsigned long finishedTime = 0;

unsigned int lcdIntensity = 120;

void setup()
{
    Serial.begin(9600);

    pinMode(swPin, INPUT_PULLUP);
    pinMode(a, OUTPUT);

    // the zero refers to the MAX7219 number, it is zero for 1 chip
    lc.shutdown(0, false); // turn off power saving, enables display

    lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
    lc.clearDisplay(0);                   // Clear the matrix display
    randomSeed(analogRead(seedPin));

    // set up the LCD's number of columns and rows:
    lcd.begin(32, 2);

    lcd.setCursor(0, 0);
    lcd.print("Hello! Ready for");
    lcd.setCursor(0, 1);
    lcd.print("some bamboozle?");
    analogWrite(a, lcdIntensity);
    delay(5000);
    lcd.clear();
}

void loop()
{
    if (showMenu)
    {
        displayMenu();
        scrollThroughMenu();
        chooseMenuOption();
    }
    else
    {
        if (!started)
        {
            generateDrawing();
            delay(startDelay);
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
}

void displayMenu()
{
    if (!settingsMenu)
    {
        lcd.setCursor(0, 0);
        lcd.print("MENU:");
        lcd.setCursor(0, 1);
        lcd.print(menuOptions[menuOptionIndex]);
    }
    else
    {
        lcd.setCursor(0, 0);
        lcd.print("BRIGHTNESS:");
        lcd.setCursor(0, 1);
        lcd.print(settingsMenuOptions[settingsMenuOptionIndex]);
    }
}

void scrollThroughMenu()
{
    unsigned int currentTime = millis();
    if (currentTime - lastScrollUpdateTime >= scrollInterval)
    {
        lastScrollUpdateTime = currentTime;
        unsigned int xRead = analogRead(xPin);
        if (xRead < minThreshold)
        {
            if (!settingsMenu)
            {
                menuOptionIndex++;
                menuOptionIndex %= menuOptionsLength;
            }
            else
            {
                settingsMenuOptionIndex++;
                settingsMenuOptionIndex %= settingsMenuOptionsLength;
            }
        }
        else if (xRead > maxThreshold)
        {
            if (!settingsMenu)
            {
                menuOptionIndex--;
                menuOptionIndex = (menuOptionIndex + menuOptionsLength) % menuOptionsLength;
            }
            else
            {
                settingsMenuOptionIndex--;
                settingsMenuOptionIndex = (settingsMenuOptionIndex + settingsMenuOptionsLength) % settingsMenuOptionsLength;
            }
        }
        lcd.clear();
    }
}

void chooseMenuOption()
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
                if (!settingsMenu)
                {
                    switch (menuOptionIndex)
                    {
                    case 0:
                        showMenu = false;
                        break;
                    case 1:
                        settingsMenu = true;
                        settings();
                        break;
                    case 2:
                        showAbout();
                        break;
                    }
                }
                else
                {
                    switch (settingsMenuOptionIndex)
                    {
                    case 0:
                        /* TODO */
                        break;
                    case 1:
                        /* TODO */
                        break;
                    case 2:
                        settingsMenu = false;
                        break;
                    }
                }
            }
        }
}

void settings()
{
    /* TODO */
}

void showAbout()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Made by: Gheorghe Robert-Mihai");
    lcd.setCursor(0, 1);
    lcd.print("GitHub: surtexx");
    lcd.scrollDisplayRight();
    for (int i = 0; i < 16; i++)
    {
        lcd.scrollDisplayLeft();
        delay(1000);
    }
    lcd.clear();
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
            if (currentDrawing[i][j] && !hiddenDrawing[i][j])
                lc.setLed(0, i, j, millis() / fastBlinkingDelay % 2); // blink fast for selecting a wrong cell
            else if (!currentDrawing[i][j] && hiddenDrawing[i][j])
                lc.setLed(0, i, j, millis() / veryFastBlinkingDelay % 2); // blink very fast for not selecting a correct cell
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