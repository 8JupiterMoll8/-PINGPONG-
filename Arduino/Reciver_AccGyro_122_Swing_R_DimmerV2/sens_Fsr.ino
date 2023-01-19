void loopForceSensitiveSensor()
{
  //Serial.println(sensorData.fsr);
  byte D_0   = 50;
  byte E_0   = 52;
  byte Fis_0 = 54;
  byte A_0   = 57;
  byte B_0   = 59;
  
  byte D   = 74;
  byte E   = 76;
  byte Fis = 78;
  byte A   = 81;
  byte B   = 83;
     
 //Serial.println(sensorData.fsr);
 usbMIDI.sendNoteOn(map(sensorData.fsr,0,900,110,80), 127 , 4);
 //delay(10);
 //CH1=map(sensorData.fsr,0,900,5,75);
//   byte D_Pentatonic[10] = {D, E, Fis, A, B, D_0, E_0, Fis_0 ,A_0 ,B_0 };
//  int rndTune = map(sensorData.fsr,0,900,0,5);
//  usbMIDI.sendNoteOn(D_Pentatonic[rndTune], 127 , 3);

}
