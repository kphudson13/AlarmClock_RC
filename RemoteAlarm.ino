#include <IRremote.h>
#include <LiquidCrystal.h>
#include <RTClib.h>

int RECV_PIN = 2;

IRrecv irrecv(RECV_PIN);
decode_results results;

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int LCD_CONTRAST = 6;
int contrastValue = 100;

const int LCD_BACKLIGHT = 3;
int backlightValue = 255;  // 0 = off, 255 = full brightness

const int BUZZER_PIN = 5;

RTC_DS3231 rtc;

bool settingTime = false;
bool settingAlarm = false;
String inputDigits = "";

int alarmHour = -1;
int alarmMin = -1;
bool alarmSet = false;

String getButtonName(unsigned long code) {
  switch (code) {
    case 0xFF6897: return "0";
    case 0xFF30CF: return "1";
    case 0xFF18E7: return "2";
    case 0xFF7A85: return "3";
    case 0xFF10EF: return "4";
    case 0xFF38C7: return "5";
    case 0xFF5AA5: return "6";
    case 0xFF42BD: return "7";
    case 0xFF4AB5: return "8";
    case 0xFF52AD: return "9";
    case 0xFFA857: return "VOL-";
    case 0xFF629D: return "VOL+";
    case 0xFF02FD: return "PLAY";
    case 0xFFA25D: return "PWR";
    case 0xFFE21D: return "STOP";
    case 0xFFE01F: return "DOWN";
    case 0xFF906F: return "UP";
    case 0xFF9867: return "EQ";
    case 0xFFC23D: return "FWD";
    case 0xFF22DD: return "BACK";
    case 0xFFB04F: return "REPT";
    default: return "UNKNOWN";
  }
}

void displayTime() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");
  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
}

void displayAlarm() {
  lcd.setCursor(0, 1);
  if (alarmSet) {
    lcd.print("Alarm: ");
    if (alarmHour < 10) lcd.print("0");
    lcd.print(alarmHour);
    lcd.print(":");
    if (alarmMin < 10) lcd.print("0");
    lcd.print(alarmMin);
    lcd.print("  ");
  } else {
    lcd.print("No Alarm        ");
  }
}

void checkAlarm() {
  if (!alarmSet) return;

  DateTime now = rtc.now();
  if (now.hour() == alarmHour && now.minute() == alarmMin && now.second() == 0) {
    digitalWrite(BUZZER_PIN, HIGH);
  }
}

void setup() {
  Serial.begin(9600);
  analogWrite(LCD_CONTRAST, contrastValue);
  analogWrite(LCD_BACKLIGHT, backlightValue);
  lcd.begin(16, 2);
  rtc.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  irrecv.enableIRIn();

  lcd.setCursor(0, 0);
  lcd.print("Time:");
  displayAlarm();
}

void loop() {
  if (!settingTime && !settingAlarm) {
    displayTime();
    checkAlarm();
  }

  if (irrecv.decode(&results)) {
    if (results.value != 0xFFFFFFFF) {
      String button = getButtonName(results.value);
      Serial.println(button);

      if (button == "STOP") {
        digitalWrite(BUZZER_PIN, LOW);
        alarmSet = false;
        displayAlarm();

      } else if (!settingTime && !settingAlarm && button == "EQ") {
        settingTime = true;
        inputDigits = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Enter HHMM:");
        lcd.setCursor(0, 1);

      } else if (!settingTime && !settingAlarm && button == "PLAY") {
        settingAlarm = true;
        inputDigits = "";
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Alarm HHMM:");
        lcd.setCursor(0, 1);


      } else if ((settingTime || settingAlarm) && button.length() == 1 && isDigit(button[0])) {
        inputDigits += button;
        lcd.print(button);

        if (inputDigits.length() == 4) {
          int newHour = inputDigits.substring(0, 2).toInt();
          int newMin = inputDigits.substring(2, 4).toInt();

          if (newHour < 24 && newMin < 60) {
            if (settingTime) {
              DateTime now = rtc.now();
              rtc.adjust(DateTime(now.year(), now.month(), now.day(), newHour, newMin, 0));
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Time set!");
              delay(1500);
            } else if (settingAlarm) {
              alarmHour = newHour;
              alarmMin = newMin;
              alarmSet = true;
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Alarm set!");
              delay(1500);
            }

          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Invalid time!");
            delay(1500);
          }

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Time:");
          settingTime = false;
          settingAlarm = false;
          inputDigits = "";
          displayAlarm();
        }
      } else if (!settingTime && !settingAlarm && button == "BACK") {
        digitalWrite(BUZZER_PIN, LOW);
        alarmSet = false;
        displayAlarm();

      } else if (button == "UP") {  // to change contrast
        contrastValue = constrain(contrastValue - 10, 0, 255);
        analogWrite(LCD_CONTRAST, contrastValue);
      } else if (button == "DOWN") {
        contrastValue = constrain(contrastValue + 10, 0, 255);
        analogWrite(LCD_CONTRAST, contrastValue);

      } else if (button == "VOL+") {  // to change brightness
        backlightValue = constrain(backlightValue + 10, 0, 255);
        analogWrite(LCD_BACKLIGHT, backlightValue);
      } else if (button == "VOL-") {
        backlightValue = constrain(backlightValue - 10, 0, 255);
        analogWrite(LCD_BACKLIGHT, backlightValue);
      }
    }
    irrecv.resume();
  }
}