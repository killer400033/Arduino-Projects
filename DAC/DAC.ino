volatile bool prev_state = 0;
volatile byte dac_out = 0;

void setup() {
  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= (1<<CS10) | (1<<WGM12); //Prescaler of 1, CTC mode, top of OCR1A
  TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B);
  OCR1A = 3100;
  OCR1B = 2500;

  SREG |= B10000000; //Global Interrupt Enable
  EICRA = 0;
  EICRA |= (1<<ISC01) | (1<<ISC00); //Set INT0 to interrupt on rising edge
  EIMSK = 0;
  EIMSK |= (1<<INT0); //Enable interrupt on INT0

  
  sei();

  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);

  pinMode(2, INPUT);
  pinMode(8, INPUT);
  
}

ISR(TIMER1_COMPA_vect){
  if(PINB & B00000001){
    PORTD = B11111000;
    PORTB = B00001110;
  }
  else{
    PORTD = 0;
    PORTB = 0;
  }
}

ISR(TIMER1_COMPB_vect){
  prev_state = PINB & B00000001;
}

ISR(INT0_vect){
  TCNT1 = 0;
  if(ICR1 > 2700 && !prev_state){
    ICR1 = 0;
  }
  dac_out = (5 * ICR1) / 56;
  PORTD = (dac_out & B00011111) * 8;
  PORTB = (dac_out & B11100000) / 16;
}

void loop() {
}
