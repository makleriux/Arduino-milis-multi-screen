/*
 * arduino mega
DHT22 pin = D2
DS12b20 pin = D3  
*/

#include <OneWire.h> //DS18B20 waterproof temp sensor 
#include <DallasTemperature.h> //DS18B20 waterproof temp sensor 
#include <DHT.h> // DTH22 temp, RH sensor
//LCD SSD1306
//#include <SPI.h>
#include <Wire.h>
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
#include <U8glib.h> //TEMP TEST

//-----------------------------------------  set pin numbers:
#define MUX_Address 0x70 // TCA9548A Encoders address
#define ONE_WIRE_BUS 3 // Data wire for DS18B20
#define DHTPIN 2     // DTH 22 what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
//LCD SSD1306
/*#define OLED_RESET A4
Adafruit_SSD1306 display1(OLED_RESET); //1st screen
Adafruit_SSD1306 display2(OLED_RESET); //2nd screen
#define NUMFLAKES 10
*///setings
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
char tmp_string[8];  // Temp string to convert numeric values to string before print to OLED display
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_FAST);  // Fast I2C / TWI
//I2C MUX
void tcaselect(uint8_t i2c_bus) {
    if (i2c_bus > 7) return;
    Wire.beginTransmission(MUX_Address);
    Wire.write(1 << i2c_bus);
    Wire.endTransmission(); 
}


void setup() {
     Serial.begin(9600);
     sensors.begin(); //DS18B20
     dht.begin();   // DTH 22
//LCD SSD1306
DisplayInit(); // Initialize the displays 
//display1.begin(SSD1306_SWITCHCAPVCC, 0x70);
//display2.begin(SSD1306_SWITCHCAPVCC, 0x70);
pinMode(relay_1,OUTPUT);//rele
}
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
     /*
     display1.clearDisplay();
     lcdText("Vanduo ", 47, 0, 1, false);
     lcdText("Temperatura: ", 0, 10, 1, false);
     lcdText(tempToStr1, 75, 10, 1, false);
     display1.display();
     delay(200);
     tcaselect(1);    
     display2.clearDisplay();
     lcd2Text("Boxas", 50, 0, 1, false);
     lcd2Text("Dregnumas: ", 0, 10, 1, false);
     lcd2Text(tempToStr3, 65, 10, 1, false);
     lcd2Text("Temperatura: ", 0, 23, 1, false);
     lcd2Text(tempToStr2, 75, 23, 1, false);
     display2.display();
     delay(200);
*/
   tcaselect(0);

      u8g.firstPage();
      do {
        /******** Display Something *********/
        u8g.setFont(u8g_font_ncenB10);
        u8g.drawStr(0, 13, " to:");

        itoa(0, tmp_string, 10);  
        u8g.setFont(u8g_font_ncenB18);
        u8g.drawStr(25, 50, tmp_string);
        /************************************/
      } while( u8g.nextPage() );
      delay(50);
tcaselect(1);

      u8g.firstPage();
      do {
        /******** Display Something *********/
        u8g.setFont(u8g_font_ncenB10);
        u8g.drawStr(0, 13, "DADA");

        itoa(1, tmp_string, 10);  
        u8g.setFont(u8g_font_ncenB18);
        u8g.drawStr(25, 50, tmp_string);
        /************************************/
      } while( u8g.nextPage() );
      delay(50);
   tcaselect(2);

      u8g.firstPage();
      do {
        /******** Display Something *********/
        u8g.setFont(u8g_font_ncenB10);
        u8g.drawStr(0, 13, "Connected");

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
/*
 * lcdText(String text, int x, int y,int size, boolean d)
 * text is the text string to be printed
 * x is the integer x position of text
 * y is the integer y position of text
 * z is the text size, 1, 2, 3 etc
 * d is either "true" or "false". Not sure, use true
 */
 /*
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
  */
