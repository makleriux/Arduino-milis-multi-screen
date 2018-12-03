/*
 * arduino mega
DHT22 pin = D2
DS12b20 pin = D3  
*/

#include <OneWire.h> //DS18B20 waterproof temp sensor 
#include <DallasTemperature.h> //DS18B20 waterproof temp sensor 
#include <DHT.h> // DTH22 temp, RH sensor
//LCD SSD1306
#include <Wire.h>
#include <U8glib.h> //LCD LIB

//-----------------------------------------  set pin numbers:
#define MUX_Address 0x70 // TCA9548A Encoders address
#define ONE_WIRE_BUS 3 // Data wire for DS18B20
#define DHTPIN 2     // DTH 22 what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
//setings
int relay_1 = 5; //relay
int targetHumidity = 50;  //RH
//DS18B20 
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices  
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 

// ---------------------------------- Variables
// millis and cycle 
long mainCycleMillis = 0;        // will store last time item was updated
long mainCycleInterval = 1000;   // interval how long pause will last (milliseconds)
int cycle = 0;                   // every cycle last 30 sec.   
int hourCycle = 0;
int daysCycle = 0;
//DTH22
int chk;
float dhtHum;  //Stores humidity value
float dhtTemp; //Stores temperature value
//DS18b20
float temp_1 = 0;
float temp_2 = 0;
float temp_3 = 0;
//LCD SSD1306
char tmp_string[8];  // Temp string to convert numeric values to string before print to OLED display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_FAST);  // Fast I2C / TWI
//I2C MUX **********************************************************
void tcaselect(uint8_t i2c_bus) {
    if (i2c_bus > 7) return;
    Wire.beginTransmission(MUX_Address);
    Wire.write(1 << i2c_bus);
    Wire.endTransmission(); 
}
//******************************************************************
void setup() {
     Serial.begin(9600);
     sensors.begin(); //DS18B20
     dht.begin();   // DTH 22
//LCD SSD1306
DisplayInit(); // Initialize the displays 
pinMode(relay_1,OUTPUT);//rele
}
//******************************************************************
//DISPLAY SETUP
void DisplayInit(){
    for (int i = 0; i < 7; i++) {
      tcaselect(i);   // Loop through each connected displays on the I2C buses  
      u8g.firstPage();
      do {
        u8g.begin();  // Initialize display
       } while( u8g.nextPage() );
    }
}
void loop()
{
  unsigned long mainCycleCurrentMillis = millis(); // cycle starts
  if(mainCycleCurrentMillis - mainCycleMillis > mainCycleInterval) {   

    sensors.requestTemperatures(); // Send the command to get temperature readings 
    dhtHum = dht.readHumidity();
    dhtTemp= dht.readTemperature();
    temp_1= sensors.getTempCByIndex(0);// You can have more than one DS18B20 on the same bus.
    String tempToStr1 = String(temp_1); // float to string DS18B20
    String tempToStr2 = String(dhtTemp); // float to string DTH22
    String tempToStr3 = String(dhtHum); // float to string DTH22
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
// //LCD SSD1306
tcaselect(0); // Selecting channel in MUX
      u8g.firstPage();
      do {
        /******** Display Something *********/
        u8g.setFont(u8g_font_ncenB10);
        u8g.drawStr(0, 13, "1st screen"); // X, Y static words 
        itoa(0, tmp_string, 10);  // get string
        u8g.setFont(u8g_font_ncenB18);
        u8g.drawStr(25, 50, tmp_string); // X, Y draw string
        /************************************/
      } while( u8g.nextPage() );
      delay(50);
tcaselect(1); // Selecting channel in MUX
      u8g.firstPage();
      do {
        /******** Display Something *********/
        u8g.setFont(u8g_font_ncenB10);
        u8g.drawStr(0, 13, "2nd screen");
        itoa(1, tmp_string, 10);  
        u8g.setFont(u8g_font_ncenB18);
        u8g.drawStr(25, 50, tmp_string);
        /************************************/
      } while( u8g.nextPage() );
      delay(50);
tcaselect(2); // Selecting channel in MUX
      u8g.firstPage();
      do {
        /******** Display Something *********/
        u8g.setFont(u8g_font_ncenB10);
        u8g.drawStr(0, 13, "3rd screen");
        itoa(1, tmp_string, 10);  
        u8g.setFont(u8g_font_ncenB18);
        u8g.drawStr(25, 50, tmp_string);
        /************************************/
      } while( u8g.nextPage() );
      delay(50);
      
    if (dhtHum < targetHumidity) {
       digitalWrite(relay_1, HIGH);
      }
      else { digitalWrite(relay_1, LOW);}
    
    cycle++; // cycle counter
    mainCycleMillis = mainCycleCurrentMillis; // reset cycle timer
  }

}
