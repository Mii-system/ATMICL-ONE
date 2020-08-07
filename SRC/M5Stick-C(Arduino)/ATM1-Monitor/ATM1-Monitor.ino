#include <M5StickC.h>

//--------------------
//  Const / Define
//--------------------
// M5Stick-C
//  G26 CN-03
//  G36 CN-04 Input Only
//  G00 CN-05 Pull UP
//  G32 Grove-3
//  G33 Grove-4
//  G37 M5 BTN(HOME)
//  G39 M5 BTN(UP)
//  G09 IR-LED
//  G10 LED

#define CPU_FREQ  (10)          // Def:240MHz(240/160/80/40/20/10) WiFi need 80 more.

#define SW1_PIN   (37)          // M5 BTN
#define SW2_PIN   (39)          // M5 RST
#define SW_ON     (LOW)
#define SW_OFF    (HIGH)
#define DI_ON     (1)
#define DI_OFF    (0)

#define LED1_PIN  (10)
#define LED_ON    (LOW)
#define LED_OFF   (HIGH)

#define DISP_T0   (200)         // 表示周期
#define LCD_X0    (4)           // LCD X 基準
#define LCD_Y0    (2)           // LCD Y 基準

//--------------------
//  Private Variable
//--------------------
char  sw1b = SW_OFF;
char  sw1 = SW_OFF;
char  sw1_U = SW_OFF;
char  sw1_L = SW_OFF;
char  sw2b = SW_OFF;
char  sw2 = SW_OFF;
char  sw2_U = SW_OFF;
char  sw2_L = SW_OFF;

//----  DI
char  di1 = SW_OFF;
char  di1b = SW_OFF;
uint32_t  di1c = 0;

//----  AI
int   aiCur = 0;
char  aiAdjust = 0;                 // -100 to 100 : -10.0 to +10.0%

//----  i2c
int   i2c_now = 0x50;
int   i2c_new = 0x51;

//----  Mode
byte  mMode = 0;                  // Mode (Scan/ADR/ZERO-P/Adjust/CNT-C)

//---- SetUP
void setup() {
  //----  Initialize the M5StickC object
  M5.begin();
  //----  CPU周波数設定
  while (!setCpuFrequencyMhz(CPU_FREQ)) {}

  //---- GPIO / Wire
  pinMode(SW1_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  digitalWrite(LED1_PIN,LED_OFF);

  //----  LCD display
  M5.Lcd.setRotation(1);            // BTN2が上になる向き
  M5.Lcd.fillScreen(BLACK);
  M5.Axp.ScreenBreath(9);           // 液晶の明るさ設定
}

//----  loop
void loop() {
  switch(mMode){                    // Mode毎の処理
    case  0:  loop_Select();    break;
    case  1:  loop_DI();        break;
    case  2:  loop_DI();        break;
    case  3:  loop_AI();        break;
    case  4:  loop_AI();        break;
    default:  loop_Select();    break;
  }
}

//----  SW Read
void  sw_read(){
  static  long sw1Cnt = 0;
  static  long sw2Cnt = 0;
  #define SWLONG    (5)                       // Hold down time
  
  sw1_U = SW_OFF;  sw1_L = SW_OFF;
  sw2_U = SW_OFF;  sw2_L = SW_OFF;

  sw1b = sw1; sw1 = digitalRead(SW1_PIN);     // Read
  if (sw1 == SW_ON){                          // ON
    if(sw1Cnt++ == SWLONG){                   // Hold down
      sw1_L = SW_ON;
    }
    if (sw1b == SW_OFF) {                     // H Edge
      sw1_U = SW_ON;
    }
  }else{
    sw1Cnt = 0;
  }

  sw2b = sw2; sw2 = digitalRead(SW2_PIN);     // Read
  if (sw2 == SW_ON){                          // ON
    if(sw2Cnt++ == SWLONG){                   // Hold down
      sw2_L = SW_ON;
    }
    if (sw2b == SW_OFF) {                     // H Edge
      sw2_U = SW_ON;
    }
  }else{
    sw2Cnt = 0;
  }
}

//----  Display
void show_info() {
  double vbat = 0.0;                          // Battery Voltage
  int8_t bat_charge_p = 0;

  // バッテリー電圧表示
  // GetVbatData()の戻り値はバッテリー電圧のステップ数で、
  // AXP192のデータシートによると1ステップは1.1mV
  vbat = M5.Axp.GetVbatData() * 1.1 / 1000;

  // バッテリー残量表示
  // 簡易的に、線形で4.15Vで100%、2.95Vで0%とする，２乗配分
  bat_charge_p = int8_t(((vbat - 2.95) * (vbat - 2.95)) / 1.44 * 100);
  if (bat_charge_p > 100) {
    bat_charge_p = 100;
  } else if (bat_charge_p < 0) {
    bat_charge_p = 0;
  }
  //----  LCD表示
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(LCD_X0 + 0, LCD_Y0 + 0);
  M5.Lcd.printf("ATM1-Monitor");
  M5.Lcd.setCursor(LCD_X0 + 80, LCD_Y0 + 0);
  M5.Lcd.printf("B:%.2fV %3d%%", vbat, bat_charge_p);
}
