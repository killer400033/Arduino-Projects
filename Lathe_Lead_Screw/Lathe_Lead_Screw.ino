#include <math.h>
#include <Wire.h>

int data[4] = {0, 0, 0, 2};
float data_in;
int count = 0;
volatile long temp = 0;
volatile long lead_counter = 0;
volatile double gear_ratio;
volatile unsigned long last_millis_time;
volatile bool pause = true;
volatile bool feed_pause = true;
volatile bool positive_dir = true;
volatile long offset = 0;
const unsigned int mask_quad_A = digitalPinToBitMask(2);
const unsigned int mask_quad_B = digitalPinToBitMask(13); 
int manual_speed = 500;
long soft_offset = 0;
long soft_offset_2 = 0;
long feed_offset = 0;
volatile bool check_enabled = false;

void setup() {
  //Serial.begin(9600);
  //motor driver
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(5, OUTPUT);
  //button
  pinMode(10, INPUT);
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);

  attachInterrupt(digitalPinToInterrupt(10), Button, RISING);
  
    //Encoder Stuff
    REG_PIOB_PDR = mask_quad_A;
    REG_PIOB_ABSR |= mask_quad_A;
    REG_PIOB_PDR = mask_quad_B;
    REG_PIOB_ABSR |= mask_quad_B;
    REG_PMC_PCER0 = (1<<27);
    REG_TC0_CMR0 = 5;
    REG_TC0_BMR = (1<<9)|(1<<8)|(1<<12);
    REG_TC0_CCR0 = 5;
    
  gear_ratio = 1.75 / 3.0 / 2.00;

  temp = (long)REG_TC0_CCR0;

  //I2C
  Wire.begin(0x8);
  Wire.onReceive(receiveEvent);
}
   
  void loop() {
    if(check_enabled == true){
      digitalWrite(8, HIGH);
      check_enabled = false;
    }
    if(pause == false){
      if(((long)REG_TC0_CV0) < temp ){
        if(((temp + offset) * gear_ratio) < lead_counter){
          subtract_motor_driver();
        }
      }
      if(((long)REG_TC0_CV0) > temp){
        if(((temp + offset) * gear_ratio) > lead_counter){
          add_motor_driver();
        }
      }
    }
    temp = (long)REG_TC0_CV0;

    if(feed_pause == false){
      if(((long)REG_TC0_CV0 - feed_offset) * gear_ratio > 1){
        add_motor_driver();
        feed_offset = (long)REG_TC0_CV0; 
      }
    }

    if(pause == true && feed_pause == true){
      if(analogRead(A2) > manual_speed){
        manual_speed += 2;
      }
      else if(analogRead(A2) < manual_speed){
        manual_speed -= 2;
      }
      
      if(manual_speed < 480){
        digitalWrite(8, HIGH);
        delayMicroseconds(8);
        subtract_motor_driver();
        delayMicroseconds(2000 - (480 - manual_speed) * 3.6);
      }
      else if(manual_speed > 544){
        digitalWrite(8, HIGH);
        delayMicroseconds(8);
        add_motor_driver();
        delayMicroseconds(2000 - (manual_speed - 544) * 3.55);
      }

      else if(data[3] == 2 && feed_pause == true){
        digitalWrite(8, LOW);
      }
    }
  }

  inline void add_motor_driver(){
      if(positive_dir == false){
        digitalWrite(4, LOW);
        delayMicroseconds(6);
        positive_dir = true;
      }
      digitalWrite(5, LOW);
      delayMicroseconds(3);
      digitalWrite(5, HIGH);
      delayMicroseconds(2.5);
      lead_counter++;
  }

  inline void subtract_motor_driver(){
      if(positive_dir == true){
        digitalWrite(4, HIGH);
        delayMicroseconds(6);
        positive_dir = false;
      }
      digitalWrite(5, LOW);
      delayMicroseconds(3);
      digitalWrite(5, HIGH);
      delayMicroseconds(2.5);
      lead_counter--;
  }

  void Button(){
    if(millis() - last_millis_time > 200){
      if(data[3] != 2){
        if(pause == false){
          pause = true;
          digitalWrite(11, HIGH);
          soft_stop();
        }
        else{
          offset = - 2400 * ceil(((long)REG_TC0_CV0 - (lead_counter / gear_ratio)) / 2400.0);
          pause = false;
          digitalWrite(11, LOW);
          temp = (long)REG_TC0_CV0;
          soft_start();
        }
      }
      if(data[3] == 2){
        if(feed_pause == false){
          feed_pause = true;
          digitalWrite(11, HIGH);
          digitalWrite(8, LOW);
        }
        else{
          feed_pause = false;
          digitalWrite(11, LOW);
          digitalWrite(8, HIGH);
          check_enabled = true;
          feed_offset = (long)REG_TC0_CV0;
        }
      }
      last_millis_time = millis();
    }
  }

  void soft_start(){
    while((((long)REG_TC0_CV0 + offset) * gear_ratio) < lead_counter){
    }
    temp = (long)REG_TC0_CV0;
    soft_offset = temp;
    soft_offset_2 = lead_counter;
    while((temp - soft_offset) < 4800){
      if(((long)REG_TC0_CV0) > temp){
        if(floor((temp - soft_offset) * (temp - soft_offset) * gear_ratio / 9600) > (lead_counter - soft_offset_2)){
          add_motor_driver();
        }
      }
      temp = (long)REG_TC0_CV0; 
    }
    offset -= 2400;
  }


void soft_stop(){
  temp = (long)REG_TC0_CV0;
  soft_offset = temp;
  soft_offset_2 = lead_counter;
  while((temp - soft_offset) < 2400){
    if(((long)REG_TC0_CV0) > temp){
      if( ((temp - soft_offset) * gear_ratio) - ((temp - soft_offset) * (temp - soft_offset) * gear_ratio / 4800) > (lead_counter - soft_offset_2) ){
        add_motor_driver();
      }
    }
    temp = (long)REG_TC0_CV0;
  }
}


void receiveEvent(int howMany) {
  while (Wire.available()) {
    int c = Wire.read();
    if(count < 4 && count > 0){
      data[count - 1] = c;
    }
    if(count == 8){
      data[3] = c;
    }
    count ++;
    if(count == 9){
      count = 0;
      decode_data();
    }
  }
}

void decode_data(){
  REG_PMC_PCER0 = (1<<27);
  REG_TC0_CMR0 = 5;
  REG_TC0_BMR = (1<<9)|(1<<8)|(1<<12);
  REG_TC0_CCR0 = 5;
  data_in = 0;
  offset = 0;
  lead_counter = 0;
  switch (data[3]){
    case 0:
    pause = true;
    feed_pause = true;
    for(int i = 0; i < 3; i++){
      data_in += data[i] * pow(10, -i);
    }
    gear_ratio = data_in / 3.0 / 2.00;
    digitalWrite(8, HIGH);
    digitalWrite(11, HIGH);
    check_enabled = true;
    break;

    case 1:
    pause = true;
    feed_pause = true;
    for(int i = 0; i < 3; i ++){
      data_in += data[i] * pow(10, (2 - i));
    }
    gear_ratio = (25.4 / data_in) / 3.0 / 2.00;
    digitalWrite(8, HIGH);
    digitalWrite(11, HIGH);
    check_enabled = true;
    break;

    case 2:
    pause = true;
    feed_pause = true;
    for(int i = 0; i < 3; i ++){
      data_in += data[i] * pow(10, (2 - i));
    }
    gear_ratio = data_in / 3.0 / 2.00 / 1000;
    digitalWrite(8, LOW);
    digitalWrite(11, HIGH);
    break;

    default:
    break;
  }
}
 
