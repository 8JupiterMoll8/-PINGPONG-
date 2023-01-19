void  setup_RF24L01(byte Channel){

  const uint64_t adress = 0xF0F0F0F0E1LL; 
   

  // Alternate SPI on Teensy LC because we are using the default on the Nrf24 Chip
  SPI.setMOSI(7);
  SPI.setMISO(8);
  SPI.setSCK(14);

  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {} 
  } else
  {
    Serial.println(F("Transmitter Left Racket!!"));
  }

  radio.setChannel(Channel);
  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setPayloadSize(sizeof(sensorData)); // float datatype occupies 4 bytes
  radio.setAutoAck(false);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(adress);     


  // For debugging info
   printf_begin();             // needed only once for printing details
   radio.printDetails();       // (smaller) function that prints raw register values
   radio.printPrettyDetails(); // (larger) function that prints human readable data
}
