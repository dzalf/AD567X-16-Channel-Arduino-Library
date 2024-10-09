# AD567X 16-channel Arduino Library
An [Arduino](http://arduino.cc) library to use the Analog Devices [AD567X](https://www.analog.com/media/en/technical-documentation/data-sheets/AD5674-5674R-5679-5679R.pdf) 16-channel Digital-to-Analog Converters with SPI.

## Supported models
- AD5674: 12-bit resolution with external voltage reference
- AD5674R: 12-bit resolution
- AD5679: 16-bit resolution with external voltage reference
- AD5679R: 16-bit resolution

### This library was only tested on the AD5674, use at your own risk on other models.

Copyright (c) 2024 Loris Mendolia

This software is distributed under an [MIT License](./LICENSE).

## Using the library
The simplest way to use this library is with [PlatformIO](https://platformio.org/), by adding `https://github.com/LorisMendolia/AD567X-16-Channel-Arduino-Library` to the `lib_deps` variable of your PlatformIO environment (in the `platformio.ini` file).

### Importing
To use in an Arduino sketch, include the library header.
```Arduino
#include <AD567X16.h>
```

### Initialization
The library defines a class for each AD567X 16-channel model. All classes share the same constructor syntax `AD567X16Class(CS_pin, LDAC_pin, RESET_pin[, Vref])`. The SCK, MOSI, and MISO pins of the Arduino must be used and connected to the device for SPI communication. The other pins (Chip Select, LDAC, and Reset pins) can be chosen freely and must be passed as arguments to the constructor. The library takes care of setting the correct pin settings and SPI settings.

For models with an external reference pin (non-R models), the value of the external reference can optionally be given to the constructor, enabling the library to handle the conversion of floating-point voltages when setting channel voltages.

For example, instantiating and initializing the device class for the AD5674 is as simple as
```Arduino
Dac = new AD5674Class(SS_DAC_PIN, LDAC_PIN, DAC_RESET_PIN, 1.8);
```
where `SS_DAC_PIN`, `LDAC_PIN`, and `DAC_RESET_PIN` are the pin numbers used on the Arduino and 1.8 corresponds to a 1.8 V external reference.

Note that when any library function is called, the SPI Bit Order is set to `MSBFIRST` and the SPI Data Mode is set to `SPI_MODE1`. These are not changed back at the end of the function call, so they must be set before using the SPI bus with another device.

### Setting a voltage
The AD567X DACs use a buffered input structure, with separate input and DAC registers. This library provides ways to write to both registers, to directly affect the DAC outputs or to buffer changes first. The `setChannel` function uses the optional argument `DAC_update`, by default set to `False` to determine whether the given value should be written to the DAC registers (when `True`) or to the input registers (when `False`). After writing to the input registers, the `updateDAC` function is used to copy the contents of the input registers to the DAC registers (by pulsing the LDAC pin).

When using the internal reference, or when the external reference is specified for supported models, the `setChannel` function can be given a floating-point voltage as argument and handles the conversion to the appropriate sequence of bits. A 16-bit word can also be used to directly set the register content independently of the reference. For the 12-bit DAC models, the 4 least significant bits are ignored.

For example, to set a 1.2 V voltage on the third channel by directly writing to the DAC registers, the function call would be
```Arduino
Dac->setChannel(2, (float)1.2, true);
```
To set the voltage of the first channel to half the reference by first writing to the input registers then updating the DAC registers, one would use
```Arduino
Dac->setChannel(0, (word)0x7FFF);
Dac->updateDAC();
```

## Library functions
### Constructors
- ```Arduino
  AD567X16Class(pin_size_t CS_pin, pin_size_t LDAC_pin, pin_size_t RESET_pin)
  ```
  Creates an object to control the DAC connected to the specified CS, LDAC, and RESET pins, and the SPI bus of the Arduino, initializes the DAC, and sets the correct pin and SPI parameters.

  Implemented classes are
  - AD5674RClass
  - AD5674Class
  - AD5679RClass
  - AD5679Class

- ```Arduino
  AD567X16Class(pin_size_t CS_pin, pin_size_t LDAC_pin, pin_size_t RESET_pin, float Vref)
  ```
  Identical to the previous constructor, but sets the DAC reference to external mode and stores the value of the reference used. Only for models that support external reference.

  Implemented classes are
  - AD5674Class
  - AD5679Class
 
### DAC operation functions
- ```Arduino
  void setChannel(uint8_t channel, word value, bool DAC_update=0, bool verbose=0);
  void setChannel(uint8_t channel, float value, bool DAC_update=0, bool verbose=0);
  ```
  Sets the value of the specified channel. `value` can be
  - a 16-bit `word` (unsigned 16-bit integer, corresponding to an `unsigned int`), in which case the register bits will directly be set according to that value, or
  - a `float` corresponding to the desired voltage, which will be converted into a `word` according to the value of the reference voltage (internal or external). This function will *fail* if the reference is set to external but the reference voltage has not been specified.
For the 12-bit models (AD5674 and AD5674R), the 4 least significant bits will be ignored.
 
The optional argument `DAC_update` indicates whether the DAC registers should be updated directly, or if the specified value should be put in the input registers (default).
The optional argument `verbose` will print error or warning messages (when the exact float value cannot be achieved with the DAC resolution) on the SPI bus. This function is disabled by default as it makes the SPI bus very busy, which is undesirable for fast operation.

- ```Arduino
  resetRegisters(unsigned long delay_ms=0);
  ```
- ```Arduino
  updateDAC(unsigned long delay_ms=0);
  ```
- ```Arduino
  updateChannels(uint8_t* channels, int num_channels);
  ```
- ```Arduino
  powerUpDown(uint8_t* channels, bool* power_up, int num_channels);
  powerUpDown(uint8_t channel, bool power_up);
  ```
- ```Arduino
  void setReference(bool internal);
	void setReference(float Vref);
  ```

## Unimplemented features
All the essential features to use the AD567X devices are implemented by this library. However, some advanced functionalities provided by these devices are not yet supported. These include
- [ ] Daisy-chaining
- [ ] Register content readback
- [ ] LDAC mask registers
- [ ] Sequential writing to all input/DAC registers
- [ ] Software reset (not using the Reset pin)
These functions are deemed not essential for an initial version of the library and may or may not be implemented in the future.
