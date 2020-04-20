// Copyright (c) 2014, Joe Krachey
// All rights reserved.
//
// Redistribution and use in binary form, with or without modification, 
// are permitted provided that the following conditions are met:
//
// 1. Redistributions in binary form must reproduce the above copyright 
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

//*****************************************************************************
// main.c
// Author: jkrachey@wisc.edu
//*****************************************************************************
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "io_expander.h"

#include "main.h"


//*****************************************************************************
// 
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
// 
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}


//*****************************************************************************
//*****************************************************************************
void initializeBoard(void)
{
  DisableInterrupts();
  init_serial_debug(true, true);
  io_expander_init();
  EnableInterrupts();
}


//*****************************************************************************
//*****************************************************************************
void eeprom_test(void)
{
  uint8_t addr;
  uint8_t values[20];
  uint8_t read_val;
  bool status = true;
  
 /* for(addr = ADDR_START; addr <(ADDR_START+NUM_BYTES); addr++)
  {
      values[ addr - ADDR_START] = rand();
      printf("Writing %i\n\r",values[addr-ADDR_START]);
      io_expander_byte_write(I2C1_BASE, addr, values[addr-ADDR_START]);
  }
  
  for(addr = ADDR_START; addr <(ADDR_START+NUM_BYTES); addr++)
  {
      eeprom_byte_read(I2C1_BASE,addr, &read_val);
      if( read_val != values[addr-ADDR_START])
      {
        printf("ERROR: addr: %i write: %i read %i\n\r",addr,values[addr-ADDR_START], read_val);
        status = false;
      }
  } */
	//i2c_status_t io_expander_wait_for_write( int32_t  i2c_base)
	//io_expander_byte_write(I2C1_BASE, 0x13, 0xFF);
	io_expander_byte_write(I2C1_BASE, 0x00, 0x00);	// ENABLE AS OUTPUTS
	io_expander_byte_write(I2C1_BASE, 0x01, 0xFF);
	
	io_expander_byte_write(I2C1_BASE, 0x02, 0x00);	// SAME POLARITY FOR GPIO
	io_expander_byte_write(I2C1_BASE, 0x03, 0x00);
	
//	io_expander_byte_write(I2C1_BASE, 0x04, 0xFF); 	// Enable INTERRUPT ON PIN
//	io_expander_byte_write(I2C1_BASE, 0x05, 0xFF);
	
//	io_expander_byte_write(I2C1_BASE, 0x06, 0x00); 	// DEFAULT VALUE
//	io_expander_byte_write(I2C1_BASE, 0x07, 0x00);
	
//	io_expander_byte_write(I2C1_BASE, 0x08, 0x00); 	// INTERRUPT CONTROL: COMPARE TO PREVIOUS VALUE
//	io_expander_byte_write(I2C1_BASE, 0x09, 0x00);
	
//	io_expander_byte_write(I2C1_BASE, 0x0E, 0xFF); 	// Enable INTERRUPT ON FLAG
//	io_expander_byte_write(I2C1_BASE, 0x0F, 0xFF);
	
//	io_expander_byte_write(I2C1_BASE, 0x10, 0xFF); 	// Enable INTERRUPT ON PIN
//	io_expander_byte_write(I2C1_BASE, 0x11, 0xFF);
	
	io_expander_byte_write(I2C1_BASE, 0x12, 0xFF);	// ENABLE GPIO
//	io_expander_byte_write(I2C1_BASE, 0x13, 0xFF);
	
//	io_expander_byte_write(I2C1_BASE, 0x14, 0x00);	// ENABLE OUTPUT LATCH
//	io_expander_byte_write(I2C1_BASE, 0x15, 0x00);
	
  
  if(status)
  {
    printf("EEPROM Test passed\n\r");
  }
  else
  {
    printf("EEPROM Test failed\n\r");
  }
  
}

//*****************************************************************************
//*****************************************************************************
int 
main(void)
{

  
  initializeBoard();

  if( !validate_ice(ICE_I2C_EEPROM) )
  {
     while(1){};
  }
  
  printf("\n\r");
  printf("**************************************\n\r");
  printf("* In Class - I2C\n\r");
  printf("**************************************\n\r");
  
  eeprom_test();
  
  while(1){
  
  };

}

