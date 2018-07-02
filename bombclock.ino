#include <SPI.h>
#include <pcmConfig.h>
#include <pcmRF.h>
#include <TMRpcm.h>
#include <SD.h>
#include <LedControl.h>
#include <Time.h>
#include <TimeLib.h>
#include <LiquidCrystal.h>

#define TIME_MSG_LEN 11 // time sync to PC is HEADER followed by Unix time_t as ten ASCII digits
#define TIME_HEADER 'T' // Header tag for serial time sync message
#define TIME_REQUEST 7 // ASCII bell character requests a time sync message 

LedControl lc = LedControl(12, 11, 10, 4);
byte zero[6] = {B01111110, B10000001, B10000001, B10000001, B10000001, B01111110};
byte one[6] = {B00000001, B10000001, B10000001, B11111111, B00000001, B0000001};
byte two[6] = {B00000000, B10011111, B10010001, B10010001, B10010001, B11110001};
byte three[6] = {B00000000, B10010001, B10010001, B10010001, B10010001, B11111111};
byte four[6] = {(byte)0, B11110000, B00010000, B00010000, B00010000, B11111111};
byte five[6] = {(byte)0, B11110001, B10010001, B10010001, B10010001, B10011111};
byte six[6] = {B01111110, B10010001, B10010001, B10010001, B10010001, B00001110};
byte seven[6] = {B10000000, B10000000, B10000000, B10000000, B10000000, B11111111};  
byte eight[6] = {B01101110, B10010001, B10010001, B10010001, B10010001, B01101110};
byte nine[6] = {(byte)0, B11110000, B10010000, B10010000, B10010000, B11111111};
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2, cs = 4, ain = 2, buzzerPin = 8;
char* alarmSound = "alarm.wav";
int prev = -1;
bool alarm = false;
long alarmTime = -1;
bool ringing = false;
unsigned long lastMillis = 0;
bool dots = true;
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup () 
{
  for (int i = 0; i < 4; i++)
  {
    lc.shutdown(i,false);
  /* Set the brightness to a medium values */
    lc.setIntensity(i,5);
  /* and clear the display */
    lc.clearDisplay(i);
  }
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  Serial.print("Status");
//  Serial.println(SD.exists(alarmSound));
//  if (!SD.begin(cs))
//  {
//    Serial.println("SD fail");
//    return;
//  }
  setTime(19, 38, 00, 9, 6, 2018);
//  setAlarm(16, 51, 01);
//  tmrpcm.setVolume(1); // arb value
}

void setAlarm(int hours, int minutes, int seconds)
{
  alarm = true;
  alarmTime = nowTime(hours, minutes, seconds);
  Serial.print("set");
}

unsigned long nowTime(int hours, int minutes, int seconds)
{
  return hours*(pow(10, 4)) + minutes*pow(10, 2) + seconds;
}
void testPlay()
{
    Serial.println("test");
    ringing = true;
}
void ringAlarm()
{
  digitalWrite(buzzerPin, HIGH);
  ringing = true;
}

void stopAlarm()
{
  digitalWrite(buzzerPin, LOW);  
  ringing = false;
}

void countTime() {
  int hours = hour();
  int minutes = minute();
  int seconds = second();
  Serial.println(analogRead(ain));
  if (alarm)
  {
    if ((alarmTime > 0 && alarmTime == nowTime(hours, minutes, seconds)) || ringing)
    {
      ringAlarm();
    }
    if (ringing && analogRead(ain) < 600)
    {
      stopAlarm();
    }
  }
  if (!ringing)
  {
    digitalWrite(buzzerPin, LOW);
  }
  int t = hour() * 100 + minute();
  Serial.print(t);
//  for (int i = 0; i < 6; i++)
//  {
//    lc.setColumn(0, i+1, eight[i]);
//  }
  for (int b = 0; b < 4; b++)
  { 
    displayToClock(t%10, b);
    t = t / 10;
  }
}

void displayToClock(int t, int addr)
{
  for (int i = 0; i < 6; i++)
  {
    switch(t){
      case 0:
        lc.setColumn(addr, i+1, zero[i]);
        break;
      case 1:
        lc.setColumn(addr, i+1, one[i]);
        break;
      case 2:
        lc.setColumn(addr, i+1, two[i]);
        break;
      case 3:
        lc.setColumn(addr, i+1, three[i]);
        break;
      case 4:
        lc.setColumn(addr, i+1, four[i]);
        break;
      case 5:
        lc.setColumn(addr, i+1, five[i]);
        break;
      case 6:
        lc.setColumn(addr, i+1, six[i]);
        break;
      case 7:
        lc.setColumn(addr, i+1, seven[i]);
        break;
      case 8:
        lc.setColumn(addr, i+1, eight[i]);
        break;
      case 9:
        lc.setColumn(addr, i+1, nine[i]);
        break;
    }
  }
//  for (int i = 0; i < 4; i++){
//    lc.setDigit(4-i, 0, (byte)(t/(10*i)), false); 
// 
}


void loop()
{
 if (second() != prev)
 {
   countTime();
   prev = second();
 }
 if (abs(millis() - lastMillis) > 700 && !dots)
 {
  lc.setColumn(2, 7, B01100110);
  lastMillis = millis();
  dots= true;
 }
 else if (abs(millis() - lastMillis) > 700)
 {
  lc.setColumn(2, 7, (byte)0);
  lastMillis = millis();
  dots = false;
 }
}
