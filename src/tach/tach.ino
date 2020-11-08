/*
 * Tach is a sketch that measures the number of microseconds between two falling edges 
 * on the INT0 pin. For this application that equates to the amount of time required
 * for one revolution of the lathe chuck.
 */
//#define DEBUG
//#define LCD
#define SERIAL_DEBUG

#ifdef LCD
#include <LiquidCrystal_I2C.h>
#endif

#include "circular_queue.h"

#define INT0_PIN  (2)
#define LED_PIN   (13)

// 1 minute, 60 second, 1 second 1000 ms, 1 ms 1000 us
#define MICRO_SECS_PER_MINUTE 60000000UL

// Number of cycles per complete rotation of the encoder. Also equivalent to 
// the number of encoder segments.
#define CYCLES_PER_ROTATION   16UL

volatile unsigned long lastCaptureTime = 0;
volatile unsigned long lastDelta = 0;

volatile CircularQueue isrCircularQueue(8);

#ifdef LCD
// LiquidCrystal_I2C  set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd(0x27, 20, 4);
#endif

void setup() {
  Serial.begin(115200);

#ifdef LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Tachometer");
#endif //LCD

  noInterrupts();

  pinMode(LED_PIN, OUTPUT);
  pinMode(INT0_PIN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(INT0_PIN), captureEvent, FALLING);
  interrupts();
}

void loop() {
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

#ifdef LCD  
  lcd.setCursor(0, 3);
  lcd.print(buf);
#endif LCD

#ifdef SERIAL_DEBUG 
  sprintf(buf, "ts:%lu, delta:%lu, rpm:%lu", lastCaptureTime, delta, rpm);
  Serial.println(buf);
#endif

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

#if defined(DEBUG) && defined(SERIAL_DEBUG)
  Serial.print("now=");
  Serial.print(now);
  Serial.print(" delta=");
  Serial.println(lastDelta);
#endif
}

static inline unsigned long computeDelta(unsigned long t0, unsigned long t1) {
  if (t1 > t0) {
    return t1 - t0;
  }

  return t0 - t1;
}


static inline unsigned long computeRPM(unsigned long cycleTimeMicros) {
  if (cycleTimeMicros == 0) {
    return 0;
  }

  return MICRO_SECS_PER_MINUTE / (cycleTimeMicros * CYCLES_PER_ROTATION);
}
