//----  loop
void loop_CountC() {
  sw_read();                                  // Switch input
  read_Counter();                             // read

//----  Display
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12);
  M5.Lcd.printf("Count CLR");
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12 + 24);
  M5.Lcd.printf("CNT%10u", diI2Cc);

//----  Key
  if(sw2_U == SW_ON){
    mCursor ++;
    if(mCursor >= 4){
      mCursor = 0;
    }
  }

  if(sw1_L == SW_ON){
    if(mCursor == 3){
      mMode = 0;
    }
    if(mCursor == 2){
      digitalWrite(LED1_PIN,LED_ON);
      req_CountC();
      digitalWrite(LED1_PIN,LED_OFF);
      mCursor = 3;    
    }
  }
//---- Display
  show_info();
  show_key();
  delay(DISP_T0);
}
