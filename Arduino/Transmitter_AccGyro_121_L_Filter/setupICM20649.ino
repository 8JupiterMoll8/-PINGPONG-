void setup_Icm20649(){

// SPI mode
#define ICM_CS 10
#define ICM_SCK 13
#define ICM_MISO 12
#define ICM_MOSI 11

  if (!icm.begin_SPI(ICM_CS, ICM_SCK, ICM_MISO, ICM_MOSI))
  {
    Serial.println("Failed to find ICM20649 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("ICM20649 Found!");

  //Accelerometer range:
  icm.setAccelRange(ICM20649_ACCEL_RANGE_30_G);
  Serial.print("Accelerometer range set to: ");
  
  switch (icm.getAccelRange()) {
    case ICM20649_ACCEL_RANGE_4_G:
      Serial.println("+-4G");
      break;
    case ICM20649_ACCEL_RANGE_8_G:
      Serial.println("+-8G");
      break;
    case ICM20649_ACCEL_RANGE_16_G:
      Serial.println("+-16G");
      break;
    case ICM20649_ACCEL_RANGE_30_G:
      Serial.println("+-30G");
      break;
  }
  
  //Gyro range:
  // icm.setGyroRange(ICM20649_GYRO_RANGE_4000_DPS);
  Serial.print("Gyro range set to: ");
  switch (icm.getGyroRange()) {
    case ICM20649_GYRO_RANGE_500_DPS:
      Serial.println("500 degrees/s");
      break;
    case ICM20649_GYRO_RANGE_1000_DPS:
      Serial.println("1000 degrees/s");
      break;
    case ICM20649_GYRO_RANGE_2000_DPS:
      Serial.println("2000 degrees/s");
      break;
    case ICM20649_GYRO_RANGE_4000_DPS:
      Serial.println("4000 degrees/s");
      break;
  }
  
  icm.setAccelRateDivisor(0);
  uint16_t accel_divisor = icm.getAccelRateDivisor();
  float accel_rate = 1125 / (1.0 + accel_divisor);

  Serial.print("Accelerometer data rate divisor set to: ");
  Serial.println(accel_divisor);
  Serial.print("Accelerometer data rate (Hz) is approximately: ");
  Serial.println(accel_rate);

  icm.setGyroRateDivisor(0.003);
  uint8_t gyro_divisor = icm.getGyroRateDivisor();
  float gyro_rate = 1100 / (1.0 + gyro_divisor);

  Serial.print("Gyro data rate divisor set to: ");
  Serial.println(gyro_divisor);
  Serial.print("Gyro data rate (Hz) is approximately: ");
  Serial.println(gyro_rate);
  Serial.println();
}
