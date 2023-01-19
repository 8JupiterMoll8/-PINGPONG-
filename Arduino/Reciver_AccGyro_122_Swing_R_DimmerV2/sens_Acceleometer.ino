void loopAcceleometer()
{
  // X_Y_Z DEBUG
  // Serial.println(sensorData.ax);
  // Serial.println(sensorData.ay);
  // Serial.println(sensorData.az);

  // TOTAL_VELOCITY
  g_totalAccXYZ = getTotalAccXYZ(sensorData.ax, sensorData.ay, sensorData.az);
  g_totalAccXYZ = map( g_totalAccXYZ, 9.0, 70.0, 127, 0);
  usbMIDI.sendNoteOn(g_totalAccXYZ, 127 , 3);
  
  static float EMA_a = 0.3;    //initialization of EMA alpha
  static int   EMA_S = 0;        //initialization of EMA S
  static int   sensorValue = 0;

  //High Pass Filter for removing Bass
  sensorValue = g_totalAccXYZ;
  EMA_S = (EMA_a * sensorValue) + ((1 - EMA_a) * EMA_S); //run the EMA
  sensorValue = sensorValue - EMA_S;
  g_totalAccXYZ = constrain (sensorValue, 10, 70);

  
  //Serial.println(g_totalAccXYZ);
  //Serial.println(constrain(sensorData.ax, 0,60));

  //Serial.println(g_totalAccXYZ);
  // MAP_TOTAL_TO_MID_IVALUES

  //Serial.println(totalAccXYZ);


  //SEND_TOTALACC_XYZ_MIDI_NOTES

}

float getTotalAccXYZ(float ax, float ay, float az)
{
  float  flTotalAccel = 0;
  flTotalAccel = sqrtf( powf(ax, 2) + powf(ay, 2) + powf(az, 2) );
  return flTotalAccel;
}
