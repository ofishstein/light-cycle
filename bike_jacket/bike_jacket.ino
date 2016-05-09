#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "Adafruit_DRV2605.h"

/*
Flora pinout partial description:
Pin D6 is shared with Analog pin A7
Pin D9 is shared with Analog pin A9
Pin D10 is shared with Analog pin A10
Pin D12 is shared with Analog pin A11
*/
 
#define LED_PIN1       9
#define LED_PIN2       A7
#define TRIANGLE_LED_PIN A11
#define N_LEDS        8
#define SONAR_PIN A10
#define TRIANGLE_LEDS 3

Adafruit_DRV2605 drv;

//variables needed to store values
long anVolt, inches, cm;
int sum = 0; //Create sum variable so it can be averaged
int avgrange = 60; //Quantity of values to average (sample size)
int redtrigger = 1;
int backflash = 1;

unsigned long previousMillis = 0;        // will store last time LED was updated
const long interval = 1000;
 
Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(N_LEDS, LED_PIN1, NEO_RGBW);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(N_LEDS, LED_PIN2, NEO_RGBW);
Adafruit_NeoPixel triangle = Adafruit_NeoPixel(TRIANGLE_LEDS, TRIANGLE_LED_PIN, NEO_GRB);
 
void setup() {
  Serial.begin(9600); // open serial port for data transmission from sonar to pc console
  setupLEDS();
  setupHapticDriver();
}
 
void loop() {
  /* Get Sonar Range */
  getDistance();
  doMagic();
  resetDistance();
}

// SETUP METHODS
static void setupLEDS() {
  strip1.begin();
  strip2.begin();
  triangle.begin();
}

static void setupHapticDriver() {
  drv.begin();
  drv.selectLibrary(1);
  drv.setMode(DRV2605_MODE_INTTRIG);
}

// FUNCTIONAL METHODS
static void getDistance() {
    for (int i = 0; i < avgrange ; i++){
        anVolt = analogRead(SONAR_PIN) / 2;
        sum += anVolt;
        delay(5);
    }
    inches = sum / avgrange;
    cm = inches * 2.54;
    Serial.print(inches);

    Serial.print("in, ");
  
    Serial.print(cm);
  
    Serial.print("cm");
  
    Serial.println();
}

static void resetDistance() {
    sum = 0;
}

static void doHaptic(int numloop, int delay_freq) {
  for(int i = 0; i < numloop; i++) {
    // set the effect to play
    drv.setWaveform(0, 17);  // play effect 
    drv.setWaveform(1, 0);       // end waveform
    
    // play the effect!
    drv.go();
    delay(delay_freq);
  }
}

static void setStrip(uint32_t c) {
  for(uint16_t i=0; i<strip1.numPixels()+4; i++) {
      strip1.setPixelColor(i, c); // Draw new pixel
      strip2.setPixelColor(i, c);
  }

  strip1.show();
  strip2.show();
  
  if (backflash == 0) {
    doBackFlash(triangle.Color(0, 0, 0));
    backflash = 1;
  } else {
    doBackFlash(triangle.Color(255, 0, 0));
    backflash = 0;
  }
}

static void doBackFlash(uint32_t color) {
    for (uint16_t i = 0; i < triangle.numPixels()+4; i++) {
      triangle.setPixelColor(i, color);
    }
    triangle.show();
}

static void doMagic() {
  if (cm <= 250) {
    if (redtrigger == 0) {
      setStrip(strip1.Color(0, 0, 0)); 
      redtrigger = 1; 
    } else {
      setStrip(strip1.Color(0, 255, 0));
      redtrigger = 0;
    }
    doHaptic(1, 0);
  } else if (cm > 250 && cm <= 350) {
    setStrip(strip1.Color(50, 255, 0));
    doHaptic(1, 50);
  } else if (cm > 10 && cm <= 500) {
    setStrip(strip1.Color(170, 255, 0));
    doHaptic(1, 75);
  } else if (cm > 500 && cm <= 600) {
    setStrip(strip1.Color(0, 0, 255));
    doHaptic(1, 100);
  } else {
    setStrip(strip1.Color(0, 0, 0));
  }
}

