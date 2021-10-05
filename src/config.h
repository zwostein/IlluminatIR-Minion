#ifndef CONFIG_INCLUDED
#define CONFIG_INCLUDED

#include <stdint.h>

//#define CONFIG_CHANNELSWITCHES_ENABLE
#define CHANNELSWITCHES_INVERT

#define PWMLED_INVERT
#define PWMLED_OCR_RED   OCR1D
#define PWMLED_OCR_GREEN OCR1B
#define PWMLED_OCR_BLUE  OCR1A

#define SOFTPWMLED_STRIDE (1U)
#define SOFTPWMLED_PORT   PORTB
#define SOFTPWMLED_DDR    DDRB
#define SOFTPWMLED_P      PB0
#define SOFTPWMLED_INVERT

#define USISERIAL_BAUDRATE (4800UL)


typedef struct __attribute__((packed)) {
	uint8_t oscCal;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t uv;
#ifndef CHANNELSWITCHES_ENABLE
	uint8_t channelOffset;
#endif
	uint8_t crc;
} config_t;


#endif
