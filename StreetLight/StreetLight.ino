int ir_pin = 8;
// int ir_pin = 9;  // Uncomment and assign a different pin if needed
int ldr_pin = A0;
int led_pin = 3;
// int led_pin = 4;  // Uncomment and assign a different pin if needed

int ir_value;
int ldr_value;

void setup()
{
  pinMode(ir_pin, INPUT);
  pinMode(ldr_pin, INPUT);
  pinMode(led_pin, OUTPUT);
}

void loop()
{
  ir_value = digitalRead(ir_pin);
  ldr_value = analogRead(ldr_pin);

  if (ir_value == HIGH && ldr_value < 50)
  {
    digitalWrite(led_pin, LOW);
  }
  else if (ir_value == HIGH && ldr_value >= 50)
  {
    analogWrite(led_pin, 100);
  }
  else if (ir_value == LOW && ldr_value >= 50)
  {
    analogWrite(led_pin, 500);
  }
}
