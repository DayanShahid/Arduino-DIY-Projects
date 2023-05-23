#include <LiquidCrystal.h>
#include <U8glib.h>

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);

const uint8_t INC_BUTTON_PIN = 12;
const uint8_t DEC_BUTTON_PIN = 11;
const uint8_t NEXT_BUTTON_PIN = 7;
const uint8_t IR_PIN = 2;

uint8_t bladeCount = 1;
volatile bool startMeasurement = false;
volatile bool irState = LOW;
unsigned long t1 = 0;
unsigned long t2 = 0;
float sum_rpm;
int h=0;

String float_to_str(float num) {
  return String(num, 3);
}

void setup() {
  u8g.begin();
  pinMode(IR_PIN, INPUT);
  pinMode(INC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(DEC_BUTTON_PIN, INPUT_PULLUP);
  pinMode(NEXT_BUTTON_PIN, INPUT_PULLUP);
}
void displayStart(){
 u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(2, 40, "Waiting for Rotation!");
      } while (u8g.nextPage()); 
}

void displayBladeCount(uint8_t bladeCount) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(5, 20, "Blade Count:");
    u8g.drawStr(80, 20, float_to_str(bladeCount).c_str());
  } while (u8g.nextPage());
}

void displayRPM(float rpm,float sum ,int i,unsigned long time) {
  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(5, 20, "Blade Count:");
    u8g.drawStr(80, 20, float_to_str(bladeCount).c_str());
    u8g.drawStr(20, 30, "RPM:");
    u8g.drawStr(60, 30, float_to_str(rpm).c_str());
    u8g.drawStr(10, 40, "Time/rev:");
    u8g.drawStr(70, 40, float_to_str((float)time/bladeCount/1000).c_str());
    u8g.drawStr(105, 40, "sec");
    u8g.drawStr(5,50,"Average RPM:");
    u8g.drawStr(80, 50, float_to_str(sum/i).c_str());
  } while (u8g.nextPage());
}

void increaseBladeCount() {
  bladeCount++;
  displayBladeCount(bladeCount);
  delay(200); // Debounce delay
}

void decreaseBladeCount() {
  bladeCount--;
  displayBladeCount(bladeCount);
  delay(200); // Debounce delay
}

void nextButtonPressed() {
  startMeasurement = true;
}

void loop() {
  if (!startMeasurement) {
    displayBladeCount(bladeCount);

    if (digitalRead(INC_BUTTON_PIN) == LOW) {
      increaseBladeCount();
      displayBladeCount(bladeCount);
    }

    if (digitalRead(DEC_BUTTON_PIN) == LOW) {
      decreaseBladeCount();
      displayBladeCount(bladeCount);
    }

    if (digitalRead(NEXT_BUTTON_PIN) == LOW) {
      displayStart();
      nextButtonPressed();
       }
  } else {
    if (irState == LOW && digitalRead(IR_PIN) == HIGH) {
      irState = HIGH;
      t1 = millis();
    }

    if (irState == HIGH && digitalRead(IR_PIN) == LOW) {
      irState = LOW;
      t2 = millis();
      if((t2-t1)!=0){
      h++;
      float rpm = (float)60000/(t2 - t1)/bladeCount;
      if(rpm<20000){
      sum_rpm+=rpm;
      displayRPM(rpm, sum_rpm, h, t2 - t1);
      }

      
      }

      t1 = t2;
    }
  }
}
