// Copyright (c) 2015-19, Joe Krachey
// All rights reserved.
//
// Redistribution and use in source or binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in source form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in 
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
// OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "main.h"
#include "io_expander.h"
#include "serial_debug.h"
#include "ft6x06.h"
#include "project_hardware_init.h"


/*
  Initializes lcd screen to the beginning of the game???
*/
void init_screen(void)
{
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
}

//initialize timers
void init_timers(void){
		//timer interrupts
//	gp_timer_config_32(TIMER2_BASE, TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE, TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  //gp_timer_config_32(TIMER4_BASE, TIMER_TAMR_TAMR_PERIOD, 50000, false, true);

}

void init_hardware(void)
{
	uint8_t read_val;
	//Initializing launchpad
	init_serial_debug(true, true);	
	
	//joystick initialize
	ps2_initialize();
	
	//Spacebar "pause" function initialization
	uart_init(UART0_BASE, true, true);
	
	//Capacitive touch controller
	ft6x06_init();
	
	//I2C I/O Expander
	io_expander_init();
	
	//LCD screen
	init_screen();
	
	//turn on interrupts from io expander
	io_expander_byte_write(I2C1_BASE, 0x00, 0x00);
	io_expander_byte_write(I2C1_BASE, MCP23017_IODIRB_R, 0xFF);	// ENABLE AS INPUTS, 
	io_expander_byte_write(I2C1_BASE, MCP23017_INTCONB_R, 0x00); // interrupt compared to previous value
	io_expander_byte_write(I2C1_BASE, MCP23017_GPINTENB_R, 0x0F);  // intrrupt on change
	io_expander_byte_write(I2C1_BASE, MCP23017_GPPUB_R, 0x0F);		// pull-up resistor
	
	// set up push button interrupts
	gpio_enable_port(IO_EXPANDER_IRQ_GPIO_BASE);
	gpio_config_digital_enable(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	gpio_config_enable_input(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	gpio_config_enable_pullup(IO_EXPANDER_IRQ_GPIO_BASE, IO_EXPANDER_IRQ_PIN_NUM);
	gpio_config_falling_edge_irq(IO_EXPANDER_IRQ_GPIO_BASE, GPIO_RIS_GPIO_M);
	
	// turn on interrupts in the NVIC
	NVIC_SetPriority(gpio_get_irq_num(IO_EXPANDER_IRQ_GPIO_BASE), 1);
	NVIC_EnableIRQ(gpio_get_irq_num(IO_EXPANDER_IRQ_GPIO_BASE));

  io_expander_byte_read(I2C1_BASE, MCP23017_GPIOB_R, &read_val);
}
