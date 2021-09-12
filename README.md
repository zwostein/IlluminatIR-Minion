# IlluminatIR-Minion

IlluminatIR synchronizes lighting devices (and possibly other gadgets) via an infrared serial protocol.
The infrared transmitter is connected to a host computer via USB and acts as a HID device.

This is the firmware for ATtiny861 microcontrollers implementing a simple receiver handling 4 PWM channels.


## How to build

### Obtaining sources:

	git clone --recurse-submodules https://github.com/zwostein/IlluminatIR-Minion.git

### Compilation:

	cd IlluminatIR-Minion
	mkdir build && cd build
	cmake ..
	make

### Then to flash the software onto the microcontroller:

	make upload_illuminatir_minion
