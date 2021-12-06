Example for use timer, usart.

files:
  ex_millis.c
  usart.c,usart.h
  millis.c, millis.h

Compile:
  avr-gcc -Wall -Os -mmcu=atmega8 -o ex_millis.bin ex_millis.c usart.c millis.c

Create hex file:
  avr-objcopy -j.text -j.data -Oihex ex_millis.bin ex_millis.hex

Upload hex file:
  avrdude -patmega8 -Pusb -cusbasp -Uflash:w:ex_millis.hex

if need to slow frequency upload, led use -B100 option.
