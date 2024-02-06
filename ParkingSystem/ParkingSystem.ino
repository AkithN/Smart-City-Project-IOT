#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

const int IR1 = 2;      // IR sensor 1 input pin
const int IR2 = 4;      // IR sensor 2 input pin
const int SERVO_PIN = 3; // Servo motor control pin
int SLOT_COUNT = 4;      // Enter total number of parking slots

Servo myservo1;
LiquidCrystal_I2C lcd(0x3F, 16, 2); // Change the HEX address

int isIR1Occupied = 0;
int isIR2Occupied = 0;

void setup() {
  lcd.init();
  lcd.clear();
  lcd.begin(2, 16);
  lcd.backlight();
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  myservo1.attach(SERVO_PIN);
  myservo1.write(100);

  lcd.setCursor(0, 0);
  lcd.print("     ARDUINO    ");
  lcd.setCursor(0, 1);
  lcd.print(" PARKING SYSTEM ");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (digitalRead(IR1) == LOW && isIR1Occupied == 0) {
    isIR1Occupied = 1;
    if (isIR2Occupied == 0) {
      myservo1.write(0);
      if (SLOT_COUNT > 0) {
        SLOT_COUNT = SLOT_COUNT - 1;
      } else {
        lcd.setCursor(0, 0);
        lcd.print("    SORRY :(    ");
        lcd.setCursor(0, 1);
        lcd.print("  Parking Full!  ");
        delay(3000);
        lcd.clear();
      }
    }
  }

  if (digitalRead(IR2) == LOW && isIR2Occupied == 0) {
    isIR2Occupied = 1;
    if (isIR1Occupied == 0) {
      myservo1.write(0);
      SLOT_COUNT = SLOT_COUNT + 1;
    }
  }

  if (isIR1Occupied == 1 && isIR2Occupied == 1) {
    delay(1000);
    myservo1.write(100);
    isIR1Occupied = 0;
    isIR2Occupied = 0;
  }

  lcd.setCursor(0, 0);
  lcd.print("    WELCOME!    ");
  lcd.setCursor(0, 1);
  lcd.print("Slot Left: ");
  lcd.print(SLOT_COUNT);
}
