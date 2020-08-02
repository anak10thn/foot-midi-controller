
int buttonPinsAll[] = {2, A2, 4, 5, 6, A1}; // all button inputs to set global pullup
int ledPins[] = {7, 8, 9, 10, 11, 12, A0 }; // output pins for bank LEDs
int fxLeds[] = {A3, A4, A5,13}; // leds for effect on
int effectOn[] = {0, 0, 0, 0}; // increments for each button pressed; even number is effect on, odd number is effect off / writes button state for momentary mode
int effectPins[] = {2, A2, 4, 5}; // effect on off button pins
int bankButton = 6; // bank change button pin 
int momentary = A1; //

int currentBank = 1; // currently selected bank, starts on 1
int prevBank = 1;  // prev selected bank, starts on 1
int bankOffset = 0; // gives buttons 1 - 4 the values 1 - 12 depending on bank selected
int bankButtonState = 1;         // current state of the bank button
int lastBankButtonState = 1;     // previous state of the bank button

int buttonState = 1;         // current state of the effect button
int lastButtonState = 1;     // previous state of the effect button
int lastButtonNum = 9; // to differentiate buttons; same button: on to off, diff button, on to on
int buttonNum[] = {1, 2, 3, 4}; // which button was pressed
int buttonDelay = 350; // time in ms delay after button press. Only way debouncing is being handled.

// midi messages:
byte progChange = 0xC0; // PC midi command
byte contChange = 0xB0; // CC Midi command
byte contNum = 11; // CC# for on/off
byte contOn = 70; // CC# value for FX on
byte contOff = 0; // CC# value for FX off
int presets[] {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC}; // hex values for presets 1 - 12

void fxOn() {
  Serial.write(contChange);
  Serial.write(contNum);
  Serial.write(contOn);
}

void fxLedsOff() {
  digitalWrite(fxLeds[0],LOW);
  digitalWrite(fxLeds[1],LOW);
  digitalWrite(fxLeds[2],LOW);
  digitalWrite(fxLeds[3],LOW);
}

void fxOff() {
  Serial.write(contChange);
  Serial.write(contNum);
  Serial.write(contOff);
}

void clearDisplay() {
  digitalWrite(ledPins[0], LOW); // Turns Off all LEDs
  digitalWrite(ledPins[1], LOW);
  digitalWrite(ledPins[2], LOW);
  digitalWrite(ledPins[3], LOW);
  digitalWrite(ledPins[4], LOW);
  digitalWrite(ledPins[5], LOW);
}

void display1() {
  digitalWrite(ledPins[0], HIGH); // Display no 1
  digitalWrite(ledPins[5], HIGH);

  return;
}

void display2() {
  digitalWrite(ledPins[0], HIGH); // Display no 2
  digitalWrite(ledPins[1], HIGH);
  digitalWrite(ledPins[2], HIGH);
  digitalWrite(ledPins[3], HIGH);
  digitalWrite(ledPins[4], HIGH);
  return;
}

void display3() {
  digitalWrite(ledPins[0], HIGH); // Display no 3
  digitalWrite(ledPins[1], HIGH);
  digitalWrite(ledPins[2], HIGH);
  digitalWrite(ledPins[4], HIGH);
  digitalWrite(ledPins[5], HIGH);
  return;
}

void setup() {
  Serial.begin(31250);

  for (int i = 0; i < 7; i++) pinMode(ledPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(fxLeds[i], OUTPUT);
  for (int i = 0; i < 5; i++) pinMode(buttonPinsAll[i], INPUT_PULLUP); // input pullup used without external resistors
  Serial.write(progChange);
  Serial.write(0x00);
  fxLedsOff();
  fxOff();
  display1(); // 
  
}

void loop() {

  int momState = digitalRead(momentary);
  int bankButtonState = digitalRead(bankButton);
  if (bankButtonState == lastBankButtonState && bankButtonState == 1) { // if no bank change then:

    for (int i = 0; i <4 ; i++) { // read all the button pin states:
      buttonState = digitalRead(effectPins[i]);

      switch (momState) { 

        case 1: // momentary mode
        digitalWrite(ledPins[6], LOW);

          if (buttonState != effectOn[i] && buttonState == 0) { //pressed
            Serial.write(progChange);
            Serial.write(presets[bankOffset + i]);
            fxOn();
            digitalWrite(fxLeds[i],HIGH);
            delay(10);
          }

          else if (buttonState != effectOn[i] && buttonState == 1) { //released
            fxOff();
            fxLedsOff();
            delay(10);
          }

          else {
          }

          effectOn[i] = buttonState;
          break;

        case 0: // on/off mode
        digitalWrite(ledPins[6], HIGH);

          if (buttonState != lastButtonState && buttonState != 1) { // if button pressed:
            lastButtonState = buttonState;
            effectOn[i]++;

            if (lastButtonNum != buttonNum[i]) { // turn effect on
              fxLedsOff();
              Serial.write(progChange);
              Serial.write(presets[bankOffset + i]);
              fxOn();
              digitalWrite(fxLeds[i],HIGH);
              effectOn[i] = 1; 
              delay(buttonDelay);

            }
            else if (lastButtonNum == buttonNum[i] && effectOn[i] % 2 == 0) { // turn effect off if same button pressed
              Serial.write(progChange);
              Serial.write(presets[bankOffset + i]);
              fxLedsOff();
              fxOff();
              delay(buttonDelay);
            }
            else if (lastButtonNum == buttonNum[i] && effectOn[i] % 2 != 0) { // turn effect back on if turned off
              Serial.write(progChange);
              Serial.write(presets[bankOffset + i]);
              fxLedsOff();
              fxOn();
              digitalWrite(fxLeds[i],HIGH);
              delay(buttonDelay);
            }
            lastButtonNum = buttonNum[i];

          }
          lastButtonState = buttonState;

          break;
      }
    }
  }

  else if (bankButtonState != lastBankButtonState && bankButtonState == 0 && currentBank == 3) { // if bank is changed and current bank is 3, change to 1
    currentBank = 1;
    bankOffset = 0;
    clearDisplay();
    display1();
    lastButtonNum = int (buttonNum + 4);
    delay(buttonDelay);

  }
  else if (bankButtonState != lastBankButtonState && bankButtonState == 0 && currentBank == 1) { // if bank 1, change to 2
    currentBank++;
    bankOffset = 4;
    clearDisplay();
    display2();
    lastButtonNum = int (buttonNum + 1);
    delay(buttonDelay);

  }
  else if (bankButtonState != lastBankButtonState && bankButtonState == 0 && currentBank == 2) { // if bank 2, change to 3
    currentBank++;
    bankOffset = 8;
    clearDisplay();
    display3();
    lastButtonNum = int (buttonNum + 1);
    delay(buttonDelay);
  }
  lastBankButtonState = bankButtonState;
}
