cmake_minimum_required(VERSION 3.12)

set(AVR_MCU attiny861)
set(AVR_H_FUSE 0xdf)
set(AVR_L_FUSE 0xe2)

set(AVR_UPLOADTOOL avrdude)
set(AVR_PROGRAMMER avrispmkII)
set(AVR_UPLOADTOOL_PORT usb)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/avr-gcc.toolchain.cmake")


project(illuminatir_minion C ASM)
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_C_STANDARD 11)
add_definitions(
	-DF_CPU=8000000UL
	-Os
	-Wall
	-Wextra
)

include_directories(
	${PROJECT_SOURCE_DIR}/ext/libilluminatir/include/
)

add_avr_executable(${PROJECT_NAME}
	${PROJECT_SOURCE_DIR}/src/main.c
	${PROJECT_SOURCE_DIR}/src/usiserial.c
	${PROJECT_SOURCE_DIR}/src/gammaLut.c
	${PROJECT_SOURCE_DIR}/ext/libilluminatir/src/illuminatir.c
	${PROJECT_SOURCE_DIR}/ext/libilluminatir/src/crc8.c
	${PROJECT_SOURCE_DIR}/ext/libilluminatir/src/cobs.c
)

avr_generate_fixed_targets()

option(UTILS "Build utilities" OFF)
if(UTILS)
	include(ExternalProject)
	ExternalProject_Add(illuminatir_minion_utils
		PREFIX util
		SOURCE_DIR ${CMAKE_SOURCE_DIR}/util
		BINARY_DIR ${CMAKE_BINARY_DIR}/util
		BUILD_ALWAYS 1
		INSTALL_COMMAND ""
	)
endif()
