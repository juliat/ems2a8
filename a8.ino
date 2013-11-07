#include <Wire.h> 
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// initialize the 7 segment number display
Adafruit_7segment matrix = Adafruit_7segment();

// pressure sensor
const int sensorPin = 0;
int pressureReading; // the analog reading from the FSR resistor driver
int sittingThreshold = 700;

// cell phone vibrator
int vibratorPin = 7;

boolean currentlySitting = false;
float overallTimeSitting = 0;
float sitStartTime = 0;
float timeBefore = 0;

boolean warningNow;

const float sitWarningThresholdInMinutes = 0.05;

#define minutesToMillisFactor 60000

void setup() {
  // get the serial port running for debugging
  Serial.begin(9600);
  Serial.println("Start");
  
  // setup the 7 segment number display
  matrix.begin(0x70);
  
  // initialize vibratorPin
  pinMode(vibratorPin, OUTPUT);
}

void loop() {
  pressureReading = analogRead(sensorPin);

  checkSitting();
  Serial.println("overallTimeSitting");
  Serial.println(overallTimeSitting);
  
  // how long have I currently been sitting?
  float currentSitDurationInMillis = millis() - sitStartTime;
  
  // warn if I've been sitting too long
  if ((currentSitDurationInMillis > sitWarningThresholdInMinutes * minutesToMillisFactor) 
     && currentlySitting) {
       warningNow = true;
  }
  
  if (warningNow == true) {
    digitalWrite(vibratorPin, HIGH);
  }
  
  // if I'm sitting, update the display, adding the time delta
  
  
  matrix.print((int)overallTimeSitting/1000, DEC);
  matrix.writeDisplay();
  delay(50);
}

void checkSitting() {
  float timeNow = millis();
  // are you sitting?
  if (pressureReading < sittingThreshold) {
      // were you sitting last time I checked?
      if (currentlySitting == false) {
        sitStartTime = millis();
        currentlySitting = true;
        Serial.println("started sitting");
      }
      else {
        Serial.println("still sitting");
        // update overall sitting time
        float thisSitDuration = timeNow - timeBefore;
        overallTimeSitting += thisSitDuration;
        
      }
  }
  // are you sitting now
  if (pressureReading > sittingThreshold) {
    // did you just get up?
    if (currentlySitting == true) {
      currentlySitting = false; 
      Serial.println("got up");
      warningNow = false;
      digitalWrite(vibratorPin, LOW);
    }
  }
  timeBefore = timeNow;
}
