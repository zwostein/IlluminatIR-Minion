cmake_minimum_required(VERSION 3.0)

#set(AVR_MCU atmega32u4)
#set(AVR_H_FUSE 0xd8)
#set(AVR_L_FUSE 0xff)

#set(AVR_UPLOADTOOL avrdude)
#set(AVR_PROGRAMMER avr109)
#set(AVR_UPLOADTOOL_PORT /dev/ttyACM1)

#set(AVR_PROGRAMMER ft232rl)
#set(AVR_UPLOADTOOL_PORT ft0)
#set(AVR_UPLOADTOOL_BAUDRATE 2000)
#set(AVR_UPLOADTOOL_OPTIONS -C +/data/Projects/avr/ft232rl.avrdude.conf)

set(AVR_MCU attiny861)
set(AVR_H_FUSE 0xdf)
set(AVR_L_FUSE 0xe2)

set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER avrispmkII)
set(AVR_UPLOADTOOL_PORT usb)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/avr-gcc.toolchain.cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

project(illuminatir_minion C CXX ASM)

add_definitions(-DF_CPU=8000000UL -Os -Wall -Wextra)
include_directories(ext/cobs-c/)
add_avr_executable(${PROJECT_NAME}
	src/main.c
	src/usiserial.c
	src/gammaLut.c
	src/illuminatir.c
	src/crc8.c
	ext/cobs-c/cobs.c
)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 11)

avr_generate_fixed_targets()
