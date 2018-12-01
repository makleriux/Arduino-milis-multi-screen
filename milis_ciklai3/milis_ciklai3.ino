/*
DHT22 pin = D2
DS12b20 pin = D3  
*/

#include <OneWire.h> //DS18B20
#include <DallasTemperature.h> //DS18B20
#include <DHT.h> // DTH22
//#include <CD74HC4067.h> // MUX
//LCD SSD1306
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//-----------------------------------------  set pin numbers:
#define ONE_WIRE_BUS 3 // Data wire for DS18B20
//CD74HC4067 my_mux(12, 11, 10, 9); // MUX

#define DHTPIN 2     // DTH 22 what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
//LCD SSD1306
#define OLED_RESET A4
Adafruit_SSD1306 display1(OLED_RESET); 1st screen
Adafruit_SSD1306 display2(OLED_RESET); 2nd screen
#define NUMFLAKES 10
int relay_1 = 5; //rele
int targetHumidity = 57;  //nustatymai Dregnumas
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

void setup() {
     Serial.begin(9600);
     sensors.begin(); //DS18B20
     dht.begin();   // DTH 22
//LCD SSD1306
//my_mux.channel(0);
display1.begin(SSD1306_SWITCHCAPVCC, 0x3C);
//my_mux.channel(1);
display2.begin(SSD1306_SWITCHCAPVCC, 0x3D);
pinMode(relay_1,OUTPUT);//rele
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
  /* uncoment to test in serial 
    Serial.print("Temperature is: "); 
    Serial.print(temp_1);  
    Serial.print(" ciklas=");
    Serial.print(cycle);
    Serial.print("\r\n"); 
    Serial.print("Humidity: ");
    Serial.print(dhtHum);
    Serial.print(" %, Temp: ");
    Serial.print(dhtTemp);
    Serial.println(" Celsius"); */
// //LCD SSD1306
     
     display1.clearDisplay();
     lcdText("Vanduo ", 47, 0, 1, false);
     lcdText("Temperatura: ", 0, 10, 1, false);
     lcdText(tempToStr1, 75, 10, 1, false);
     display1.display();
     delay(200);    
     display2.clearDisplay();
     lcd2Text("Boxas", 50, 0, 1, false);
     lcd2Text("Dregnumas: ", 0, 10, 1, false);
     lcd2Text(tempToStr3, 65, 10, 1, false);
     lcd2Text("Temperatura: ", 0, 23, 1, false);
     lcd2Text(tempToStr2, 75, 23, 1, false);
     display2.display();
     delay(200);

    if (dhtHum < targetHumidity) {
       digitalWrite(relay_1, HIGH);
      }
      else { digitalWrite(relay_1, LOW);}
    
    cycle++; // cycle counter
    mainCycleMillis = mainCycleCurrentMillis; // reset cycle timer
  }

}
/*
 * lcdText(String text, int x, int y,int size, boolean d)
 * text is the text string to be printed
 * x is the integer x position of text
 * y is the integer y position of text
 * z is the text size, 1, 2, 3 etc
 * d is either "true" or "false". Not sure, use true
 */
void lcdText(String text, int x, int y,int size, boolean d) {
  display1.setTextSize(size);
  display1.setTextColor(WHITE);
  display1.setCursor(x,y);
  display1.println(text);
  if(d){
    display1.display();
  }
  delay(100);
}
void lcd2Text(String text, int x, int y,int size, boolean d) {
  display2.setTextSize(size);
  display2.setTextColor(WHITE);
  display2.setCursor(x,y);
  display2.println(text);
  if(d){
    display2.display();
  }
  delay(100);
}
