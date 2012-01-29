#include <msp430.h>
#include <timera.h>
#include "HD44780LIB.h"
#include <delay.h>


HD44780 theHD44780;
const char msg[] = "Hej all the things! o/";	// Todo: custom characters

void setup(){
	HD44780_init(&theHD44780,4,5,0,1,2,3);
	HD44780_begin(&theHD44780,16, 2);
	HD44780_setCursor(&theHD44780, 1,1);
	HD44780_print_string(&theHD44780, msg);
}

void loop() {
	delay(0xFFFF);
	delay(0xFFFF);
	
	HD44780_scrollDisplayLeft(&theHD44780);

}
