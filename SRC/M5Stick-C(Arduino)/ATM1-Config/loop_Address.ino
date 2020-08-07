//----  loop
void loop_Address() {
  sw_read();                                  // Switch input
  read_Current();                             // read

//----  Display
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12);
  M5.Lcd.printf(" i2c Address");
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 12 + 24);
  M5.Lcd.printf("  %02X to %02X",i2c_now,i2c_new);

//----  Key
  if(sw2_U == SW_ON){
    mCursor ++;
    if(mCursor >= 4){
      mCursor = 0;
    }
  }

  if(sw1 == SW_ON){
    if(mCursor == 0){ i2c_new ++;   }
    if(mCursor == 1){ i2c_new --;   }
    i2c_new = constrain(i2c_new,0x08,0x77);
  }

  if(sw1_L == SW_ON){
    if(mCursor == 3){
      mMode = 0;
    }
    if(mCursor == 2){
      digitalWrite(LED1_PIN,LED_ON);
      write_Address();
      digitalWrite(LED1_PIN,LED_OFF);
      mCursor = 3;    
    }
  }

//---- Display
  show_info();
  show_key();
  delay(DISP_T0);
}
