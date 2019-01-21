#include "Wire.h"
extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}
#define TCAADDR 0x70 // TCA9548A Encoders address

#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
#define rx 3                                          //define what pin rx is going to be
#define tx 2                                          //define what pin tx is going to be
SoftwareSerial myserial(rx, tx);                      //define how the soft serial port is going to work

String inputstring = "";                              //a string to hold incoming data from the PC
String sensorstring = "";                             //a string to hold the data from the Atlas Scientific product
boolean input_string_complete = false;                //have we received all the data from the PC
boolean sensor_string_complete = false;               //have we received all the data from the Atlas Scientific product
float pH;                                             //used to hold a floating point number that is the pH

long mainCycleMillis = 0; 
long mainCycleInterval = 5000; 
int cycle = 0; 

//I2C **********************************************************
void tcaselect(uint8_t i) {
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void setup() {
  while (!Serial);
    delay(1000); 
    Wire.begin();//set up the hardware
  
  Serial.begin(9600);                                 //set baud rate for the hardware serial port_0 to 9600
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
  
  for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      Serial.print("TCA Port #"); Serial.println(t);
 
      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) continue;
      
        uint8_t data;
        if (! twi_writeTo(addr, &data, 0, 1, 1)) {
           Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");
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

  Serial.print("float PH =");
  Serial.print(pH);
  Serial.print("\r\n");

       tcaselect(3); // Selecting channel in MUX
         
  cycle++;
  mainCycleMillis = mainCycleCurrentMillis; // reset cycle time 
}
}
