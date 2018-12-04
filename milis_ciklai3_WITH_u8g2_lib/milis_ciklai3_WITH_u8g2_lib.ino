/*
 * arduino mega
DHT22 pin = D2
DS12b20 pin = D3  
*/
#include <DS1302RTC.h> //real time clock
#include <TimeLib.h>
#include <OneWire.h> //DS18B20 waterproof temp sensor 
#include <DallasTemperature.h> //DS18B20 waterproof temp sensor 
#include <DHT.h> // DTH22 temp, RH sensor
//LCD SSD1306
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//-----------------------------------------  set pin numbers:
#define MUX_Address 0x70 // TCA9548A Encoders address
#define ONE_WIRE_BUS 3 // Data wire for DS18B20
#define DHTPIN 2     // DTH 22 what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
//setings
DS1302RTC RTC(2, 3, 4); // Set pins DS1302:  CE, IO,CLK
int relay_1 = 5; //relay
int targetHumidity = 50;  //RH
//DS18B20 
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices  
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 

// ---------------------------------- Variables
// millis and cycle 
long mainCycleMillis = 0;        // will store last time item was updated
long mainCycleInterval = 5000;   // interval how long pause will last (milliseconds)
int cycle = 0;// every cycle last 5 sec.
int minCycle = 0; // 12 cycle = 1 min   
int hourCycle = 0;
int daysCycle = 0;
/* TIME */
float minut;
float val;
float sec;
//DTH22
int chk;
float timtim;
float dhtHum;  //Stores humidity value
float dhtTemp; //Stores temperature value
//DS18b20
float temp_1 = 0;
float temp_2 = 0;
float temp_3 = 0;
char tmp_string[8];  // Temp string to convert numeric values to string before print to OLED display
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
     sensors.begin(); //DS18B20
     dht.begin();   // DTH 22
pinMode(relay_1,OUTPUT);//rele
}
// Initialize the displays 
void DisplayInit(){
    for (int i = 0; i < 7; i++) { // get through all MUX ports
      tcaselect(i);   // Loop through each connected displays on the I2C buses       
        u8g2.begin();  // Initialize display
        u8g2.setFont(u8g2_font_profont15_tr); //LCD font
}
tcaselect(2); // Selecting channel in MUX
      u8g2.firstPage();  
      do {
        /******** Display Something *********/
    u8g2.drawStr(0,12,"RTC Check");
    if (RTC.haltRTC())  // Check clock oscillation  
    u8g2.drawStr(0,27, "Clock stopped!");
  else
    u8g2.drawStr(0,27, "Clock working.");
     
  if (RTC.writeEN()) // Check write-protection
   u8g2.drawStr(0,40,"Write allowed.");
  else
    u8g2.drawStr(0,40,"Write protected."); 
        /************************************/
      } while( u8g2.nextPage() );
      delay(2000);
// Setup Time library  
 setSyncProvider(RTC.get); // the function to get the time from the RTC
 u8g2.firstPage();  
      do {
  if (timeStatus() == timeSet)
    u8g2.drawStr(0,53,"Sync Ok!");
  
  else
    u8g2.drawStr(0,53,"Sync FAIL!");
    } while( u8g2.nextPage() );
      delay(2000);
} 
//RTC remove errors
void print2digits(int number) {
  if (number >= 0 && number < 10)
    Serial.write('0');
  Serial.print(number);
}

//******************************************************************
void loop()
{ tmElements_t tm;
if (! RTC.read(tm)) {
//val = print2digits(tm.Hour);
//minut = print2digits(tm.Minute);
//sec = print2digits(tm.Second);
//  Day    (tm.Day);
//  month  (tm.Month);
//  Year   (tmYearToCalendar(tm.Year));
    delay(50);  
  }
  unsigned long mainCycleCurrentMillis = millis(); // cycle starts
  if(mainCycleCurrentMillis - mainCycleMillis > mainCycleInterval) {   

    sensors.requestTemperatures(); // Send the command to get temperature readings 
    dhtHum = dht.readHumidity();
    dhtTemp= dht.readTemperature();
    temp_1= sensors.getTempCByIndex(0);// You can have more than one DS18B20 on the same bus.
    temp_2= sensors.getTempCByIndex(1);// You can have more than one DS18B20 on the same bus.
    
  // uncoment to test in serial 
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
 
//LCD SSD1306
tcaselect(0); // Selecting channel in MUX
      u8g2.firstPage();  
      do {
        /******** Display Something *********/
    u8g2.drawStr(0,12,"1st Screen");
    u8g2.drawStr(0,25, "On: ");
    u8g2.setCursor(58,25); 
    u8g2.print(minCycle); //minute
    u8g2.drawStr(80,25, "m. ");
    u8g2.setCursor(25,25);
    u8g2.print(hourCycle); // hour
    u8g2.drawStr(40,25, "h. ");
        /************************************/
      } while( u8g2.nextPage() );
      delay(50);
      
tcaselect(1); // Selecting channel in MUX
      u8g2.firstPage();  
      do {
        /******** Display Something *********/
    u8g2.drawStr(0,12,"Temperature");
    u8g2.drawStr(0,25,"Enviroment: ");
    u8g2.setCursor(85,25);
    u8g2.print(/*dhtTemp*/);
    u8g2.drawStr(110,25,"C");
    u8g2.drawStr(0,38,"Water 1: ");
    if (temp_1 = -127){u8g2.drawStr(60,38,"Offline");
    }
    else {u8g2.setCursor(60,38);
          u8g2.print(temp_1);
          u8g2.drawStr(110,38,"C");
          }        
    u8g2.drawStr(0,52,"Water 2: ");
   if (temp_2 = -127){u8g2.drawStr(60,52,"Offline");
    }
    else {u8g2.setCursor(60,52);
          u8g2.print(temp_2);
          u8g2.drawStr(110,52,"C");
          }        
        /************************************/
      } while( u8g2.nextPage() );
      delay(50);
tcaselect(2); // Selecting channel in MUX
     u8g2.firstPage();  
      do {
        /******** Display Something *********/
    u8g2.drawStr(0,12,"3rd Screen");
        /************************************/
      } while( u8g2.nextPage() );
      delay(50);
      
    if (dhtHum < targetHumidity) {
       digitalWrite(relay_1, HIGH);
      }
      else { digitalWrite(relay_1, LOW);}
      /******** Counters ***************/
   if (cycle < 12 ){ 
    cycle++; }
    else if (minCycle < 60) {
    minCycle++;
    cycle = 0;}
    else { hourCycle++;
    minCycle = 0;
    cycle = 0; }// cycle counter reset
 mainCycleMillis = mainCycleCurrentMillis; // reset cycle timer
  }
RTC.read(tm);
}
