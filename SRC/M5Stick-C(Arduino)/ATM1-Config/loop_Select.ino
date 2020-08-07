//----  loop
void loop_Select() {
  static  char  wMode = 2;

  sw_read();                                  // Switch input

  if(sw2_U == SW_ON){
    wMode ++;
    if(wMode > 5){
      wMode = 2;
    }
  }

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12);
  M5.Lcd.printf("Mode Select");
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 42);
  switch(wMode){
    case  2:  M5.Lcd.printf(" 1:Address  ");      break;
    case  3:  M5.Lcd.printf(" 2:Ai Zero  ");      break;
    case  4:  M5.Lcd.printf(" 3:Ai Adjust");      break;
    case  5:  M5.Lcd.printf(" 4:Count-CLR");      break;
  }

//----  Display
  show_info();

//----  Confirm
  if(sw1_U == SW_ON){
    digitalWrite(LED1_PIN,LED_ON);
    mMode = wMode;
    mCursor = 3;
    digitalWrite(LED1_PIN,LED_OFF);
    M5.Lcd.fillScreen(BLACK);
  }
  
  delay(DISP_T0);
}
