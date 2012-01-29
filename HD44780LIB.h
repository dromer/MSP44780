#ifndef HD44780LIB_H_

	#include "Wiring.h"
	
	#define HD44780LIB_H_
	// commands
	#define LCD_CLEARDISPLAY 0x01
	#define LCD_RETURNHOME 0x02
	#define LCD_ENTRYMODESET 0x04
	#define LCD_DISPLAYCONTROL 0x08
	#define LCD_CURSORSHIFT 0x10
	#define LCD_FUNCTIONSET 0x20
	#define LCD_SETCGRAMADDR 0x40
	#define LCD_SETDDRAMADDR 0x80
	
	// flags for display entry mode
	#define LCD_ENTRYRIGHT 0x00
	#define LCD_ENTRYLEFT 0x02
	#define LCD_ENTRYSHIFTINCREMENT 0x01
	#define LCD_ENTRYSHIFTDECREMENT 0x00
	
	// flags for display on/off control
	#define LCD_DISPLAYON 0x04
	#define LCD_DISPLAYOFF 0x00
	#define LCD_CURSORON 0x02
	#define LCD_CURSOROFF 0x00
	#define LCD_BLINKON 0x01
	#define LCD_BLINKOFF 0x00
	
	// flags for display/cursor shift
	#define LCD_DISPLAYMOVE 0x08
	#define LCD_CURSORMOVE 0x00
	#define LCD_MOVERIGHT 0x04
	#define LCD_MOVELEFT 0x00
	
	// flags for function set
	#define LCD_8BITMODE 0x10
	#define LCD_4BITMODE 0x00
	#define LCD_2LINE 0x08
	#define LCD_1LINE 0x00
	#define LCD_5x10DOTS 0x04
	#define LCD_5x8DOTS 0x00
	
	
	typedef struct _HD44780{
	  uint8_t _rs_pin; // LOW: command.  HIGH: character.
	
	  uint8_t _enable_pin; // activated by a HIGH pulse.
	  uint8_t _data_pins[4];
	
	  uint8_t _displayfunction;
	  uint8_t _displaycontrol;
	  uint8_t _displaymode;
	
	  uint8_t _initialized;
	
	  uint8_t _numlines,_currline;
	
	} HD44780;
	
	void HD44780_begin(HD44780 *me, uint8_t cols, uint8_t lines);
	void HD44780_send(HD44780 *me, uint8_t value, uint8_t mode);
//	void HD44780_command(HD44780 *me, uint8_t value);
	void HD44780_write4bits(HD44780 *me, uint8_t value);
	void HD44780_pulseEnable(HD44780 *me);
	void HD44780_clear(HD44780 *me);
	void HD44780_display(HD44780 *me);
	void HD44780_print_byte(HD44780 *me, uint8_t b);
	void HD44780_print_string(HD44780 *me, const char c[]);
	
	inline void HD44780_write(HD44780 *me, uint8_t value) {
	  HD44780_send(me, value, HIGH);
	}
	
	inline void HD44780_command(HD44780 *me, uint8_t value) {
	  HD44780_send(me, value, LOW);
	}
	
	
	
	
	
	
	void HD44780_send(HD44780 *me, uint8_t value, uint8_t mode) {
	  digitalWrite(me->_rs_pin, mode);
	    HD44780_write4bits(me, value>>4);
	    HD44780_write4bits(me, value);
	}
	
	
	
	
	void HD44780_init(HD44780 *me, int rs, int enable, int d0, int d1, int d2, int d3)
	{
	  me->_rs_pin = rs;
	  me->_enable_pin = enable;
	  
	  me->_data_pins[0] = d0;
	  me->_data_pins[1] = d1;
	  me->_data_pins[2] = d2;
	  me->_data_pins[3] = d3; 
		
	  pinMode(rs, OUTPUT);
	  pinMode(enable, OUTPUT);
	  
	  me->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	}
	
	void HD44780_begin(HD44780 *me, uint8_t cols, uint8_t lines) {
	  if (lines > 1) {
	    me->_displayfunction |= LCD_2LINE;
	  }
	  me->_numlines = lines;
	  me->_currline = 0;
	
	
	  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	  // according to datasheet, we need at least 40ms after power rises above 2.7V
	  // before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	  delayMicroseconds(50000); 
	  // Now we pull both RS and R/W low to begin commands
	  digitalWrite(me->_rs_pin, LOW);
	  digitalWrite(me->_enable_pin, LOW);
	  
	  //put the LCD into 4 bit or 8 bit mode
	  if (! (me->_displayfunction & LCD_8BITMODE)) {
	    // this is according to the hitachi HD44780 datasheet
	    // figure 24, pg 46
	
	    // we start in 8bit mode, try to set 4 bit mode
	    HD44780_write4bits(me, 0x03);
	    delayMicroseconds(4500); // wait min 4.1ms
	
	    // second try
	    HD44780_write4bits(me, 0x03);
	    delayMicroseconds(4500); // wait min 4.1ms
	    
	    // third go!
	    HD44780_write4bits(me, 0x03); 
	    delayMicroseconds(150);
	
	    // finally, set to 8-bit interface
	   HD44780_write4bits(me, 0x02); 
	  } else {
	    // this is according to the hitachi HD44780 datasheet
	    // page 45 figure 23
	
	    // Send function set command sequence
	    HD44780_command(me, LCD_FUNCTIONSET | me->_displayfunction);
	    delayMicroseconds(4500);  // wait more than 4.1ms
	
	    // second try
	    HD44780_command(me, LCD_FUNCTIONSET | me->_displayfunction);
	    delayMicroseconds(150);
	
	    // third go
	    HD44780_command(me, LCD_FUNCTIONSET | me->_displayfunction);
	  }
	
	  // finally, set # lines, font size, etc.
	  HD44780_command(me, LCD_FUNCTIONSET | me->_displayfunction);  
	
	  // turn the display on with no cursor or blinking default
	  me->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;  
	  HD44780_display(me);
	
	  // clear it off
	  HD44780_clear(me);
	
	  // Initialize to default text direction (for romance languages)
	  me->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	  // set the entry mode
	  HD44780_command(me, LCD_ENTRYMODESET | me->_displaymode);
	
	}
	
	void HD44780_write4bits(HD44780 *me, uint8_t value) {
	  int i;
	  for (i = 0; i < 4; i++) {
	    pinMode(me->_data_pins[i],OUTPUT);
	    digitalWrite(me->_data_pins[i], (value >> i) & 0x01);
	  }
	
	  HD44780_pulseEnable(me);
	}
	
	
	void HD44780_pulseEnable(HD44780 *me) {
	  digitalWrite(me->_enable_pin, LOW);
	  delayMicroseconds(2);    
	  digitalWrite(me->_enable_pin, HIGH);
	  delayMicroseconds(2);    // enable pulse must be >450ns
	  digitalWrite(me->_enable_pin, LOW);
	  delayMicroseconds(100);   // commands need > 37us to settle
	}
	
	void HD44780_clear(HD44780 *me)
	{
	  HD44780_command(me, LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	  delayMicroseconds(2000);  // this command takes a long time!
	}
	
	void HD44780_display(HD44780 *me) {
	  me->_displaycontrol |= LCD_DISPLAYON;
	  HD44780_command(me, LCD_DISPLAYCONTROL | me->_displaycontrol);
	}
	
	void HD44780_print_byte(HD44780 *me, uint8_t b)
	{
	 HD44780_write(me, b);
	}
	
	void HD44780_print_string(HD44780 *me, const char c[])
	{
	 while (*c)
	   HD44780_print_byte(me, *c++);
	}

	void HD44780_scrollDisplayLeft(HD44780 *me){
		HD44780_command(me, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);

	}	

	void HD44780_scrollDisplayRight(HD44780 *me){
		HD44780_command(me, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);

	} 


#endif /*HD44780LIB_H_*/
