


void circle_shadow() {

  static int circle_Counter = 0;
  static int toggleOnOff    = 0;
  static  elapsedMillis sinceTest1;
  int intervall     = 25;


  if (sinceTest1 >= intervall)
  {
    sinceTest1 = sinceTest1 - intervall;
    // Serial.println(  ++intervall*10);
    toggleOnOff++;

    if (toggleOnOff == 1 && circle_Counter <= 8)
    {
        canal[circle_Counter].consigne=map(30, 0, 1024, 5, 48);
      //canal[circle_Counter].consigne_cible=map(record_3_DNXL2, 0, 1024, 5, 48);


    }

    if (toggleOnOff == 2 && circle_Counter <= 8)

    {
        canal[circle_Counter].consigne=48;
      //canal[circle_Counter].consigne_cible=48;


      toggleOnOff = 0;
      circle_Counter++;
    }

    if (circle_Counter == 8)
    {


      circle_Counter = 0;
      toggleOnOff = 0;
    }
  }
}
