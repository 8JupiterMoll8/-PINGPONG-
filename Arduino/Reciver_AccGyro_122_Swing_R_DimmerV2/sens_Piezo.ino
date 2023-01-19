void loopPiezo()
{

  // HPF
  static float EMA_a = 0.3;    //initialization of EMA alpha
  static int   EMA_S = 0;        //initialization of EMA S
  static int   sensorValue = 0;

  //High Pass Filter for removing Bass
  //sensorValue = sensorData.pz;
  //EMA_S = (EMA_a * sensorValue) + ((1 - EMA_a) * EMA_S); //run the EMA
  //g_highPassPiezo = sensorValue - EMA_S;
  //g_highPassPiezo = constrain(g_highPassPiezo, 0, 1024);
 

  // DEBUG HPF
    //Serial.println(g_highPassPiezo);
//   Serial.print(",");
//   Serial.println(sensorData.pz);
}
