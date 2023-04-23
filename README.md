# SPI without SS pin in AVR and other microcontrollers
Piece of code in C to communicate microcontrollers using SPI without needing to use SS signal and only using addressing.



for slave section you will need to connect SS signal of your MCU to GND and copy the SPI interrupt service routine to your project. 
you may need to choose a addressing pattern that do not appear in the data series. I used a 16 bit address pattern for my slaves.

the master part is simpler. it doesn't need an address but it should know slaves. you can simply copy two master read and write functions.
