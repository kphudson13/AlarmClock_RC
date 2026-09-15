#ifndef ALARM_SONG_H
#define ALARM_SONG_H

#include "pitches.h"

extern IRrecv irrecv;
extern decode_results results;
extern bool alarmSet;
extern const int BUZZER_PIN;

// Generates a tone without using hardware timers
void buzzTone(int pin, int frequency, int duration) {
  if (frequency == 0) {
    delay(duration);
    return;
  }
  long period = 1000000L / frequency;  // Period in microseconds
  long cycles = (long)frequency * duration / 1000;
  for (long i = 0; i < cycles; i++) {
    digitalWrite(pin, HIGH);
    delayMicroseconds(period / 2);
    digitalWrite(pin, LOW);
    delayMicroseconds(period / 2);
  }
}


void playAlarm() {
  int melody[] = {
    NOTE_AS4, NOTE_AS4, NOTE_AS4,
    NOTE_F5, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_C5,
    NOTE_F5, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
    NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5,
    NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
    NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_C5,
    NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
    NOTE_C6, NOTE_G5, NOTE_G5, REST, NOTE_C5,
    NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
    NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C6, NOTE_C6,
    NOTE_F6, NOTE_DS6, NOTE_CS6, NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_F5,
    NOTE_C6
  };

  int durations[] = {
    8, 8, 8,
    2, 2,
    8, 8, 8, 2, 4,
    8, 8, 8, 2, 4,
    8, 8, 8, 2, 8, 8, 8,
    2, 2,
    8, 8, 8, 2, 4,
    8, 8, 8, 2, 4,
    8, 8, 8, 2, 8, 16,
    4, 8, 8, 8, 8, 8,
    8, 8, 8, 4, 8, 4, 8, 16,
    4, 8, 8, 8, 8, 8,
    8, 16, 2, 8, 8,
    4, 8, 8, 8, 8, 8,
    8, 8, 8, 4, 8, 4, 8, 16,
    4, 8, 4, 8, 4, 8, 4, 8,
    1
  };

  int size = sizeof(durations) / sizeof(int);
  for (int repeat = 0; repeat < 10; repeat++) {  // Play the alarm up to 10 times
    for (int note = 0; note < size; note++) {
      if (irrecv.decode(&results)) {                   // Check for button press before each note
        String button = getButtonName(results.value);  // get button name
        if (button == "STOP" || button == "PWR" || button == "PLAY") {
          digitalWrite(BUZZER_PIN, LOW);
          alarmSet = false;
          irrecv.resume();
          return;
        }
        irrecv.resume();
      }

      int duration = 1000 / durations[note];
      buzzTone(BUZZER_PIN, melody[note], duration);
      delay(duration * 0.30);  // Pause between notes
    }
  }
  digitalWrite(BUZZER_PIN, LOW);  // Turn buzzer off after 10 repetitions
}
#endif