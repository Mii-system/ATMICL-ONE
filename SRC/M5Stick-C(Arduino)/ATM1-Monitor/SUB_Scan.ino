//----  loop
void loop_Scan() {
  byte  error;
  int   wAdr = 0;
  int   i;

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12);
  M5.Lcd.printf("i2c Scanning.");
  show_info();

//  for(i = 1;i < 127;i++){
  for(i = 0x40;i < 0x60;i++){
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12 + 20);
    M5.Lcd.printf(" Adr  :%02X", i);

    pinMode(32, INPUT_PULLUP);            // SDAをプルアップする
    pinMode(33, INPUT_PULLUP);            // SCLをプルアップする
    Wire.beginTransmission(i);
    error = Wire.endTransmission();
    if (error == 0){
      digitalWrite(LED1_PIN,LED_ON);
      wAdr = i;
      i = 127;
      M5.Lcd.setTextSize(2);
      M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 32 + 20);
      M5.Lcd.printf(" Find!:%02X", wAdr);
      digitalWrite(LED1_PIN,LED_OFF);
    }
  }
  if(wAdr == 0){
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12 + 20);
    M5.Lcd.printf("Not Found!");
    M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 32 + 20);
    M5.Lcd.printf(" BTN Retry");
    sw_read();                                // Switch input
    while(sw1_U != SW_ON){
      show_info();
      sw_read();                              // Switch input
    }
    mMode = 0;
  }else{
    show_info();
    delay(2000);
    M5.Lcd.fillScreen(BLACK);
    i2c_now = i2c_new = wAdr;
  }
  pinMode(32, INPUT_PULLUP);            // SDAをプルアップする
  pinMode(33, INPUT_PULLUP);            // SCLをプルアップする
}
