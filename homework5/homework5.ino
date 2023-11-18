#include <EEPROM.h>

// Initialize pins
const int ldrPin = A0;
const int ultrasonicEchoPin = 9;
const int ultrasonicTrigPin = 10;
const int redPin = 3;
const int greenPin = 5;
const int bluePin = 6;

// Initialize variables for sensor readings
unsigned int ldrValue;
unsigned long durationValue;
unsigned int distance;
unsigned int option;
bool valueRead, optionCompleted, completedSetUltrasonicSamplingRate, completedSetLdrSamplingRate,
    printedTextUltrasonicSamplingRate, printedTextSetLdrSamplingRate, printedTextUltrasonicAlertThreeshold,
    printedTextLdrAlertThreeshold, printedTextManualColorControl, alert;

// Initialize variables for menus
unsigned int currentSubmenu; // initialized with 0, meaning the main menu
unsigned int ultrasonicRate;
unsigned int ldrRate;
unsigned int ultrasonicAlertThreeshold;
unsigned int ldrAlertThreeshold;
bool autoMode;
unsigned int ldrReadingIndex;
unsigned int ultrasonicReadingIndex;

void setup()
{
    pinMode(ultrasonicTrigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(ultrasonicEchoPin, INPUT);  // Sets the echoPin as an Input
    Serial.begin(9600);
    printMenu(currentSubmenu);
}

void loop()
{
    chooseOption(currentSubmenu);
    if (optionCompleted)
        printMenu(currentSubmenu);
}

void chooseOption(unsigned int submenu)
{
    if (!submenu)
    {
        if (!valueRead && Serial.available())
        {
            option = Serial.parseInt();
            valueRead = true;
        }
        else if (valueRead)
        {
            if (option == 1)
            {
                currentSubmenu = 1;
                optionCompleted = true;
            }
            else if (option == 2)
            {
                currentSubmenu = 2;
                optionCompleted = true;
            }
            else if (option == 3)
            {
                currentSubmenu = 3;
                optionCompleted = true;
            }
            else if (option == 4)
            {
                currentSubmenu = 4;
                optionCompleted = true;
            }
            else
            {
                Serial.println("Invalid option.");
                optionCompleted = true;
            }
        }
    }
    else if (submenu == 1)
    {
        if (!valueRead && Serial.available())
        {
            option = Serial.parseInt();
            valueRead = true;
        }
        else if (valueRead)
        {
            if (option == 1)
            {
                setUltrasonicSamplingRate();
                if (completedSetUltrasonicSamplingRate)
                    setLdrSamplingRate();
            }
            else if (option == 2)
                setUltrasonicAlertThreeshold();
            else if (option == 3)
                setLdrAlertThreeshold();
            else if (option == 4)
            {
                currentSubmenu = 0;
                optionCompleted = true;
            }
            else
            {
                Serial.println("Invalid option.");
                optionCompleted = true;
            }
        }
    }
    else if (submenu == 2)
    {
        if (!valueRead && Serial.available())
        {
            option = Serial.parseInt();
            valueRead = true;
        }
        else if (valueRead)
        {
            if (option == 1)
                resetLoggedData();
            else if (option == 2)
            {
                currentSubmenu = 0;
                optionCompleted = true;
            }
            else
            {
                Serial.println("Invalid option.");
                optionCompleted = true;
            }
        }
    }
    else if (submenu == 3)
    {
        if (!valueRead && Serial.available())
        {
            option = Serial.parseInt();
            valueRead = true;
        }
        else if (valueRead)
        {
            if (option == 1)
                sensorsRead();
            else if (option == 2)
                displaySettings();
            else if (option == 3)
                displayLoggedData();
            else if (option == 4)
            {
                currentSubmenu = 0;
                optionCompleted = true;
            }
            else
            {
                Serial.println("Invalid option.");
                optionCompleted = true;
            }
        }
    }
    else if (submenu == 4)
    {
        if (!valueRead && Serial.available())
        {
            option = Serial.parseInt();
            valueRead = true;
        }
        else if (valueRead)
        {
            if (option == 1)
                manualColorControl();
            else if (option == 2)
                toggleAutomatic();
            else if (option == 3)
            {
                currentSubmenu = 0;
                optionCompleted = true;
            }
            else
            {
                Serial.println("Invalid option.");
                optionCompleted = true;
            }
        }
    }
    else
    {
        Serial.println("Invalid option.");
        optionCompleted = true;
    }
}

void printMenu(unsigned int submenu)
{
    if (!submenu)
    {
        Serial.println("Choose an option (1-4): ");
        Serial.println("    1. Sensor Settings");
        Serial.println("    2. Reset Logger Data");
        Serial.println("    3. System Status");
        Serial.println("    4. RGB Led Control");
    }
    else if (submenu == 1)
    {
        Serial.println("Choose an option (1-4):");
        Serial.println("    1.1 Sensors Sampling Interval");
        Serial.println("    1.2 Ultrasonic Alert Threeshold");
        Serial.println("    1.3 LDR Alert Threeshold");
        Serial.println("    1.4 Back");
    }
    else if (submenu == 2)
    {
        Serial.println("Are you sure? (1/2)");
        Serial.println("    2.1 Yes.");
        Serial.println("    2.2 No.");
    }
    else if (submenu == 3)
    {
        Serial.println("Choose an option (1-4):");
        Serial.println("    3.1 Current Sensor Readings");
        Serial.println("    3.2 Current Sensor Settings");
        Serial.println("    3.3 Display Logged Data");
        Serial.println("    3.4 Back");
    }
    else if (submenu == 4)
    {
        Serial.println("Choose an option (1-3):");
        Serial.println("    4.1 Manual Color Control");
        Serial.println("    4.2 LED: Toggle Automatic ON/OFF");
        Serial.println("    4.3 Back");
    }
    else
        Serial.println("Invalid option.");
    valueRead = false;
    optionCompleted = false;
    completedSetUltrasonicSamplingRate = false;
    completedSetLdrSamplingRate = false;
}

void setUltrasonicSamplingRate()
{
    if (!completedSetUltrasonicSamplingRate)
    {
        if (!printedTextUltrasonicSamplingRate)
        {
            Serial.println("Set ultrasonic sensor sampling rate (The value must be between 1 and 10 seconds) ");
            printedTextUltrasonicSamplingRate = true;
        }
        else if (Serial.available())
        {
            ultrasonicRate = Serial.parseInt();

            if (ultrasonicRate < 1 || ultrasonicRate > 10)
                Serial.println("Invalid value.");
            else
            {
                Serial.print("Success! Ultrasonic sampling rate set to ");
                Serial.print(ultrasonicRate);
                Serial.println(" seconds.");
                completedSetUltrasonicSamplingRate = true;
                printedTextUltrasonicSamplingRate = false;
            }
        }
    }
}

void setLdrSamplingRate()
{
    if (!completedSetLdrSamplingRate)
    {
        if (!printedTextSetLdrSamplingRate)
        {
            Serial.println("Set LDR sensor sampling rate (The value must be between 1 and 10 seconds)");
            printedTextSetLdrSamplingRate = true;
        }
        else if (Serial.available())
        {
            ldrRate = Serial.parseInt();

            if (ldrRate < 1 || ldrRate > 10)
                Serial.println("Invalid value.");
            else
            {
                Serial.print("Success! LDR sampling rate set to ");
                Serial.print(ldrRate);
                Serial.println(" seconds.");
                completedSetLdrSamplingRate = true;
                printedTextSetLdrSamplingRate = false;
                optionCompleted = true;
            }
        }
    }
}

void setUltrasonicAlertThreeshold()
{
    if (!printedTextUltrasonicAlertThreeshold)
    {
        Serial.println("Set ultrasonic alert threeshold (The value must be between 1 and 100 cm)");
        printedTextUltrasonicAlertThreeshold = true;
    }
    else if (Serial.available())
    {
        ultrasonicAlertThreeshold = Serial.parseInt();

        if (ultrasonicAlertThreeshold < 1 || ultrasonicAlertThreeshold > 100)
            Serial.println("Invalid value.");
        else
        {
            Serial.print("Success! The alert will be triggered when the distance is less than ");
            Serial.print(ultrasonicAlertThreeshold);
            Serial.println(" cm.");
            printedTextUltrasonicAlertThreeshold = false;
            optionCompleted = true;
        }
    }
}

void setLdrAlertThreeshold()
{
    if (!printedTextLdrAlertThreeshold)
    {
        Serial.println("Set LDR alert threeshold (The value must be between 1 and 1022 lux)");
        printedTextLdrAlertThreeshold = true;
    }
    else if (Serial.available())
    {
        ldrAlertThreeshold = Serial.parseInt();

        if (ldrAlertThreeshold < 1 || ldrAlertThreeshold > 1022)
            Serial.println("Invalid value.");
        else
        {
            Serial.print("Success! The alert will be triggered when the light intensity is less than ");
            Serial.print(ldrAlertThreeshold);
            Serial.println(" lux.");
            printedTextLdrAlertThreeshold = false;
            optionCompleted = true;
        }
    }
}

void resetLoggedData()
{
    Serial.println("Resetting logger data...");
    for (int i = 0; i < 20; i++)
        EEPROM.update(i, 0);

    Serial.println("Success! Logger data reset.");

    currentSubmenu = 0;
    optionCompleted = true;
}

void sensorsRead()
{
    Serial.println("To stop the readings, press any key.");
    if (!ldrRate)
        Serial.println("You must set the LDR sampling rate first.");
    else if (!ultrasonicRate)
        Serial.println("You must set the ultrasonic sampling rate first.");
    else
        while (!Serial.available())
        {
            if (!(millis() % (ldrRate * 1000)))
                ldrRead();
            if (!(millis() % (ultrasonicRate * 1000)))
                ultrasonicRead();
            if (autoMode)
            {
                if(ldrValue < ldrAlertThreeshold || distance < ultrasonicAlertThreeshold)
                    bool alert = true;
                else
                    bool alert = false;
                analogWrite(redPin, 255*(int)alert);
                analogWrite(greenPin, 255*(int)!alert);
                analogWrite(bluePin, 0);
            }
        }
    Serial.read();
    optionCompleted = true;
}

void ldrRead()
{
    ldrValue = analogRead(ldrPin);

    Serial.print("Light intensity = ");
    Serial.print(ldrValue);
    Serial.println(" lux");

    if (ldrValue < ldrAlertThreeshold)
        Serial.println("ALERT! Night is coming.");

    EEPROM.put(ldrReadingIndex, ldrValue);
    ldrReadingIndex += sizeof(ldrValue);
    ldrReadingIndex %= 10 * sizeof(ldrValue);
    ldrReadingIndex += 10 * sizeof(distance);
}

void ultrasonicRead()
{
    digitalWrite(ultrasonicTrigPin, LOW);
    delayMicroseconds(2);

    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(ultrasonicTrigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(ultrasonicTrigPin, LOW);

    // Reads the ultrasonicEchoPin, returns the sound wave travel time in microseconds
    durationValue = pulseIn(ultrasonicEchoPin, HIGH);

    // Sound wave reflects from the obstacle, so to calculate the distance we
    // consider half of the distance traveled.
    distance = durationValue * 0.034 / 2;

    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance < ultrasonicAlertThreeshold)
        Serial.println("ALERT! Object too close.");

    EEPROM.put(ultrasonicReadingIndex, distance);
    ultrasonicReadingIndex += sizeof(distance);
    ultrasonicReadingIndex %= 10 * sizeof(distance);
}

void displaySettings()
{
    Serial.print("Ultrasonic sensor sampling rate: ");
    Serial.print(ultrasonicRate);
    Serial.println(" seconds.");

    Serial.print("LDR sensor sampling rate: ");
    Serial.print(ldrRate);
    Serial.println(" seconds.");

    Serial.print("Ultrasonic alert threeshold: ");
    Serial.print(ultrasonicAlertThreeshold);
    Serial.println(" cm.");

    Serial.print("LDR alert threeshold: ");
    Serial.print(ldrAlertThreeshold);
    Serial.println(" lux.");

    optionCompleted = true;
}

void displayLoggedData()
{
    Serial.println("Last 10 ultrasonic sensor readings:");
    for (int i = 0; i < 10; i++)
    {
        unsigned int value;
        EEPROM.get(i * sizeof(unsigned int), value);
        Serial.print(value);
        Serial.println(" cm");
    }

    Serial.println("Last 10 LDR sensor readings:");
    for (int i = 10; i < 20; i++)
    {
        unsigned int value;
        EEPROM.get(i * sizeof(unsigned int), value);
        Serial.print(value);
        Serial.println(" lux");
    }
    optionCompleted = true;
}

void manualColorControl()
{
    if(!printedTextManualColorControl){
        // Prompt the user for RGB values
        Serial.println("Enter RGB values (0-255) separated by spaces:");
        printedTextManualColorControl = true;
    }
    else if (Serial.available())
    {
        // Read the user input
        String input = Serial.readStringUntil('\n');

        // Parse the RGB values from the input
        int r, g, b;
        sscanf(input.c_str(), "%d %d %d", &r, &g, &b);
        Serial.println(input);
        Serial.println(r);
        Serial.println(g);
        Serial.println(b);
        // Validate the RGB values
        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
        {
            Serial.println("Invalid RGB values. Please enter values between 0 and 255.");
            return;
        }

        // Set the RGB values
        analogWrite(redPin, r);
        analogWrite(greenPin, g);
        analogWrite(bluePin, b);

        // Print the set RGB values
        Serial.print("RGB values set: (");
        Serial.print(r);
        Serial.print(", ");
        Serial.print(g);
        Serial.print(", ");
        Serial.print(b);
        Serial.println(")");

        optionCompleted = true;
    }
}

void toggleAutomatic()
{
    autoMode = !autoMode;

    Serial.print("Automatic mode: ");
    Serial.println(autoMode ? "ON" : "OFF");

    optionCompleted = true;
}