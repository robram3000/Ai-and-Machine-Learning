#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int trig = 6; 
const int echo = 7; 

LiquidCrystal_I2C lcd(0x27, 16, 2);


long totaltime;
int distance;

void setup()
{
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.backlight();
  lcd.display();
  
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  Serial.begin(9600);
}

void loop()
{
 
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  totaltime = pulseIn(echo, HIGH);
  distance = totaltime * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.println(distance);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(distance);
  lcd.print(" cm");

  delay(500);
}


