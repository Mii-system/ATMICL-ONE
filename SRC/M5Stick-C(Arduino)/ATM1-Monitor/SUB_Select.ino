//----  loop
void loop_Select() {
  static  char  wMode = 1;

  sw_read();                                  // Switch input

  if(sw2_U == SW_ON){
    wMode ++;
    if(wMode > 4){
      wMode = 1;
    }
  }

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12);
  M5.Lcd.printf("Mode Select");
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 42);
  switch(wMode){
    case  1:
      M5.Lcd.printf(" 1:DI-GPIO");
      break;
    case  2:
      M5.Lcd.printf(" 2:DI-i2c ");
      break;
    case  3:
      M5.Lcd.printf(" 3:AI-GPIO");
      break;
    case  4:
      M5.Lcd.printf(" 4:AI-i2c ");
      break;
  }

//----  Display
  show_info();

//----  Confirm
  if(sw1_U == SW_ON){
    digitalWrite(LED1_PIN,LED_ON);
    mMode = wMode;
    digitalWrite(LED1_PIN,LED_OFF);

    switch(mMode){                    // Mode毎のsetup
      case  0:
        loop_Select();
        break;
      case  1:
        setup_DI_GPIO();
        break;
      case  2:
        setup_DI_i2c();
        break;
      case  3:
        setup_AI_GPIO();
        break;
      case  4:
        setup_AI_i2c();
        break;
      default:
        loop_Select();
        break;
    }
    //----  LCD display
    M5.Lcd.setRotation(1);          // BTN2が上になる向き
    M5.Lcd.fillScreen(BLACK);
    M5.Axp.ScreenBreath(8);         // 液晶の明るさ設定
  }
  
  delay(DISP_T0);
}
