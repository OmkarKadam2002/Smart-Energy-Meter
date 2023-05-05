#include <Blynk.h>

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <EmonLib.h>

#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

char auth[] = ""; // Enter your Blynk auth token here

char ssid[] = ""; // Enter your WiFi network name here
char pass[] = ""; // Enter your WiFi password here

EnergyMonitor emon;
BlynkTimer timer;

int currentPin = 34;
int zmpt101b = 35; 

int bulbPin = 15; // Enter the digital pin number that the bulb is connected to

float voltage = 0; // Variable to store voltage value
float current = 0; // Variable to store current value
float power = 0; // Variable to store power value
float kwh = 0; // Variable to store KWH value

// Declare global variables
int bulbState = 0; // Initialize with default value

int delayTime = 1000; // Delay time in milliseconds
unsigned long lastmillis = millis();

void setup() {
  Serial.begin(9600);
  pinMode(bulbPin, OUTPUT);
  emon.current(currentPin, 1.1);
  emon.voltage(zmpt101b, 190, 1.7);; 
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, myTimerEvent);
}

void loop() {
  Blynk.run();
  timer.run();
}

void myTimerEvent() {
  if(bulbState==0){// check if load is on
    Serial.print(bulbState);
    emon.calcVI(20, 2000);
    Serial.print("Vrms: ");
    Serial.print(emon.Vrms, 2);//Vrms value
    Serial.print("V");
    Serial.print("\tIrms: ");
    Serial.print(emon.Irms, 4);//Irms value
    Serial.print("A");
    Serial.print("\tPower: ");
    Serial.print(emon.apparentPower, 4);//Power Value
    Serial.print("W");
    Serial.print("\tkWh: ");
    kwh = kwh + emon.apparentPower*(millis()-lastmillis)/3600000000.0;
    Serial.print(kwh, 4);
    Serial.println("kWh");//Energy Value
    lastmillis = millis();
    Blynk.virtualWrite(V2, emon.Vrms);
    Blynk.virtualWrite(V3, emon.Irms);
    Blynk.virtualWrite(V4, emon.apparentPower);
    Blynk.virtualWrite(V5, kwh);
    if(kwh>1.0000){//Can be any value, depending on the use case
      Blynk.logEvent("electricity_consumption_event");
      Serial.println("Alert Sent Successfully! ");
    }
    else{
      Serial.print("No Alert Sent.");
    }
  }
  else{// if load is off
    Serial.println("Appliance is Switched Off...");
  }
}

// This function will be called every time the Blynk app sends a value to the Virtual Pin 1
BLYNK_WRITE(V1) {
  bulbState = param.asInt();// Get the value of the virtual pin
  digitalWrite(bulbPin, bulbState); // Set the bulb on or off based on the virtual pin value
}

