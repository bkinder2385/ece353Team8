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
#include "project_images.h"

#define CROUCH_COMP 6

//image locations
volatile uint16_t PTERODACTYL_X_COORD = 214;
volatile uint16_t PTERODACTYL_Y_COORD = 165; //LOWEST DESIRED Y COORD	
volatile uint16_t TREX_X_COORD = 34;
volatile uint16_t TREX_Y_COORD = 204;
volatile uint16_t CACTUS_X_COORD = 219;
volatile uint16_t CACTUS_Y_COORD = 207;

//alerts for image rendering
volatile bool ALERT_TREX = true;
volatile bool ALERT_PTER = false;
volatile bool CLEAR_PTER = false;
volatile bool ALERT_CACTUS = false;
volatile bool CLEAR_CACTUS = false;

//Action Determination
volatile bool BUTTON_PRESS = false;
volatile bool JUMP = false;
volatile bool CROUCH = false;
int PRESSED_EVEN;
int DEBOUNCE_INCREMENT;

//pause status
volatile bool PAUSED = false;


//*****************************************************************************
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}

//*****************************************************************************
// Determines if any part of the image would be off the screen if the image
// is moved in the specified direction. Only left and right valid for cactus 
// and pterodactyls.
//*****************************************************************************
bool contact_edge(
    volatile PS2_DIR_t direction,
    volatile uint16_t x_coord, 
    volatile uint16_t y_coord, 
    uint8_t image_width
)
{
	switch(direction) {
		case PS2_DIR_RIGHT:
		{
			if((x_coord - (image_width/2)) <= 1){
				return true;
			}
		}
		case PS2_DIR_LEFT:
		{
			if((x_coord + (image_width/2)) >= 240){
				return true;
			}
		}
		default:{
		}
	}
	return false;
}

//*****************************************************************************
// Moves the image by one pixel in the provided direction.  The second and 
// third parameter should modify the current location of the image (pass by
// reference)
//*****************************************************************************
void move_image(
        volatile PS2_DIR_t direction,
        volatile uint16_t *x_coord, 
        volatile uint16_t *y_coord, 
        uint8_t image_height, 
        uint8_t image_width)
{
	switch (direction){
		 case PS2_DIR_DOWN: //ONLY USE FOR JUMP METHOD
		 {
			 *y_coord = *y_coord + 1;
			 break;
		 }
		 case PS2_DIR_UP: //ONLY USE FOR JUMP METHOD
		 {
			 *y_coord = *y_coord - 1;
			 break;
		 }
		 case PS2_DIR_RIGHT:
		 {
			 *x_coord = *x_coord - 1;
			 break;
		 }
		 case PS2_DIR_LEFT:
		 {
			 *x_coord = *x_coord + 1;
			 break;
		 }
		 default:
			 break;
	 }
	 return;
}

//*****************************************************************************
// Determines is any portion of the two images are overlapping.  An image is
// considered to be overlapping if the two rectangles determined by the image
// height and widths are overlapping.  An overlap occurs even if the area that
// overlaps are portions of the images where the pixels do not display on the
// screen.
//*****************************************************************************
bool check_if_hit(
        volatile uint16_t trex_x_coord, 
        volatile uint16_t trex_y_coord, 
        uint8_t trex_height, 
        uint8_t trex_width,
        volatile uint16_t cactus_x_coord, 
        volatile uint16_t cactus_y_coord, 
        uint8_t cactus_height, 
        uint8_t cactus_width,
				volatile uint16_t pterodactyl_x_coord, 
        volatile uint16_t pterodactyl_y_coord, 
        uint8_t pterodactyl_height, 
        uint8_t pterodactyl_width
)
{
	bool overlap = false;
	
	//Cactus hit		
	if ((( (cactus_x_coord + (cactus_width/2)) < (trex_x_coord + (trex_width/2)) ) & 
						( (cactus_x_coord + (cactus_width/2)) > (trex_x_coord - (trex_width/2)))) |
						(( (cactus_x_coord - (cactus_width/2)) < (trex_x_coord + (trex_width/2))) & 
						( (cactus_x_coord - (cactus_width/2)) > (trex_x_coord - (trex_width/2))))){
							
							if( (((cactus_y_coord + (cactus_height/2)) < (trex_x_coord + (trex_width/2) )) &
						( (cactus_y_coord + (cactus_height/2)) > (trex_x_coord - (trex_width/2))) ) |
							(((cactus_y_coord - (cactus_height/2)) < (trex_y_coord + (trex_height/2))) & 
						( (cactus_y_coord - (cactus_height/2)) > (trex_y_coord - (trex_height/2))))){
							
							  overlap = true;
								return overlap;
							}
	}
			
	//Pterodactyl hit
	if ( ( ( (pterodactyl_y_coord + (pterodactyl_height/2)) < (trex_y_coord + (trex_height/2)) ) & 
			( (pterodactyl_y_coord + (pterodactyl_height/2)) > (trex_y_coord - (trex_height/2)) ) ) |
			( ( (pterodactyl_y_coord - (pterodactyl_height/2)) < (trex_y_coord + (trex_height/2)) ) & 
			( (pterodactyl_y_coord - (pterodactyl_height/2)) > (trex_y_coord - (trex_height/2)) ) ) ){
				if ((( (pterodactyl_x_coord + (pterodactyl_width/2)) < (trex_x_coord + (trex_width/2)) ) & 
						( (pterodactyl_x_coord + (pterodactyl_width/2)) > (trex_x_coord - (trex_width/2))) ) |
						(( (pterodactyl_x_coord - (pterodactyl_width/2)) < (trex_x_coord + (trex_width/2))) & 
						( (pterodactyl_x_coord - (pterodactyl_width/2)) > (trex_x_coord - (trex_width/2))))){
							
							overlap = true;
							return overlap;
				}
	}
	return overlap;
  
}

//*****************************************************************************
// Updates the health bar. When hit, decrease the number of leds by two.
//*****************************************************************************
void update_health_bar(int health_bar){
	
	io_expander_byte_write(I2C1_BASE, 0x00, 0x00);	// ENABLE AS OUTPUTS
	io_expander_byte_write(I2C1_BASE, 0x01, 0xFF);
	io_expander_byte_write(I2C1_BASE, 0x02, 0x00);	// SAME POLARITY FOR GPIO
	io_expander_byte_write(I2C1_BASE, 0x03, 0x00);
	io_expander_byte_write(I2C1_BASE, 0x0E, 0xFF); 	// Enable INTERRUPT ON FLAG
	io_expander_byte_write(I2C1_BASE, 0x10, 0xFF); 	// Enable INTERRUPT ON PIN
	
	if(health_bar == 8){
		io_expander_byte_write(I2C1_BASE, 0x12, 0xFF);	// ENABLE GPIO
	}
	if(health_bar == 7){
		io_expander_byte_write(I2C1_BASE, 0x12, 0x7F);	// ENABLE GPIO
	}
	if(health_bar == 6){
		io_expander_byte_write(I2C1_BASE, 0x12, 0x3F);	// ENABLE GPIO
	}
	if(health_bar == 5){
		io_expander_byte_write(I2C1_BASE, 0x12, 0x1F);	// ENABLE GPIO
	}
	if(health_bar == 4){
			io_expander_byte_write(I2C1_BASE, 0x12, 0x0F);	// ENABLE GPIO
	}
	if(health_bar == 3){
		io_expander_byte_write(I2C1_BASE, 0x12, 0x07);	// ENABLE GPIO
	}
	if(health_bar == 2){
		io_expander_byte_write(I2C1_BASE, 0x12, 0x03);	// ENABLE GPIO
	}
	if(health_bar == 1){
		io_expander_byte_write(I2C1_BASE, 0x12, 0x01);	// ENABLE GPIO
	}
	if(health_bar == 0){
		io_expander_byte_write(I2C1_BASE, 0x12, 0x00);	// ENABLE GPIO
	}
}

// debounces the button
bool debounce(void){

	uint8_t read_val;
	io_expander_byte_read(I2C1_BASE, MCP23017_INTFB_R, &read_val);
	printf("%x\n", read_val);
	if(read_val == 0) {
		DEBOUNCE_INCREMENT++;
	}
	else {
		DEBOUNCE_INCREMENT = 0;
	}
	if (DEBOUNCE_INCREMENT >= 2) {
		return true;
	}
	else {
		return false;
	}
}

//*****************************************************************************
// Reads button presses and determines which action to take.
//*****************************************************************************
void read_buttons(void){
	
	uint8_t read_val;
	uint8_t unwanted_val;
	bool debounced;
	int i;
	
	io_expander_byte_read(I2C1_BASE, MCP23017_INTFB_R, &read_val);
	io_expander_byte_read(I2C1_BASE, 0x13, &unwanted_val); // put data at GPIOB in 

	do{
		debounced = debounce();
		while(i < 20000){
			i++;
		}
		i = 0;
	}while(!debounced);
	
	PRESSED_EVEN++;
	if (PRESSED_EVEN == 1) {
		return;
	}
	else{
		PRESSED_EVEN = 0;
	}

	if (read_val == 0x01) {
		JUMP = true;
	}
	if (read_val == 0x02){
		if(CROUCH){
			CROUCH = false;
			TREX_Y_COORD -= CROUCH_COMP;
		}else{
			CROUCH = true;
			//Clear standing trex
					lcd_draw_image(
                          TREX_X_COORD,                       // X Center Point
                          trexstandingWidthPixels,   					// Image Horizontal Width
                          TREX_Y_COORD,								        // Y Center Point
                          trexstandingHeightPixels,  					// Image Vertical Height
                          trexstandingBitmaps,       					// Image
                          LCD_COLOR_BLACK,           					// Foreground Color
                          LCD_COLOR_BLACK          						// Background Color
                        );
			TREX_Y_COORD += CROUCH_COMP;
		}
	}
		
}

//*****************************************************************************
// Game Setup. Initializes lcd and capacitive touch. Shows START button and 
// game title (TBD) and trex_standing?? if START is touched, it returns true.
// Returns false otherwise.
//*****************************************************************************
bool game_menu(void){
	
	uint16_t x_value;
	uint16_t y_value;
	//Prints game menu screen
	lcd_draw_image(
                          120,                       // X Center Point
                          start_btnWidthPixels,   // Image Horizontal Width
                          220,                       // Y Center Point
                          start_btnHeightPixels,  // Image Vertical Height
                          start_btnBitmaps,       // Image
                          LCD_COLOR_BROWN,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
	lcd_draw_image(
                          120,                       // X Center Point
                          trexstandingWidthPixels,   // Image Horizontal Width
                          130,                       // Y Center Point
                          trexstandingHeightPixels,  // Image Vertical Height
                          trexstandingBitmaps,       // Image
                          LCD_COLOR_ORANGE,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
												
	lcd_draw_image(
                          120,                       // X Center Point
                          game_titleWidthPixels,   // Image Horizontal Width
                          75,                       // Y Center Point
                          game_titleHeightPixels,  // Image Vertical Height
                          game_titleBitmaps,       // Image
                          LCD_COLOR_RED,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
	
	//Checks if touch happened
	if(ft6x06_read_td_status() > 0){
		//check x and y to see if on start
		x_value = ft6x06_read_x();
		if((x_value >= (120 - (start_btnWidthPixels/2))) & (x_value <= (120 + (start_btnWidthPixels/2)))){
			y_value = ft6x06_read_y();
			if((y_value >= (220 - (start_btnHeightPixels/2))) & (y_value <= (220 + (start_btnHeightPixels/2)))){
				return true;
			}
		}
		
	}
	
	return false;
}

//*****************************************************************************
// Main play of game.
//*****************************************************************************
int 
main(void)
{
		bool game_start = false;
		bool game_over = false;
	  bool hit = false;
		int health_bar = 8;
		
		//init_screen();
		init_hardware();	
	
		

		//start game
		while(!game_start){			
			//Checks if player starts game
			game_start = game_menu();
			
			
		}
		
		lcd_clear_screen(LCD_COLOR_BLACK);
		lcd_draw_image(
                          (groundWidthPixels/2),                       // X Center Point
                          groundWidthPixels,   // Image Horizontal Width
                          (319 - (groundHeightPixels/2)),                       // Y Center Point
                          groundHeightPixels,  // Image Vertical Height
                          groundBitmaps,       // Image
                          LCD_COLOR_BROWN,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
		lcd_draw_image(
                          TREX_X_COORD,                       // X Center Point
                          trexstandingWidthPixels,   // Image Horizontal Width
                          TREX_Y_COORD,                       // Y Center Point
                          trexstandingHeightPixels,  // Image Vertical Height
                          trexstandingBitmaps,       // Image
                          LCD_COLOR_ORANGE,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
		
		update_health_bar(health_bar);
		init_timers();
		
		//play game
    while(!game_over){
			
			hit = false;
			
			//SPACEBAR/PAUSE FUNCTIONALITY: needs to be done
		  
			if(fgetc(stdin) == ' '){
				if(PAUSED){
					PAUSED = false;
				}else{
					PAUSED = true;
				}
			}
 			if(PAUSED){
				 
				//print PAUSED in upper right corner
 				continue;
			}	
			
			//if a button was pressed, determine status
			if(BUTTON_PRESS) {
				read_buttons();
				BUTTON_PRESS = false;
			}
			
			//CACTUS
			if(ALERT_CACTUS){
				ALERT_CACTUS = false;
				
				//DRAW CACTUS
				lcd_draw_image(
                          CACTUS_X_COORD,                       // X Center Point
                          cactusWidthPixels,   // Image Horizontal Width
                          CACTUS_Y_COORD,                       // Y Center Point
                          cactusHeightPixels,  // Image Vertical Height
                          cactusBitmaps,       // Image
                          LCD_COLOR_GREEN,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
				
				//check if hit
				if(CROUCH){
					hit = check_if_hit(TREX_X_COORD, TREX_Y_COORD, trexcrouchingHeightPixels, trexcrouchingWidthPixels,
														 CACTUS_X_COORD, CACTUS_Y_COORD, cactusHeightPixels, cactusWidthPixels,
														 PTERODACTYL_X_COORD, PTERODACTYL_Y_COORD, pterodactylHeightPixels, pterodactylWidthPixels);
				}else {
					hit = check_if_hit(TREX_X_COORD, TREX_Y_COORD, trexstandingHeightPixels, trexstandingWidthPixels,
														 CACTUS_X_COORD, CACTUS_Y_COORD, cactusHeightPixels, cactusWidthPixels,
														 PTERODACTYL_X_COORD, PTERODACTYL_Y_COORD, pterodactylHeightPixels, pterodactylWidthPixels);
				}
			
				if(hit){
					//decrease led (health bar)
					health_bar--;
					//decrease leds
					update_health_bar(health_bar);
				
					//check if game over (all leds off)
					if(health_bar == 0){
						//show game over
						game_over = true;
						continue;
					}
					CLEAR_CACTUS = true;
				}
			}
			if(CLEAR_CACTUS){ //Clears cactus
				lcd_draw_image(
                          CACTUS_X_COORD,                       // X Center Point
                          cactusWidthPixels,   // Image Horizontal Width
                          CACTUS_Y_COORD,                       // Y Center Point
                          cactusHeightPixels,  // Image Vertical Height
                          cactusBitmaps,       // Image
                          LCD_COLOR_BLACK,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
				CACTUS_X_COORD = 219;
				CACTUS_Y_COORD = 207;
				CLEAR_CACTUS = false;
				CACTUS_RUN = false;
			}
			
			//PTERODACTYL
			if(ALERT_PTER){
				ALERT_PTER = false;
				
				//DRAW PTERODACTYL
				lcd_draw_image(
                          PTERODACTYL_X_COORD,                       // X Center Point
                          pterodactylWidthPixels,   // Image Horizontal Width
                          PTERODACTYL_Y_COORD,                       // Y Center Point
                          pterodactylHeightPixels,  // Image Vertical Height
                          pterodactylBitmaps,       // Image
                          LCD_COLOR_RED,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
				
				//check if hit
				if(CROUCH){
					hit = check_if_hit(TREX_X_COORD, TREX_Y_COORD, trexcrouchingHeightPixels, trexcrouchingWidthPixels,
														 CACTUS_X_COORD, CACTUS_Y_COORD, cactusHeightPixels, cactusWidthPixels,
														 PTERODACTYL_X_COORD, PTERODACTYL_Y_COORD, pterodactylHeightPixels, pterodactylWidthPixels);
				}else {
					hit = check_if_hit(TREX_X_COORD, TREX_Y_COORD, trexstandingHeightPixels, trexstandingWidthPixels,
														 CACTUS_X_COORD, CACTUS_Y_COORD, cactusHeightPixels, cactusWidthPixels,
														 PTERODACTYL_X_COORD, PTERODACTYL_Y_COORD, pterodactylHeightPixels, pterodactylWidthPixels);
				}
			
				if(hit){
					//decrease led (health bar)
					health_bar--;
					//decrease leds
					update_health_bar(health_bar);
				
					//check if game over (all leds off)
					if(health_bar == 0){
						//show game over
						game_over = true;
						continue;
					}
					CLEAR_PTER = true;
				}
			}
			if(CLEAR_PTER){ //Clears cactus
				lcd_draw_image(
                          PTERODACTYL_X_COORD,                       // X Center Point
                          pterodactylWidthPixels,   // Image Horizontal Width
                          PTERODACTYL_Y_COORD,                       // Y Center Point
                          pterodactylHeightPixels,  // Image Vertical Height
                          pterodactylBitmaps,       // Image
                          LCD_COLOR_BLACK,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
				PTERODACTYL_X_COORD = 214;
				CLEAR_PTER = false;
				P_FLY = false;
			}
			
			//TREX
			if(ALERT_TREX){
				ALERT_TREX = false;
				
				if(CROUCH){
					//Draw crouched trex
					lcd_draw_image(
                          TREX_X_COORD,                       // X Center Point
                          trexcrouchingWidthPixels,   // Image Horizontal Width
                          TREX_Y_COORD,                       // Y Center Point
                          trexcrouchingHeightPixels,  // Image Vertical Height
                          trexcrouchingBitmaps,       // Image
                          LCD_COLOR_ORANGE,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
				}else {
					lcd_draw_image(
                          TREX_X_COORD,                       // X Center Point
                          trexstandingWidthPixels,   // Image Horizontal Width
                          TREX_Y_COORD,                       // Y Center Point
                          trexstandingHeightPixels,  // Image Vertical Height
                          trexstandingBitmaps,       // Image
                          LCD_COLOR_ORANGE,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
				}
			}
		};
		
		//GAME OVER SCREEN
		lcd_clear_screen(LCD_COLOR_BLACK);
		lcd_draw_image(
                          120,                       // X Center Point
                          trexstandingWidthPixels,   // Image Horizontal Width
                          240,                       // Y Center Point
                          trexstandingHeightPixels,  // Image Vertical Height
                          trexstandingBitmaps,       // Image
                          LCD_COLOR_ORANGE,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
		lcd_draw_image(
                          120,                       // X Center Point
                          gameoverWidthPixels,   // Image Horizontal Width
                          70,                       // Y Center Point
                          gameoverHeightPixels,  // Image Vertical Height
                          gameoverBitmaps,       // Image
                          LCD_COLOR_RED,           // Foreground Color
                          LCD_COLOR_BLACK          // Background Color
                        );
		//print score???
		
		//forever loop to keep the blinker going?
		while(1){};
}
