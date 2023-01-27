
// WE NEED THIS FOR  FOR IDENTYFING OUR PING PONG OBJECTS
// ON THE TRANSCIVER " pipeNum "

// CHANNEL 1 RACKET LEFT
// CHANNEL 2 TABLE  LEFT
// CHANNEL 3 NETZ 
// CHANNEL 4 TABLE  RIGHT
// CHANNEL 5 RACKET RIGHT

// CLASS EEPROM
// #include <EEPROM.h>
 
/*
void write_EEPROM(){
  
  int addr = 0; // WHERE TO STORE 0 - 512
  int val  = 2; // CHANNEL 1 RACKET LEFT
  
  EEPROM.write(addr, val);
  
}
void clear_EEPROM(){
  
   // write a 0 to all 512 bytes of the EEPROM
  for (int i = 0; i < 512; i++)
    EEPROM.write(i, 0);
   
}

void read_EEPROM(){
  
  value = EEPROM.read(address);
  
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();
  
  // advance to the next address of the EEPROM
  address = address + 1;
  
  // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
  // on address 512, wrap around to address 0
  if (address == 512)
    address = 0;
    
  delay(500);
  
}
*/
