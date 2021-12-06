#define F_CPU 8000000UL
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>

#define R 0
#define Y 1
#define G 7

volatile unsigned long timer1_millis;
volatile int state = R;

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
  //ATOMIC_BLOCK(ATOMIC_FORCEON) {
    millis_return = timer1_millis;
  //}
  return millis_return;
}

void UART_init(uint16_t ubrr){
  UBRRL = (uint8_t)(ubrr);
  UBRRH = (uint8_t)(ubrr>>8);
  UCSRB = (1<<RXEN) | (1<<TXEN);
  UCSRC = (1<<URSEL) | (3<<UCSZ0);
}

unsigned char UART_RxChar(){
  while ((UCSRA & (1<<RXC)) == 0);
  return (UDR);
}

void UART_TxChar(char ch){
  while (!(UCSRA & (1<<UDRE)));
  UDR = ch;
}

void UART_SendString(char *str){
  unsigned char j = 0;
  while (str[j] != 0){
    UART_TxChar(str[j]);
    ++j;
  }
}

void UART_putdec8(uint8_t val){
  uint8_t dig1 = '0', dig2 = '0';
  while (val >= 100){
    val -= 100;
    ++dig1;
  }
  while (val >= 10){
    val -= 10;
    ++dig2;
  }
  if (dig1 != '0') UART_TxChar(dig1);
  if ((dig1 != '0') || (dig2 != '0')) UART_TxChar(dig2);
  UART_TxChar(val+'0');
}

void UART_putdec16(uint16_t val){
  uint8_t dig1, dig2, dig3, dig4;
  dig1=dig2=dig3=dig4='0';
  while (val >= 10000){
    val -= 10000;
    ++dig1;
  }
  while (val >= 1000){
    val -= 1000;
    ++dig2;
  }
  while (val >= 100){
    val -= 100;
    ++dig3;
  }
  while (val >= 10){
    val -= 10;
    ++dig4;
  }

  uint8_t prevPrinted = 0;
  if (dig1 != '0') {UART_TxChar(dig1); prevPrinted = 1;}
  if (prevPrinted || (dig2 != '0')) {UART_TxChar(dig2); prevPrinted = 1;}
  if (prevPrinted || (dig3 != '0')) {UART_TxChar(dig3); prevPrinted = 1;}
  if (prevPrinted || (dig4 != '0')) {UART_TxChar(dig4); prevPrinted = 1;}

  UART_TxChar(val+'0');
}

int main(void){
  UART_init(MYUBRR);

  //DDRB = (1 << DDB5); //setting PORTB5 to an output
  DDRB = 0xFF;
  DDRC = 0xFF;
  PORTB = 0xff;
  //PORTB ^= (1 << PORTB5);
  PORTC = (0<<R) | (0<<Y) | (0<<G);

  int8_t led = 0;

  init_millis(F_CPU);    //frequency the atmega8p is running at
  unsigned long prev_millis; //the last time the led was toggled
  unsigned long r;           //the last time the led RED was changed.
  unsigned long y;           //the last time the led YELLOW was changed.
  unsigned long g;           //the last time the led GREEN was changed.

  prev_millis = millis();
  r=y=g=millis();
  sei();

  uint8_t stop = 0;
  int8_t direct = 0;

  //unsigned long w;
  while (stop == 0){
    //w = millis();
    //_delay_ms(1000);
    //UART_putdec16(millis()-w);
    //UART_SendString("\r\n");
    PORTB ^= 1<<led;
    if (PIND & (1<<PIN7)) stop = (stop)?0:1;
      if (millis() - prev_millis > 50){
        PORTB |= 1<<led;
        if (direct == 0){
          ++led;
          if (led > 7){
            led = 6;
            direct = 1;
          }
        }
        if (direct == 1){
        --led;
        if (led < 0){
          led = 1;
          direct = 0;
        }
      }
      prev_millis = millis();
    }
    if (state == R){
      PORTC |= 1<<R;
      if (millis() - r > 5000){
        PORTC &= ~(1<<R);
        state = G;
        r=y=g=millis();
      }
    }
    if (state == Y){
      PORTC |= 1<<Y;
      if (millis() - y > 2000){
        PORTC &= ~(1<<Y);
        state = R;
        r=y=g=millis();
      }
    }
    if (state == G){
      PORTC |= 1<<G;
      if (millis() - g > 4000){
        PORTC &= ~(1<<G);
        state = Y;
        r=y=g=millis();
      }
    }
  }
  PORTB = 0xFF;
  PORTC = 0x00;
  return 0;
}
