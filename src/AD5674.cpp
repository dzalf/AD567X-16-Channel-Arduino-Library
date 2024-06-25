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

#include <Arduino.h>
#include <SPI.h>
#include <AD5674.h>

AD5674Class::AD5674Class(int SS_pin, int LDAC_pin, int RESET_pin){
	_SS_pin = SS_pin;
	_LDAC_pin = LDAC_pin;
	_RESET_pin = RESET_pin;
	
	pinMode(_SS_pin, OUTPUT);
	pinMode(_LDAC_pin, OUTPUT);
	pinMode(_RESET_pin, OUTPUT);
	
	digitalWrite(_SS_pin, HIGH);
	digitalWrite(_LDAC_pin, HIGH);
	digitalWrite(_RESET_pin, HIGH);

	resetRegisters();
}

void AD5674Class::setChannel(int channel, word value, bool DAC_update){

	// Check if the channel is within the valid range
	if(channel < 0 || channel > 15){
		Serial.println("Error: Channel out of range");
		return;
	}
	
	byte command;
	// Select the desired update mode
	if(DAC_update){
		command = AD5674_CMD_WRITE_DAC_REG;
	}
	else{
		command = AD5674_CMD_WRITE_INPUT_REG;
	}

	// If the value is greater than 12 bits, warn the user about data loss
	if(!(value & 0xF000)){
		Serial.println("Warning: Data loss, value is greater than 12 bits");
	}

	// Extract the 4-bit address from the channel
	byte address = static_cast<byte>(channel) & 0x0F;

	// Send the 12-bit data to the DAC
	writeData(command, address, value<<4);
}

void AD5674Class::updateChannels(int* channels, int num_channels){

	word data = 0;
	for(int i = 0; i < num_channels; i++){
		// Check if the channel is within the valid range
		if(channels[i] < 0 || channels[i] > 15){
			Serial.println("Error: Channel out of range");
			return;
		}
		// Set the corresponding bit in the data word
		data |= (1 << channels[i]);
	}

	// Send the data to the DAC
	writeData(AD5674_CMD_UPDATE_DAC_REG, 0x00, data);
}

void AD5674Class::powerUpDown(int channel, bool power_up){
	// Check if the channel is within the valid range
	if(channel < 0 || channel > 15){
		Serial.println("Error: Channel out of range");
		return;
	}

	// Call the overloaded function
	int channels[1] = {channel};
	bool power_ups[1] = {power_up};
	powerUpDown(channels, power_ups, 1);
}

void AD5674Class::powerUpDown(int* channels, bool* power_up, int num_channels){
	// Create flags to check if the status registers have been updated
	bool update_0 = false;
	bool update_1 = false;

	for(int i = 0; i < num_channels; i++){
		int channel = channels[i];
		// Check if the channel is within the valid range
		if(channel < 0 || channel > 15){
			Serial.println("Error: Channel out of range");
			return;
		}
		// Check if the channel is in the first or second batch of 8 channels
		if(channel > 7){
			update_1 = true;
			word power_status = _DAC_status_1;
			channel %= 8; // Get the channel number within the second batch
			// Set or clear the corresponding bit in the status register
			if(!power_up[i]){
				power_status |= (1 << 2*channels[i]);
			}
			else{
				power_status &= ~(1 << 2*channels[i]);
			}
			_DAC_status_1 = power_status; // Update the status register
		}
		else{
			update_0 = true;
			word power_status = _DAC_status_0;
			// Set or clear the corresponding bit in the status register
			if(!power_up[i]){
				power_status |= (1 << 2*channels[i]);
			}
			else{
				power_status &= ~(1 << 2*channels[i]);
			}
			_DAC_status_0 = power_status; // Update the status register
		}
		
	}

	// If the status registers have been updated, send the new data to the DAC
	if(update_0){
		writeData(AD5674_CMD_POWER_UPDOWN, AD5674_POWER_BATCH_0, _DAC_status_0);
	}
	if(update_1){
		writeData(AD5674_CMD_POWER_UPDOWN, AD5674_POWER_BATCH_1, _DAC_status_1);
	}
}

void AD5674Class::resetRegisters(){
	// Pulse the RESET pin
	digitalWrite(_RESET_pin, LOW);
	delay(1);
	digitalWrite(_RESET_pin, HIGH);
}

void AD5674Class::updateDAC(){
	// Pulse the LDAC pin
	digitalWrite(_LDAC_pin, LOW);
	delay(1);
	digitalWrite(_LDAC_pin, HIGH);
}

void AD5674Class::setReference(bool internal){
	// Set the reference voltage to internal or external
	if(internal){
		writeData(AD5674_CMD_REF_SETUP, 0x00, AD5674_REF_INTERNAL_MESSAGE);
	}
	else{
		writeData(AD5674_CMD_REF_SETUP, 0x00, AD5674_REF_EXTERNAL_MESSAGE);
	}
}

void AD5674Class::writeData(byte command, byte address, word data){

	// SPI settings
	SPI.setDataMode(SPI_MODE1);
	SPI.setBitOrder(MSBFIRST);

	// Start SPI by selecting the slave
	digitalWrite(_SS_pin, LOW);

	// Send the command and address (4 bits each)
	SPI.transfer((command << 4) | address);

	// Send the data (16 bits)
	SPI.transfer(highByte(data));
	SPI.transfer(lowByte(data));

	// End SPI by deselecting the slave
	digitalWrite(_SS_pin, HIGH);
}

/*
To-do:
- Add support for daisy-chaining
- Add support for readback
- Add support for LDAC mask register
- Add support for writing to all input registers
- Add support for writing to all DAC registers
- Add support for software reset
*/