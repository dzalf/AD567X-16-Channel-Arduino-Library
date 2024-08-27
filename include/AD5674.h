/*
Copyright (c) 2024 Loris Mendolia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef AD5674_h
#define AD5674_h

#include <Arduino.h>
#include <SPI.h>

#define AD5674_CMD_WRITE_INPUT_REG B0001 // Write to Input Register n
#define AD5674_CMD_UPDATE_DAC_REG B0010 // Update DAC Register n with Input Register n
#define AD5674_CMD_WRITE_DAC_REG B0011 // Write to DAC Register n
#define AD5674_CMD_POWER_UPDOWN B0100 // Power up/down DAC Register n
#define AD5674_CMD_LDAC_MASK_REG B0101 // Set LDAC Mask Register
#define AD5674_CMD_RESET B0110 // Software Reset
#define AD5674_CMD_REF_SETUP B0111 // Reference Setup
#define AD5674_CMD_DAISY_CHAIN B1000 // Daisy-Chain Setup
#define AD5674_CMD_READBACK B1001 // Set Readback DAC Register
#define AD5674_CMD_WRITE_ALL_INPUT B1010 // Write to All Input Registers
#define AD5674_CMD_WRITE_ALL_DAC B1011 // Write to All Input and DAC Registers

#define AD5674_POWER_BATCH_0 B0000 // First 8 DAC channels for power operation
#define AD5674_POWER_BATCH_1 B1000 // Last 8 DAC channels for power operation

#define AD5674_REF_INTERNAL_MESSAGE 0x0000
#define AD5674_REF_EXTERNAL_MESSAGE 0x0001

class AD5674Class{
	
	public:
	AD5674Class(pin_size_t SS_pin, pin_size_t LDAC_pin, pin_size_t RESET_pin);
	AD5674Class(pin_size_t SS_pin, pin_size_t LDAC_pin, pin_size_t RESET_pin, float Vref);
	void setChannel(uint8_t channel, word value, bool DAC_update=0, bool verbose=0);
	void setChannel(uint8_t channel, float value, bool DAC_update=0, bool verbose=0);
	void resetRegisters(unsigned long delay_ms=0);
	void updateDAC(unsigned long delay_ms=0);
	void updateChannels(uint8_t* channels, int num_channels);
	void powerUpDown(uint8_t* channels, bool* power_up, int num_channels);
	void powerUpDown(uint8_t channel, bool power_up);
	void setReference(bool internal);
	void setReference(float Vref);

	private:
		pin_size_t _SS_pin;
		pin_size_t _LDAC_pin;
		pin_size_t _RESET_pin;
		float _Vref = 2.5;

		word _DAC_status_0 = 0x0000;
		word _DAC_status_1 = 0x0000;

		void writeData(byte command, byte address, word data);
};

#endif

/*
To-do:
- Add support for daisy-chaining
- Add support for readback
- Add support for LDAC mask register
- Add support for writing to all input registers
- Add support for writing to all DAC registers
- Add support for software reset
*/