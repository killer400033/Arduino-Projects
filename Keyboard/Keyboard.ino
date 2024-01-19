#include <Keyboard.h>
#include <Wire.h>
#include <XIO.h>
#include <FastLED.h>

XIO xioBoard;
XIO xioBoard_1;

#define LCM2004A_I2C_ADR1 ((uint8_t) 0x25)
#define LCM2004A_I2C_ADR2 ((uint8_t) 0x25)

#include <jm_PCF8574.h>
jm_PCF8574 pcf8574;

byte pins[] = {XIO_PIN_A0, XIO_PIN_A1, XIO_PIN_A2, XIO_PIN_A3, XIO_PIN_A4, XIO_PIN_A5, XIO_PIN_A6, XIO_PIN_A7, XIO_PIN_B0, XIO_PIN_B1, XIO_PIN_B2, XIO_PIN_B3, XIO_PIN_B4, XIO_PIN_B5, XIO_PIN_B6, XIO_PIN_B7, XIO_PIN_C0, XIO_PIN_C1, XIO_PIN_C2, XIO_PIN_C3, XIO_PIN_C4, XIO_PIN_C5, XIO_PIN_C6, XIO_PIN_C7, XIO_PIN_D0, XIO_PIN_D1, XIO_PIN_D2, XIO_PIN_D3, XIO_PIN_D4, XIO_PIN_D5, XIO_PIN_D6, XIO_PIN_D7, XIO_PIN_E0, XIO_PIN_E1, XIO_PIN_E2, XIO_PIN_E3, XIO_PIN_E4, XIO_PIN_E5, XIO_PIN_E6, XIO_PIN_E7, 0, 1, 2, 3};
bool state[84];
int keys[] = {0xB1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0x62, 0xD1, 0xD4, 0x60, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x30, 0x2D, 0x3D, 0xB2, 0xD2, 0xB3, 0x71, 0x77, 0x65, 0x72, 0x74, 0x79, 0x75, 0x69, 0x6F, 0x70, 0x5B, 0x5D, 0x5C, 0xD3, 0xC1, 0x61, 0x73, 0x64, 0x66, 0x67, 0x68, 0x6A, 0x6B, 0x6C, 0x3B, 0x27, 0xB0, 0xD6, 0x81, 0x7A, 0x78, 0x63, 0x76, 0x62, 0x6E, 0x6D, 0x2C, 0x2E, 0x2F, 0x85, 0xDA, 0xD5, 0x80, 0x83, 0x82, 0x20, 0x86, 0x84, 0xD8, 0xD9, 0xD7};
byte fn_pins[] = {XIO_PIN_A5, XIO_PIN_A6, XIO_PIN_A7, XIO_PIN_B0, XIO_PIN_E3, XIO_PIN_B3, XIO_PIN_D1, XIO_PIN_E0, XIO_PIN_A5, XIO_PIN_C3, 2};
bool fn_state[11];

CRGB leds[84];

struct xy_pos{
float x;
float y;
};

xy_pos pos[84];

int special_keys[] = {29, 31, 44, 46, 58, 60, 71, 74, 75, 76, 77};
float special_pos[] = {28.575,28.575, 4.826,23.876, 23.749,23.749, 7.112,26.162, 30.861,30.861, 11.938,30.988, 26.162,26.162, 4.696,23.746, 23.746,23.876, 23.876,71.374, 71.374,69.088};

void setup() {
  map_keys();
  
  FastLED.addLeds<WS2811, 1, GRB>(leds, 84);
  
  for(int i = 0; i < 40; i++){
    xioBoard.xioPinMode(pins[i], INPUT_PULLUP);
  }
  for(int i = 40; i < 80; i++){
    xioBoard_1.xioPinMode(pins[i], INPUT_PULLUP);
  }
  for(int i = 80; i < 84; i++){
    pcf8574.pinMode(pins[i], INPUT_PULLUP);
  }

Wire.begin();
xioBoard.begin(HIGH, HIGH, LOW, -1, -1);  
xioBoard_1.begin(HIGH, HIGH, HIGH, -1, -1);
if (!pcf8574.begin(LCM2004A_I2C_ADR1)) pcf8574.begin(LCM2004A_I2C_ADR2);

}

int prev_time = 0;
int hue = 0;
int temp = 0;
int light_speed = 5;
int center = -51 - light_speed;
int brightness = 255;

int red = 100;
int green = 0;
int blue = 0;

int mode = 0;

void loop()
{
  if(millis() - prev_time > 50){
    switch(mode){
      case 0:
        wave();
        break;
      case 1:
        single_wave();
        break;
      default :
        break;
    }
    prev_time = millis();
  } 
   Keyboard.begin();
   {
    if(xioBoard_1.xioDigitalRead(XIO_PIN_E7) == HIGH){
      for(int i = 0; i < 40; i++){
        if(xioBoard.xioDigitalRead(pins[i]) == HIGH && state[i] == true){
          state[i] = false;
          Keyboard.release(keys[i]);
        }
        else if(xioBoard.xioDigitalRead(pins[i]) == LOW && state[i] == false){
          state[i] = true;
          Keyboard.press(keys[i]);
        }
      }
      
      for(int i = 40; i < 79; i++){
        if(xioBoard_1.xioDigitalRead(pins[i - 40]) == HIGH && state[i] == true){
          state[i] = false;
          Keyboard.release(keys[i]);
        }
        else if(xioBoard_1.xioDigitalRead(pins[i - 40]) == LOW && state[i] == false){
          state[i] = true;
          Keyboard.press(keys[i]);
        }
      }
      
      for(int i = 79; i < 83; i++){
        if(pcf8574.digitalRead(pins[i - 39]) == HIGH && state[i] == true){
          state[i] = false;
          Keyboard.release(keys[i]);
        }
        else if(pcf8574.digitalRead(pins[i - 39]) == LOW && state[i] == false){
          state[i] = true;
          Keyboard.press(keys[i]);
        }
      }
    }
    else{
      for(int i = 0; i < 5; i++){
        if(xioBoard.xioDigitalRead(fn_pins[i]) == HIGH && fn_state[i] == true){
          fn_state[i] = false;
        }
        else if(xioBoard.xioDigitalRead(fn_pins[i]) == LOW && fn_state[i] == false){
          fn_state[i] = true;
          if(i < 4){
            ode = i;
            hue = 0;
            temp = 0;
            red = 100;
            green = 0;
            blue = 0; 
            if(i == 2){
              solid();
            }
            else if(i == 3){
              for(int i = 0; i < 84; i++){
                leds[i] = CRGB(0, 0, 0);
                FastLED.show();
                delay(1);
              }
            }
          }
          else{
            if(red >= 100){
              red = -10;
            }
            red += 10;
            solid();
          }
        }
      }
      for(int i = 5; i < 10; i++){
        if(xioBoard_1.xioDigitalRead(fn_pins[i]) == HIGH && fn_state[i] == true){
          fn_state[i] = false;
        }
        else if(xioBoard_1.xioDigitalRead(fn_pins[i]) == LOW && fn_state[i] == false){
          fn_state[i] = true;
          switch(i){
            case 5:
              if(green >= 100){
                green = -10;
              }
              green += 10;
              solid();
              break;
            case 6:
              if(blue >= 100){
                blue = -10;
              }
              blue += 10;
              solid();
              break;
            case 7:
              if(brightness < 255){
                brightness += 17;
                if(mode == 2){
                  solid();
                }
              }
              break;
            case 8:
              if(light_speed < 15){
                light_speed += 1;
              }
              break;
            case 9:
              if(light_speed > 1){
                light_speed -= 1;
              }
              break;
          }
        }
      }
      if(pcf8574.digitalRead(fn_pins[10]) == HIGH && fn_state[10] == true){
        fn_state[10] = false;
      }
      else if(pcf8574.digitalRead(fn_pins[10]) == LOW && fn_state[10] == false){
        fn_state[10] = true;
        if(brightness > 0){
          brightness -= 17;
          if(mode == 2){
            solid();
          }
        }
      }
    }
  }
}

inline void single_wave(){
  for(int i = 0; i < 84; i++){
    temp = brightness - 5 * abs(center - pos[i].x);
    if(temp < 0){
      temp = 0;
    }
  leds[i] = CHSV(hue, 255, temp);
  }
  if(center > 331 + light_speed){
    center = -51 - light_speed;
    if(hue >= 250){
      hue = -5;
    }
    hue += 10;
  }
  center += light_speed;
  FastLED.show();
}

inline void wave(){
  for(int i = 0; i < 84; i++){
    hue = (pos[i].x - temp) * 255 / 286;
    leds[i] = CHSV(hue, 255, brightness);
  }
  if(temp > 280){
    temp = - light_speed;
  }
  temp += light_speed;
  FastLED.show();
}

inline void solid(){
  if(mode == 2){
    for(int i = 0; i < 84; i++){
      leds[i] = CRGB(red * brightness / 100, green * brightness / 100, blue * brightness / 100);
    }
    FastLED.show(); 
  }
}

void map_keys(){
  int level = 0;
  int special_num = 0;
  float offset = 0;
  for(int i = 1; i < 84; i++){
    if(special_keys[special_num] != i){
      if(pos[i - 1].x + 19.05 < 295){
        if(offset == 0){
          pos[i].x = pos[i - 1].x + 19.05;
          pos[i].y = level * 19.05; 
        }
        else{
          pos[i].x = pos[i - 1].x + offset;
          pos[i].y = level * 19.05; 
          offset = 0;
        }
      }
      else{
        level ++;
        pos[i].x = 0;
        pos[i].y = level * 19.05;
      }
    }
    else{
      if(pos[i - 1].x + 19.05 < 295){
        pos[i].x = pos[i - 1].x + special_pos[2 * special_num];
        pos[i].y = level * 19.05;
        offset = special_pos[2 * special_num + 1];
      }
      else{
        level ++;
        pos[i].x = special_pos[2 * special_num];
        pos[i].y = level * 19.05;
        offset = special_pos[2 * special_num + 1];
      }
      if(special_num < 10){
        special_num ++; 
      }
    }
  }
}
