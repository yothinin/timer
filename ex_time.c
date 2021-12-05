#define F_CPU 8000000UL
#include<avr/io.h>
#include<util/delay.h>

void main(){
  int count=0;
  DDRB=0xff; // set o/p mode
  TCNT0=6; // initialize the timer
  TCCR0=0x02; // 8 prescalar if no prescalar accuracy is much more
  while(1){
    if(TIFR & 0x01) // we will enter here after every 4us
    {
      TIFR=0x01; // clear the flag
      TCNT0=6;
      count++;
    }
    
    if(count>=4000) // (4000 count = 1s)
    {
      PORTB=~PORTD;
      count=0;
    }
  }
}
