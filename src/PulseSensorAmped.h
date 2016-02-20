/*
 * Developed by Bill Siever (http://siever.info)
 * 
 * Based on http://pulsesensor.com/
 *
 * This utilizes timer 2 and the associated interrupt
 *
 * References (this is a deriviative work): 
 *   Based on the algorithm and code available from: 
 *     https://github.com/WorldFamousElectronics/PulseSensor_Amped_Arduino
 *
 */

/*
  PulseSensorAmped.h - Collect data from an Amped Pulse Sensors
  (http://pulsesensor.com/)
*/

#ifndef PulseSensorAmped_h
#define PulseSensorAmped_h

#include "Arduino.h"
#include <inttypes.h>


#if !defined(_VARIANT_SIMBLEE_)
#error "PulseSensorAmped.h is only for use with Simblee-based boards"
#endif

// The "beats updated" callback; Called when valid pulse data is available. 
// Passed Estimated Beats Per Minute and Interbeat Interval
extern void PulseSensorAmped_data(int BPM, int IBI) __attribute__((weak));
// The "signal lost" callback; Called when no more valid pulse data is avaialble.
extern void PulseSensorAmped_lost(void) __attribute__((weak));

class PulseSensorAmpedClass
{
public:
   PulseSensorAmpedClass();
   void attach(int pin);  // Use the given pin as the analog input channel
   void start(void);      // Start measuring beats
   void stop(void);       // Stop measuring beats
   void process();        // Dispatch messages / process  
  
private:
   int analogPin;                   // Pin being used

  // Internal State variables
   volatile int BPM;                // Beats Per Minute (updated every 2ms)
   volatile int IBI;                // Inter Beat Interval
   unsigned long sampleCounter;     // used to determine pulse timing
   unsigned long lastBeatTime;      // used to find IBI
   int peak;                        // used to find peak in pulse wave, seeded
   int trough;                      // used to find trough in pulse wave, seeded
   int thresh;                      // used to find instant moment of heart beat, seeded
   int amplitude;                   // used to hold amplitude of pulse waveform, seeded

   boolean pulse;
   boolean pulseLost;               // Indicate that the pulse has been lost
   unsigned beatCount;              // Number of beats since successful pulse detection
  
   volatile boolean postUpdate;     // Post an update to one of the call back functions
  
   void update(void);               // Update internal state variables
  friend void TIMER2_Interrupt(void);
};

extern PulseSensorAmpedClass PulseSensorAmped;

#endif
