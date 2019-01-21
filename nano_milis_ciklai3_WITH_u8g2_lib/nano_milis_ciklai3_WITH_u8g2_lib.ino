/* -----------------------------------------------------------------------------------
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
#include "Wire.h"
#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
#define rx 2                                          //define what pin rx is going to be
#define tx 3                                          //define what pin tx is going to be

SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C
//-----------------------------------------  set pin numbers:
# define MUX_Address 0x70 // TCA9548A Encoders address
# define ONE_WIRE_BUS 6 // Data wire for DS18B20
# define DHTPIN 7 // DTH 22 what pin we're connected to
# define DHTTYPE DHT22 // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
int relay_1 = 8; //relay
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
//PH
String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float pH; 
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
uint8_t t=0;         // Port selection (0...7)
void tcaselect(uint8_t i2c_bus) {
    if (i2c_bus > 7) return;
    Wire.beginTransmission(MUX_Address);
    Wire.write(1 << i2c_bus);
    Wire.endTransmission();
}
int tempas1, tempas2, tempas3, tempas4, tempas5, tempas6, tempas7, tempas8;
//******************************************************************
void setup() {
    for (t=0; t<7; t++) {  // Initialize the displays
      tcaselect(t);
                 u8g2.firstPage();  
                do {
                     u8g2.begin();  // Initialize display
                   
                   } while(  u8g2.nextPage() );
        }      
    Serial.begin(9600);
    myserial.begin(9600);//set baud rate for the software serial port to 9600
    sensors.begin(); //DS18B20 Temp
    dht.begin(); // DTH 22 Temp/humidity
    pinMode(relay_1, OUTPUT); //relay1                                   
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);

} // end of setup 
void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}
//******************************************************************
void loop() {
   if (input_string_complete) {                        //if a string from the PC has been received in its entirety
    myserial.print(inputstring);                      //send that string to the Atlas Scientific product
    myserial.print('\r');                             //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }
  if (myserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)myserial.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }
  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    Serial.println(sensorstring);                     //send that string to the PC's serial monitor
    pH = sensorstring.toFloat();
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  }
  
    unsigned long mainCycleCurrentMillis = millis(); // cycle starts
    if (mainCycleCurrentMillis - mainCycleMillis > mainCycleInterval) {
       Serial.print(" PH =");
  Serial.print(pH);
  Serial.print("\r\n");//uncomment this section to see how to convert the pH reading from a string to a float
    
        sensors.requestTemperatures(); // Send the command to get temperature readings
        dhtHum = dht.readHumidity();
        dhtTemp = dht.readTemperature();
        temp_1 = sensors.getTempCByIndex(0); // You can have more than one DS18B20 on the same bus.
        temp_2 = sensors.getTempCByIndex(1); // You can have more than one DS18B20 on the same bus

        //LCD SSD1306
//       tcaselect(3); // Selecting channel in MUX
//        u8g2.firstPage();
//        do {
            /******** Display Something *********/  
//            u8g2.setFont(u8g_font_ncenB18); //LCD font
//            u8g2.drawStr(0, 18, "PH: ");
//            u8g2.setCursor(55, 18);
//            u8g2.print("ph");
            /************************************/
//        } while (u8g2.nextPage());
//        delay(20);

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
     //   Serial.println(voltage); //ph
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
        //Serial.print(dhtTemp);
        //Serial.println(" Celsius");    
    }
}
