/*
Name: Timer demo
Author: Monoclecat
This demo will make the pin PORTB5 (pin 13 on the Arduino board) toggle every second
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>

//NOTE: A unsigned long holds values from 0 to 4,294,967,295 (2^32 - 1). It will roll over to 0 after reaching its maximum value.

#define R 0
#define Y 1
#define G 2

volatile unsigned long timer1_millis;
volatile int state = R;

ISR(TIMER1_COMPA_vect){
  timer1_millis++;
}

void init_millis(unsigned long f_cpu){
  unsigned long ctc_match_overflow;
  ctc_match_overflow = ((f_cpu / 1000) / 8);  //when timer1 is this value, 1ms has passed
  // (Set timer to clear when matching ctc_match_overflow) | (Set clock divisor to 8)
  TCCR1B |= (1 << WGM12) | (1 << CS11);
  // high byte first, then low byte
  OCR1AH = (ctc_match_overflow >> 8);
  OCR1AL = ctc_match_overflow;
  // Enable the compare match interrupt
  //TIMSK1 |= (1 << OCIE1A);
  TIMSK |= (1 << OCIE1A);
  //REMEMBER TO ENABLE GLOBAL INTERRUPTS AFTER THIS WITH sei(); !!!
}

unsigned long millis (void){
  unsigned long millis_return;
  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer1_millis;
  }
  return millis_return;
}



int main(void)
{
  //DDRB = (1 << DDB5); //setting PORTB5 to an output
  DDRB = 0xFF;
  PORTB ^= (1 << PORTB5);

  init_millis(8000000UL);    //frequency the atmega8p is running at
  unsigned long prev_millis; //the last time the led was toggled
  unsigned long r;           //the last time the led RED was changed.
  unsigned long y;           //the last time the led YELLOW was changed.
  unsigned long g;           //the last time the led GREEN was changed.
  
  prev_millis = millis();
  r=y=g=millis();
  
  sei();
  
  PORTB = (0<<R) | (0<<Y) | (0<<G);
  
  for(;;){
    if (millis() - prev_millis > 250){
      PORTB ^= (1 << PORTB5); //Turn on / Turn off the LED
      prev_millis = millis();
    }
    if (state == R){
      PORTB |= 1<<R;
      if (millis() - r > 5000){
        //PORTB ^= (1<<R);
        PORTB &= ~(1<<R);
        state = G;
        r=y=g=millis();
      }
    }
    if (state == Y){
      PORTB |= 1<<Y;
      if (millis() - y > 1000){
        //PORTB ^= (1<<Y);
        PORTB &= ~(1<<Y);
        state = R;
        r=y=g=millis();
      }
    }
    if (state == G){
      PORTB |= 1<<G;
      if (millis() - g > 4000){
        //PORTB ^= (1<<G);
        PORTB &= ~(1<<G);
        state = Y;
        r=y=g=millis();
      }
    }
  }
  return 0;
}
