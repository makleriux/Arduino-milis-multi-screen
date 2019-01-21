/*
arduino due or mega

stepper STEPS, 22, 24, 23, 25  //ready but not ceded
2x DS18B20 - pin 28
DTH22 - pin 29
Relay 1,2,3,4 - 30, 31, 32, 33  // not coded no pins
// not implemented ph pumps need more 2 relays
*/


#include <OneWire.h > //DS18B20 waterproof temp sensor 
#include <DallasTemperature.h > //DS18B20 waterproof temp sensor

#include <DHT.h > // DTH22 temp, RH sensor

#include "Wire.h"

#include <Stepper.h> // stepper motor
// change this to the number of steps on your motor
#define STEPS 64
// create an instance of the stepper class using the steps and pins
Stepper stepper(STEPS, 22, 24, 23, 25);

//DISPLAY
#include <Adafruit_GFX.h>
#if defined(_GFXFONT_H_)           //are we using the new library?
#include <Fonts/FreeSans9pt7b.h>
#define ADJ_BASELINE 11            //new fonts setCursor to bottom of letter
#else
#define ADJ_BASELINE 0             //legacy setCursor to top of letter
#endif
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define RGB(r, g, b) (((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3))

#define GREY      RGB(127, 127, 127)
#define DARKGREY  RGB(64, 64, 64)
#define TURQUOISE RGB(0, 128, 128)
#define PINK      RGB(255, 128, 192)
#define OLIVE     RGB(128, 128, 0)
#define PURPLE    RGB(128, 0, 128)
#define AZURE     RGB(0, 128, 255)
#define ORANGE    RGB(255,128,64)
// END DISPLAY

# define ONE_WIRE_BUS 28 // Data wire for DS18B20
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors( & oneWire); // Pass our oneWire reference to Dallas Temperature.

# define DHTPIN 29 // DTH 22 what pin we're connected to
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
int longCycle = 0; // should be 30min
int oldLongCycle = 0;

//DTH22
int chk;
float dhtHum; //Stores humidity value
float dhtTemp; //Stores temperature value

//DS18b20
float temp_1, temp_2 = 0;

//Stepper
int stepVal = 0;
int previousStepVal = 0;

uint16_t ID; //DISPLAY ID

void setup(void)
{
   while (!Serial);
   delay(1000);
 
  Wire.begin();
  Serial.begin(9600);                                 //set baud rate for the hardware serial port_0 to 9600
  Serial3.begin(9600);                               //set baud rate for the serial port 3 to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC to Atlas
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  sensors.begin(); //DS18B20 Temp
  dht.begin(); // DTH 22 Temp/humidity
  stepper.setSpeed(200); // motor speed 
   
// screen    
    tft.reset();
    ID = tft.readID();
    Serial.print("TFT ID = 0x");  // check if its working in serial
    Serial.println(ID, HEX);
    //    if (ID == 0xD3D3) ID = 0x9481; // write-only shield
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(1); //screen rotation landscape
#if defined(_GFXFONT_H_)
    tft.setFont(&FreeSans9pt7b);   
#endif
} //END of setup
//Atlas PH
void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}
void serialEvent3() {                                 //if the hardware serial port_3 receives a char
  sensorstring = Serial3.readStringUntil(13);         //read the string until we see a <CR>
  sensor_string_complete = true;                      //set the flag used to tell if we have received a completed string from the PC
}

void loop() {
   
  if (input_string_complete == true) {                //if a string from the PC has been received in its entirety
    Serial3.print(inputstring);                       //send that string to the Atlas Scientific product
    Serial3.print('\r');                              //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }
  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    Serial.println(sensorstring);                     //send that string to the PC's serial monitor
    if (isdigit(sensorstring[0])) {                   //if the first character in the string is a digit
      pH = sensorstring.toFloat();                    //convert the string to a floating point number so it can be evaluated by the Arduino
    }
  }
  sensorstring = "";                                  //clear the string:
  sensor_string_complete = false;                     //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
  
   delay(2000); // does it need?????????????????????????<-------------------------------

   unsigned long mainCycleCurrentMillis = millis(); // cycle starts
    if (mainCycleCurrentMillis - mainCycleMillis > mainCycleInterval) { 
    
        sensors.requestTemperatures(); // Send the command to get temperature readings
        dhtHum = dht.readHumidity();
        dhtTemp = dht.readTemperature();
        temp_1 = sensors.getTempCByIndex(0); // You can have more than one DS18B20 on the same bus.
        temp_2 = sensors.getTempCByIndex(1); // You can have more than one DS18B20 on the same bus.

// OUTPUT to display
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    tft.setTextSize(1);
    // 1st line
    tft.setCursor(5, 15 + ADJ_BASELINE);        
    tft.print("Vanduo: ");
    if (temp_1 = -127){
      tft.setCursor(90, 15 + ADJ_BASELINE);
      tft.println("Sensor offfline");
      }
      else {
    tft.setCursor(90, 15 + ADJ_BASELINE);   
    tft.println(temp_1);
    tft.setCursor(160, 15 + ADJ_BASELINE);   
    tft.println("C");
    }
    tft.setCursor(250, 15 + ADJ_BASELINE);
    tft.println("PH: ");
    tft.setCursor(300, 15 + ADJ_BASELINE);   
    tft.println(pH);
    //2nd line
    tft.setCursor(15, 80 + ADJ_BASELINE);
    tft.print("Oras: ");
    tft.setCursor(90, 80 + ADJ_BASELINE);   
    tft.println(dhtTemp);
    tft.setCursor(160, 80 + ADJ_BASELINE);   
    tft.println("C");
    tft.setCursor(275, 80 + ADJ_BASELINE);   
    tft.println("Dregme: ");
    tft.setCursor(400, 80 + ADJ_BASELINE);   
    tft.println(dhtHum);
    //3nd line
    tft.setCursor(15, 130 + ADJ_BASELINE);
    tft.print("Zemes: ");
     if (temp_2 = -127){
      tft.setCursor(90, 130 + ADJ_BASELINE);
      tft.println("Sensor offfline");
      }
      else {
    tft.setCursor(90, 130 + ADJ_BASELINE);   
    tft.println(temp_1);
    tft.setCursor(160, 130 + ADJ_BASELINE);   
    tft.println("C");
    }
    tft.setCursor(275, 130 + ADJ_BASELINE);   
    tft.println("Dregme: ");
    tft.setCursor(400, 130 + ADJ_BASELINE);   
    tft.println("blabla");
     //4nd line
    tft.setCursor(15, 180 + ADJ_BASELINE);
    tft.print("Sviesos galingumas: ");
    tft.setCursor(90, 180 + ADJ_BASELINE);   
    tft.println("sepper %");
    tft.setCursor(160, 180 + ADJ_BASELINE);   
    tft.println("%");
    
    cycle++;
  mainCycleMillis = mainCycleCurrentMillis; // reset cycle timer

  if (cycle >=720){
    longCycle++;
    cycle = 0;}
}
}
