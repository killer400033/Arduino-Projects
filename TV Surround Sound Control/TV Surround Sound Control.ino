#define BLYNK_PRINT Serial


//#include <SoftwareSerial.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <SPI.h>
#include  <FastLED.h>
//#include <SPI.h>

#include <PS2Mouse.h>

#define MDATA 20 // touchpad ps/2 data pin
#define MCLK 21 // touchpad ps/2 clock pinpin

PS2Mouse mouse_one(MCLK, MDATA, STREAM);

int16_t data[3];

int last_trackpad_clear = 0;
int prev_x;
int prev_y;
int sensitivity = 5;
int total_x = 0;
int total_y = 0;
int x_max = 0;
int y_max = 0;
int x_min = 0;
int y_min = 0;
bool command = false;
bool circle = false;
int time_since_release = 0;
int dirx = 0;
int diry = 0;
bool line_taken;
float g_1 = 0;
float g_2 = 0;
bool prev_dir = 0;
bool check = false;
float volume = 0;
int speeds[3];
int curr_speed = 0;
float av_speed = 0;

int curr_led = 0;
bool curr_on_state = false;
bool curr_bass_state = false;
CRGB leds[94];
char auth[] = "..."; // Blynk API key
char ssid[] = "..."; // Wifi Name
char pass[] = "..."; // Wifi Password
int temp = 0;
int led = 0;
int x = 0;
bool on = false;
bool _bass = false;
int channel = 2;
int last_millis = 0;
BlynkTimer timer;
int ReCnctFlag;
int ReCnctCount = 0;

#define EspSerial Serial3

// Your ESP8266 baud rate:
#define ESP8266_BAUD 115200

ESP8266 wifi(&EspSerial);

void setup()
{
  SPI.begin();
//  pinMode(sumthin, OUTPUT);
//  digitalWrite(sumthing, HIGH);
  pinMode(9, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(4, HIGH);
  FastLED.addLeds<WS2812, 7, GRB>(leds, 94);
  // Debug console
  Serial.begin(9600);
  SPI.begin();

  delay(10);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

  mouse_one.initialize();
  mouse_one.set_scaling_1_1();
}

void loop()
{
  timer.run();
  if (Blynk.connected()) {
    Blynk.run();
  } else if (ReCnctFlag == 0) {
    ReCnctFlag = 1;
    Serial.println("Starting reconnection timer in 30 seconds...");
    timer.setTimeout(30000L, []() {
      ReCnctFlag = 0;
      ReCnctCount++;
      Serial.print("Attempting reconnection #");
      Serial.println(ReCnctCount);
      Blynk.connect();
    });
  }
  trackpad();
  if(millis() - last_trackpad_clear > 43200000){ //clear trackpad every 12 hours;
//    digitalWrite(sumthin, LOW);
//    delay(100);
//    digitalWrite(sumthin, HIGH);
//    delay(200);
    mouse_one.initialize();
    mouse_one.set_scaling_1_1();
  }
}

BLYNK_WRITE(V0)
{
  on = param.asInt();
  if(on == HIGH && !curr_on_state){
    digitalWrite(5, HIGH);
    startup();
    curr_on_state = true;
  }
  else if(curr_on_state){
    digitalWrite(5, LOW);
    finish_anim();
    curr_on_state = false;
  }
}

BLYNK_WRITE(V1)
{
  volume = param.asInt();
  display_volume();
}

BLYNK_WRITE(V2)
{
  channel = param.asInt();
  update_channel(channel);
}

BLYNK_WRITE(V3)
{
  _bass = param.asInt();
  if(_bass == HIGH && !curr_bass_state){
    digitalWrite(8, HIGH);
    bass(true);
    curr_bass_state = true;
  }
  else if(curr_bass_state){
    digitalWrite(8, LOW);
    bass(false);
    curr_bass_state = false;
  }
}

int calculate(int i, int rgb){ //This will graph 3 parabolas with -a that will be centered at 0, 1/3 and 2/3 of the 94 led length, with each parabola representing r,g or b value.
  switch(rgb){
    case 0:
      return constrain(50 - (((i - 63) * (i - 63)) * 0.1), 0, 50);
    break;
    case 1:
      return constrain(50 - (((i - 31) * (i - 31)) * 0.1), 0, 50);
    break;
    case 2:
      return constrain(50 - ((i * i * (i - 93) * (i - 93)) * 0.000016), 0, 50);
    break;
  }
}

int vol_calculate(int i, int rgb){ //uses 3 parabolas with -a to represent value of r g and b colours at different points in the 94 led string
  switch(rgb){
    case 0:
      return constrain(60 - (0.003 * (i - 140) * (i - 140)), 0, 50);
    break;
    case 1:
      return constrain(60 - (0.003 * (i+67) * (i+67)), 0, 50);
    break;
    case 2:
      return constrain(50 - (0.01 * (i + 50) * (i + 50)), 0, 50);
    break;
  }
}

void display_volume(){
  write_volume();
  int led_num = (volume/ 100.0) * 93;
  for (int i = 0; i <= led_num; i++) {
    if(i > 93 ){
      temp = i - 93;
    }
    else{
      temp = i;
    }
    leds[temp] = CRGB (vol_calculate(temp, 0), vol_calculate(temp, 1), vol_calculate(temp, 2));
    FastLED.show();
    delay(3);
  }
  delay(2000);
  for(int i = led_num; i > -1; i--){
    leds[i] = CRGB (0, 0, 0);
    FastLED.show();
    delay(5);
  }
}

void startup(){
  for (int i = 0; i <= 93; i++) {
    leds[i] = CRGB (0, 0, 0);
    FastLED.show();
  }

  while(x < 2){
    if(led > 4){
      leds[led - 5] = CRGB (0, 0, 0); 
    }
    if(led > 93){
      leds[led - 94] = CRGB (0, 0, 255);
    }
    else{
      leds[led] = CRGB (0, 0, 255);
    }
    FastLED.show();
    delay(2);
    led ++;
    if(led > 97){
      led = 5;
      x++;
    }
  }
  for (int i = 0; i <= 93; i++) {
    leds[i] = CRGB (calculate(i, 0), calculate(i, 1), calculate(i, 2));
    FastLED.show();
    delay(3);
  }
  
  for(int i = 0; i < 93; i++){
    leds[i] = CRGB (0, 0, 0);
    FastLED.show();
    delay(3);
  }

  delay(200);
  display_volume();
  x = 0;
}

void finish_anim(){
  for (int i = 0; i <= 93; i++) {
    leds[i] = CRGB (0, 0, 0);
    FastLED.show();
  }
  for (int i = 92; i > -1; i--) {
    leds[i] = CRGB (calculate(i, 0), calculate(i, 1), calculate(i, 2));
    FastLED.show();
    delay(3);
  }
  
  for(int i = 92; i > -1; i--){
    leds[i] = CRGB (0, 0, 0);
    FastLED.show();
    delay(3);
  }
}

void update_channel(int channel){
  if(channel == 2){
    digitalWrite(9, LOW);
  }
  else if(channel == 4){
    digitalWrite(9, HIGH);
  }
}

void write_volume(){
  digitalWrite(4, LOW);
  SPI.transfer(0);
  int spi = 1 + volume * volume * volume * 1.5 / 5905.511811;
  SPI.transfer(spi);
  digitalWrite(4, HIGH);
}

void bass(bool on){
  if(on == true){
    for(int i = 0; i < 93; i++){
      leds[i] = CRGB (255, 0, 0);
      FastLED.show();
      delay(3);
    }
    for(int i = 0; i < 93; i++){
      leds[i] = CRGB (0, 0, 0);
      FastLED.show();
      delay(3);
    }
  }
  else{
    for(int i = 92; i > -1; i--){
      leds[i] = CRGB (255, 0, 0);
      FastLED.show();
      delay(3);
    }
    for(int i = 92; i > -1; i--){
      leds[i] = CRGB (0, 0, 0);
      FastLED.show();
      delay(3);
    }
  }
}

inline void trackpad(){
  mouse_one.report(data);
  if(prev_x != data[1] && prev_y != data[2]){
    if(abs(data[1]) > sensitivity || abs(data[2]) > sensitivity){
      command = true;
      total_x += data[1];
      total_y += data[2];
      if(total_x > x_max){
        x_max = total_x;
      }
      else if(total_x < x_min){
        x_min = total_x;
      }
      if(total_y > y_max){
        y_max = total_y;
      }
      else if(total_y < y_min){
        y_min = total_y;
      }
      if(x_max - x_min + y_max - y_min > 300 && x_max + x_min + y_max + y_min < 100 && !circle){
        circle = true;
        begin_led_circle();
      }
      if(circle){
        circular_move();
        curr_speed = data[1] * data[1] + data[2] * data[2];
      }
    }
    prev_x = data[1];
    prev_y = data[2];
    time_since_release = millis();
  }
  else if(millis() - time_since_release > 500){
    if(command && !circle){
      command = false;
      if(total_x > 50 && !curr_on_state){
        digitalWrite(5, HIGH);
        Blynk.virtualWrite(0,HIGH);
        startup();
        curr_on_state = true;
      }
      else if(total_x < -50 && curr_on_state){
        digitalWrite(5, LOW);
        Blynk.virtualWrite(0,LOW);
        finish_anim();
        curr_on_state = false;
      }
      else if(total_y > 50 && !curr_bass_state){
        digitalWrite(8, HIGH);
        Blynk.virtualWrite(3,HIGH);
        bass(true);
        curr_bass_state = true;
      }
      else if(total_y < -50 && curr_bass_state){
        digitalWrite(8, LOW);
        Blynk.virtualWrite(3,LOW);
        bass(false);
        curr_bass_state = false;
      }
    }

    if(command && circle){
      Blynk.virtualWrite(1, int(volume));
      end_led_circle();
    }
    total_x = 0;
    total_y = 0;
    y_min = 0;
    x_min = 0;
    y_max = 0;
    x_max = 0;
    circle = false;
    dirx = 0;
    diry = 0;
  }
}

inline void circular_move(){
  speeds[0] = speeds[1];
  speeds[1] = speeds[2];
  speeds[2] = curr_speed;
  av_speed = float(speeds[0] + speeds[1] + speeds[2]) / 4000.0;
  if(prev_dir && volume <= 100 - av_speed){
    volume += av_speed;
  }
  else if(volume >= av_speed){
    volume -= av_speed;
  }
  update_led();
  if(!line_taken){
    line_taken = true;
    if(data[1] == 0){
      data[1] = 1;
    }
    g_1 = data[2] / data[1];
  }
  else{
    if(data[1] == 0){
      data[1] = 1;
    }
    g_2 = data[2] / data[1];
    if(g_2 - g_1 > 0){
      prev_dir = 0;
    }
    else if(g_2 - g_1 < 0){
      prev_dir = 1;
    }
    g_1 = g_2;
  }
}

void update_led(){
  write_volume();
  int led_num = (volume/ 100.0) * 93;
  if(curr_led - led_num < 0){
    for(int i = curr_led + 1; i <=led_num; i++){
      if(i > 93 ){
        temp = i - 93;
      }
      else{
        temp = i;
      }
      leds[temp] = CRGB (vol_calculate(temp, 0), vol_calculate(temp, 1), vol_calculate(temp, 2));
    }
  }
  else if(curr_led - led_num > 0){
    for(int i = led_num + 1; i <=curr_led; i++){
      if(i > 93 ){
        temp = i - 93;
      }
      else{
        temp = i;
      }
      leds[temp] = CRGB (0, 0, 0);
    }
  }
  curr_led = led_num;
  FastLED.show();
}

void begin_led_circle(){
  int led_num = (volume/ 100.0) * 93;
  for (int i = 0; i <= led_num; i++) {
    if(i > 93 ){
      temp = i - 93;
    }
    else{
      temp = i;
    }
    leds[temp] = CRGB (vol_calculate(temp, 0), vol_calculate(temp, 1), vol_calculate(temp, 2));
  }
  FastLED.show();
}

void end_led_circle(){
  int led_num = (volume/ 100.0) * 93;
  for(int i = led_num; i > -1; i--){
    leds[i] = CRGB (0, 0, 0);
    FastLED.show();
    delay(5);
  }
}
