#define BUTTON_A      0x00000001
#define BUTTON_B      0x00000002
#define BUTTON_C	  0x00000004
#define BUTTON_D	  0x00000008
#define BUTTON_UP     0x00000010
#define BUTTON_DOWN   0x00000020
#define BUTTON_LEFT   0x00000040
#define BUTTON_RIGHT  0x00000080

const byte PIN_BUTTON_A = 2;
const byte PIN_BUTTON_B = 3;
const byte PIN_BUTTON_C = 4;
const byte PIN_BUTTON_D = 5;

const byte PIN_BUTTON_SELECT = 6;
const byte PIN_BUTTON_START  = 7;

const byte PIN_ANALOG_X = 0;
const byte PIN_ANALOG_Y = 1;

typedef struct JoystickReport {
  int16_t x;
  int16_t y;
  uint8_t buttons;
} __attribute__((packed));

JoystickReport joystickReport;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BUTTON_A, INPUT);
  digitalWrite(PIN_BUTTON_A, HIGH);

  pinMode(PIN_BUTTON_B, INPUT);
  digitalWrite(PIN_BUTTON_B, HIGH);

  pinMode(PIN_BUTTON_C, INPUT);
  digitalWrite(PIN_BUTTON_C, HIGH);

  pinMode(PIN_BUTTON_D, INPUT);
  digitalWrite(PIN_BUTTON_D, HIGH);

  pinMode(PIN_BUTTON_SELECT, INPUT);
  digitalWrite(PIN_BUTTON_SELECT, HIGH);

  pinMode(PIN_BUTTON_START, INPUT);
  digitalWrite(PIN_BUTTON_START, HIGH);

  delay(250);
}

void loop() {
  memset(&joystickReport, 0, sizeof(JoystickReport));

  if (!digitalRead(PIN_BUTTON_A)) {
    joystickReport.buttons |= BUTTON_A;
  }

  if (!digitalRead(PIN_BUTTON_B)) {
    joystickReport.buttons |= BUTTON_B;
  }

  if (!digitalRead(PIN_BUTTON_C)) {
    joystickReport.buttons |= BUTTON_C;
  }

  if (!digitalRead(PIN_BUTTON_D)) {
    joystickReport.buttons |= BUTTON_D;
  }

  int16_t x = analogRead(PIN_ANALOG_X);
  int16_t y = analogRead(PIN_ANALOG_Y);

  if (x > 800)
    joystickReport.buttons |= BUTTON_RIGHT;
  else if (x < 200)
    joystickReport.buttons |= BUTTON_LEFT;
  if (y > 800)
    joystickReport.buttons |= BUTTON_DOWN;
  else if (y < 200)
    joystickReport.buttons |= BUTTON_UP;

  joystickReport.x = x;
  joystickReport.y = y;

  Serial.write((uint8_t *)&joystickReport, sizeof(joystickReport));
  Serial.flush();

  delay(50);
}

