/*
 * Future addons
Menu with potentiometer
wi-fi module 
Ink/led screen as main station gathers and display summary in ink or led 7-9 inch
add dosers, timing for lamps (after GPS date is runing)
button to flush
button to activate pump for drineage
 * arduino mega
 * -----------------------------------------------------------------------------------
//relay_1 = 7
//relay_2 = 8 lights
//DHT22 pin = 6
//DS12b20 pin = D5

// DS1302 CLK/SCLK --> 4
// DS1302 DAT/IO --> 3
// DS1302 RST/CE --> 2
// DS1302 VCC --> 3.3v - 5v
// DS1302 GND --> GND
--------------------------------------------------------------------------------------*/

#include <ThreeWire.h>   //real time clock
#include <RtcDS1302.h>  //real time clock
#include <OneWire.h > //DS18B20 waterproof temp sensor 
#include <DallasTemperature.h > //DS18B20 waterproof temp sensor 
#include <DHT.h > // DTH22 temp, RH sensor
//LCD SSD1306
#include <Arduino.h > 
#include <U8g2lib.h > 
#include <SPI.h > 
#include <Wire.h >

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//-----------------------------------------  set pin numbers:
# define MUX_Address 0x70 // TCA9548A Encoders address
# define ONE_WIRE_BUS 5 // Data wire for DS18B20
# define DHTPIN 6 // DTH 22 what pin we're connected to
# define DHTTYPE DHT22 // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
ThreeWire myWire(3,4,2); //RTC IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(myWire); //RTC
int relay_1 = 7; //relay
int relay_2 = 8; //relay
int targetHumidity = 50; //RH
//DS18B20
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors( & oneWire); // Pass our oneWire reference to Dallas Temperature.

// ---------------------------------- Variables
// millis and cycle
long mainCycleMillis = 0; // will store last time item was updated
long mainCycleInterval = 5000; // interval how long pause will last (milliseconds)
int cycle = 0; // every cycle last 5 sec.
int minCycle = 0; // 12 cycle = 1 min
int hourCycle = 0;
int daysCycle = 0;
// Lights options
int lightCycle = 0; // 0 is for 12/12 Flovering, 1 for 18/6, 3 for 20/4, 4 for 24/0
String lightState = String("Unknown");
String nightCycleStartHours = String(22);
String nightCycleStartMin = String(11);
/* TIME */
float minut;
float val;
float sec;
String time_year;
String time_month;
String time_day;
String time_val;
String time_min;
//relays
int relayState1 = 0;
//DTH22
int chk;
float dhtHum; //Stores humidity value
float dhtTemp; //Stores temperature value
//DS18b20
float temp_1 = 0;
float temp_2 = 0;
float temp_3 = 0;
char tmp_string[8]; // Temp string to convert numeric values to string before print to OLED display
//I2C MUX **********************************************************
void tcaselect(uint8_t i2c_bus) {
    if (i2c_bus > 7) return;
    Wire.beginTransmission(MUX_Address);
    Wire.write(1 << i2c_bus);
    Wire.endTransmission();
}
//******************************************************************
void setup() {
    DisplayInit(); // Initialize the displays
    Serial.begin(9600);
    Rtc.Begin(); //DS1302 
    sensors.begin(); //DS18B20 Temp
    dht.begin(); // DTH 22 Temp/humidity
    pinMode(relay_1, OUTPUT); //relay1
    pinMode(relay_2, OUTPUT); //relay2
}
// Initialize the displays
void DisplayInit() {
    for (int i = 0; i < 7; i++) { // get through all MUX ports
        tcaselect(i); // Loop through each connected displays on the I2C buses
        u8g2.begin(); // Initialize display
        u8g2.setFont(u8g2_font_profont15_tr); //LCD font
    } 
}
//******************************************************************
void loop() {
   
    unsigned long mainCycleCurrentMillis = millis(); // cycle starts
    if (mainCycleCurrentMillis - mainCycleMillis > mainCycleInterval) {
    
        sensors.requestTemperatures(); // Send the command to get temperature readings
        dhtHum = dht.readHumidity();
        dhtTemp = dht.readTemperature();
        temp_1 = sensors.getTempCByIndex(0); // You can have more than one DS18B20 on the same bus.
        temp_2 = sensors.getTempCByIndex(1); // You can have more than one DS18B20 on the same bus.
        RtcDateTime now = Rtc.GetDateTime();
        printDateTime(now);
        //Serial.println(digitalRead (relay_2)); //Debug
        relayState1 = digitalRead(relay_2);
        Serial.println(relayState1);
//Auto Lights
switch (lightCycle) {
    case 0:
      if (nightCycleStartHours == time_val && nightCycleStartMin == time_min ||  nightCycleStartHours < time_val && nightCycleStartHours+12 > time_val  ){ // not right statement as 00h is less than 23
        if (relayState1 == 0){
          digitalWrite(relay_2, HIGH);
          lightState = "Lights is On";         
          }
        }
      if (nightCycleStartHours+12 == time_val && nightCycleStartMin == time_min){
        if (relayState1 == 0){
          digitalWrite(relay_2, LOW);
          lightState = "Lights is Off";          
          }
        }
      break;
    case 1:
      //do something when var equals 2
      break;
    case 2:
      //do something when var equals 2
      break;
    case 3:
      //do something when var equals 2
      break;
    default:
      // if nothing else matches, do the default
      // default is optional
      break;
  }
  Serial.print(lightState);
/*  uncoment to test in serial
        
        Serial.print("Temperature is: ");
        Serial.print(temp_1);
        Serial.print(" ciklas=");
        Serial.print(cycle);
        Serial.print("\r\n");
        Serial.print("Humidity: ");
        Serial.print(dhtHum);
        Serial.print(" %, Temp: ");
        Serial.print(dhtTemp);
        Serial.println(" Celsius");
*/
        //LCD SSD1306
        tcaselect(0); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/
            u8g2.drawStr(0, 12, "Data: ");
            u8g2.setCursor(38, 12);
            u8g2.print(time_year); //years
            u8g2.setCursor(72, 12);
            u8g2.print(time_month); //month
            u8g2.setCursor(93, 12);
            u8g2.print(time_day); //days
            u8g2.drawStr(0, 30, "Laikas: ");
            u8g2.setCursor(52, 30);
            u8g2.print(time_val); //Hours
            u8g2.drawStr(65, 30, ":");
            u8g2.setCursor(70, 30);
            u8g2.print(time_min); //minute
            /************************************/
        } while (u8g2.nextPage());
        delay(50);

        tcaselect(1); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/
            u8g2.drawStr(0, 12, "Temperature");
            u8g2.drawStr(0, 25, "Enviroment: ");
            u8g2.setCursor(85, 25);
            u8g2.print(dhtTemp);
            u8g2.drawStr(110, 25, "C");
            u8g2.drawStr(0, 38, "Water 1: ");
            if (temp_1 = -127) {
                u8g2.drawStr(60, 38, "Offline");
            } else {
                u8g2.setCursor(60, 38);
                u8g2.print(temp_1);
                u8g2.drawStr(110, 38, "C");
            }
            u8g2.drawStr(0, 52, "Water 2: ");
            if (temp_2 = -127) {
                u8g2.drawStr(60, 52, "Offline");
            } else {
                u8g2.setCursor(60, 52);
                u8g2.print(temp_2);
                u8g2.drawStr(110, 52, "C");
            }
            /************************************/
        } while (u8g2.nextPage());
        delay(50);
        tcaselect(2); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/
            u8g2.drawStr(0, 12, "3rd Screen");
            u8g2.setCursor(0, 26);
                u8g2.print(lightState);
            /************************************/
        } while (u8g2.nextPage());
        delay(50);

       if (dhtHum < targetHumidity) {
            digitalWrite(relay_1, HIGH);
        } else {
            digitalWrite(relay_1, LOW);
        } 
        /******** Counters ***************/
        if (cycle < 12) {
            cycle++;
        } else if (minCycle < 60) {
            minCycle++;
            cycle = 0;
        } else {
            hourCycle++;
            minCycle = 0;
            cycle = 0;
        } // cycle counter reset
        mainCycleMillis = mainCycleCurrentMillis; // reset cycle timer
    }
}
#define countof(a) (sizeof(a) / sizeof(a[0])) //RTC
void printDateTime(const RtcDateTime& dt) //RTC
{
    char datestring[20];
    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute());
   if (dt.Year() >= 2018){ //geting only valid data and put them into string variable for future usage
    time_year = dt.Year();
    time_month = dt.Month();
    time_day = dt.Day();
    time_val = dt.Hour();
    time_min = dt.Minute();
    Serial.print(datestring);
    Serial.print(" ");}
}
