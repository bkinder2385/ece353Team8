#include "ft6x06.h"

//*****************************************************************************
// Sets the address to read/write from in the FT6x06  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    reg_address:    8-bit address of which register is going to be accessed.
//                    This is NOT the same as the I2C device address.
//
// Returns
// I2C_OK if the byte was written to the FT6X06.
//*****************************************************************************
static i2c_status_t ft6x06_set_addr
( 
  uint32_t  i2c_base,
  uint8_t  reg_address
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // ADD CODE
  // Set the I2C device address 
  //==============================================================
  
  status =   i2cSetSlaveAddr( i2c_base, FT6X06_DEV_ID, I2C_WRITE);
    
  if ( status != I2C_OK )
  {
    return status;
  }
  
//	status =   i2cSetSlaveAddr( i2c_base, FT6X06_DEV_ID, I2C_READ);
    
//  if ( status != I2C_OK )
//  {
//    return status;
//  }
  //==============================================================
  // ADD CODE
  // Send the register address
  //==============================================================
  status = i2cSendByte(i2c_base, reg_address, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);

  return status;
}

//*****************************************************************************
// Reads one byte of data from the current address of the FT6x06  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    data:       data to read.
//
// Returns
// I2C_OK if the byte was written to the FT6X06.
//*****************************************************************************
static i2c_status_t ft6x06_read_data
( 
  uint32_t  i2c_base,
  uint8_t *data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // ADD CODE
  // Set the I2C address 
  //==============================================================
  status = i2cSetSlaveAddr(i2c_base, FT6X06_DEV_ID, I2C_READ);
    
  
  if ( status != I2C_OK )
  {
    return status;
  }

  //==============================================================
  // ADD CODE
  // get the data
  //==============================================================
  status = i2cGetByte(i2c_base, data, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);


  return status;
}


//*****************************************************************************
// Read the number of active touch points.
//*****************************************************************************
uint8_t ft6x06_read_td_status(void)
{ 
  // ADD CODE
  // Return the number of active touch points.  The only valid values of the 
  // register will be 0, 1, or 2.
//	i2c_status_t status;
//	uint8_t td_status = 0;
	
//	status = ft6x06_set_addr(FT6X06_I2C_BASE, FT6X06_TD_STATUS_R);
//	if ( status != I2C_OK )
// {
//    return -1;
//  }
	
//	status = ft6x06_read_data(FT6X06_I2C_BASE, &td_status);
//	if ( status != I2C_OK )
//  {
//    return -1;
//  }
	
//	if(td_status != 0){
//		if(td_status != 1) {
//			if(td_status != 2) {
//				return -1;
//			}
//		}
//	}
	
//	return td_status;

//ANDY's Code
	uint8_t data;
	
	ft6x06_set_addr(FT6X06_I2C_BASE, FT6X06_TD_STATUS_R); 
	ft6x06_read_data(FT6X06_I2C_BASE, &data);
	if (data == 2 || data == 1){
		return data;
	}
	else {
	return 0;
	}
} 


//*****************************************************************************
// Read the X value of last touch event
//*****************************************************************************
uint16_t ft6x06_read_x(void)
{ 
  // ADD CODE
  // Return the X coordinate of the last touch point
  // This will require reading P1_XH and P1_XL
	i2c_status_t status;
	uint16_t x_val;
	uint8_t X_register;
	
	//Read upper 8 bits of x position and put into x_val
	status = ft6x06_set_addr(FT6X06_I2C_BASE, FT6X06_P1_XH_R);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	status = ft6x06_read_data(FT6X06_I2C_BASE, &X_register);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	x_val = X_register << 8;
	
	//Read lower 8 bits of x position and add to x_val
	status = ft6x06_set_addr(FT6X06_I2C_BASE, FT6X06_P1_XL_R);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	status = ft6x06_read_data(FT6X06_I2C_BASE, &X_register);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	x_val |= X_register;
	x_val = x_val & 0xFFF;
	
	// Change value so that 0 is left edge and 239 is the right edge
	x_val = ~((x_val%240) - 239) +1;
	
	return x_val;

} 

//*****************************************************************************
// Read the Y value of last touch event
//*****************************************************************************
uint16_t ft6x06_read_y(void)
{ 
  // ADD CODE
  // Return the Y coordinate of the last touch point 
  // This will require reading P1_YH and P1_YL
	i2c_status_t status;
	uint16_t y_val;
	uint8_t Y_register;
	
	//Read upper 8 bits of y position and put into y_val
	status = ft6x06_set_addr(FT6X06_I2C_BASE, FT6X06_P1_YH_R);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	status = ft6x06_read_data(FT6X06_I2C_BASE, &Y_register);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	y_val = Y_register << 8;
	
	//Read lower 8 bits of y position and add to y_val
	status = ft6x06_set_addr(FT6X06_I2C_BASE, FT6X06_P1_YL_R);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	status = ft6x06_read_data(FT6X06_I2C_BASE, &Y_register);
	if ( status != I2C_OK )
  {
    return -1;
  }
	
	y_val |= Y_register;
	y_val = y_val & 0xFFF;
	
	// Change value so that 0 is upper edge and 319 is lower edge
	y_val = ~((y_val%320) - 319) + 1;
	
	return y_val;
} 

//*****************************************************************************
// Test the ft6x06
//*****************************************************************************
bool ft6x06_init(void)
{ 
  //==============================
  // Configure I2C GPIO Pins
  //==============================  
  if(gpio_enable_port(FT6X06_GPIO_BASE) == false)
  {
    return false;
  }
  
  // Configure SCL 
  if(gpio_config_digital_enable(FT6X06_GPIO_BASE, FT6X06_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(FT6X06_GPIO_BASE, FT6X06_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(FT6X06_GPIO_BASE, FT6X06_I2C_SCL_PCTL_M, FT6X06_I2C_SCL_PIN_PCTL)== false)
  {
    return false;
  }
    

  
  // Configure SDA 
  if(gpio_config_digital_enable(FT6X06_GPIO_BASE, FT6X06_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_open_drain(FT6X06_GPIO_BASE, FT6X06_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(FT6X06_GPIO_BASE, FT6X06_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(FT6X06_GPIO_BASE, FT6X06_I2C_SDA_PCTL_M, FT6X06_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
  
  
  //  Initialize the I2C peripheral
  if( initializeI2CMaster(FT6X06_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
  
  return true;
  
} 

