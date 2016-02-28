

/*
 * A SimbleeForMobile App for monitoring the user's pulse.
 */
#include <SimbleeForMobile.h>
#include <PulseSensorAmped.h>

#include "HeartImage.h"

const int pulseSignalPin = 6;
uint8_t   pulseLabel;
uint8_t   imageHandle;
int updateTime;

void setup() {
  SimbleeForMobile.advertisementData = "Pulse";
  SimbleeForMobile.deviceName = "Pulser1";

  // use a shared cache
  SimbleeForMobile.domain = "siever.info";
  // Set the time of the art updates
  SimbleeForMobile.baseline = "Feb 2, 2016";
  SimbleeForMobile.begin();

  PulseSensorAmped.attach(pulseSignalPin);
}


void loop() {
  SimbleeForMobile.process();  
  PulseSensorAmped.process();
  if(SimbleeForMobile.updatable && updateTime && millis()-updateTime > 250) {
    SimbleeForMobile.setVisible(imageHandle, false);
    updateTime = 0;
  }
}

void ui() {
  color_t darkgray = rgb(85,85,85);
  int width = SimbleeForMobile.screenWidth;
  int height = SimbleeForMobile.screenHeight;
    
  SimbleeForMobile.beginScreen(darkgray);
  SimbleeForMobile.drawText(width/2-45, height/2-250, "Pulse", WHITE, 45);
  SimbleeForMobile.imageSource(1, PNG, HeartImage_png, HeartImage_png_len);
  imageHandle = SimbleeForMobile.drawImage(1, 5, 140);
  SimbleeForMobile.setVisible(imageHandle, false);
  pulseLabel = SimbleeForMobile.drawText(width/2-30, height/2-25, "---", WHITE,45);
  SimbleeForMobile.endScreen();
}

void SimbleeForMobile_onConnect(void) {
  PulseSensorAmped.start();
}

void SimbleeForMobile_onDisconnect(void) {
  PulseSensorAmped.stop();  
}

// An optional function to recieve the Beats Per Minute (BPM) and Interbeat Interval (IBI) in mS
void PulseSensorAmped_data(int BPM, int IBI) {
  if(SimbleeForMobile.updatable) {
    SimbleeForMobile.updateValue(pulseLabel,BPM);
    SimbleeForMobile.setVisible(imageHandle, true);
    updateTime = millis();
  }
}

// An optional function to indicate that the pulse signal has been lost
// (Due to the nature of pulse detection via reflected light, spurios signals are possible. 
//  i.e. received data may not be valid) 
void PulseSensorAmped_lost(void) {
  if(SimbleeForMobile.updatable)
    SimbleeForMobile.updateText(pulseLabel,"---");
}

