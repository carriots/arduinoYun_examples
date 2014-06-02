/*
- LDR Alarm with Arduino Yun and Carriots -

This sketch sends streams to Carriots according to the values read by a LDR sensor

Created 20 May 2014
By Carriots
*/

#include <Process.h>

#define APIKEY        "98346673a637...d83045425407ab4" 	// TO BE REPLACED with your Carriots APIKEY
#define DEVICE        "defaultDevice@myusername" 						// TO BE REPLACED with your Device's ID developer

String dataString = "";  //Constant to store the payload that will be sent

const int ON = 1;      // Constant to indicate that lights are on
const int OFF = 2;     // Constant to indicate that lights are off

int LDRPin = 3;        // LDR sensor pin number
int ledPin = 7;        // Led pin number

int lights = OFF;      // Current status
int newLights = OFF;   // New status

void setup() {
  
  pinMode(ledPin, OUTPUT);     // Initialize the digital pin as an output
  
  // start serial port:
  Bridge.begin();
  Serial.begin(9600);    

  while (!Serial);   // wait for Network Serial to open
  Serial.println("Carriots client");

}

void loop() {
  int val = analogRead(LDRPin);   // Read the value from the sensor
  Serial.println(val);

  if (val > 950) {  // This value is the limit between day or night with our LDR sensor. You may need to adjust this value
    newLights = OFF;             // Now it's night. We have to turn on the LED
    digitalWrite(ledPin, HIGH);   // Turn the LED on (HIGH is the voltage level)
  }
  else {
    newLights = ON;               // Now it's day. We have to turn off the LED
    digitalWrite(ledPin, LOW);    // Turn the LED off by making the voltage LOW
  }

  if (lights != newLights) {        // Check if we have a change in status
    Serial.println(F("Send Stream"));
    lights = newLights;    // Status update and send stream
    updateData();
    sendData();
  }
  delay(500);
}

void updateData() {
  String txt = "";          // Text to send
  if ( lights == OFF ) {   // Alarm OFF
     txt = "OFF";
  } else {                  // Alarm ON
     txt = "ON";
  }
  Serial.println(txt);      // For debugging purpose only
  
  // convert the readings to a String to send it:
  dataString = "{\"protocol\":\"v1\",\"checksum\":\"\",\"device\":\"";
  dataString += DEVICE;
  dataString += "\",\"at\":\"now\",\"data\":{\"light\":";
  dataString += "\""+txt+"\"";
  dataString += "}}";
  
}

void sendData() {
  // form the string for the APIKEY header parameter:
  String apiString = "carriots.apikey: ";
  apiString += APIKEY;

  // Send the HTTP POST request
  Process carriots;
  Serial.print("nnSending data... ");
  carriots.begin("curl");
  carriots.addParameter("-k");
  carriots.addParameter("--request");
  carriots.addParameter("POST");
  carriots.addParameter("--data");
  carriots.addParameter(dataString);
  carriots.addParameter("--header");
  carriots.addParameter(apiString);
  carriots.addParameter("https://api.carriots.com/streams/");
  carriots.run();
  Serial.println("done!");

  // If there's incoming data from the net connection,
  // send it out the Serial:
  while (carriots.available() > 0) {
    char c = carriots.read();
    Serial.write(c);
  }

}
