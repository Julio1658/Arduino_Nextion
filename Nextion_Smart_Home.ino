#include <Servo.h>
#include <SPI.h>
#include <Wire.h>

//Temperature
float Temp;
uint8_t x, y;

//Servo
int Position = 7;
Servo servo;
uint8_t i = 10;

//Buzzer
int Buzzer = 8;
int alarm = 0;

//Keypad
int Data = 0;
uint8_t Page = 0;
uint8_t counter = 0;
uint8_t Lock = 0;
uint8_t times = 0;
uint8_t button = 0;
uint8_t password[] = {1, 6, 5, 8};

//DS1307
#define Address 0x68
String Date[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};

uint8_t DecToBcd(uint8_t val)
{
  return ((val/10*16) + (val%10));
}

int BcdToDec(uint8_t val)
{
  return ((val/16*10) + (val%16));
}

typedef struct
{
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hour;
  uint8_t dayOfWeek;
  uint8_t dayOfMonth;
  uint8_t month;
  uint8_t year;
}Time;

Time time;

void Set_Time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom, uint8_t mon, uint8_t year)
{
  Wire.beginTransmission(Address);
  Wire.write(0);
  Wire.write(DecToBcd(sec));
  Wire.write(DecToBcd(min));
  Wire.write(DecToBcd(hour));
  Wire.write(DecToBcd(dow));
  Wire.write(DecToBcd(dom));
  Wire.write(DecToBcd(mon));
  Wire.write(DecToBcd(year));
  Wire.endTransmission();
}

void Get_Time()
{
  Wire.beginTransmission(Address);
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(Address, 7);
  
  time.seconds = BcdToDec(Wire.read() & 0x7F);
  time.minutes = BcdToDec(Wire.read());
  time.hour = BcdToDec(Wire.read() & 0x3F);
  time.dayOfWeek = BcdToDec(Wire.read());
  time.dayOfMonth = BcdToDec(Wire.read());
  time.month = BcdToDec(Wire.read());
  time.year = BcdToDec(Wire.read());
}

void Display_Time()
{
  Get_Time();
  
  Serial.print("n0.val=");
  Serial.print(time.hour);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("n1.val=");
  Serial.print(time.minutes);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
  
  Serial.print("n2.val=");
  Serial.print(time.seconds);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("n3.val=");
  Serial.print(time.dayOfMonth);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("n4.val=");
  Serial.print(time.month);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("n5.val=");
  Serial.print(time.year);
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);

  Serial.print("t7.txt=");
  Serial.print("\""); 
  Serial.print(Date[time.dayOfWeek]);
  Serial.print("\""); 
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.write(0xff);
}

void CheckNumber(int Data)
{
  if(Data == 0x90)
  {
    if(counter > 0 && times > 0)
    {
      counter--;
      times--;
    }
    else
    {
      times--;
    }
  }
  
  else if(Data == password[counter])
  {
    counter++;
    times++;
  }
  else if(Data != password[counter])
  {
    times++;
  }
}

void setup()
{
  Serial.begin(9600);

  Wire.begin();
  Set_Time(55, 59, 23, 2, 15, 7, 21);

  servo.attach(Position);
  servo.write(10);

  pinMode(Buzzer, OUTPUT);
}

void loop()
{
  if(Serial.available())
  {
    Data = Serial.read();
    
    if(Data == 0x10)
    {
      Page = 0;
    }
    else if(Data == 0x20)
    {
      Page = 1;
    }

    else if(Data == 0x30)
    {
      Page = 2;
    }
    
    else if(Page == 1)
    {
      CheckNumber(Data);
      
      if(times == 4 && counter == 4 && Lock == 0)
      {      
        for(i = 10; i < 150; i++)
        {
          servo.write(i);
          delay(10);
        }
        
        Lock = 1;
        counter = 0;
        times = 0;
        alarm = 0;
      }
      
      if(times == 4 && counter == 4 && Lock == 1)
      {
        for(i = 150; i > 10; i--)
        {
          servo.write(i);
          delay(10);
        }
        
        Lock = 0;
        counter = 0;
        times = 0;
        alarm = 0;
      }
      
      if(times == 4 && counter < 4)
      {
        counter = 0;
        times = 0;
        alarm++;
        if(alarm == 3)
        {
          digitalWrite(Buzzer, HIGH);
          delay(3000);
          digitalWrite(Buzzer, LOW);
          alarm = 0;
        }
      }
      
      if(times == 1)
      {
        Serial.print("t0.txt=");
        Serial.print("\"");    
        Serial.print("*");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
      
      if(times == 2)
      {
        Serial.print("t0.txt=");
        Serial.print("\"");    
        Serial.print("**");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
      
      if(times == 3)
      {
        Serial.print("t0.txt=");
        Serial.print("\"");    
        Serial.print("***");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
      
      if(times == 0 && Lock == 0 && alarm == 0)
      {
        Serial.print("t0.txt=");
        Serial.print("\"");    
        Serial.print("Lock");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
      
      if(times == 0 && Lock == 1 && alarm == 0)
      { 
        Serial.print("t0.txt=");
        Serial.print("\"");    
        Serial.print("Unlock");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
      
      if(times == 0 && alarm >= 1)
      {
        Serial.print("t0.txt=");
        Serial.print("\"");    
        Serial.print("Try again");
        Serial.print("\"");
        Serial.write(0xff);
        Serial.write(0xff);
        Serial.write(0xff);
      }
    }
  }

  if(Page == 0)
  {
    Temp = analogRead(A0);
    Temp = Temp * (5.0 / 1024.0) * 10;
    x = Temp;
    y = ((Temp - x) * 100);
    
    Serial.print("n1.val=");
    Serial.print(x);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);
    
    Serial.print("n0.val=");
    Serial.print(y);
    Serial.write(0xff);
    Serial.write(0xff);
    Serial.write(0xff);
    
    if(x < 0)
    {
      Serial.print("j0.val=");
      Serial.print(0);
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);
      
      Serial.print("j1.val=");
      Serial.print(100 - x);
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);
    }
    
    else if(x > 0)
    {
      Serial.print("j0.val=");
      Serial.print(x);
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);
      
      Serial.print("j1.val=");
      Serial.print(100);
      Serial.write(0xff);
      Serial.write(0xff);
      Serial.write(0xff);      
    }
    delay(1000);
  }

  if(Page == 2)
  {
    Display_Time();
    delay(1000);
  }
}
