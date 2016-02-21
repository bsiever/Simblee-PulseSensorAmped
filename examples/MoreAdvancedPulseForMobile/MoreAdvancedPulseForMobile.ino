


/*
 * A SimbleeForMobile App for monitoring the user's pulse w/ two screens and histogram.
 * 
 * This example reuires the BarGraph library: https://github.com/bsiever/SimbleeForMobile-BarGraph
 * This example requires an appropriate configured SimbleeCloud account:
 *   1. Add this device
 *   2. Place the contents of the web directory on an appropriate server.
 *   3. Create a service with a unique ESN
 */
#include <SimbleeForMobile.h>
#include <BarGraph.h>
#include <PulseSensorAmped.h>
#include <SimbleeForMobileClient.h>
#include <SimbleeCloud.h>

#include "HeartImage.h"

// your user id from the admin.simbleecloud.com (required to authorize the module)
const unsigned userID =  0xXXXXXXXX;
const unsigned destESN = 0xXXXXXXXX; // 0x00000001; 

SimbleeForMobileClient client;
SimbleeCloud cloud(&client);


const int pulseSignalPin = 6;
uint8_t   pulseLabel;
uint8_t   imageHandle;
uint8_t   histogramButton;
uint8_t   portraitButton;
uint8_t   landScapePulseLabel;

int updateTime;
int currentScreen;
BarGraph pulseHistory(240,250);

const static int PORTRAIT_VIEW = 1;
const static int LANDSCAPE_VIEW = 2;

void setup() {
  Serial.begin(9600);
  printf("Module ESN is 0x%08x\n", cloud.myESN);
  SimbleeForMobile.advertisementData = "Pulse";

  // use a shared cache
  SimbleeForMobile.domain = "siever.info";

  // Set the time of the art updates
  SimbleeForMobile.baseline = "Feb 2, 2016";
  SimbleeForMobile.begin();

  // Setup the cloud service
  cloud.userID = userID;

  PulseSensorAmped.attach(pulseSignalPin);
}


void loop() {
  // Update Pulse sensor values  
  PulseSensorAmped.process();

  // If the UI is visible, update any dependent components
  if(SimbleeForMobile.updatable) {
    switch(SimbleeForMobile.screen) {
      case PORTRAIT_VIEW:
        if(updateTime && millis()-updateTime > 250) {
          SimbleeForMobile.setVisible(imageHandle, false);
          updateTime = 0;
        }
        break;

      case LANDSCAPE_VIEW:
        break;
    }
  }

  // Update mobile device
  SimbleeForMobile.process();
  // Update cloud services
  cloud.process();
}

void drawPortrait() {
  color_t darkgray = rgb(85,85,85);
  int width = SimbleeForMobile.screenWidth;
  int height = SimbleeForMobile.screenHeight;
    
  SimbleeForMobile.beginScreen(darkgray);

  SimbleeForMobile.drawText(width/2-45, height/2-250, "Pulse", WHITE, 45);
  
  SimbleeForMobile.imageSource(1, PNG, HeartImage_png, HeartImage_png_len);
  imageHandle = SimbleeForMobile.drawImage(1, 5, 140);
  SimbleeForMobile.setVisible(imageHandle, false);

  pulseLabel = SimbleeForMobile.drawText(width/2-30, height/2-25, "---", WHITE,45);

  // Add a button to transition to the other screen (landscape)
  histogramButton = SimbleeForMobile.drawButton(0, height-40, width, "View Histogram", BLUE, TEXT_TYPE);
  SimbleeForMobile.setEvents(histogramButton, EVENT_PRESS);

  SimbleeForMobile.endScreen();
}

void drawLandscape() {
  color_t darkgray = rgb(85,85,85);
  SimbleeForMobile.beginScreen(darkgray, LANDSCAPE);

  int height = SimbleeForMobile.screenWidth;
  int width = SimbleeForMobile.screenHeight;

  landScapePulseLabel = SimbleeForMobile.drawText(0, 30, "---", WHITE,45);

  portraitButton = SimbleeForMobile.drawButton(width-100, 40, 100, "Back", BLUE, TEXT_TYPE);
  SimbleeForMobile.setEvents(portraitButton, EVENT_PRESS);

  pulseHistory.createUI(0, 80, width, height-80);

  SimbleeForMobile.endScreen();

  pulseHistory.resetData();
}

void ui() {
  cloud.connect();
  
  if(SimbleeForMobile.screen == currentScreen) 
    return;

  currentScreen = SimbleeForMobile.screen;
  
  switch(currentScreen)
  {
    case 1:
      drawPortrait();
      break;
       
    case 2:
      drawLandscape();
      break;
  }
}

void SimbleeForMobile_onConnect(void) {
  PulseSensorAmped.start();
}

void SimbleeForMobile_onDisconnect(void) {
  PulseSensorAmped.stop();  
}

// An optional function to recieve the Beats Per Minute (BPM) and Interbeat Interval (IBI) in mS
void PulseSensorAmped_data(int BPM, int IBI) {
  if(cloud.active()) {
     cloud.send(destESN, &BPM, sizeof(BPM));
  }
  if(SimbleeForMobile.updatable) {
    switch(currentScreen) {
      case PORTRAIT_VIEW: 
        SimbleeForMobile.updateValue(pulseLabel,BPM);
        SimbleeForMobile.setVisible(imageHandle, true);
        updateTime = millis();
        break;

      case LANDSCAPE_VIEW:
        pulseHistory.appendData(BPM);
        SimbleeForMobile.updateValue(landScapePulseLabel,BPM);
        break;
    }
  }
}

// An optional function to indicate that the pulse signal has been lost
// (Due to the nature of pulse detection via reflected light, spurios signals are possible. 
//  i.e. received data may not be valid) 
void PulseSensorAmped_lost(void) {
  if(SimbleeForMobile.updatable)
    switch(currentScreen) {
        case PORTRAIT_VIEW:
          SimbleeForMobile.updateText(pulseLabel,"---");
          break;
        case LANDSCAPE_VIEW: 
          pulseHistory.appendData(0);
          SimbleeForMobile.updateText(landScapePulseLabel,"---");
          break;
    }
}


void ui_event(event_t &event)
{
  switch(currentScreen) {
    case LANDSCAPE_VIEW:
      if(event.id == portraitButton) {
        SimbleeForMobile.showScreen(PORTRAIT_VIEW);
      }      
      break;

    case PORTRAIT_VIEW:
      if(event.id == histogramButton) {
        SimbleeForMobile.showScreen(LANDSCAPE_VIEW);
      }      
      break;
  }
  
}
