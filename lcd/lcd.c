 /******************************************************************************
 *
 * Module: LCD
 *
 * File Name: lcd.c
 *
 * Description: Source file for the LCD driver
 *
 * Author: Mohamed Tarek
 *
 *******************************************************************************/

#include <util/delay.h> /* For the delay functions */
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "lcd.h"
#include "GPIO.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Initialize the LCD:
 * 1. Setup the LCD pins directions by use the GPIO driver.
 * 2. Setup the LCD Data Mode 4-bits or 8-bits.
 */
void LCD_init(void)
{
	/* Configure the direction for RS, RW and E pins as output pins */
	GPIO_setupPinDirection(LCD_RS_PORT_ID,LCD_RS_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_RW_PORT_ID,LCD_RW_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_E_PORT_ID,LCD_E_PIN_ID,PIN_OUTPUT);

	/* Configure the data port as output port */
	GPIO_setupPortDirection(LCD_DATA_PORT_ID,PORT_OUTPUT);

	 _delay_ms(20); // Delay for processing power on
	 LCD_sendCommand(0x38); // 8-bit mode, 2-line, 5x7 font
	 _delay_ms(5); // Delay for processing Tdsw = 100ns
	 LCD_sendCommand(0x0E); // Display on, cursor on, blinking cursor on
	 _delay_ms(5); // Delay for processing Tdsw = 100ns
	 LCD_sendCommand(0x01); // Clear display
	 _delay_ms(5); // Delay for processing Tdsw = 100ns
	 LCD_sendCommand(0x06); // Entry mode: increment cursor, no display shift
	 _delay_ms(5); // Delay for processing Tdsw = 100ns

	 // Initialize LCD in 4-bit mode
	 LCD_sendCommand(0x02); // 4-bit mode
}

/*
 * Description :
 * Send the required command to the screen
 */
void LCD_sendCommand(uint8 command)
{
	GPIO_writePin(LCD_RS_PORT_ID, LCD_RS_PIN_ID, LOGIC_LOW); // Instruction mode RS=0
	GPIO_writePin(LCD_RW_PORT_ID, LCD_RW_PIN_ID, LOGIC_LOW); // Write data to LCD so RW=0
	_delay_ms(1); // Delay for processing Tas = 50ns
	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_HIGH); // Enable LCD E=1
	_delay_ms(1); // Delay for processing Tpw - Tdws = 190ns
	GPIO_writePort(LCD_DATA_PORT_ID, (command >> 4)); // Send higher four bits
	_delay_ms(1); // Delay for processing Tdsw = 100ns
	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW); // Disable LCD E=0
	_delay_ms(1); // Delay for processing Th = 13ns

	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_HIGH); // Enable LCD E=1
	_delay_ms(1); // Delay for processing Tpw - Tdws = 190ns
	GPIO_writePort(LCD_DATA_PORT_ID, (command & 0x0F)); // Send lower four bits
	_delay_ms(1); // Delay for processing Tdsw = 100ns
	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW); // Disable LCD E=0
	_delay_ms(1); // Delay for processing Th = 13ns
}

/*
 * Description :
 * Display the required character on the screen
 */
void LCD_displayCharacter(uint8 data)
{
	GPIO_writePin(LCD_RS_PORT_ID, LCD_RS_PIN_ID, LOGIC_HIGH); // Data mode RS=1
	GPIO_writePin(LCD_RW_PORT_ID, LCD_RW_PIN_ID, LOGIC_LOW); // Write data to LCD so RW=0
	_delay_ms(1); // Delay for processing Tas = 50ns
	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_HIGH); // Enable LCD E=1
	_delay_ms(1); // Delay for processing Tpw - Tdws = 190ns
	GPIO_writePort(LCD_DATA_PORT_ID, (data >> 4)); // Send higher four bits
	_delay_ms(1); // Delay for processing Tdsw = 100ns
	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW); // Disable LCD E=0
	_delay_ms(1); // Delay for processing Th = 13ns

	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_HIGH); // Enable LCD E=1
	_delay_ms(1); // Delay for processing Tpw - Tdws = 190ns
	GPIO_writePort(LCD_DATA_PORT_ID, (data & 0x0F)); // Send lower four bits
	_delay_ms(1); // Delay for processing Tdsw = 100ns
	GPIO_writePin(LCD_E_PORT_ID, LCD_E_PIN_ID, LOGIC_LOW); // Disable LCD E=0
	_delay_ms(1); // Delay for processing Th = 13ns
}

/*
 * Description :
 * Display the required string on the screen
 */
void LCD_displayString(const char *Str)
{
	uint8 i = 0;
	while(Str[i] != '\0')
	{
		LCD_displayCharacter(Str[i]);
		i++;
	}
	/***************** Another Method ***********************
	while((*Str) != '\0')
	{
		LCD_displayCharacter(*Str);
		Str++;
	}		
	*********************************************************/
}

/*
 * Description :
 * Move the cursor to a specified row and column index on the screen
 */
void LCD_moveCursor(uint8 row,uint8 col)
{
	uint8 lcd_memory_address;
	
	/* Calculate the required address in the LCD DDRAM */
	switch(row)
	{
		case 0:
			lcd_memory_address=col;
				break;
		case 1:
			lcd_memory_address=col+0x40;
				break;
		case 2:
			lcd_memory_address=col+0x10;
				break;
		case 3:
			lcd_memory_address=col+0x50;
				break;
	}					
	/* Move the LCD cursor to this specific address */
	LCD_sendCommand(lcd_memory_address | LCD_SET_CURSOR_LOCATION);
}

/*
 * Description :
 * Display the required string in a specified row and column index on the screen
 */
void LCD_displayStringRowColumn(uint8 row,uint8 col,const char *Str)
{
	LCD_moveCursor(row,col); /* go to to the required LCD position */
	LCD_displayString(Str); /* display the string */
}

/*
 * Description :
 * Display the required decimal value on the screen
 */
void LCD_intgerToString(int data)
{
   char buff[16]; /* String to hold the ascii result */
   itoa(data,buff,10); /* Use itoa C function to convert the data to its corresponding ASCII value, 10 for decimal */
   LCD_displayString(buff); /* Display the string */
}

/*
 * Description :
 * Send the clear screen command
 */
void LCD_clearScreen(void)
{
	LCD_sendCommand(LCD_CLEAR_COMMAND); /* Send clear display command */
}
