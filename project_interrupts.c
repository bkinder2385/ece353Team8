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

//IS A HAZARD CURRENTLY OUT?
volatile bool P_FLY = false;
volatile bool CACTUS_RUN = false;

//RANDOM PTER Y LOCATIONS
static const uint16_t  START_STATE = 0xACE7u;
static const uint16_t  YLOCATION[] = {165, 0, 0, 0, 0, 150, 155, 0};

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
uint16_t proc_pterodactyl(){
	return YLOCATION[generate_random_number()%8];
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
// TIMER2 ISR is used to determine when to move the PTERODACTYL and CACTUS
//*****************************************************************************
void TIMER2A_Handler(void)
{	
	PS2_DIR_t direction = PS2_DIR;
	bool contact = contact_edge( direction, CACTUS_X_COORD,  CACTUS_Y_COORD, cactusWidthPixels);
	uint16_t pterLocation = proc_pterodactyl();
	uint16_t cactusProc = generate_random_number()%3;
	
	if (contact) {
		//clear cactus
		CLEAR_CACTUS = true;
		CACTUS_RUN = false;
		ALERT_CACTUS = false;
	}else if(CACTUS_RUN){
		move_image(direction, &CACTUS_X_COORD, &CACTUS_Y_COORD, cactusHeightPixels, cactusWidthPixels);
		ALERT_CACTUS = true;
	}
	
	contact = contact_edge( direction, PTERODACTYL_X_COORD,  PTERODACTYL_Y_COORD, pterodactylWidthPixels);
	
	if (contact) {
		//clear pter
		CLEAR_PTER = true;
		P_FLY = false;
		ALERT_PTER = false;
	}else if (P_FLY){
		move_image(PS2_DIR_RIGHT, &PTERODACTYL_X_COORD, &PTERODACTYL_Y_COORD, pterodactylHeightPixels, pterodactylWidthPixels);
		ALERT_PTER = true;
	}
	
	if(!P_FLY){
		if(pterLocation>0){
			P_FLY = true;
		}
	}
	if(!CACTUS_RUN){
		if(cactusProc == 0){
			CACTUS_RUN = true;
		}
	}
	
  // Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//This controls when and how the Trex jumps
void TIMER3A_Handler(void){
	
	if(jump_count > 0){
		JUMP = false;
		if(jump_count > 75){
			if(CROUCH){
				move_image(PS2_DIR_UP, &TREX_X_COORD, &TREX_Y_COORD, trexcrouchingHeightPixels, trexcrouchingWidthPixels);
			}else {
				move_image(PS2_DIR_UP, &TREX_X_COORD, &TREX_Y_COORD, trexstandingHeightPixels, trexstandingWidthPixels);
			}
		}else {
			if(CROUCH){
				move_image(PS2_DIR_DOWN, &TREX_X_COORD, &TREX_Y_COORD, trexcrouchingHeightPixels, trexcrouchingWidthPixels);
			}else {
				move_image(PS2_DIR_DOWN, &TREX_X_COORD, &TREX_Y_COORD, trexstandingHeightPixels, trexstandingWidthPixels);
			}
		}
		jump_count--;
	}
	if(JUMP){
		jump_count = 150;
		JUMP = false;
	}
	ALERT_TREX = true;
	TIMER3->ICR |= TIMER_ICR_TATOCINT;
}

// is called when a push button interrupts. Sets button boolean.
void GPIOF_Handler(void)
{
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
	ADC0->PSSI |= ADC_PSSI_SS2;
	// Clear the interrupt
	TIMER4->ICR |= TIMER_ICR_TATOCINT; 
}

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void)
{
	PS2_X_DATA = ADC0->SSFIFO2;
	PS2_Y_DATA = ADC0->SSFIFO2;
	PS2_DIR = ps2_get_direction();
  // Clear the interrupt
  ADC0->ISC |= ADC_ISC_IN2;
}

