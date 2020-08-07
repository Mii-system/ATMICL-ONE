//--------------------
//  Private Variable
//--------------------
int   i2c_now = 0x50;
int   i2c_new = 0x51;

char  diI2C = SW_OFF;
uint32_t  diI2Cc = 0;
int   aiCur = 0;
char  aiAdjust = 0;                 // -100 to 100 : -10.0 to +10.0%

//--  Read Current
void  read_Current(){
  int   wCur;
  byte  rxCnt;
  
  rxCnt = Wire.requestFrom(i2c_now, 3);
  if(rxCnt == 3){                   // OK
    wCur = Wire.read();             // 0:
    wCur = Wire.read();             // 1:
    wCur = wCur *256 + Wire.read(); // 2:
    aiCur = wCur;
  }
}

//--  Read Counter
void  read_Counter(){
  char  wc;
  byte  rxCnt;
  uint32_t  wCnt = 0;

  rxCnt = Wire.requestFrom(i2c_now, 6);
  if(rxCnt == 6){                     // OK
    wc = Wire.read();                 // 0:Dummy
    wc = Wire.read();                 // 1:ON/OFF
    if(wc == 0x01){
      diI2C = DI_ON;
    }else{
      diI2C = DI_OFF;
    }
    wCnt = Wire.read();                 // 2:
      wCnt = wCnt *256 + Wire.read();   // 3:
      wCnt = wCnt *256 + Wire.read();   // 4:
      wCnt = wCnt *256 + Wire.read();   // 5:
    diI2Cc = wCnt;
  }
}

//--  Write Address
void  write_Address(){
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xA0);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xAA);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xA5);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(i2c_new);  Wire.endTransmission();   delay(100);
}

//--  Write Adjust
void  write_Adjust(){
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xA0);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xF2);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(aiAdjust); Wire.endTransmission();   delay(100);
}

//--  Zero-P Req.
void  req_Zero(){
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xA0);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xF1);     Wire.endTransmission();   delay(100);
}

//--  Counter-C Req.
void  req_CountC(){
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xA0);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xF9);     Wire.endTransmission();   delay(100);
}
