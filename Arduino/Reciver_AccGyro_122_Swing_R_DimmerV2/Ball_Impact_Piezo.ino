void ballImpactPiezo (int piezo) {


  const int thresholdMin     = 5;  // minimum reading, avoid noise and false starts
  const int peakTrackMillis  = 10;
  const int aftershockMillis = 65; // aftershocks & vibration reject


  // "static" variables keep their numbers between each run of this function
  static int state;  // 0=idle, 1=looking for peak, 2=ignore aftershocks
  static float peakPiezo;   // remember the highest reading
  static float peakAcc;   // remember the highest reading
  static float peakGyro;   // remember the highest reading
  static elapsedMillis msec;      // timer to end states 1 and 2

  switch (state) {
    // IDLE state: wait for any reading is above threshold.  Do not set
    // the threshold too low.  You don't want to be too sensitive to slight
    // vibration.
    case 0:
      if (piezo > thresholdMin)
      {
        Serial.println("==================");
        Serial.print("begin peak track Piezo at ");
        Serial.println(piezo);
        peakPiezo = piezo;
        peakGyro= g_totalVelXYZ;
        peakGyro = map(peakGyro, 0.00, 0.30, 0, 127);
        peakGyro = constrain(peakGyro, 0, 127);
        Serial.print("TotalVel: ");
        Serial.println(peakGyro);
        msec = 0;
        state = 1;
        //}
      }
      return;

    // Peak Tracking state: capture largest reading
    case 1:
//  
        //Serial.print("Largest Peak ");
        //Serial.println(peakPiezo);
    if (msec >= peakTrackMillis) 
    {
        
        peakPiezo = map(peakPiezo, 0, 200, 0, 127);
        peakPiezo = constrain(peakPiezo, 0, 127);
        Serial.print("Piezo: ");
        Serial.println(peakPiezo);

        peakAcc = g_totalAccXYZ;
        peakAcc = map(peakAcc, 10, 70.0, 0, 127);
        peakAcc = constrain(peakAcc, 0, 127);
        Serial.print("TotalAcc: ");
        Serial.println(peakAcc);
     
     

        Serial.print("Summ = TotalVel + TotalAcc : ");
        Serial.println(sqrtf( powf(peakGyro , 2) + powf( peakAcc , 2 )));

        Serial.print("Average = TotalVel + TotalAcc : ");
        Serial.println( (peakGyro + peakAcc)/ 2);

        //int velocity = sqrtf( powf(g_totalVelXYZ, 2) + powf(g_totalAccXYZ , 2) + powf(peakPiezo , 2 ));
        //int velocity = sqrtf( powf(g_totalVelXYZ, 2)  + powf(peakPiezo , 2 ));
        //int velocity = sqrtf( powf(g_totalAccXYZ , 2) + powf(peakPiezo , 2 ));
        //int velocity = sqrtf( powf(peakGyro , 2) + powf( peakAcc , 2 ));
          int velocity = peakGyro;
        //int velocity = peakAcc;
        //int velocity = peakPiezo;
        //int velocity = ( peakPiezo + peakGyro + peakAcc)/ 3;
         //int velocity = (peakGyro + peakAcc)/ 2;
        Serial.print("Velocity = ");
        Serial.println(velocity);
        
       usbMIDI.sendNoteOn(getOpenD(), velocity, MIDI_CH);
       CH1=map( velocity,0,36,70,5);
       
        msec = 0;
        state = 2;
    }
      return;

    // Ignore Aftershock state: wait for things to be quiet again.
    default:
      if (piezo > thresholdMin) 
      {    
        msec = 0; // keep resetting timer if above threshold
      } 
      else if (msec > aftershockMillis) 
      {
        usbMIDI.sendNoteOff(getOpenD(), 0, MIDI_CH);
         CH1=75;
        peakAcc = 0;
        peakGyro = 0;
        state = 0; // go back to idle when
      }
  }
}
