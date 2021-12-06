#define F_CPU 8000000UL
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1

//define Red Yello Green led pin.
#define R 0
#define Y 1
#define G 2 //for my board, Atmega8 G=2, Atmega16 G=7

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/delay.h>
#include "usart.h"
#include "millis.h"

//for r,y,b_state
volatile int state = R;
unsigned long blink; //the last time the led was toggled
unsigned long r;           //the last time the led RED was changed.
unsigned long y;           //the last time the led YELLOW was changed.
unsigned long g;           //the last time the led GREEN was changed.

//for blink_led
int8_t direct = 0;
int8_t led = 0;

void turnoff_all_led(){
  PORTB = 0xFF;
  PORTC = 0x00;
}

void red_state(int delay){
  if (state == R){
    PORTC |= 1<<R;
    if (millis() - r > delay){
      PORTC &= ~(1<<R);
      state = G;
      r=y=g=millis();
    }
  }
}

void yellow_state(int delay){
  if (state == Y){
    PORTC |= 1<<Y;
    if (millis() - y > delay){
      PORTC &= ~(1<<Y);
      state = R;
      r=y=g=millis();
    }
  }
}

void green_state(int delay){
  if (state == G){
    PORTC |= 1<<G;
    if (millis() - g > delay){
      PORTC &= ~(1<<G);
      state = Y;
      r=y=g=millis();
    }
  }
}

void blink_led(int delay){
  PORTB |= 1<<led;
  if (millis() - blink > 50){
    PORTB &= 1<<led;
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
    blink = millis();
  }
}

int main(void){
  UART_init(MYUBRR);
  init_millis(F_CPU);    //frequency the atmega8p is running at

  blink = millis();
  r=y=g=millis();
  sei();

  uint8_t stop = 0;

  //DDRB = (1 << DDB5); //setting PORTB5 to an output
  DDRB = 0xFF;
  DDRC = 0xFF; 
  //PORTB = 0xff;   //for common anode led.
  PORTB = 0x00;     //for common cathode led.
  PORTC = (0<<R) | (0<<Y) | (0<<G);
  //unsigned long w; // 1second delay check.
  while (stop == 0){
    /* uncomment to check 1second delay, result 1000, 1001
    w = millis();
    _delay_ms(1000);
    UART_putdec16(millis()-w);
    UART_SendString("\r\n");
    */
    if (PIND & (1<<PIN7)) stop = (stop)?0:1;
    blink_led(50);
    red_state(5000);
    yellow_state(2000);
    green_state(4000);
  }
  turnoff_all_led();
  return 0;
}

