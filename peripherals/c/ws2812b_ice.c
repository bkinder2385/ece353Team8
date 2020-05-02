#include "ws2812b.h"

/*******************************************************************************
* Function Name: rotate_ws2812bs
********************************************************************************
* Summary:
*
*   Rotates the WS2812B structs are rotates in the array by one position.  This
*   can be done by coping each WS2812B stuct to the next highest index in the
*   array.  The last WS2812B struct should be moved to base[0]
*
*
* Parameters:
*   base         Base address of the WS2812B_t array
*   num_leds     Size number WS2812B_t structs in the array
*
* Return:
*  void
*
*******************************************************************************/
void ws2812b_rotate(
	WS2812B_t* base,
	uint8_t num_leds
)
{
	static uint32_t i = 0;
//	base->red = 0x00;
//	base->blue = 0x80;
	if (i == (num_leds - 1)) {
		base = base - num_leds + 1;
		i = 0;
	}
	else {
		base = base + 1;
		i++;
	}
}

/*******************************************************************************
* Function Name: pulse_ws2812b
********************************************************************************
* Summary:
*
*   Pulses the RED LED for a each WS2812B LED in a series of WS2812B LEDs.
*
*   The function will contain a static variable named direction that indicates
*   if the value RED color codes are currently decrementing or incrementing.
*
*   If the direction is incrementing AND the value of the red field is less than
*   0xFF, the function will increment the red filed by 1 for each structure in
*   the array.
*
*   If the direction is incrementing and the value of the red filed is equal to
*   0xFF, the function will change the direction to be counting down and decrement
*   the red filed by 1 for each structure in the array.
*
*   If the direction is decrementing AND the value of the red field is greater than
*   0x00, the function will decrement the red filed by 1 for each structure in
*   the array.
*
*   If the direction is decrementing and the value of the red filed is equal to
*   0x00, the function will change the direction to be counting up and increment
*   the red filed by 1 for each structure in the array.
*
* Parameters:
*   base         Base address of the WS2812B_t array
*   num_leds     Size number WS2812B_t structs in the array
*
* Return:
*  void
*
*******************************************************************************/
void ws2812b_pulse(
	WS2812B_t* base,
	uint8_t num_leds
)
{
	static uint8_t direction = 1; // 1 to increment, 0 to decrement
	int i = 0;
	while (i < num_leds) {
		if ((direction == 1) && (base[i].red < 0xFF)) {
			base->red++;
		}
		else if ((direction == 1) && ((base+i)->red == 0xFF)) {
			direction = 0;
			base->red--;
		}
		else if ((direction == 0) && ((base+i)->red > 0)) {
			base->red--;
		}
		else if ((direction == 0) && ((base+i)->red == 0)) {
			direction = 1;
			base->red++;
		}
		i++;
	}
}
