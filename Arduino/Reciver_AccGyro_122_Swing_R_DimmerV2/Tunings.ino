
byte getOpenD()
{   

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
     
  byte D_Pentatonic[10] = {D, E, Fis, A, B, D_0, E_0, Fis_0 ,A_0 ,B_0 };

 return D_Pentatonic[random(0,5)];
  
}
