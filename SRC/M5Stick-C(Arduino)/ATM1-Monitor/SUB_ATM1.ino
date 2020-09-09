//----  Define
#define CPU_FREQD (10)          // Def:240MHz(240/160/80/40/20/10) WiFi need 80 more.
#define INT_TIMD0 (3000)        // Interrupt Interval[ms]
#define INT_TIMD1 (10)          // Interrupt Interval[ms]

#define CPU_FREQA (10)          // Def:240MHz(240/160/80/40/20/10) WiFi need 80 more.
#define INT_TIMA0 (3000)        // Interrupt Interval[ms]

#define DI1_PIN   (32)          // Grove2
#define DP_PIN    (32)          // Grove2
#define CHART_MAX (14400)       // Chart Buffer

//--------------------
//  Private Variable
//--------------------
//----  Timer
hw_timer_t *timer0 = NULL;
hw_timer_t *timer1 = NULL;

long  dpStart;                    // Pulse Start(0.01A/ms)
int   cBuf[CHART_MAX];            // ChartBuf(3s * 14400 = 12H)
int   cBP = 0;                    // Buf Pointer

byte  reqDisp = 1;                // Display Req.
byte  mCur = 0;                   // Current Mode (0/1:20A/2A)
byte  mDisp = 0;                  // Display Mode (0/1:STS/稼働率)
byte  mTime = 0;                  // Time Mode (0-3:3s/30s/2m/12m)

//--------------------
//  Timer0 Interrupt
//--------------------
void IRAM_ATTR onTimer0() {
    switch(mMode){                    // Mode毎のsetup
      case  1:  cBuf[cBP] = di1;      break;
      case  2:  cBuf[cBP] = di1;      break;
      case  3:  cBuf[cBP] = aiCur;    break;
      case  4:  cBuf[cBP] = aiCur;    break;
    }
  if (cBP++ >= CHART_MAX) {       // Pointer
    cBP = 0;
  }
  reqDisp = 1;                    // Display
}

//--------------------
//  Timer1 Interrupt
//--------------------
void IRAM_ATTR onTimer1() {
  if (!digitalRead(DI1_PIN) == DI_ON) {
    if (di1b == DI_OFF) {
      di1c ++;
    }
    di1 = DI_ON;
    di1b = DI_ON;
  } else {
    di1 = DI_OFF;
    di1b = DI_OFF;
  }
}

//--------------------
//  Setup
//--------------------
//---- DI_GPIO
void setup_DI_GPIO() {
  //---- GPIO / Wire
  pinMode(DI1_PIN, INPUT_PULLUP);

  //----  CPU周波数設定
  while (!setCpuFrequencyMhz(CPU_FREQD)) {}

  //----  Timer SetUP
  timer0 = timerBegin(0,CPU_FREQD , true);            // Timer0:CPU_FREQ = 1us
  timerAttachInterrupt(timer0, &onTimer0, true);      // Interrupt Set
  timerAlarmWrite(timer0, (INT_TIMD0 * 1000), true);  // Interval 3[s]

  timer1 = timerBegin(1,CPU_FREQD , true);            // Timer1:CPU_FREQ = 1us
  timerAttachInterrupt(timer1, &onTimer1, true);      // Interrupt Set
  timerAlarmWrite(timer1, (INT_TIMD1 * 1000), true);  // Interval

  timerAlarmEnable(timer0);                           // Start an alarm
  timerAlarmEnable(timer1);                           // Start an alarm
}

//---- DI_i2c
void setup_DI_i2c() {
  //---- GPIO / Wire
  Wire.begin(32, 33);
  pinMode(32, INPUT_PULLUP);            // SDAをプルアップする
  pinMode(33, INPUT_PULLUP);            // SCLをプルアップする
  loop_Scan();

  //----  CPU周波数設定
  while (!setCpuFrequencyMhz(CPU_FREQD)) {}

  //----  Timer SetUP
  timer0 = timerBegin(0,CPU_FREQD , true);            // Timer0:CPU_FREQ = 1us
  timerAttachInterrupt(timer0, &onTimer0, true);      // Interrupt Set
  timerAlarmWrite(timer0, (INT_TIMD0 * 1000), true);  // Interval 3[s]
  timerAlarmEnable(timer0);                           // Start an alarm
}

//---- AI_GPIO
void setup_AI_GPIO() {
  //---- GPIO / Wire
  pinMode(DP_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(DP_PIN), Event_DP, CHANGE);

  //----  CPU周波数設定
  while (!setCpuFrequencyMhz(CPU_FREQA)) {}

  //----  Timer SetUP
  timer0 = timerBegin(0,CPU_FREQA , true);            // Timer0:CPU_FREQ = 1us
  timerAttachInterrupt(timer0, &onTimer0, true);      // Interrupt Set
  timerAlarmWrite(timer0, (INT_TIMA0 * 1000), true);  // Interval 3[s]
  timerAlarmEnable(timer0);                           // Start an alarm
}

//---- AI_i2c
void setup_AI_i2c() {
  //---- GPIO / Wire
  Wire.begin(32, 33);      // join i2c bus (address optional for master)
  pinMode(32, INPUT_PULLUP);            // SDAをプルアップする
  pinMode(33, INPUT_PULLUP);            // SCLをプルアップする
  loop_Scan();

  //----  CPU周波数設定
  while (!setCpuFrequencyMhz(CPU_FREQA)) {}

  //----  Timer SetUP
  timer0 = timerBegin(0,CPU_FREQA , true);            // Timer0:CPU_FREQ = 1us
  timerAttachInterrupt(timer0, &onTimer0, true);      // Interrupt Set
  timerAlarmWrite(timer0, (INT_TIMA0 * 1000), true);  // Interval 3[s]
  timerAlarmEnable(timer0);                           // Start an alarm
}

//--------------------
//  Loop
//--------------------
//----  DI
void loop_DI() {
  sw_read();                                  // Switch input
  if(sw1_U == SW_ON){   Event_SW1();  }       // Events
  if(sw2_U == SW_ON){   Event_SW2D(); }       // Events
  if((sw2_L == SW_ON) && (sw1 == SW_ON)){     // Reset
    esp_restart();
  }
  if(mMode == 2){                             // DI-i2c
      read_Counter();
  }
  //--  Display
  show_info();
  show_DI();                                  // Display
  delay(DISP_T0);
}

//----  AI
void loop_AI() {
  sw_read();                                  // Switch input
  if(sw1_U == SW_ON){   Event_SW1();  }       // Events
  if(sw2_U == SW_ON){   Event_SW2A(); }       // Events
  if((sw2_L == SW_ON) && (sw1 == SW_ON)){     // Reset
    esp_restart();
  }

  if(mMode == 4){                             // AI-i2c
    read_Current();
  }

  //--  Display
  show_info();
  show_AI();                                  // Display
  delay(DISP_T0);
}

//--------------------
//  Events
//--------------------
void  Event_SW1() {
  mTime ++;
  if(mTime == 4){
    mTime = 0;
  }
  reqDisp = 1;                                // Display
}

void  Event_SW2D() {
  mDisp ++;
  if (mDisp == 2) {
    mDisp = 0;
  }
  M5.Lcd.fillRect(0, 20, 160, 60, BLACK);     // Delete
  reqDisp = 1;                                // Display
}

void  Event_SW2A() {
  mCur ++;
  if(mCur == 2){
    mCur = 0;
  }
  reqDisp = 1;                                // Display
}

void  Event_DP() {
  long now = millis();                        // now

  if(digitalRead(DP_PIN) == SW_ON){
    dpStart = now;                            // Start
    Serial.printf("NOW:%d\n",dpStart);
  }else{
    now -= dpStart;                           // Cal.
    if(now > 2000){                           // Limit
      now = 2000;
    }
    aiCur = now;
    Serial.printf("Cur:%04d\n",aiCur);
  }
}

//--------------------
//  Display
//--------------------
//----  DI
void show_DI() {
  //  LCD表示

  if (mDisp == 0) {                                 // CNT
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 18);
    M5.Lcd.printf("C: %10u", di1c);
    if(mMode == 2){                               // DI-i2c
      M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 36);
      M5.Lcd.printf("F:%2d.%03d", (diFreq / 1000), (diFreq % 1000));
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(LCD_X0+ 100, LCD_Y0 + 44);
      M5.Lcd.printf("kHz");
    }
    M5.Lcd.setTextSize(2);
    M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 54);
    if (di1 == DI_ON) {
      M5.Lcd.printf("S: ON ");
      if(mMode == 2){                               // DI-i2c
        M5.Lcd.fillCircle(140, 60, 16, BLUE);
      }else{
        M5.Lcd.fillCircle(140, 60, 16, GREEN);
      }
    } else {
      M5.Lcd.printf("S: OFF");
      M5.Lcd.fillCircle(140, 60, 14, BLACK);
    }
  } else {                                           // Chart
    M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 40);
    M5.Lcd.setTextSize(1);
    if (di1 == DI_ON) {
      M5.Lcd.printf("ON ");
    } else {
      M5.Lcd.printf("OFF");
    }
    if (reqDisp == 1) {                              // Chart
      reqDisp = 0;
      show_chartD();
    }
  }
}

//----  chartD
void show_chartD() {
  int   i, j;
  int   bp = cBP;
  int   wStep, wSum = 0, wRun;
  double  fwRun;

  M5.Lcd.drawFastHLine(38, 79, 121, GREEN);         // Bottom
  M5.Lcd.drawFastHLine(38, 39, 121, GREEN);         // TOP

  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(140, LCD_Y0 + 27);               // Mode
  if(mMode == 1){  M5.Lcd.printf("DG ");   }        // DI-GPIO
  if(mMode == 2){  M5.Lcd.printf("i2c");   }        // DI-i2c

  M5.Lcd.setCursor(8, 39);                          // Current Mode
  switch (mTime) {                                  // X Steps
    case  0:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf(" 3s/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf("  6m");
      wStep = 1;
      break;
    case  1:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf("30s/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf("  1H");
      wStep = 10;
      break;
    case  2:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf(" 2m/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf("  4H");
      wStep = 40;
      break;
    case  3:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf(" 6m/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf(" 12H");
      wStep = 120;
      break;
  }
  for (i = 0; i < (wStep * 120); i ++){       // Loop
    wSum += cBuf[bp--];
    if (bp < 0) {                           // Limit
      bp += CHART_MAX;
    }
  }
  Serial.println(wSum);
  fwRun = wSum * 100;
  fwRun = fwRun / (wStep * 120);
  Serial.println(fwRun);
  
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 18);
  M5.Lcd.printf("RUN:%4.1f%%", fwRun);

  bp = cBP;
  for (i = 159; i >= 40; i --) {              // Loop
    wSum = 0;
    for (j = 0; j < wStep; j ++) {
      wSum += cBuf[bp--];
      if (bp < 0) {                           // Limit
        bp += CHART_MAX;
      }
    }
    wRun = wSum * 100;
    wRun = wRun / wStep;
    if (wRun == 0) {                          // Plot
      M5.Lcd.drawRect(i, 40, 1, 38, RED);
    } else if (wRun < 25) {
      M5.Lcd.drawRect(i, 40, 1, 38, PINK);
    } else if (wRun < 50) {
      M5.Lcd.drawRect(i, 40, 1, 38, ORANGE);
    } else if (wRun < 75) {
      M5.Lcd.drawRect(i, 40, 1, 38, GREENYELLOW);
    } else if (wRun < 90) {
      M5.Lcd.drawRect(i, 40, 1, 38, DARKGREEN);
    } else {
      M5.Lcd.drawRect(i, 40, 1, 38, GREEN);
    }
  }
}

//--  AI
void show_AI() {
  //  LCD表示
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(LCD_X0, LCD_Y0 + 18);
  M5.Lcd.printf("%2d.%02dA", (aiCur / 100), (aiCur % 100));
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(LCD_X0 + 80, LCD_Y0 + 24);
  M5.Lcd.printf("%3d.%02dHz", (aiFreq / 100), (aiFreq % 100));

  if (reqDisp == 1) {                         // Chart
    reqDisp = 0;
    show_chartA();
  }
}

//--  chartA
void show_chartA() {
  int   i;
  int   bp = cBP;
  int   cy, wy;

  M5.Lcd.fillRect(0,36,159,43,BLACK);               // Delete
  M5.Lcd.drawFastVLine(39,39,40,GREEN);             // V Line
  M5.Lcd.drawFastHLine(38,79,121,GREEN);            // Bottom
  M5.Lcd.drawFastHLine(38,59,121,GREEN);            // Center
  M5.Lcd.drawFastHLine(38,39,121,GREEN);            // TOP

  M5.Lcd.setTextSize(1);

  M5.Lcd.setCursor(140, LCD_Y0 + 27);               // Current Mode
  if(mMode == 3){  M5.Lcd.printf("DP ");   }        // AI-GPIO
  if(mMode == 4){  M5.Lcd.printf("i2c");   }        // AI-i2c

  M5.Lcd.setCursor(8, 39);                          // Current Mode
  if(mCur == 0){                                    // 20A
    M5.Lcd.printf("  20A");
  }else{                                            // 2A
    M5.Lcd.printf("2A   ");
  }

  switch(mTime){                                    // X Steps
    case  0:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf(" 3s/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf("  6m");
      break;
    case  1:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf("30s/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf("  1H");
      break;
    case  2:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf(" 2m/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf("  4H");
      break;
    case  3:
      M5.Lcd.setCursor(10, 60); M5.Lcd.printf(" 6m/");
      M5.Lcd.setCursor(10, 70); M5.Lcd.printf(" 12H");
      break;
  }

  for(i = 159;i >= 40;i --){                  // Loop
    if(mCur == 0){                            // 20A
      wy = (cBuf[bp]) / 50;
    }else{                                    // 2A
      wy = (cBuf[bp]) / 5;
    }
    cy = 79 - wy - 1;                         // Cal.Y
    if(cy < 39){                              // Limit
      cy = 39;
    }
    M5.Lcd.drawRect(i,cy,1,3,CYAN);           // Plot

    switch(mTime){                            // X steps
      case  0:                                // 3s/6min
        bp = bp - 1;
        break;
      case  1:                                // 30s/60min
        bp = bp - 10;
        break;
      case  2:
        bp = bp - 40;                         // 2m/4H
        break;
      case  3:
        bp = bp - 120;                        // 6m/12H
        break;
      default:
        mTime = 0;
        break;
    }
    if(bp < 0){                               // Limit
      bp += CHART_MAX;
    }
  }
}
