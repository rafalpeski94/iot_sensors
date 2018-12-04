#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <TimeLib.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Math.h>

#define TRIG 12
#define ECHO 11

typedef struct
{
  double pressure;
  double dampness;
  double temperature;
  double lightIntensity1;
  double lightIntensity2;
  double distance;
}SensorValue;

SensorValue sensorsValue;

OneWire oneWire(A2);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 20, 4);

void SensorsMainLoopService(void);
void LCDMainLoopService(void);
void Print(int number);
double Distance(int echo, int trig);

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  Serial.begin(9600);
}

void loop() 
{
  SensorsMainLoopService();
  LCDMainLoopService();
}


void SensorsMainLoopService(void)
{
  static String infoString;
  
  sensorsValue.pressure = analogRead(A0) * 200.0 / 1023.0 + 900.0;
  sensorsValue.dampness = analogRead(A1) * 100.0 / 1023.0;
  sensorsValue.lightIntensity1 = analogRead(A3) * 100.0 / 1023.0;
  sensorsValue.lightIntensity2 = analogRead(A4) * 100.0 / 1023.0;
  sensorsValue.distance = Distance(ECHO, TRIG);
  sensors.requestTemperatures();
  sensorsValue.temperature = sensors.getTempCByIndex(0);


  
  infoString = String(sensorsValue.pressure) + " " + String(sensorsValue.dampness) + " " + String(sensorsValue.temperature) + " " + String(sensorsValue.lightIntensity1) + " " + String(sensorsValue.lightIntensity2) + " " + String(sensorsValue.distance);
  
  Serial.println(infoString);
  delay(900);
}

void LCDMainLoopService(void)
{
  static tmElements_t tm;
  lcd.clear();
  if(RTC.read(tm))
  {
    lcd.setCursor(1,0);
    Print(tm.Hour);
    lcd.setCursor(3,0);
    lcd.print(':');
    lcd.setCursor(4,0);
    Print(tm.Minute);

    lcd.setCursor(7,0);
    Print(tm.Day); 
    lcd.setCursor(9,0);
    lcd.print('.');
    lcd.setCursor(10,0);
    Print(tm.Month);
    lcd.setCursor(12,0);
    lcd.print('.');
    lcd.setCursor(13,0);
    lcd.print(tmYearToCalendar(tm.Year));
    lcd.print("r.");   
  }
  else
  {
    lcd.setCursor(0,0);
    lcd.print("Brak zegara");
  }

  
  
  lcd.setCursor(0,1);
  lcd.print("Temperatura: ");
  lcd.setCursor(13,1);
  lcd.print((int)sensorsValue.temperature);
  lcd.setCursor(15,1);
  lcd.print("C");

  lcd.setCursor(0,2);
  lcd.print("Cisnienie: ");
  lcd.setCursor(13,2);
  lcd.print((int)sensorsValue.pressure);
  lcd.setCursor(17,2);
  lcd.print("hPa");

  lcd.setCursor(0,3);
  lcd.print("Wilgotnosc: ");
  lcd.setCursor(13,3);
  lcd.print((int)sensorsValue.dampness);
  lcd.setCursor(15,3);
  lcd.print("%");
}

void Print(int number)
{
  if(number >= 0 && number < 10)
  {
    lcd.print('0');
  }
  lcd.print(number);
}

double Distance(int echo, int trig)
{
  double tim, dis;

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  tim = pulseIn(echo, HIGH);
  dis = tim / 58.0;
  return dis;
}
