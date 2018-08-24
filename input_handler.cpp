#include "input_handler.h"
#include "MK64F12.h"

/**
 * This file contains the codebase for reading inputs from the NES controller.
 * The source code was implemented based on the guideline provided in the link below,
 *
 * https://www.allaboutcircuits.com/projects/nes-controller-interface-with-an-arduino-uno/
 *
 *
 * which shows the details of NES controller integration into Arduino UNO.
 * We modified the code accordingly so that the same logic can be applied to K64F.
 *
 *
 *
 *
 */

// constants for GPIO pin index
const int CLOCK_GPIO_INDEX = 3;
const int LATCH_GPIO_INDEX = 2;
const int DATA_GPIO_INDEX  = 2;

/**
 * Set the clock to high signal.
 */
void set_clock_high() {
	PTC->PSOR |= (1 << CLOCK_GPIO_INDEX);
}

/**
 * Set the clock to low signal.
 */
void set_clock_low() {
	PTC->PCOR |= (1 << CLOCK_GPIO_INDEX);
}

/**
 * Set the latch to high signal.
 */
void set_latch_high() {
	PTC->PSOR |= (1 << LATCH_GPIO_INDEX);
}

/**
 * Set the latch to low signal.
 */
void set_latch_low() {
	PTC->PCOR |= (1 << LATCH_GPIO_INDEX);
}

/**
 * Initializes the input handler by opening the gates to the port
 * and setting the mux, input, output format
 */
void initialize_input_handler() {
	
	// enable clock line
	SIM->SCGC5 |= (1 << 11);
	PORTC->PCR[CLOCK_GPIO_INDEX] = PORT_PCR_MUX(001);
	// set the bit to output
	PTC->PDDR |= (1 << CLOCK_GPIO_INDEX);
	
	// enable latch line
	PORTC->PCR[LATCH_GPIO_INDEX] = PORT_PCR_MUX(001);
	// set the bit to output
	PTC->PDDR |= (1 << LATCH_GPIO_INDEX);
	
	// enable data line
	SIM->SCGC5 |= (1 << 9);
	PORTA->PCR[DATA_GPIO_INDEX] = PORT_PCR_MUX(001);
	// clear the bit to input
	PTA->PDDR &= ~(1 << DATA_GPIO_INDEX);
	
	// initialize the clock and latch to be high signal
	set_clock_high();
	set_latch_high();
}

/**
 * Read one bit information that just arrived at the
 * [DATA_GPIO_INDEX]-th location of PortA's PDIR register.
 * @return one bit read
 */
int read_bit(void){
	int read_bit = (1 << DATA_GPIO_INDEX);
	return (PTA->PDIR & read_bit);
}

/**
 * Read data input from the NES controller by periodically toggling
 * the latch and clock signal for all 8 bits.
 * @return 8-bit signal that was read from the input
 */
uint8_t read_data_input_from_input_handler() {
	// Initialize the return data to 0
	uint8_t key_pressed = 0;
	
	// initially low the clock and latch
	set_clock_low();
	set_latch_low();
	
	// toggle the latch so that the register detects the initial data bit to read
	set_latch_high();
	for (int d = 0; d < (DEFAULT_SYSTEM_CLOCK / 1000000); d++);
	set_latch_low();
	
	// overwrite the new data
	key_pressed += read_bit();
	
	// for the next 7 bits, we toggle the clock signal and follow the same logic as above.
	// In order to fit the bit into the right location, we should shift our bit by one before
	// reading the bit from the data line.
	for (int b = 0; b < 8; b++) {
		
		// shift the data by one for the future data overwrite
		key_pressed <<= 1;
		
		// high clock signal
		set_clock_high();
		
		// delay
		for (int d = 0; d < 1.5 * (DEFAULT_SYSTEM_CLOCK / 1000000); d++);
		
		// overwrite the new data
		key_pressed += read_bit();
		
		// delay
		for (int d = 0; d < 1.5 * (DEFAULT_SYSTEM_CLOCK / 1000000); d++);
		
		// low clock signal
		set_clock_low();
	}
	
	// reverse the output to fit into our NES convention declared in the main module
	key_pressed = ~key_pressed;
	
	// return the output
	return key_pressed;
}