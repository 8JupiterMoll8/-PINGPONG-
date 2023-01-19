void setupRf24(byte channel)
{

  const uint64_t adress = 0xF0F0F0F0E1LL;
  // Alternate SPI on Teensy 4.1
 // SPI.setMOSI(7);
 // SPI.setMISO(8);
  //SPI.setSCK(14);

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}
  } else
  {
    Serial.println(F("Reciver Teensy LC - 122 RIGHT  Racket!!"));
  }

  radio.setChannel(channel);
  radio.setPALevel(RF24_PA_MAX);
  radio.setPayloadSize(sizeof(sensorData));
  radio.setAutoAck(false);
  radio.setDataRate(RF24_2MBPS);
  radio.openReadingPipe(0, adress);
  radio.startListening();

  // For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  radio.printPrettyDetails(); // (larger) function that prints human readable data
}

void loopRf24()
{
  static  elapsedMillis nCurrentTime;
  uint8_t pipe;

  if (radio.available(&pipe))
  {
    Serial.println(sensorData.fsr);
    loopGyroscope();
    loopAcceleometer();
    loopPiezo();

    loopForceSensitiveSensor();
    //loopRollPitchYaw();
    loopSwing();
    //swordImpactPiezoAcceleometer (g_totalAccXYZ);
    //Serial.println(sensorData.pz);
    ballImpactPiezo(map(sensorData.pz, 5, 1024, 0, 1024));



    uint8_t bytes = radio.getPayloadSize();
    radio.read(&sensorData, bytes);
  }
}
