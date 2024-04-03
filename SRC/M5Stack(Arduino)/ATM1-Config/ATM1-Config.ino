#include <M5Stack.h>

//--------------------
//  Const / Define
//--------------------
// M5Stack
//  G21 Grove-SDA
//  G22 Grove-SCL
//  G39 M5 BTN(A)
//  G38 M5 BTN(B)
//  G37 M5 BTN(C)

//--------------------
//  Private Variable
//--------------------
int   i2c_now = 1;
int   i2c_new = 1;
int   aiCur = 0;

#define ADR_MAX   (0x5F)
#define ADR_MIN   (0x50)

//---- SetUP
void setup() {
  //----  Initialize the M5Stack object
  M5.begin();        // Init M5Core
  M5.Power.begin();  // Init Power module\

  //---- GPIO / Wire
  Wire.begin();
  i2c_Scan();

  //----  LCD display
  M5.Lcd.fillScreen(BLACK);
}

//----  loop
void loop() {
  M5.update();
  if(M5.BtnA.wasPressed()){
    i2c_new --;
  }
  if(M5.BtnB.wasPressed()){
    i2c_new ++;
  }
  i2c_new = constrain(i2c_new,ADR_MIN,ADR_MAX);

  if(M5.BtnC.pressedFor(500)){                  // LONG
    M5.Lcd.setTextSize(4);
    M5.Lcd.setCursor(0, 100);
    M5.Lcd.printf("ADR Update!!");
    M5.Lcd.setCursor(0, 180);
    M5.Lcd.printf(" Reconnect!!");
    write_Address();

    byte  error = 1;
    while(error != 0){
      Wire.beginTransmission(i2c_now);
      error = Wire.endTransmission();
    }
    error = 1;
    while(error != 0){
      Wire.beginTransmission(i2c_new);
      error = Wire.endTransmission();
    }
    M5.Lcd.setCursor(0, 180);
    M5.Lcd.printf(" Update OK!");
    delay(2000);
    ESP.restart();
  }
  read_Current();
  show_info();
  delay(100);
}

//----  Display
void show_info() {
  //----  LCD表示
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("ATM1-Config");

  M5.Lcd.setCursor(0, 40);
  M5.Lcd.printf(" Cur: %2d.%02dA ", (aiCur / 100), (aiCur % 100));

  M5.Lcd.setCursor(0, 100);
  M5.Lcd.print("ADR Chanege");
  M5.Lcd.setCursor(0, 140);
  M5.Lcd.printf(" Now:%02XH->%02XH",i2c_now,i2c_new);

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(60, 220);
  M5.Lcd.print("-       +    Update");
}

//----  scan
void i2c_Scan() {
  byte  error;
  int   wAdr = 0;
  int   i;

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, 24);
  M5.Lcd.printf("i2c Scanning.");
  delay(500);

  for(i = ADR_MIN;i <= ADR_MAX;i++){
    M5.Lcd.setCursor(0, 80);
    M5.Lcd.printf(" Adr  :%02XH", i);

    Wire.beginTransmission(i);
    error = Wire.endTransmission();
    if (error == 0){
      wAdr = i;
      i = ADR_MAX + 1;
      M5.Lcd.setCursor(0, 120);
      M5.Lcd.printf(" Find!:%02XH", wAdr);
    }
  }

  if(wAdr != 0){
    i2c_now = i2c_new = wAdr;
    delay(2000);
  }else{
    M5.Lcd.setCursor(0, 120);
    M5.Lcd.printf(" Not Found!");
    delay(2000);
    ESP.restart();
  }
}

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

//--  Write Address
void  write_Address(){
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xA0);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xAA);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(0xA5);     Wire.endTransmission();   delay(100);
  Wire.beginTransmission(i2c_now);  Wire.write(0x00);   Wire.write(i2c_new);  Wire.endTransmission();   delay(100);
}
