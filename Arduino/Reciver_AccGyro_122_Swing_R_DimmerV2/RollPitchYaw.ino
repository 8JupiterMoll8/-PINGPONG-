void loopRollPitchYaw()
{
  rollPitchYaw(sensorData.gx, sensorData.gy, sensorData.gz, sensorData.ax, sensorData.ay, sensorData.az);
}


void rollPitchYaw(float gx, float gy, float gz, float ax, float ay, float az)
{

  static float deltat;
  deltat = fusion.deltatUpdate();                       //this have to be done before calling the fusion update
  fusion.MahonyUpdate(gx, gy, gz, ax, ay, az, deltat);  //mahony is suggested if there isn't the mag and the mcu is slow

  float roll, pitch, yaw;
  roll    = fusion.getRoll ();
  pitch   = fusion.getPitch();
  yaw     = fusion.getYaw();

  yaw   = map(  yaw,   0.00,360.0, 0, 127);
  pitch = map(pitch, -90.00, 90.0, 0, 127);
  roll  = map( roll,-180.00,180.0, 0, 127);
   
  usbMIDI.sendControlChange(CC_ROLL,  roll , MIDI_CH);
  usbMIDI.sendControlChange(CC_PITCH, pitch, MIDI_CH);
  usbMIDI.sendControlChange(CC_YAW,   yaw,   MIDI_CH);
 
  
//  Serial.print(" ");
//  Serial.print(yaw);
//  Serial.print(" ");
//  Serial.print(pitch);
//  Serial.print(" ");
//  Serial.println(roll);

}
