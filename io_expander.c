#include "io_expander.h"

//*****************************************************************************
// Used to determine if the IO_Expander is busy writing the last transaction to 
// non-volatile storage
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
// Returns
// I2C_OK is returned one the IO_Expander is ready to write the next byte
//*****************************************************************************
static 
i2c_status_t io_expander_wait_for_write( int32_t  i2c_base)
{
  
  i2c_status_t status;
  
  if( !i2cVerifyBaseAddr(i2c_base) )
  {
    return  I2C_INVALID_BASE;
  }

  // Set the I2C address to be the IO_Expander and in Write Mode
  status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);     

  // Poll while the device is busy.  The  MCP24LC32AT will not ACK
  // writing an address while the write has not finished.
  do 
  {
    // The data we send does not matter.  This has been set to 0x00, but could
    // be set to anything
    status = i2cSendByte( i2c_base, 0x00, I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
    
    // Wait for the address to finish transmitting
    while ( I2CMasterBusy(i2c_base)) {};
    
    // If the address was not ACKed, try again.
  } while (I2CMasterAdrAck(i2c_base) == false);

  return  status;
}
  
  
//*****************************************************************************
// Writes a single byte of data out to the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being written.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       Data written to the EEPROM.
//
// Returns
// I2C_OK if the byte was written to the EEPROM.
//*****************************************************************************
i2c_status_t io_expander_byte_write
( 
  uint32_t  i2c_base,
  uint8_t  address,
  uint8_t   data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  //==============================================================
  // Set the I2C address to be the EEPROM
	// ADD CODE
  //==============================================================
		status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
		if (status != I2C_OK){
			return status;
		}
		
  
  // If the EEPROM is still writing the last byte written, wait
		io_expander_wait_for_write(i2c_base);
  
  //==============================================================
  // Send the Upper byte of the address
	// ADD CODE	
  //==============================================================
	//uint8_t addressHigh = (address >> 8);
	status = i2cSendByte(
     i2c_base,
     (uint8_t) address,
     I2C_MCS_START | I2C_MCS_RUN);
	if (status != I2C_OK){
		return status;
	} 

  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================
	//uint8_t addressLow = (address);
/*	status = i2cSendByte(
     i2c_base,
     (uint8_t) address ,
     I2C_MCS_RUN);
	if (status != I2C_OK){
		return status;
	} */
  
  //==============================================================
  // Send the Byte of data to write
	// ADD CODE
  //==============================================================
		status = i2cSendByte(
     i2c_base,
     data ,
     I2C_MCS_RUN /*| I2C_MCS_STOP */);
	if (status != I2C_OK){
		return status;
	}

  return status;
}

//*****************************************************************************
// Reads a single byte of data from the  MCP24LC32AT EEPROM.  
//
// Paramters
//    i2c_base:   a valid base address of an I2C peripheral
//
//    address:    16-bit address of the byte being read.  Only the lower
//                12 bits is used by the EEPROM
//
//    data:       data read from the EEPROM is returned to a uint8_t pointer.
//
// Returns
// I2C_OK if the byte was read from the EEPROM.
//*****************************************************************************
i2c_status_t io_expander_byte_read
( 
  uint32_t  i2c_base,
  uint8_t  address_read,
  uint8_t   *data
)
{
  i2c_status_t status;
  
  // Before doing anything, make sure the I2C device is idle
  while ( I2CMasterBusy(i2c_base)) {};

  // If the EEPROM is still writing the last byte written, wait
  io_expander_wait_for_write(i2c_base);

  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Write Mode
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_WRITE);
	if (status != I2C_OK){
		return status;
	}

  //==============================================================
  // Send the Upper byte of the address
	// ADD CODE
  //==============================================================
	//uint8_t addressHigh = (address >> 8);
/*	status = i2cSendByte(
     i2c_base,
     (uint8_t)(address >> 8) ,
     I2C_MCS_START | I2C_MCS_RUN);
	if (status != I2C_OK){
		return status;
	}  */

  //==============================================================
  // Send the Lower byte of the address
	// ADD CODE
  //==============================================================
	//uint8_t addressLow = (address);
	status = i2cSendByte(
     i2c_base,
     (uint8_t)address_read,
     I2C_MCS_START | I2C_MCS_RUN);
	if (status != I2C_OK){
		return status;
	}

  //==============================================================
  // Set the I2C slave address to be the EEPROM and in Read Mode
	// ADD CODE
  //==============================================================
	status = i2cSetSlaveAddr(i2c_base, MCP23017_DEV_ID, I2C_READ);
	if (status != I2C_OK){
		return status;
	}

  //==============================================================
  // Read the data returned by the EEPROM
	// ADD CODE
  //==============================================================
		status = i2cGetByte(
     i2c_base,
     data ,
     I2C_MCS_START | I2C_MCS_RUN | I2C_MCS_STOP);
	if (status != I2C_OK){
		return status;
	}
  
  return I2C_OK;
}

//*****************************************************************************
// Initialize the I2C peripheral
//*****************************************************************************
bool io_expander_init(void)
{
  
  if(gpio_enable_port(IO_EXPANDER_GPIO_BASE) == false)
  {
    return false;
  }
  
  // Configure SCL 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SCL_PCTL_M, IO_EXPANDER_I2C_SCL_PIN_PCTL)== false)
  {
    return false;
  }
    

  
  // Configure SDA 
  if(gpio_config_digital_enable(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_open_drain(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_alternate_function(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PIN)== false)
  {
    return false;
  }
    
  if(gpio_config_port_control(IO_EXPANDER_GPIO_BASE, IO_EXPANDER_I2C_SDA_PCTL_M, IO_EXPANDER_I2C_SDA_PIN_PCTL)== false)
  {
    return false;
  }
    
  //  Initialize the I2C peripheral
  if( initializeI2CMaster(IO_EXPANDER_I2C_BASE)!= I2C_OK)
  {
    return false;
  }
  
  return true;
  
}
