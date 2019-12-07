/* Arduino menu library example
Oct. 2016 Rui Azevedo (ruihfazevedo@gmail.com) www.r-site.net
Use the menu library with user code ctrl command
calling doNav with user command mode:
noCmd - clamp field values or do nothing
escCmd - exit
enterCmd - enter current option or validate field and exit
upCmd - move up or increment field value
downCmd - move down or decrement field value
leftCmd - move left or escape (not tested yet)
rightCmd - move right or enter (not tested yet)
idxCmd - enter option by index
this mode allows you to implement ANY input device
on this example only using
*/

#include <Arduino.h>
#include <menu.h>
#include <menuIO/liquidCrystalOut.h>
#include <menuIO/serialIO.h>
#include <menuIO/stringIn.h>
#include <menuIO/chainStream.h>
#include <EEPROM.h>
#include "DS1307RTC.h"
#include <OneWire.h>
#include <DallasTemperature.h>

using namespace Menu;

#define MAX_DEPTH 3

// Setting the LCD shields pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// сигнальный провод датчика
#define ONE_WIRE_BUS 2
OneWire  oneWire(ONE_WIRE_BUS);

// создадим объект для работы с библиотекой DallasTemperature
DallasTemperature sensor(&oneWire);

// define some values used by the panel and buttons
#define btnNONE   0
#define btnRIGHT  1
#define btnUP     2
#define btnDOWN   3
#define btnLEFT   4
#define btnSELECT 5

#define FIRST_LINE 0 //text position for first line
#define SECOND_LINE 1 //text position for second line

#define PIN_LIGHT_LINE 11
#define PIN_HEATER_LINE 12

byte hrsLightStart=9;
byte minsLightStart=9;
byte hrsLightStop=9;
byte minsLightStop=9;

byte tempHeaterStart=0;
byte tempHeaterStop=0;

byte lightHHStartTimeAddress=0;
byte lightMMStartTimeAddress=1;
byte lightHHStopTimeAddress=2;
byte lightMMStopTimeAddress=3;

byte heaterTempStartTimeAddress=4;
byte heaterTempStopTimeAddress=5;

byte clockHours;
byte clockMinutes;

byte clockYear;
byte clockMonth;
byte clockDay;

boolean navigateMenu = false;
tmElements_t tm;
byte previous_ss;

result showEvent(eventMask e) {
  Serial.print("event: ");
  Serial.println(e);
  if(e == exitEvent){
    EEPROM.write(lightHHStartTimeAddress, hrsLightStart);
    EEPROM.write(lightMMStartTimeAddress, minsLightStart);
    EEPROM.write(lightHHStopTimeAddress, hrsLightStop);
    EEPROM.write(lightMMStopTimeAddress, minsLightStop);
    
    EEPROM.write(heaterTempStartTimeAddress, tempHeaterStart);
    EEPROM.write(heaterTempStopTimeAddress, tempHeaterStop);
    Serial.println("EEPROM saved");
  }
  return proceed;
}

result updateClockMenu(eventMask e){
  clockHours = tm.Hour;
  clockMinutes = tm.Minute;

  clockYear = tmYearToY2k(tm.Year);
  clockMonth = tm.Month;
  clockDay = tm.Day;
  return proceed;
}

result saveDate(eventMask e){
  tmElements_t tm_set;
  tm_set = tm;
  tm_set.Year = y2kYearToTm(clockYear); // year can be given as '2010' or '10'.  It is converted to years since 2000
  tm_set.Month = clockMonth; //from 5th character to 6th character
  tm_set.Day = clockDay;
  
  // checking for valid date, save settings if a date is valid
  if (check_date(tm_set.Year, tm_set.Month, tm_set.Day)) {
    RTC.write(tm_set);
    lcd.clear();
    lcd.print("Date changed");
  }
  else {
    lcd.clear();
    lcd.print("Err: ");
    lcd.setCursor(0, SECOND_LINE);
    lcd.print(dateStr(tm_set));
  }
}

result saveTime(eventMask e){
  tmElements_t tm_set;
  tm_set.Hour = clockHours;
  tm_set.Minute = clockMinutes;
  
  // checking for valid date, save settings if a date is valid
  if (check_time(tm_set.Hour, tm_set.Minute, tm_set.Second)) {
    RTC.write(tm_set);
    lcd.clear();
    lcd.print("Time changed");
  }
  else {
    lcd.clear();
    lcd.print("Time Err: ");
    lcd.setCursor(0, SECOND_LINE);
    lcd.print(timeStr(tm_set));
  }
}

//define a pad style menu (single line menu)
//here with a set of fields to enter a time in hh:mm format
PADMENU(lightStartTimeMenu,"Start:",doNothing,noEvent,noStyle
  ,FIELD(hrsLightStart,"",":",0,23,1,0,showEvent,exitEvent,noStyle)
  ,FIELD(minsLightStart,"","",0,59,10,1,showEvent,exitEvent,wrapStyle)
);

//define a pad style menu (single line menu)
//here with a set of fields to enter a time in hh:mm format
PADMENU(lightStopTimeMenu,"Stop :",doNothing,noEvent,noStyle
  ,FIELD(hrsLightStop,"",":",0,23,1,0,showEvent,exitEvent,noStyle)
  ,FIELD(minsLightStop,"","",0,59,10,1,showEvent,exitEvent,wrapStyle)
);

//define a pad style menu (single line menu)
//here with a set of fields to enter a time in hh:mm format
PADMENU(heaterMenu,"",doNothing,noEvent,noStyle
  ,FIELD(tempHeaterStart,"Min","",0,100,1,0,showEvent,exitEvent,noStyle)
  ,FIELD(tempHeaterStop,"Max","",0,100,1,0,showEvent,exitEvent,noStyle)
);

PADMENU(timeMenu,"",doNothing,noEvent,noStyle
  ,FIELD(clockHours,"HH","",0,23,1,0,saveTime,exitEvent,noStyle)
  ,FIELD(clockMinutes,"MM","",0,59,1,0,saveTime,exitEvent,noStyle)
);

PADMENU(dateMenu,"",doNothing,noEvent,noStyle
  ,FIELD(clockYear,"Y","",19,99,1,0,saveDate,exitEvent,noStyle)
  ,FIELD(clockMonth,"M","",1,12,1,0,saveDate,exitEvent,noStyle)
  ,FIELD(clockDay,"D","",1,31,1,0,saveDate,exitEvent,noStyle)
);

MENU(lightSubmenu,"Light",doNothing,anyEvent,wrapStyle
  ,SUBMENU(lightStartTimeMenu)
  ,SUBMENU(lightStopTimeMenu)
  ,OP("Lisght test",showEvent,enterEvent)
  ,EXIT("<Back")
);

MENU(heaterSubmenu,"Heater",doNothing,anyEvent,wrapStyle
  ,SUBMENU(heaterMenu)
  ,OP("Heater test",showEvent,enterEvent)
  ,EXIT("<Back")
);

MENU(clockSubmenu,"Clock",updateClockMenu,enterEvent,wrapStyle
  ,SUBMENU(timeMenu)
  ,SUBMENU(dateMenu)
  ,EXIT("<Back")
);

MENU(mainMenu,"Main menu",doNothing,noEvent,wrapStyle
  ,SUBMENU(lightSubmenu)
  ,SUBMENU(heaterSubmenu)
  ,SUBMENU(clockSubmenu)
  ,EXIT("<Back")
);

MENU_OUTPUTS(out, MAX_DEPTH
  ,LIQUIDCRYSTAL_OUT(lcd,{0,0,16,2})
  ,NONE
);

stringIn<0> strIn;//buffer size: 2^5 = 32 bytes, eventually use 0 for a single byte
serialIn serial(Serial);

NAVROOT(nav,mainMenu,MAX_DEPTH,serial,out);

void setup() {
  Serial.begin(115200);
  while(!Serial);
  lcd.begin(16,2);
  Serial.println("menu 4.x");
  lcd.setCursor(0, 0);
  lcd.print("Menu 4.x LCD");
  lcd.setCursor(0, 1);
  lcd.print("r-site.net");
  nav.showTitle=false;
  hrsLightStart =EEPROM.read(lightHHStartTimeAddress);
  minsLightStart=EEPROM.read(lightMMStartTimeAddress);
  hrsLightStop  =EEPROM.read(lightHHStopTimeAddress);
  minsLightStop =EEPROM.read(lightMMStopTimeAddress);

  tempHeaterStart=EEPROM.read(heaterTempStartTimeAddress);
  tempHeaterStop =EEPROM.read(heaterTempStopTimeAddress);
  lcd.clear();

  pinMode(PIN_LIGHT_LINE, OUTPUT);
  pinMode(PIN_HEATER_LINE, OUTPUT);
  digitalWrite(PIN_LIGHT_LINE, HIGH);
  digitalWrite(PIN_HEATER_LINE, HIGH);

  sensor.begin();
  // устанавливаем разрешение датчика от 9 до 12 бит
  sensor.setResolution(12);
  //non-blocking read
  sensor.setWaitForConversion(false);
}

#define SOFT_DEBOUNCE_MS 100

int loopCounter = 0;

void loop() {
  
  loopCounter++;
  
  // переменная для хранения температуры
  float temperature;


  if (RTC.read(tm)) {
    if(previous_ss != tm.Second) { //if current seconds diff. from previous seconds
      previous_ss = tm.Second;
      int minutes = tm.Minute+tm.Hour*60;
      if(minutes>=minsLightStart + hrsLightStart*60 && minutes<=minsLightStop + hrsLightStop*60){
        //turn on line
        digitalWrite(PIN_LIGHT_LINE, LOW);
      } else {
        //turn off line
        digitalWrite(PIN_LIGHT_LINE, HIGH);
      }   
    }
  }
  if(loopCounter % 1000 ==0){
    // отправляем запрос на измерение температуры
    sensor.requestTemperatures();
    // считываем данные из регистра датчика
    temperature = sensor.getTempCByIndex(0);
    if(temperature<tempHeaterStart){
      //turn on heater
      digitalWrite(PIN_HEATER_LINE, LOW);
    }
    if(temperature>tempHeaterStop){
      //turn off heater
      digitalWrite(PIN_HEATER_LINE, HIGH);
    }
    loopCounter=0;
  }
  
  byte lcd_key = key_press();   // read the buttons
  
  switch (lcd_key) {
    case btnRIGHT:
      nav.doNav(rightCmd);
      Serial.println("Right");
      delay(SOFT_DEBOUNCE_MS);
      navigateMenu = true;
      nav.doOutput();//if not doing poll the we need to do output "manualy"
      break;
    case btnLEFT:
      nav.doNav(noCmd);
      Serial.println("Left");
      delay(SOFT_DEBOUNCE_MS);
      navigateMenu = true;
      nav.doOutput();//if not doing poll the we need to do output "manualy"
      break;
    case btnUP:
      nav.doNav(downCmd);
      Serial.println("Down");
      delay(SOFT_DEBOUNCE_MS);
      navigateMenu = true;
      nav.doOutput();//if not doing poll the we need to do output "manualy"
      break;
    case btnDOWN:
      nav.doNav(upCmd);
      Serial.println("Up");
      delay(SOFT_DEBOUNCE_MS);
      navigateMenu = true;
      nav.doOutput();//if not doing poll the we need to do output "manualy"
      break;
    case btnSELECT:
      Serial.println("Enter");
      nav.doNav(enterCmd);
      delay(SOFT_DEBOUNCE_MS);
      navigateMenu = true;
      nav.doOutput();//if not doing poll the we need to do output "manualy"
      break;
    case btnNONE:
      if(!navigateMenu){
        if (RTC.read(tm)) {
          if (previous_ss != tm.Second) { //if current seconds diff. from previous seconds
            previous_ss = tm.Second;
            show_time(FIRST_LINE); //show time on first line of LCD screen
            show_date(SECOND_LINE); //show time on second line of LCD screen
            show_temperature(FIRST_LINE,temperature);
          }
        } else {
          lcd.clear();
          if (RTC.chipPresent()) {
            lcd.print("DS1307 stopped");
            lcd.setCursor(0, SECOND_LINE);
            lcd.print("Run SetTime");
          } else {
            lcd.print("DS1307 error");
            lcd.setCursor(0, SECOND_LINE);
            lcd.print("Check circuitry");
          }
          delay(500);
          lcd.clear();
        }
      }
      break;
  }
}

//////////////////////////////////////////////////////////////////////////
// show time on display
//////////////////////////////////////////////////////////////////////////
void show_time(byte pos_y) {
  lcd.setCursor(0, pos_y);
  byte hh = tm.Hour;
  lcd.print(int2str(hh));
  lcd.print(":");
  lcd.print(int2str(tm.Minute));
  lcd.print(":");
  lcd.print(int2str(tm.Second));
}

//////////////////////////////////////////////////////////////////////////
// show date on display
//////////////////////////////////////////////////////////////////////////
void show_date(byte pos_y) {
  lcd.setCursor(0, pos_y);
  lcd.print(String(tmYearToCalendar(tm.Year)));
  lcd.print("-");
  lcd.print(int2str(tm.Month));
  lcd.print("-");
  lcd.print(int2str(tm.Day));
}

//////////////////////////////////////////////////////////////////////////
// show temperature
//////////////////////////////////////////////////////////////////////////
void show_temperature(byte pos_y, float temp) {
  if(!isnan(temp)){
    lcd.setCursor(10, pos_y);
    lcd.print("T:");
    lcd.print(temp);
  }
}

//////////////////////////////////////////////////////////////////////////
// convert integer (max. 2 digits) to string with leading zero
//////////////////////////////////////////////////////////////////////////
String int2str(int i) {
  String str = String(i); //convert to string
  if (i < 10) {
    str = "0" + str; //add leading zero
  }
  return str;
}
