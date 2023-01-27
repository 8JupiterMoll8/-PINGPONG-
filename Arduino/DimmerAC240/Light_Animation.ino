  
  void blink_Table(){
    for (int a=0; a < 9; a++){
           
      canal[a].consigne=0;
    }
    
    delay(500);
    
    for (int a=0; a < 9; a++){
           
      canal[a].consigne=50;
    }
    
  }
  
  
  void  barGraph(){
//
//     
//    int sensorReading = joystick[1];
//    //Serial.println(joystick[0]);
//     Serial.println(joystick[1]);
//    int bulbLevel = map(sensorReading, 150, 90, 9,0);
//    
//    for (int a=0; a < 9; a++){
//      if ( a < bulbLevel){
//      canal[a].consigne=map(joystick[0], 275, 75, 0, 50);
//      }
//      else{
//      canal[a].consigne=50;
//      }
//    } 

    
  }
