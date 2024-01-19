#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
//U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 12, /* reset=*/ 8);
U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);

int heating_bar = 1;
int set_temp = 0;
int current_temp = 0;
String temp = "";
char temps[8];
bool heat_ready = false;
bool unloaded = false;
int av_count = 0;
int av_total = 0;
bool v_mode = false;
int resistance = 0;
bool button_lift = true;
int prev_state = LOW;

void draw(void) {
  if(unloaded == false){
    u8g2.setFont(u8g_font_6x10r);
    u8g2.drawStr(3, 16, "Current Temp:");
    u8g2.drawStr(3, 42, "Set Temp:");
  
    temp = String(current_temp) + " C";
    temp.toCharArray(temps, 8);
    u8g2.drawStr(3, 27, temps);
  
    temp = String(set_temp) + " C";
    temp.toCharArray(temps, 6);
    u8g2.drawStr(3, 53, temps);

    if(heat_ready == true){ 
      u8g2.drawBox(105, 10, 6, 22);
      u8g2.drawBox(107, 7, 2, 3);
      u8g2.drawBox(103, 30, 10, 2);
      u8g2.drawFrame(106, 30, 4, 18);
      u8g2.drawBox(107, 48, 2, 4);
      u8g2.setFont(u8g_font_4x6r);
      u8g2.drawStr(99, 59, "Ready");
    }
    else{
      u8g2.drawFrame(100, 10, 16, 44);
      u8g2.drawBox(100, (54 - heating_bar), 16, heating_bar);
    }
  }
  else{
    u8g2.drawStr(10, 10, "Power Unloaded, Shutdown Initiated!");
  }
}

void setup() {
  //Serial.begin(9600);

  pinMode(3, OUTPUT);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, OUTPUT);
  analogWrite(3, 100);
  pinMode(2, OUTPUT);
  delay(500);
  digitalWrite(2, HIGH);

  u8g2.begin();
}

void loop(void) {
  if(analogRead(A0) > 100){
    unloaded = true;
  }
  if(digitalRead(7) == LOW && button_lift == true){
    prev_state = !prev_state;
    digitalWrite(6, prev_state);
    button_lift = false;
  }
  else if(digitalRead(7) == HIGH){
     button_lift = true;
  }
  if(current_temp < set_temp + 10 && current_temp > set_temp - 10){
    heat_ready = true;
  }
  else{
    heat_ready = false;
    if(current_temp < set_temp){
      if(heating_bar != 44){
        heating_bar++;
      }
      else{
        heating_bar = 0;
      }
    }
    else{
      if(heating_bar != 0){
        heating_bar--;
      }
      else{
        heating_bar = 44;
      }
    }
  }
  
  resistance = 1507.0 / (2455.2 / analogRead(A2) - 1);
  if(av_count == 19){
    current_temp = av_total / 20.0;
    av_count = 0;
    av_total = 0;
  }
  else{
    av_total += (resistance * 3.8098) - (105.369);
    av_count ++;
  }

  if(v_mode == true){
    if(av_count == 19){
      current_temp = round(av_total * 5.0) / 100.0;
      av_count = 0;
      av_total = 0;
    }
    else{
      av_total += 
      av_count ++;
    }
  }
  u8g2.clearBuffer();
  draw();
  u8g2.sendBuffer();

  //set_temp = 5 * map(analogRead(A5), 0, 1023, 0, 102) + 290;
  set_temp = map(analogRead(A5), 0, 1023, 450, 250);
}
