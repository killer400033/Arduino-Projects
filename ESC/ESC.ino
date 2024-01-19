// Low Side Mosfets : D10, D9, D3
// High Side Mosfets : D12, D11, D8

byte duty_cyc = 95;
byte input = 0;
int avg = 0;
int count = 0;
byte prev_input = 0;
unsigned long prev_time = 0;

void setup() {
  // LOW SIDE
  pinMode(3, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);

  // HIGH SIDE
  pinMode(8, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  // HALL EFFECT INPUT
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  
  SREG |= B10000000; // Enable all interrupts (Global interrupts)

  //Interrupt For Pins 4, 5, 6
  PCICR = 0;
  PCICR |= (1<<PCIE2); // Set interrupt Register to enable interrupts on PCIE0 (Port B)
  PCMSK2 = 0;
  PCMSK2 |= (1<<PCINT20) | (1<<PCINT21) | (1<<PCINT22); // Mask all pins from interrupt except for D4, D5, D6

  //Timer Setup
  GTCCR = (1<<TSM)|(0<<PSRASY)|(1<<PSRSYNC); // halt timer0 and timer1
  
  //Timer 2
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2A |= (1<<WGM20) | (1<<WGM21)| (1<<COM2B1) | (1<<COM2B0); // Port B inverting
  TCCR2B |= (1<<WGM22) | (1<<CS21); // Fast PWM, top of OCR2A with prescaler of 8
  OCR2A = 100;
  OCR2B = duty_cyc; 
  TCNT2 = 50; //Offset to put timer 2 and 1 in phase
  
  //Timer 1
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1A |= (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1) | (1<<COM1B0) | (1<<COM1A0); // Port A and B inverting
  TCCR1B |= (1<<WGM12) | (1<<WGM13) | (1<<CS11); // Fast PWM, top of ICR1 with prescaler of 8
  OCR1A = duty_cyc;
  OCR1B = duty_cyc;
  ICR1 = 100;
  TCNT1 = 0;

  GTCCR = 0; //Restart Prescaler
}

ISR(PCINT2_vect){
  input = (PIND & B01110000) >> 4;
  switch(input){
    case B011:
      while (TIFR1 && (1<<OCF1A) == 0);
      TIFR1 |= (1<<OCF1A);
      // HIGH SIDE:
      // ON: 12, OFF: 11, 8
      // LOW SIDE:
      // ON: 3, OFF: 10, 9
      PORTB = B00010000;
      OCR1B = 101;
      OCR1A = 101;
      OCR2B = duty_cyc;
      break;
    case B010:
      while (TIFR1 && (1<<OCF0B) == 0);
      TIFR1 |= (1<<OCF0B);
      // HIGH SIDE:
      // ON: 11, OFF: 12, 8
      // LOW SIDE:
      // ON: 3, OFF: 10, 9
      PORTB = B00001000;
      OCR1B = 101;
      OCR1A = 101;
      OCR2B = duty_cyc;
      break;
    case B110:
      while (TIFR1 && (1<<OCF0B) == 0);
      TIFR1 |= (1<<OCF0B);
      // HIGH SIDE:
      // ON: 11, OFF: 12, 8
      // LOW SIDE:
      // ON: 10, OFF: 9, 3
      PORTB = B00001000;
      OCR1A = 101;
      OCR2B = 101;
      OCR1B = duty_cyc;
      break;
    case B100:
      while (TIFR1 && (1<<OCF1B) == 0);
      TIFR1 |= (1<<OCF1B);
      // HIGH SIDE:
      // ON: 8, OFF: 12, 11
      // LOW SIDE:
      // ON: 10, OFF: 9, 3
      PORTB = B00000001;
      OCR1A = 101;
      OCR2B = 101;
      OCR1B = duty_cyc;
      break;
    case B101:
      while (TIFR1 && (1<<OCF1B) == 0);
      TIFR1 |= (1<<OCF1B);
      // HIGH SIDE:
      // ON: 8, OFF: 12, 11
      // LOW SIDE:
      // ON: 9, OFF: 10, 3
      PORTB = B00000001;
      OCR1B = 101; 
      OCR2B = 101;
      OCR1A = duty_cyc;
      break;
    case B001:
      while (TIFR1 && (1<<OCF1A) == 0);
      TIFR1 |= (1<<OCF1A);
      // HIGH SIDE:
      // ON: 12, OFF: 11, 8
      // LOW SIDE:
      // ON: 9, OFF: 10, 3
      PORTB = B00010000;
      OCR1B = 101;
      OCR2B = 101;
      OCR1A = duty_cyc;
      break;
    default:
      break;
  }
}

void loop() {
  if(count < 10){
    avg += map(analogRead(A1), 0, 1023, 10, 100);
    count++;
  }
  else{
    count = 0;
    duty_cyc = avg / 10;
    avg = 0;
  }

  // For startup only
  input = (PIND & B01110000) >> 4;
  if (prev_input == input) {
    if ((millis() - prev_time) > 20) {
      switch(input){
        case B011:
          while (TIFR1 && (1<<OCF1A) == 0);
          TIFR1 |= (1<<OCF1A);
          // HIGH SIDE:
          // ON: 12, OFF: 11, 8
          // LOW SIDE:
          // ON: 3, OFF: 10, 9
          delayMicroseconds(5); // These delays are to charge the bootstrap capacitor for the MOSFET driver
          PORTB = 0;
          delayMicroseconds(1);
          PORTB = B00010000;
          OCR1B = 101;
          OCR1A = 101;
          OCR2B = duty_cyc;
          break;
        case B010:
          while (TIFR1 && (1<<OCF0B) == 0);
          TIFR1 |= (1<<OCF0B);
          // HIGH SIDE:
          // ON: 11, OFF: 12, 8
          // LOW SIDE:
          // ON: 3, OFF: 10, 9
          delayMicroseconds(5); // These delays are to charge the bootstrap capacitor for the MOSFET driver
          PORTB = 0;
          delayMicroseconds(1);
          PORTB = B00001000;
          OCR1B = 101;
          OCR1A = 101;
          OCR2B = duty_cyc;
          break;
        case B110:
          while (TIFR1 && (1<<OCF0B) == 0);
          TIFR1 |= (1<<OCF0B);
          // HIGH SIDE:
          // ON: 11, OFF: 12, 8
          // LOW SIDE:
          // ON: 10, OFF: 9, 3
          delayMicroseconds(5); // These delays are to charge the bootstrap capacitor for the MOSFET driver
          PORTB = 0;
          delayMicroseconds(1);
          PORTB = B00001000;
          OCR1A = 101;
          OCR2B = 101;
          OCR1B = duty_cyc;
          break;
        case B100:
          while (TIFR1 && (1<<OCF1B) == 0);
          TIFR1 |= (1<<OCF1B);
          // HIGH SIDE:
          // ON: 8, OFF: 12, 11
          // LOW SIDE:
          // ON: 10, OFF: 9, 3
          delayMicroseconds(5); // These delays are to charge the bootstrap capacitor for the MOSFET driver
          PORTB = 0;
          delayMicroseconds(1);
          PORTB = B00000001;
          OCR1A = 101;
          OCR2B = 101;
          OCR1B = duty_cyc;
          break;
        case B101:
          while (TIFR1 && (1<<OCF1B) == 0);
          TIFR1 |= (1<<OCF1B);
          // HIGH SIDE:
          // ON: 8, OFF: 12, 11
          // LOW SIDE:
          // ON: 9, OFF: 10, 3
          delayMicroseconds(5); // These delays are to charge the bootstrap capacitor for the MOSFET driver
          PORTB = 0;
          delayMicroseconds(1);
          PORTB = B00000001;
          OCR1B = 101; 
          OCR2B = 101;
          OCR1A = duty_cyc;
          break;
        case B001:
          while (TIFR1 && (1<<OCF1A) == 0);
          TIFR1 |= (1<<OCF1A);
          // HIGH SIDE:
          // ON: 12, OFF: 11, 8
          // LOW SIDE:
          // ON: 9, OFF: 10, 3
          delayMicroseconds(5); // These delays are to charge the bootstrap capacitor for the MOSFET driver
          PORTB = 0;
          delayMicroseconds(1);
          PORTB = B00010000;
          OCR1B = 101;
          OCR2B = 101;
          OCR1A = duty_cyc;
          break;
        default:
          break;
      }
    }
  }
  else {
    prev_input = input;
    prev_time = millis();
  }
}
