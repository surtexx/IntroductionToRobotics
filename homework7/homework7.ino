#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;
const int yPin = A0;
const int xPin = A1;
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
unsigned int textScrollAmount = 0;

// Create an LedControl object to manage the LED matrix
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); // DIN, CLK, LOAD, No. DRIVER

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;

// Shapes to be displayed on the LCD
byte fullRectangle[8] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111};

byte emptyRectangle[8] = {
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000,
    B00000};

byte crown[8] = {
    B00000,
    B10101,
    B10101,
    B11111,
    B11111,
    B11111,
    B00000,
    B00000};

// Thresholds
const int minThreshold = 200;
const int maxThreshold = 600;
const unsigned int debounceDelay = 50;
const unsigned int blinkingDelay = 500;
const unsigned int fastBlinkingDelay = 200;
const unsigned int veryFastBlinkingDelay = 30;
const unsigned int buttonHoldTime = 3000;
const unsigned int startDelay = 5000;
const unsigned int lcdColumns = 16;
const unsigned int lcdRows = 2;

const unsigned int moveInterval = 125;   // Timing variable to control the speed of LED movement
const unsigned int scrollInterval = 250; // Timing variable to control the speed of menu scrolling
unsigned long lastClicked = 0, lastDebounceTime = 0;

const byte matrixSize = 8; // Size of the LED matrix
bool matrixChanged = true; // Flag to track if the matrix display needs updating
bool started = false;
bool finished = false;
bool showMenu = true;
bool settingsMenu = false;
bool aboutChosen = false;
bool lcdBrightnessChosen = false;
bool matrixBrightnessChosen = false;

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
unsigned long startTime = 0;

unsigned int lcdBrightness;
unsigned int matrixBrightness;

void setup()
{
    Serial.begin(9600);

    pinMode(swPin, INPUT_PULLUP);
    pinMode(a, OUTPUT);

    lcdBrightness = map(EEPROM.read(0), 0, 255, 0, 255);
    matrixBrightness = map(EEPROM.read(4), 0, 255, 0, 15);

    // the zero refers to the MAX7219 number, it is zero for 1 chip
    lc.shutdown(0, false); // turn off power saving, enables display

    lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
    lc.clearDisplay(0);                   // Clear the matrix display
    randomSeed(analogRead(seedPin));

    // set up the LCD's number of columns and rows:
    lcd.begin(lcdColumns, lcdRows);
    lcd.createChar(0, emptyRectangle);
    lcd.createChar(1, fullRectangle);
    lcd.createChar(2, crown);

    lcd.setCursor(0, 0);
    lcd.print("Hello! Ready for");
    lcd.setCursor(0, 1);
    lcd.print("some bamboozle?");
    analogWrite(a, lcdBrightness);
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
            startTime = millis();
            clearDrawing();
            xPos = 0;
            yPos = 0;
            matrix[yPos][xPos] = 1;
            lcd.clear();
        }
        else
        {
            if (!finished)
            {
                displayStatus();
                click();
                move();
                submitDrawing();
            }
            else
            {
                showResult();
                swState = digitalRead(swPin);
                if (swState && !swLastState)
                    lastDebounceTime = millis();
                if (millis() - lastDebounceTime > debounceDelay)
                    if (swState != swDebounceState)
                    {
                        swDebounceState = swState;
                        if (swDebounceState == LOW)
                        {
                            started = false;
                            finished = false;
                            showMenu = true;
                            clearDrawing();
                        }
                    }
            }
        }
    }
}

void displayMenu()
{
    if (aboutChosen || lcdBrightnessChosen || matrixBrightnessChosen)
        return;
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
    if (aboutChosen || lcdBrightnessChosen || matrixBrightnessChosen)
        return;
    unsigned int currentTime = millis();
    if (currentTime - lastScrollUpdateTime >= scrollInterval)
    {
        lastScrollUpdateTime = currentTime;
        unsigned int yRead = analogRead(yPin);
        if (yRead < minThreshold)
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
        else if (yRead > maxThreshold)
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
    if (aboutChosen)
        showAbout();
    else if (lcdBrightnessChosen)
        setLcdBrightness();
    else if (matrixBrightnessChosen)
        setMatrixBrightness();
    else
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
                            break;
                        case 2:
                            aboutChosen = true;
                            break;
                        }
                    }
                    else
                    {
                        switch (settingsMenuOptionIndex)
                        {
                        case 0:
                            lcdBrightnessChosen = true;
                            lcd.clear();
                            break;
                        case 1:
                            matrixBrightnessChosen = true;
                            lcd.clear();
                            break;
                        case 2:
                            settingsMenu = false;
                            break;
                        }
                    }
                }
            }
    }
}

// Function to set the brightness of the LCD
void setLcdBrightness()
{
    lcd.setCursor(0, 0);
    lcd.print("DISPLAY: ");
    lcd.setCursor(0, 1);
    lcd.print("-");
    for (int i = 0; i < lcdBrightness / lcdColumns; i++)
        lcd.write(byte(1));
    for (int i = lcdBrightness / lcdColumns; i < lcdColumns; i++)
        lcd.write(byte(0));
    lcd.setCursor(lcdColumns - 1, 1);
    lcd.print("+");

    unsigned int currentTime = millis();
    if (currentTime - lastScrollUpdateTime >= scrollInterval)
    {
        lastScrollUpdateTime = currentTime;
        unsigned int xRead = analogRead(xPin);
        if (xRead < minThreshold)
        {
            lcdBrightness -= lcdColumns;
            lcdBrightness = (lcdBrightness + 255) % 255;
        }
        else if (xRead > maxThreshold)
        {
            lcdBrightness += lcdColumns;
            lcdBrightness %= 255;
        }
        analogWrite(a, lcdBrightness);
        EEPROM.put(0, lcdBrightness);
    }

    // check if sw was pressed for exit
    swState = digitalRead(swPin);
    if (swState && !swLastState)
        lastDebounceTime = millis();
    if (millis() - lastDebounceTime > debounceDelay)
        if (swState != swDebounceState)
        {
            swDebounceState = swState;
            if (swDebounceState == LOW)
            {
                lcdBrightnessChosen = false;
                lcd.clear();
            }
        }
}

// Function to set the brightness of the LED matrix
void setMatrixBrightness()
{
    for (int i = 0; i < matrixSize; i++)
        for (int j = 0; j < matrixSize; j++)
            lc.setLed(0, i, j, 1);

    lcd.setCursor(0, 0);
    lcd.print("GAME: ");
    lcd.setCursor(0, 1);
    lcd.print("-");
    for (int i = 0; i < matrixBrightness; i++)
        lcd.write(byte(1));
    for (int i = matrixBrightness; i < lcdColumns; i++)
        lcd.write(byte(0));
    lcd.setCursor(lcdColumns - 1, 1);
    lcd.print("+");

    unsigned int currentTime = millis();
    if (currentTime - lastScrollUpdateTime >= scrollInterval)
    {
        lastScrollUpdateTime = currentTime;
        unsigned int xRead = analogRead(xPin);
        if (xRead < minThreshold)
        {
            matrixBrightness--;
            matrixBrightness = (matrixBrightness + lcdColumns - 1) % (lcdColumns - 1);
        }
        else if (xRead > maxThreshold)
        {
            matrixBrightness++;
            matrixBrightness %= lcdColumns - 1;
        }
        lc.setIntensity(0, matrixBrightness);
        EEPROM.put(4, matrixBrightness);
    }

    swState = digitalRead(swPin);
    if (swState && !swLastState)
        lastDebounceTime = millis();
    if (millis() - lastDebounceTime > debounceDelay)
        if (swState != swDebounceState)
        {
            swDebounceState = swState;
            if (swDebounceState == LOW)
            {
                matrixBrightnessChosen = false;
                for (int i = 0; i < matrixSize; i++)
                    for (int j = 0; j < matrixSize; j++)
                        lc.setLed(0, i, j, 0);
                lcd.clear();
            }
        }
}

// Function to display the about screen
void showAbout()
{
    unsigned int currentTime = millis();
    if (currentTime - lastScrollUpdateTime >= scrollInterval)
    {
        lastScrollUpdateTime = currentTime;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Made by: Gheorghe Robert-Mihai");
        lcd.setCursor(0, 1);
        lcd.print("GitHub: surtexx");
        for (int i = 0; i < textScrollAmount; i++)
            lcd.scrollDisplayLeft();
        textScrollAmount++;
    }

    swState = digitalRead(swPin);
    if (swState && !swLastState)
        lastDebounceTime = millis();
    if (millis() - lastDebounceTime > debounceDelay)
        if (swState != swDebounceState)
        {
            swDebounceState = swState;
            if (swDebounceState == LOW)
            {
                aboutChosen = false;
                textScrollAmount = 0;
                lcd.clear();
            }
        }
}

// Function to generate a random drawing
void generateDrawing()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Memorize this...");
    lcd.setCursor(0, 1);
    lcd.print("Quickly!!!");
    lcd.print("");
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
    updateMatrix();
}

void displayStatus()
{
    unsigned int minutes = (millis() - startTime) / 1000 / 60;
    unsigned int seconds = (millis() - startTime) / 1000 % 60;
    lcd.setCursor(0, 0);
    lcd.print("Elapsed time");
    lcd.setCursor(0, 1);
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10)
        lcd.print("0");
    lcd.print(seconds);
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
            matrix[yLastPos][xLastPos] = currentDrawing[yLastPos][xLastPos];
            matrixChanged = true;
            xLastPos = xPos;
            yLastPos = yPos;
            matrix[yLastPos][xLastPos] = 1;
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
                currentDrawing[yPos][xPos] = !currentDrawing[yPos][xPos];
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
        currentDrawing[yPos][xPos] = !currentDrawing[yPos][xPos];
        finishedTime = millis();
    }
    else if (swState)
        buttonHoldStartTime = 0;
}

// Function to display the result of the game
void showResult()
{
    unsigned int correctCells = 0;
    unsigned int incorrectCells = 0;
    unsigned int missedCells = 0;
    for (int i = 0; i < matrixSize; i++)
        for (int j = 0; j < matrixSize; j++)
            if (currentDrawing[i][j] && !hiddenDrawing[i][j])
            {
                lc.setLed(0, i, j, millis() / fastBlinkingDelay % 2); // blink fast for selecting a wrong cell
                incorrectCells++;
            }
            else if (!currentDrawing[i][j] && hiddenDrawing[i][j])
            {
                lc.setLed(0, i, j, millis() / veryFastBlinkingDelay % 2); // blink very fast for not selecting a correct cell
                incorrectCells++;
            }
            else
            {
                lc.setLed(0, i, j, currentDrawing[i][j]);
                if (currentDrawing[i][j])
                    correctCells++;
            }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Time: ");
    unsigned int minutes = (finishedTime - startTime) / 1000 / 60;
    unsigned int seconds = (finishedTime - startTime) / 1000 % 60;
    lcd.print(minutes);
    lcd.print(":");
    if (seconds < 10)
        lcd.print("0");
    lcd.print(seconds);
    lcd.setCursor(0, 1);
    lcd.print("Accuracy: ");
    float accuracy;
    if (!correctCells)
        accuracy = 0;
    else
        accuracy = (float)correctCells / (correctCells + incorrectCells) * 100;
    if (accuracy == 100)
    {
        lcd.print(accuracy, 0);
        lcd.print("%");
        lcd.write(byte(2));
    }
    else
    {
        lcd.print(accuracy);
        lcd.print("%");
    }
}

// Function to blink the LED cursor
void blink()
{
    lc.setLed(0, yPos, xPos, millis() / blinkingDelay % 2);
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
        xPos--;
        xPos = (xPos + matrixSize) % matrixSize;
    }
    else if (xRead > maxThreshold)
    {
        xPos++;
        xPos %= matrixSize;
    }
    if (yRead < minThreshold)
    {
        yPos++;
        yPos %= matrixSize;
    }
    else if (yRead > maxThreshold)
    {
        yPos--;
        yPos = (yPos + matrixSize) % matrixSize;
    }

    if (matrixChanged)
    {
        updateMatrix();
        matrixChanged = false;
    }
}