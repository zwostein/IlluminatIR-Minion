# IlluminatIR-Minion

IlluminatIR synchronizes lighting devices (and possibly other gadgets) in real-time via a unidirectional infrared serial protocol.

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
