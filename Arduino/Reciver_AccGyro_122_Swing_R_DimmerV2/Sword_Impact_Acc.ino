void swordImpactPiezoAcceleometer (float totalAccel) {


  const int thresholdMin     = 10;  // minimum reading, avoid noise and false starts
  const int peakTrackMillis  = 65;
  const int aftershockMillis = 25; // aftershocks & vibration reject
  static float peakGyro;   // remember the highest reading
  
  // "static" variables keep their numbers between each run of this function
  static int state;  // 0=idle, 1=looking for peak, 2=ignore aftershocks
  static float peak;   // remember the highest reading
  static elapsedMillis msec;      // timer to end states 1 and 2
  
  switch (state) {
    // IDLE state: wait for any reading is above threshold.  Do not set
    // the threshold too low.  You don't want to be too sensitive to slight
    // vibration.
    case 0:
    if (totalAccel > thresholdMin)
    {
      //if (totalSwingVelocity > thresholdMin   ) { //
        Serial.println("==================");
        Serial.println("==================");
        Serial.print("begin peak track Accel at ");
        Serial.println(totalAccel);
        peakGyro= g_totalVelXYZ;
        peakGyro = map(peakGyro, 0.00, 0.40, 0, 127);
        peakGyro = constrain(peakGyro, 0, 127);
        Serial.print("TotalVel: ");
        Serial.println(peakGyro);

        peak = totalAccel;
        msec = 0;
        state = 1;
      //}
    }
      return;

    // Peak Tracking state: capture largest reading
    case 1:
      if (totalAccel > peak) {
        peak = totalAccel;
        Serial.print("Largest Acceleration ");
        Serial.println(peak);
      }
      
      if (msec >= peakTrackMillis) {
    
       
      
        Serial.print("Acceleration = ");
        Serial.println(peak);
        //int velocity = map(peak, 10, 70, 1, 127);
        int velocity = peakGyro;
        usbMIDI.sendNoteOn(72, velocity,MIDI_CH);
        
      
        msec = 0;
        state = 2;
       
      }
      return;

    // Ignore Aftershock state: wait for things to be quiet again.
    default:
      if (totalAccel > thresholdMin) {
        msec = 0; // keep resetting timer if above threshold
      } else if (msec > aftershockMillis) {
        usbMIDI.sendNoteOff(72, 0, MIDI_CH);
        state = 0; // go back to idle when
      }
  }
}
