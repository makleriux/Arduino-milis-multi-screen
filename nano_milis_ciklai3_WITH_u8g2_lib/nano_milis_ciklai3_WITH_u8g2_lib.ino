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
//DHT22 pin = D6
//DS12b20 pin = D5
//SDA = A4
//SLC = A5

// no clock
--------------------------------------------------------------------------------------*/

#include <OneWire.h > //DS18B20 waterproof temp sensor 
#include <DallasTemperature.h > //DS18B20 waterproof temp sensor 
#include <DHT.h > // DTH22 temp, RH sensor
//LCD SSD1306
#include <Arduino.h > 
#include <U8g2lib.h > 
#include <SPI.h > 
#include <Wire.h >

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);  // Adafruit ESP8266/32u4/ARM Boards + FeatherWing OLED
//-----------------------------------------  set pin numbers:
# define MUX_Address 0x70 // TCA9548A Encoders address
# define ONE_WIRE_BUS 5 // Data wire for DS18B20
# define DHTPIN 6 // DTH 22 what pin we're connected to
# define DHTTYPE DHT22 // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
int relay_1 = 7; //relay
int targetHumidity = 50; //RH
//DS18B20
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors( & oneWire); // Pass our oneWire reference to Dallas Temperature.

// ---------------------------------- Variables
// millis and cycle ----------------------------------------------------------------"Need fix and shortify"
long mainCycleMillis = 0; // will store last time item was updated
long mainCycleInterval = 5000; // interval how long pause will last (milliseconds)
int cycle = 0; // every cycle last 5 sec.
int minCycle = 0; // 12 cycle = 1 min
int hourCycle = 0; // 60 = 1h
int daysCycle = 0; // 24 = day not used

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

//I2C MUX **********************************************************
char tmp_string[8]; // Temp string to convert numeric values to string before print to OLED display
void tcaselect(uint8_t i2c_bus) {
    if (i2c_bus > 7) return;
    Wire.beginTransmission(MUX_Address);
    Wire.write(1 << i2c_bus);
    Wire.endTransmission();
}
int tempas1, tempas2, tempas3, tempas4, tempas5, tempas6, tempas7, tempas8;
//******************************************************************
void setup() {
    DisplayInit(); // Initialize the displays
    Serial.begin(9600);
    sensors.begin(); //DS18B20 Temp
    dht.begin(); // DTH 22 Temp/humidity
    pinMode(relay_1, OUTPUT); //relay1
    

} // end of setup
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

        //LCD SSD1306
        tcaselect(1); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/
            u8g2.drawStr(0, 5, "Data: ");
            u8g2.drawStr(0, 20, "Laikas: ");
            /************************************/
        } while (u8g2.nextPage());
        delay(50);
        tcaselect(2); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/
            u8g2.drawStr(0, 5, "Nr. 2 ");
            u8g2.drawStr(0, 20, "Laikas: ");
            /************************************/
        } while (u8g2.nextPage());
        delay(50);

        tcaselect(5); // Selecting channel in MUX
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
        tcaselect(6); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/
            u8g2.drawStr(0, 12, "3rd Screen");
            u8g2.setCursor(0, 26);
          
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
//DEBUG
        
        //Serial.println(lightLeftToRun);
        //Serial.println("time_val*60+time_min;");
        //Serial.println(tempas1);
        //Serial.println("viskas cia");
        //Serial.println(tempas3);
        //Serial.println("24-LightStarTime"); //Debug
        //Serial.println(tempas4);
        //Serial.println("LightStarTimeMin");
        //Serial.println(tempas5);
        //Serial.println(tempas5);
        //Serial.println(tempas6);
        //Serial.print(screenLightState);
        //Serial.print("Temperature is: ");
        //Serial.print(relayState1);
        //Serial.print(" ciklas=");
        //Serial.print(cycle);
        //Serial.print("\r\n");
        //Serial.print("Humidity: ");
        //Serial.print(dhtHum);
        //Serial.print(" %, Temp: ");
        Serial.print(dhtTemp);
        //Serial.println(" Celsius");    
    }
}
