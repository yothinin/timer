#include <avr/io.h>

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
