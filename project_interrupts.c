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
// TIMER2 ISR is used to determine when to move the TREX and CACTUS
//*****************************************************************************
void TIMER2A_Handler(void)
{	
	PS2_DIR_t direction = PS2_DIR;
	bool contact = contact_edge( direction, CACTUS_X_COORD,  CACTUS_Y_COORD, cactusWidthPixels);
	
	if (contact) {
		//clear cactus
		ALERT_CACTUS = false;
	}else{
		move_image(direction, &CACTUS_X_COORD, &CACTUS_Y_COORD, cactusHeightPixels, cactusWidthPixels);
		ALERT_CACTUS = true;
	}
	
  // Clear the interrupt
	TIMER2->ICR |= TIMER_ICR_TATOCINT;
}

//This controls when and how the Trex jumps
void TIMER3A_Handler(void){
	if(JUMP){
		jump_count = 110;
		JUMP = false;
	}
	if(jump_count > 0){
		if(jump_count > 55){
			if(CROUCH){
				move_image(PS2_DIR_UP, &TREX_X_COORD, &TREX_Y_COORD, trexcrouchingHeightPixels, trexcrouchingWidthPixels);
			}else {
				move_image(PS2_DIR_UP, &TREX_X_COORD, &TREX_Y_COORD, trexstandingHeightPixels, trexstandingWidthPixels);
			}
		}else {
			if(jump_count > 55){
				if(CROUCH){
					move_image(PS2_DIR_DOWN, &TREX_X_COORD, &TREX_Y_COORD, trexcrouchingHeightPixels, trexcrouchingWidthPixels);
				}else {
					move_image(PS2_DIR_DOWN, &TREX_X_COORD, &TREX_Y_COORD, trexstandingHeightPixels, trexstandingWidthPixels);
				}
			}
		}
		jump_count--;
	}
	ALERT_TREX = true;
}

// is called when a push button interrupts. Sets button boolean.
void GPIOF_Handler(void)
{
	if (!BUTTON_PRESS) {
		BUTTON_PRESS = true;
	}
	GPIOF->ICR |= 0x01; // clear the interrupt
}
