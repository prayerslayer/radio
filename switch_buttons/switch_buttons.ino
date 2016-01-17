const int STATUS_LED = 13;
const int BUTTONS[] = {8, 9};

int getPressedButton() {
  for (int i = 0; i < (sizeof(BUTTONS)/sizeof(int)); i++) {
    int state = digitalRead(BUTTONS[i]);
    if (state == HIGH) {
      return i;
    }
  }
  return -1;
}

void setup() {
  Serial.begin( 9600 );
  // put your setup code here, to run once:
  pinMode(STATUS_LED, OUTPUT);
  for (int i = 0; i < sizeof(BUTTONS) - 1; i++) {
    pinMode(BUTTONS[i], INPUT);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  int btn = getPressedButton();
  if (btn >= 0) {
    digitalWrite(STATUS_LED, HIGH);
  } else {
    digitalWrite(STATUS_LED, LOW);
  }
}
