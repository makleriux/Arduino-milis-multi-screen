 #include <OneWire.h > //DS18B20 waterproof temp sensor 
#include <DallasTemperature.h > //DS18B20 waterproof temp sensor 
#include <DHT.h > // DTH22 temp, RH sensor
//LCD SSD1306
#include <Arduino.h > 
#include <U8g2lib.h > 
#include "Wire.h"

#define TCAADDR 0x70 // TCA9548A Encoders address

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
#define rx 3                                          //define what pin rx is going to be
#define tx 2                                          //define what pin tx is going to be
SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work

# define ONE_WIRE_BUS 8 // Data wire for DS18B20
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors( & oneWire); // Pass our oneWire reference to Dallas Temperature.

# define DHTPIN 7 // DTH 22 what pin we're connected to
# define DHTTYPE DHT22 // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float pH;                                             //used to hold a floating point number that is the pH

long mainCycleMillis = 0; // will store last time item was updated
long mainCycleInterval = 5000; // interval how long pause will last (milliseconds)
int cycle = 0; // every cycle last 5 sec.

//DTH22
int chk;
float dhtHum; //Stores humidity value
float dhtTemp; //Stores temperature value

//DS18b20
float temp_1 = 0;

//I2C MUX **********************************************************
char tmp_string[8]; // Temp string to convert numeric values to string before print to OLED display
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() {                                        //set up the hardware
   while (!Serial);
   delay(1000);
 
  Wire.begin();
  Serial.begin(9600);                                 //set baud rate for the hardware serial port_0 to 9600
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC to Atlas
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  sensors.begin(); //DS18B20 Temp
  dht.begin(); // DTH 22 Temp/humidity
  
  for (uint8_t t=0; t<8; t++) {  // Initialize the displays
      tcaselect(t);
                 u8g2.firstPage();  
                do {
                     u8g2.begin();  // Initialize display                   
                   } while(  u8g2.nextPage() );
        }      
}

void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}

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
      //DEBUG
      Serial.println(pH);
      
        sensors.requestTemperatures(); // Send the command to get temperature readings
        dhtHum = dht.readHumidity();
        dhtTemp = dht.readTemperature();
        temp_1 = sensors.getTempCByIndex(0); // You can have more than one DS18B20 on the same bus.
  
  //LCD SSD1306
       tcaselect(2); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/  
            u8g2.setFont(u8g_font_ncenB18); //LCD font
            u8g2.drawStr(0, 18, "PH: ");
            u8g2.setCursor(55, 18);
            u8g2.print(pH);
            /************************************/
        } while (u8g2.nextPage());
        delay(20);
        tcaselect(6); // Selecting channel in MUX
        u8g2.firstPage();
        do {
            /******** Display Something *********/  
            u8g2.setFont(u8g_font_ncenB18); //LCD font
            u8g2.drawStr(0, 18, "PH: ");
            u8g2.setCursor(55, 18);
            u8g2.print("ph");
            /************************************/
        } while (u8g2.nextPage());
        delay(20);
  
  cycle++;
  mainCycleMillis = mainCycleCurrentMillis; // reset cycle timer

  }
  
  
}
