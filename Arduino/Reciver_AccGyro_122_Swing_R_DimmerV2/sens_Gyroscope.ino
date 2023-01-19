void loopGyroscope()
{

    // X_Y_Z DEBUG
    // Serial.println(sensorData.gx);
    // Serial.println(sensorData.gy);
    // Serial.println(sensorData.gz);

    // TOTAL_VELOCITY
         g_totalVelXYZ = getTotalVelocityXYZ(sensorData.gx, sensorData.gy, sensorData.gz);
         //g_totalVelXYZ =  map(g_totalVelXYZ, 0.00, 0.40,0,127);
      //Serial.println(g_totalVelXYZ);

    // MAP_TOTAL_TO_MID_IVALUES
     
    // Serial.println(getTVel);
    
    //PEAK_DETECTION_TOTAL_X_Y_Z
    // peakVelocity (totalVelXYZ);
    
}



float getTotalVelocityXYZ(float gx, float gy, float gz){

  float flTotalRotation        = sqrtf( powf(gx, 2) + powf(gy, 2) + powf(gz, 2) );
  float SmoothSwingSensitivity = 100;                                                 // a total of 540 degrees per second or greater between all axes will apply the loudest swing sound
  float SwingStrength          = min( 1.0f, flTotalRotation / SmoothSwingSensitivity );
  //SwingStrength =   map(SwingStrength, 0.00, 0.40,0,127);
  return SwingStrength;
  
}
