/*
 * Tach is a sketch that measures the number of microseconds between two falling edges 
 * on the INT0 pin. For this application that equates to the amount of time required
 * for one revolution of the lathe chuck.
 */
#define DEBUG
#ifdef DEBUG
#define MS_ROTATION 18
#define PD7_PIN   (7)
#endif

#define INT0_PIN  (2)
#define LED_PIN   (13)

volatile byte state = LOW;
volatile unsigned long lastCaptureTime = 0;

void setup() {
  Serial.begin(115200);
  
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
  digitalWrite(LED_PIN, state);

#ifdef DEBUG  
  digitalWrite(PD7_PIN, LOW);
  delay(MS_ROTATION/2);
  digitalWrite(PD7_PIN, HIGH);
  delay(MS_ROTATION/2);
#endif 
}

void captureEvent() {
  unsigned long now = micros();
  Serial.print("now=");
  Serial.print(now);
  if (lastCaptureTime == 0) {
    lastCaptureTime = now;
    Serial.println();
    return;
  }

  unsigned long delta = computeDelta(now, lastCaptureTime);
  lastCaptureTime = now;  
  Serial.print(", delta=");
  Serial.print(delta);
  Serial.print(", rpm=");
  Serial.println(computeRPM(delta));
}

static inline unsigned long computeDelta(unsigned long t0, unsigned long t1) {
  if (t1 > t0) {
    return t1 - t0;
  }

  return t0 - t1;
}

static inline unsigned long computeRPM(unsigned long cycleTimeMicros) {
  // 1 minute, 60 second, 1 second 1000 ms, 1 ms 1000 us
  return 60000000 / cycleTimeMicros;
}
