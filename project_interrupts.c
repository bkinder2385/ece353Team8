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
#include "project_interrupts.h"


volatile uint16_t PS2_X_DATA = 0;
volatile uint16_t PS2_Y_DATA = 0;
volatile PS2_DIR_t PS2_DIR = PS2_DIR_CENTER;
volatile uint16_t jump_count = 0;
volatile bool red = false;

//IS A HAZARD CURRENTLY OUT?
volatile bool P_FLY = false;
volatile bool CACTUS_RUN = false;

//RANDOM PTER Y LOCATIONS
static const uint16_t  START_STATE = 0xACE7u;
static const uint16_t  YLOCATION[] = {167, 0, 167, 100, 0, 140, 120, 0};

//*****************************************************************************
// Generates a random number
// https://en.wikipedia.org/wiki/Linear-feedback_shift_register  -- DO NOT MODIFY
//*****************************************************************************
uint16_t generate_random_number(
)
{   
    static uint16_t lfsr = START_STATE;
    uint16_t bit;
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    return lfsr;
}

//*****************************************************************************
// Returns whether a pterodactyl will appear
//*****************************************************************************
uint16_t proc_pterodactyl(uint16_t appear){
	if(appear == 0){
		return YLOCATION[generate_random_number()%8];
	}
	return 0;
}

//*****************************************************************************
// Returns the most current direction that was pressed.
//*****************************************************************************
PS2_DIR_t ps2_get_direction(void)
{

	if (PS2_X_DATA > (0xC00)) {  
			return PS2_DIR_LEFT;
	}else if (PS2_X_DATA < 0x3FF) {
			return PS2_DIR_RIGHT;
	}else if (PS2_Y_DATA > 0xC00) {
			return PS2_DIR_DOWN;
	}else if (PS2_Y_DATA < 0x3FF) {
			return PS2_DIR_UP;
	}
	return PS2_DIR_CENTER;
}

//*****************************************************************************
// TIMER1 ISR is used to blink the red led.
//*****************************************************************************
void TIMER1A_Handler(void)
{
	if (red) {
	    red = false;
	    lp_io_set_pin(RED_BIT);
	}
	else{
	   red = true;
	   lp_io_clear_pin(RED_BIT);
	}
	TIMER1->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// TIMER2 ISR is used to determine when to move the CACTUS
//*****************************************************************************
void TIMER2A_Handler(void)
{	
	PS2_DIR_t direction = PS2_DIR;
	
	if(PAUSED){
		// Clear the interrupt and return
	  TIMER2->ICR |= TIMER_ICR_TATOCINT;
		return;
	}
	
	if((direction == PS2_DIR_LEFT) | (direction == PS2_DIR_RIGHT)){
		ALERT_CACTUS = true;
		ALERT_GRASS = true;
	}
	// Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
//This controls when and how the Trex jumps
//*****************************************************************************
void TIMER3A_Handler(void){
	if(PAUSED){
		TIMER3->ICR |= TIMER_ICR_TATOCINT;
		return;
	}
	
	if(JUMP){
		if(jump_count==0){
			jump_count = 200;
		}
		JUMP = false;
	}
	ALERT_TREX = true;
	TIMER3->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// is called when a push button interrupts. Sets button boolean.
//*****************************************************************************
void GPIOF_Handler(void)
{
	if(PAUSED){
		GPIOF->ICR |= 0x01;
		return;
	}
	if (!BUTTON_PRESS) {
		BUTTON_PRESS = true;
	}
	GPIOF->ICR |= 0x01; // clear the interrupt
}

//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
//*****************************************************************************
void TIMER4A_Handler(void)
{	
	if(PAUSED){
		TIMER4->ICR |= TIMER_ICR_TATOCINT;
		return;
	}
	ADC0->PSSI |= ADC_PSSI_SS2;
	// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
}

//*****************************************************************************
// TIMER5 ISR is used to determine when to move the PTERODACTYL
//*****************************************************************************
void TIMER5A_Handler(void){

	
	if(PAUSED){
		TIMER5->ICR |= TIMER_ICR_TATOCINT;
		return;
	}

	ALERT_PTER = true;
	
	TIMER5->ICR |= TIMER_ICR_TATOCINT;
}

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void)
{
	if(PAUSED){
		ADC0->ISC |= ADC_ISC_IN2;
		return;
	}
	PS2_Y_DATA = ADC0->SSFIFO2;
	PS2_X_DATA = ADC0->SSFIFO2;
	PS2_DIR = ps2_get_direction();
  // Clear the interrupt
  ADC0->ISC |= ADC_ISC_IN2;
}

