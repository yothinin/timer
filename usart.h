void UART_init(uint16_t ubrr);
unsigned char UART_RxChar();
void UART_TxChar(char ch);
void UART_SendString(char *str);
void UART_putdec8(uint8_t val);
void UART_putdec16(uint16_t val);
