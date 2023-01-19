/* Use a Piezo sensor (percussion / drum) to send USB MIDI note on
   messages, where the "velocity" represents how hard the Piezo was
   tapped.

   Connect a Pieze sensor to analog pin A0.  This example was tested
   with Murata 7BB-27-4L0.  Almost any piezo sensor (not a buzzer with
   built-in oscillator electronics) may be used.  However, Piezo
   sensors are easily damaged by excessive heat if soldering.  It
   is highly recommended to buy a Piezo with wires already attached!

   Use a 100K resistor between A0 to GND, to give the sensor a "load".
   The value of this resistor determines how "sensitive" the circuit is.

   A pair of 1N4148 diodes are recommended to protect the analog pin.
   The first diode connects to A0 with its stripe (cathode) and the other
   side to GND.  The other diode connects its non-stripe (anode) side to
   A0, and its stripe (cathode) side to 3.3V.

   Sensitivity may also be tuned with the map() function.  Uncomment
   the Serial.print lines to see the actual analog measurements in the
   Arduino Serial Monitor.

   You must select MIDI from the "Tools > USB Type" menu

   This example code is in the public domain.
*/

#include <ResponsiveAnalogRead.h>

// define the pin you want to use
const int ANALOG_PIN = A16;

// make a ResponsiveAnalogRead object, pass in the pin, and either true or false depending on if you want sleep enabled
// enabling sleep will cause values to take less time to stop changing and potentially stop changing more abruptly,
//   where as disabling sleep will cause values to ease into their correct position smoothly and more accurately
ResponsiveAnalogRead analog(ANALOG_PIN, false);

const int channel = 10;  // General MIDI: channel 10 = percussion sounds
const int note = 38;     // General MIDI: note 38 = acoustic snare

const int thresholdMin = 55;  // minimum reading, avoid noise and false starts
const int peakTrackMillis = 12;
const int aftershockMillis = 25; // aftershocks & vibration reject


void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2500) /* wait for serial monitor */ ;
  Serial.println("Piezo Peak Capture");
}


void loop() {
  
    analog.update();

  //Serial.print(analog.getRawValue());
  //Serial.print("\t");
  //Serial.println(analog.getValue());
  int piezo = analog.getValue();
  
  peakDetect(piezo);
  // Add other tasks to loop, but avoid using delay() or waiting.
  // You need loop() to keep running rapidly to detect Piezo peaks!

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash

}


void peakDetect(int voltage) {
  // "static" variables keep their numbers between each run of this function
  static int state;  // 0=idle, 1=looking for peak, 2=ignore aftershocks
  static int peak;   // remember the highest reading
  static elapsedMillis msec; // timer to end states 1 and 2

  switch (state) {
    // IDLE state: wait for any reading is above threshold.  Do not set
    // the threshold too low.  You don't want to be too sensitive to slight
    // vibration.
    case 0:
      if (voltage > thresholdMin) {
        Serial.print("begin peak track ");
        Serial.println(voltage);
        peak = voltage;
        msec = 0;
        state = 1;
      }
      return;

    // Peak Tracking state: capture largest reading
    case 1:
      if (voltage > peak) {
        peak = voltage;     
      }
      if (msec >= peakTrackMillis) {
        Serial.print("peak = ");
        Serial.println(peak);
        int velocity = map(peak, thresholdMin, 1023, 1, 127);
        
        msec = 0;
        state = 2;
      }
      return;

    // Ignore Aftershock state: wait for things to be quiet again.
    default:
      if (voltage > thresholdMin) {
        msec = 0; // keep resetting timer if above threshold
      } else if (msec > aftershockMillis) {
        state = 0; // go back to idle when
      }
  }
}
