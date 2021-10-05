#ifndef USISERIAL_INCLUDED
#define USISERIAL_INCLUDED

#include <stdint.h>

#include <avr/io.h>

#include "config.h"

#ifndef USISERIAL_BAUDRATE
#	define USISERIAL_BAUDRATE 9600UL
#endif

#ifndef USISERIAL_PORTA
#	define USISERIAL_PORTA
#endif

#if defined(USISERIAL_PORTA) && !defined(USISERIAL_PORTB)
#	define USISERIAL_PORT     PORTA
#	define USISERIAL_PIN      PINA
#	define USISERIAL_DDR      DDRA
#	define USISERIAL_DO_P     PA1
#	define USISERIAL_DI_P     PA0
#	define USISERIAL_PCINT    PCINT0
#	define USISERIAL_PCMSK    PCMSK0
#	define USISERIAL_PCIE     PCIE1
#	define USISERIAL_PCIF     PCIF
#elif defined(USISERIAL_PORTB) && !defined(USISERIAL_PORTA)
#	define USISERIAL_PORT     PORTB
#	define USISERIAL_PIN      PINB
#	define USISERIAL_DDR      DDRB
#	define USISERIAL_DO_P     PB1
#	define USISERIAL_DI_P     PB0
#	define USISERIAL_PCINT    PCINT8
#	define USISERIAL_PCMSK    PCMSK1
#	define USISERIAL_PCIE     PCIE1
#	define USISERIAL_PCIF     PCIF
#else
#	error "Define either USISERIAL_PORTA or USISERIAL_PORTB"
#endif

#ifndef USISERIAL_RX_BUFFER_SIZE
#	define USISERIAL_RX_BUFFER_SIZE 16
#endif
#ifndef USISERIAL_TX_BUFFER_SIZE
#	define USISERIAL_TX_BUFFER_SIZE 8
#endif


void usiserial_clearBuffers( void );
void usiserial_init( void );
void usiserial_putByte( uint8_t data );
uint8_t usiserial_getByte( void );
uint8_t usiserial_rxAvailable( void );
uint8_t usiserial_txAvailable( void );

#endif
