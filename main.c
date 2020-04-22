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

volatile uint16_t PTERODACTYL_X_COORD = 240;
volatile uint16_t PTERODACTYL_Y_COORD = 240;
volatile uint16_t TREX_X_COORD = 0;
volatile uint16_t TREX_Y_COORD = 0;
volatile uint16_t CACTUS_X_COORD = 240;
volatile uint16_t CACTUS_Y_COORD = 240;

//alerts for image rendering
volatile bool ALERT_TREX = true;
volatile bool ALERT_PTER = false;
volatile bool ALERT_CACTUS = false;

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
	if ( ( ( (cactus_y_coord + (cactus_height/2)) < (trex_y_coord + (trex_height/2)) ) & 
			( (cactus_y_coord + (cactus_height/2)) > (trex_y_coord - (trex_height/2)) ) ) |
			( ( (cactus_y_coord - (cactus_height/2)) < (trex_y_coord + (trex_height/2)) ) & 
			( (cactus_y_coord - (cactus_height/2)) > (trex_y_coord - (trex_height/2)) ) )){
				if ((( (cactus_x_coord + (cactus_width/2)) < (trex_x_coord + (trex_width/2)) ) & 
						( (cactus_x_coord + (cactus_width/2)) > (trex_x_coord - (trex_width/2)))) |
						(( (cactus_x_coord - (cactus_width/2)) < (trex_x_coord + (trex_width/2))) & 
						( (cactus_x_coord - (cactus_width/2)) > (trex_x_coord - (trex_width/2))))){
							
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
	  bool crouching = false;
		int health_bar = 8;
		
		//init_screen();
		init_hardware();	
	
		

		//start game
		while(!game_start){			
			//Checks if player starts game
			game_start = game_menu();
			
			
		}
		
		lcd_clear_screen(LCD_COLOR_BLACK);
		update_health_bar(health_bar);
		
		//play game
    while(!game_over){
			hit = false;
			//SPACEBAR/PAUSE FUNCTIONALITY: needs to be done
				
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
				if(crouching){
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
				}
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
				if(crouching){
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
				}
			}
			
			//TREX
			if(ALERT_TREX){
				ALERT_TREX = false;
				
				if(crouching){
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
				
				//check if hit
				if(crouching){
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
				}
			}
		};
}
