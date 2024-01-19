int set_temp = 0;
float current_temp = 0;
int temp_buffer = 500;
bool heat_ready = false;
int resistance = 0;
int current_temp_2 = 0;


void setup() {
  // put your setup code here, to run once:
  //Serial.begin(9600);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  delay(600);

  //Timer1 Setup
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 290;
  OCR1B = 10;
  TCCR1A |= (1<<WGM10) | (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1); //Non inverting PWM signal
  TCCR1B |= (1<<CS10) | (1<<WGM12) | (1<<WGM13); //Prescaler of 1, Fast PWM 16 bit mode, top of OCR1A
}

void loop(void) {
  if(analogRead(A3) < 100){
  OCR1A = temp_buffer;
  if(set_temp > current_temp && temp_buffer > 300){
    temp_buffer -= 1;
  }
  if(set_temp < current_temp && temp_buffer < 800){
    temp_buffer += 1;
  }
  resistance = 1507.0 / (2455.2 / analogRead(A1) - 1);
  current_temp = (resistance * 2.9) - (27.0 * 2.9);
  set_temp = map(analogRead(A5), 0, 1023, 450, 250); 
  }
  else{
    OCR1B = 0;
  }
}
