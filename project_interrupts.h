// Copyright (c) 2015-17, Joe Krachey
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
#ifndef __PROJECT_INTERRUPTS_H__
#define __PROJECT_INTERRUPTS_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "TM4C123.h"
#include "gpio_port.h"
#include "ps2.h"
#include "lcd.h"
#include "project_images.h"
#include "main.h"

extern volatile uint16_t PS2_X_DATA;
extern volatile uint16_t PS2_Y_DATA;
extern volatile PS2_DIR_t PS2_DIR;
extern volatile bool BUTTON_PRESS;
extern volatile uint16_t jump_count;

extern volatile bool P_FLY;
extern volatile bool CACTUS_RUN;


uint16_t proc_pterodactyl(uint16_t appear);

//*****************************************************************************
// TIMER2 ISR is used to determine when to move the TREX and CACTUS
//*****************************************************************************
void TIMER2A_Handler(void);

//This controls when and how the Trex jumps
void TIMER3A_Handler(void);

// is called when a push button interrupts. Sets button boolean.
void GPIOF_Handler(void);

//*****************************************************************************
// TIMER4 ISR is used to trigger the ADC
//*****************************************************************************
void TIMER4A_Handler(void);

//*****************************************************************************
// TIMER5 ISR is used to determine when to move the PTERODACTYL
//*****************************************************************************
void TIMER5A_Handler(void);

//*****************************************************************************
// ADC0 SS2 ISR
//*****************************************************************************
void ADC0SS2_Handler(void);

#endif
