void loopSwing()
{
    //SMOTHSWING
    static  elapsedMillis nCurrentTime;
     smoothSwing(nCurrentTime, sensorData.gx, sensorData.gy, sensorData.gz);
}

void smoothSwing (float msTime, float gx, float gy, float gz)
{

  static float SwingStrength;
  float SmoothSwingSensitivity = 75;  // a total of 540 degrees per second or greater between all axes will apply the loudest swing sound
  float flTotalRotation = sqrtf( powf(gx, 2) + powf(gy, 2) + powf(gz, 2) );
  SwingStrength = min( 1.0f, flTotalRotation / SmoothSwingSensitivity );

  //Serial.println(flTotalRotation);
  //Serial.println(SwingStrength);
  //Serial.println(map(SwingStrength,0.00, 1.00,0,127));

  static float SmoothSwingAmountA;
  static float SmoothSwingAmountB;

  float m_flMultiHumCyclePeriod = 2 * PI / 1; // a full 2PI transition takes 1 seconds
  float flCycleTime = ( msTime / 1000.0f ) * m_flMultiHumCyclePeriod;
  SmoothSwingAmountA  = (sinf(flCycleTime       ) + 1) / 2;
  SmoothSwingAmountB  = (sinf(flCycleTime + PI  ) + 1) / 2;

  // Serial.print(" ");
  // Serial.print(  SmoothSwingAmountA);
  // Serial.print(" ");
  // Serial.println(  SmoothSwingAmountB);


  float MaxGainHum     = .75; // 1 for loudest hum, or something less than 1 so swing sound is comparatively louder.
  float MinGainHum     = .25;
  float SwingOverdrive = .1;
  #define MAX_GAIN_SWING (1 - MinGainHum + SwingOverdrive)

  float SmoothSwing_GetSwingAGain = SmoothSwingAmountA * SwingStrength * MAX_GAIN_SWING;
  float SmoothSwing_GetSwingBGain = SmoothSwingAmountB * SwingStrength * MAX_GAIN_SWING;
  float SmoothSwing_GetHumGain    = MinGainHum + (1 - SmoothSwingAmountA * SwingStrength - SmoothSwingAmountB * SwingStrength) * (MaxGainHum - MinGainHum);

  SmoothSwing_GetSwingAGain = map( SmoothSwing_GetSwingAGain, 0.00, 0.5, 0, 127);
  SmoothSwing_GetSwingBGain = map( SmoothSwing_GetSwingBGain, 0.00, 0.5, 0, 127);
  SmoothSwing_GetHumGain    = map( SmoothSwing_GetHumGain   , 0.00, 0.75, 25, 100);

  
  //CH1=map( SmoothSwing_GetSwingBGain, 0.00, 0.5, 75, 5);
 //Serial.println(  SmoothSwing_GetSwingAGain);

  
  usbMIDI.sendControlChange(CC_SWING_A, SmoothSwing_GetSwingAGain , MIDI_CH);
  usbMIDI.sendControlChange(CC_SWING_B, SmoothSwing_GetSwingBGain , MIDI_CH);
 // usbMIDI.sendNoteOn(SmoothSwing_GetSwingAGain, 127 , CH);
 // usbMIDI.sendNoteOn(SmoothSwing_GetSwingBGain, 127 , CH);

}
