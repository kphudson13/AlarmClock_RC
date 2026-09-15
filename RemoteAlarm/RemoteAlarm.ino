/* 
Kyle Hudson
2026
Live laugh love
*/

#include <IRremote.h>
#include <LiquidCrystal.h>
#include <RTClib.h>
#include "pitches.h"  // to send notes to buzzer, not my code
#include "buttons.h"  // you may need to modify the button signal to match your remote
#include "alarm_song.h"

int RECV_PIN = 2;  // set reciever pin

IRrecv irrecv(RECV_PIN);  // Create IR receiver object on the defined pin
decode_results results;   // Stores the decoded result from the IR receiver

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);  // let lcd pins

const int LCD_CONTRAST = 6;   // set contrast pin
const int BUZZER_PIN = 5;     // set buzzer pin
const int BACKLIGHT_PIN = 3;  // set LCD backlight pin

bool backlightOn = true;  // Track backlight state
int contrastValue = 60;   // set initial contrast value

RTC_DS3231 rtc;  // Create RTC object to interact with the time module

bool settingTime = false;  // Flags to track whether the user is currently entering a time or alarm
bool settingAlarm = false;
String inputDigits = "";  // Stores the digits typed in by the user during input

int alarmHour = -1;  // Stores the set alarm time (-1 means no alarm set)
int alarmMin = -1;
bool alarmSet = false;  // Tracks whether an alarm is currently scheduled

// Reads the current time from the RTC and prints it to the top line of the LCD
void displayTime() {
  DateTime now = rtc.now();  // Get the current date and time

  lcd.setCursor(0, 0);                  // Move cursor to start of first line
  if (now.hour() < 10) lcd.print("0");  // Pad single digit hours with a leading zero
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
}

// Prints the alarm status
void displayAlarm() {
  lcd.setCursor(0, 1);  // Move cursor to start of second line
  if (alarmSet) {
    lcd.print("Alarm: ");
    if (alarmHour < 10) lcd.print("0");
    lcd.print(alarmHour);
    lcd.print(":");
    if (alarmMin < 10) lcd.print("0");
    lcd.print(alarmMin);
    lcd.print("  ");  // Trailing spaces to overwrite any leftover characters
  } else {
    lcd.print("No Alarm        ");  // Padded to clear the full line
  }
}

// Checks if the current time matches the alarm time and triggers the buzzer
void checkAlarm() {
  if (!alarmSet) return;  // Skip check if no alarm is scheduled

  DateTime now = rtc.now();
  if (now.hour() == alarmHour && now.minute() == alarmMin && now.second() == 0) {  // Trigger buzzer when time matches
    playAlarm();                                                                   // Play the song instead of a continuous buzz                                               // Turn buzzer on
  }
}

void setup() {
  Serial.begin(9600);  // Start serial monitor for debugging
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);         // Backlight on at startup
  analogWrite(LCD_CONTRAST, contrastValue);  // Set initial contrast
  lcd.begin(16, 2);                          // Initialize LCD as 16 columns, 2 rows
  rtc.begin();                               // Initialize the RTC module
  pinMode(BUZZER_PIN, OUTPUT);               // Set buzzer pin as an output
  digitalWrite(BUZZER_PIN, LOW);             // Ensure buzzer is off at startup
  irrecv.enableIRIn();                       // Start listening for IR signals
  lcd.setCursor(0, 0);                       // Initial label on the first line
  lcd.print("Time:");                        // Show time
  displayAlarm();                            // Show alarm status on the second line
}

void loop() {
  if (!settingTime && !settingAlarm) {  // Continuously update the time and check the alarm when not in a setting mode
    displayTime();
    checkAlarm();
  }

  if (irrecv.decode(&results)) {                     // Check if an IR signal has been received
    if (results.value != 0xFFFFFFFF) {               // Ignore repeat/held button signals
      String button = getButtonName(results.value);  // Convert hex code to button name
      Serial.println(button);                        // Print button name

      if (button == "STOP") {  // Stop the buzzer and cancel the alarm
        alarmSet = false;      // Reset alarm
        displayAlarm();

      } else if (!settingTime && !settingAlarm && button == "EQ") {  // Enter time-setting mode
        settingTime = true;
        inputDigits = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter HHMM:");
        lcd.setCursor(0, 1);

      } else if (!settingTime && !settingAlarm && button == "PLAY") {  // Enter alarm-setting mode
        settingAlarm = true;
        inputDigits = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alarm HHMM:");
        lcd.setCursor(0, 1);

      } else if (button == "PWR") {
        backlightOn = !backlightOn;
        digitalWrite(BACKLIGHT_PIN, backlightOn ? HIGH : LOW);

      } else if ((settingTime || settingAlarm) && button.length() == 1 && isDigit(button[0])) {  // Handle digit input while in setting mode
        inputDigits += button;                                                                   // String of buttons
        lcd.print(button);                                                                       // Show the digit on the LCD as it's typed

        if (inputDigits.length() == 4) {
          int newHour = inputDigits.substring(0, 2).toInt();  // First 2 digits = hour
          int newMin = inputDigits.substring(2, 4).toInt();   // Last 2 digits = minute

          if (newHour < 24 && newMin < 60) {  // Validate the entered time
            if (settingTime) {
              DateTime now = rtc.now();  // Update the RTC with the new time
              rtc.adjust(DateTime(now.year(), now.month(), now.day(), newHour, newMin, 0));
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Time set!");
              delay(1500);
            } else if (settingAlarm) {  // Save the alarm time
              alarmHour = newHour;
              alarmMin = newMin;
              alarmSet = true;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Alarm set!");
              delay(1500);
            }

          } else {  // Invalid fallback
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Invalid time!");
            delay(1500);
          }

          lcd.clear();  // Return to normal display after setting
          lcd.setCursor(0, 0);
          lcd.print("Time:");
          settingTime = false;
          settingAlarm = false;
          inputDigits = "";
          displayAlarm();
        }

      } else if (!settingTime && !settingAlarm && button == "BACK") {  // Cancel a scheduled alarm
        digitalWrite(BUZZER_PIN, LOW);
        alarmSet = false;
        displayAlarm();

      } else if (button == "UP") {  // to change contrast
        contrastValue = constrain(contrastValue - 10, 0, 255);
        analogWrite(LCD_CONTRAST, contrastValue);

      } else if (button == "DOWN") {
        contrastValue = constrain(contrastValue + 10, 0, 255);
        analogWrite(LCD_CONTRAST, contrastValue);
      }
    }
    irrecv.resume();  // Ready the IR receiver
  }
}