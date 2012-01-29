#include <msp430x20x3.h>// Headers for specific device
#include "HD44780LIB.h"

//TO DO Arduino stuff
 
HD44780 theHD44780;
const char msg[] = "Hello MSP430!";

void setup(){
	HD44780_init(&theHD44780,4,5,0,1,2,3);
	HD44780_begin(&theHD44780,16, 2);
	HD44780_print_string(&theHD44780, msg);
}

void loop() {

}

