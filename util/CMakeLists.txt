cmake_minimum_required(VERSION 3.12)

project(illuminatir_minion_utils C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_compile_options(
	-Wall
	-Wextra
)

add_executable(eeprom_config
	${CMAKE_SOURCE_DIR}/eeprom_config.c
	${CMAKE_SOURCE_DIR}/../ext/libilluminatir/src/crc8.c
)

target_include_directories(eeprom_config
	PRIVATE
		../src/
		../ext/libilluminatir/include
)
