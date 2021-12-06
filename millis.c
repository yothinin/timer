#include <util/atomic.h>

volatile unsigned long timer1_millis;

ISR(TIMER1_COMPA_vect){
  timer1_millis++;
}

void init_millis(unsigned long f_cpu){
  unsigned long ctc_match_overflow;
  ctc_match_overflow = ((f_cpu / 994) / 8);  //when timer1 is this value, 1ms has passed
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
