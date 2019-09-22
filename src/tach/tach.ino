/*
 * Tach is a sketch that measures the number of microseconds between two falling edges 
 * on the INT0 pin. For this application that equates to the amount of time required
 * for one revolution of the lathe chuck.
 */
#include <LiquidCrystal_I2C.h>
#include "circular_queue.h"

#define DEBUG
#ifdef DEBUG
#define MS_ROTATION 600
#define PD7_PIN   (7)
#endif

#define INT0_PIN  (2)
#define LED_PIN   (13)

volatile unsigned long lastCaptureTime = 0;
volatile unsigned long lastDelta = 0;

volatile CircularQueue isrCircularQueue(8);

// LiquidCrystal_I2C  set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Tachometer");

  noInterrupts();

#ifdef DEBUG
  pinMode(PD7_PIN, OUTPUT);
  digitalWrite(PD7_PIN, HIGH);
#endif

  pinMode(LED_PIN, OUTPUT);
  pinMode(INT0_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INT0_PIN), captureEvent, FALLING);
  interrupts();
}

void loop() {
//#ifdef DEBUG  
//  digitalWrite(PD7_PIN, LOW);
//  delay(MS_ROTATION/2);
//  digitalWrite(PD7_PIN, HIGH);
//  delay(MS_ROTATION/2);
//#endif 

  unsigned long delta = 0;
  noInterrupts();
  if (lastCaptureTime != 0) {
    if (computeDelta(micros(), lastCaptureTime) >= 1000000UL) {
      lastCaptureTime = 0;
      lastDelta = 0;
    } else {
      delta = lastDelta;
    }
  }
  interrupts();

  unsigned long rpm = computeRPM(delta);
  char buf[128];
  sprintf(buf, "RPM: %6lu", rpm);
  lcd.setCursor(0, 3);
  lcd.print(buf);

//#ifdef DEBUG 
//  sprintf(buf, "ts:%lu, delta:%lu, rpm:%lu", lastCaptureTime, delta, rpm);
//  Serial.println(buf);
//#endif

  delay(100);
}

void captureEvent() {
  unsigned long now = micros();
  if (lastCaptureTime == 0) {
    lastCaptureTime = now;
    return;
  }

  lastDelta = computeDelta(now, lastCaptureTime);
  lastCaptureTime = now;
  
  Serial.print("now=");
  Serial.print(now);
  Serial.print(" delta=");
  Serial.println(lastDelta);
}

static inline unsigned long computeDelta(unsigned long t0, unsigned long t1) {
  if (t1 > t0) {
    return t1 - t0;
  }

  return t0 - t1;
}

static inline unsigned long computeRPM(unsigned long cycleTimeMicros) {
  // 1 minute, 60 second, 1 second 1000 ms, 1 ms 1000 us
  if (cycleTimeMicros == 0) {
    return 0;
  }

  return 60000000UL / cycleTimeMicros;
}
