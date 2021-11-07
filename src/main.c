/*
 * IlluminatIR
 * Copyright (C) 2021  zwostein
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>

#include <util/delay.h>

#include <illuminatir.h>

#include "usiserial.h"
#include "gammaLut.h"


////////////////////////////////////////////////////////////////
////////////////////////////  USI   ////////////////////////////
int wrap_putchar( char c, FILE * stream )
{
	(void)stream;
#if 0 // output on serial interferes with receiver - enable only for debugging sessions
	if( c == '\n')
		usiserial_putByte('\r');
	usiserial_putByte( c );
#else
	(void)c;
#endif
	return 0;
}
int wrap_getchar( FILE * stream )
{
	(void)stream;
	return usiserial_getByte();
}
static FILE usi = FDEV_SETUP_STREAM( wrap_putchar, wrap_getchar, _FDEV_SETUP_RW );
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
/////////////////////////// pwmLed  ////////////////////////////
#define PWMLED_INVERT
#define PWMLED_OCR_RED   OCR1D
#define PWMLED_OCR_GREEN OCR1B
#define PWMLED_OCR_BLUE  OCR1A
void pwmLed_init( void )
{
	TC1H = 0;
	OCR1A = 0;
	OCR1B = 0;
	OCR1D = 0;

	TIMSK = 0;

	// 10 Bit Timer 1: Generate PWM signal on OC1A/OC1B/OC1D
	// PWM1x = 3
	// WGM1x (Waveform Generation Mode) = 0 (Fast PWM)
	// COM1xx (Compare Output Mode) = 2 (Clear OC1A/OC1B on compare match, set OC1A/OC1B at TOP)
	// CS1x (Clock Select) = 1 (clk)
	TCCR1A = 0
	       | _BV(COM1A1)
//	       | _BV(COM1A0)
	       | _BV(COM1B1)
//	       | _BV(COM1B0)
		   | _BV(PWM1A)
		   | _BV(PWM1B)
		   ;
	TCCR1B = 0
#ifdef PWMLED_INVERT
	       | _BV(PWM1X)
#endif
//	       | _BV(PSR1)
//	       | _BV(CS13)
//	       | _BV(CS12)
//	       | _BV(CS11)
	       | _BV(CS10)
	       ;
	TCCR1C = 0
	       | _BV(COM1A1S)
//	       | _BV(COM1A0S)
	       | _BV(COM1B1S)
//	       | _BV(COM1B0S)
	       | _BV(COM1D1)
//	       | _BV(COM1D0)
	       | _BV(PWM1D)
	       ;
	TCCR1D = 0
//	       | _BV(WGM11)
//	       | _BV(WGM10)
	       ;
	TCCR1E = 0;

	TC1H = 0x3;
	OCR1C = 0xff;

	DDRB |= _BV(PB5) | _BV(PB3) | _BV(PB1);
}

static inline void pwmLed_setRed( uint8_t value )
{
	uint16_t v = pgm_read_word(&gammaLut_2p4_10[value]);
	TC1H = v >> 8;
	PWMLED_OCR_RED = v;
}

static inline void pwmLed_setGreen( uint8_t value )
{
	uint16_t v = pgm_read_word(&gammaLut_2p4_10[value]);
	TC1H = v >> 8;
	PWMLED_OCR_GREEN = v;
}

static inline void pwmLed_setBlue( uint8_t value )
{
	uint16_t v = pgm_read_word(&gammaLut_2p4_10[value]);
	TC1H = v >> 8;
	PWMLED_OCR_BLUE = v;
}
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
////////////////////////// softPwmLed //////////////////////////
#define SOFTPWMLED_STRIDE (1U)
#define SOFTPWMLED_PORT   PORTB
#define SOFTPWMLED_DDR    DDRB
#define SOFTPWMLED_P      PB0
#define SOFTPWMLED_INVERT
static volatile uint8_t softPwmLed_value = 0;
static uint8_t softPwmLed_counter = 0;

void softPwmLed_init( void )
{
	SOFTPWMLED_PORT &= ~_BV(SOFTPWMLED_P);
	SOFTPWMLED_DDR |= _BV(SOFTPWMLED_P);
}

static inline void softPwmLed_update( void )
{
	if( softPwmLed_counter >= softPwmLed_value ) {
#ifdef SOFTPWMLED_INVERT
		SOFTPWMLED_PORT |= _BV(SOFTPWMLED_P);
#else
		SOFTPWMLED_PORT &= ~_BV(SOFTPWMLED_P);
#endif
	} else {
#ifdef SOFTPWMLED_INVERT
		SOFTPWMLED_PORT &= ~_BV(SOFTPWMLED_P);
#else
		SOFTPWMLED_PORT |= _BV(SOFTPWMLED_P);
#endif
	}
	softPwmLed_counter += SOFTPWMLED_STRIDE;
}

static inline void softPwmLed_set( uint8_t value )
{
	softPwmLed_value = pgm_read_byte(&gammaLut_2p4_8[value]);
}
////////////////////////////////////////////////////////////////


static volatile uint8_t red;
static volatile uint8_t green;
static volatile uint8_t blue;
static volatile uint8_t uv;
static volatile uint8_t channelOffset = 0;

config_t EEMEM eepromConfig;

static bool config_get( config_t * config, const config_t * defaultConfig )
{
	eeprom_read_block( config, (const void*)&eepromConfig, sizeof(config_t) );
	uint8_t crc = illuminatir_crc8( (const void*)config, sizeof(config_t)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	if( crc == config->crc ) {
		return true;
	}
	if( defaultConfig != NULL ) {
		*config = *defaultConfig;
		return false;
	}
	memset( config, 0, sizeof(config_t) );
	config->oscCal = OSCCAL;
	return false;
}

static void config_update( config_t * config )
{
	config->crc = illuminatir_crc8( (const void*)config, sizeof(config_t)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
	eeprom_update_block( (const void*)config, &eepromConfig, sizeof(config_t) );
}


static void animation_beginSaving()
{
	pwmLed_setRed  ( 0 );
	pwmLed_setGreen( 0 );
	pwmLed_setBlue ( 0 );
	softPwmLed_set ( 0 );
	_delay_ms( 1 );
	for( int i=1; i<256; i++ ) {
		pwmLed_setRed( i );
		_delay_ms( 1 );
	}
	for( int i=255; i>=0; i-- ) {
		pwmLed_setRed( i );
		_delay_ms( 1 );
	}
	for( int i=1; i<256; i++ ) {
		pwmLed_setRed( i );
		_delay_ms( 1 );
	}
}

static void animation_endSaving()
{
	_delay_ms( 1 );
	for( int i=255; i>=0; i-- ) {
		pwmLed_setRed( i );
		_delay_ms( 1 );
	}
	for( int i=1; i<256; i++ ) {
		pwmLed_setRed( i );
		_delay_ms( 1 );
	}
	for( int i=255; i>=0; i-- ) {
		pwmLed_setRed( i );
		_delay_ms( 1 );
	}
	pwmLed_setRed  ( red   );
	pwmLed_setGreen( green );
	pwmLed_setBlue ( blue  );
	softPwmLed_set ( uv    );
}

static void config_makeDefault()
{
	config_t config;
	config_get( &config, NULL );

	config.red   = red;
	config.green = green;
	config.blue  = blue;
	config.uv    = uv;
#ifndef CHANNELSWITCHES_ENABLE
	config.channelOffset = channelOffset;
#endif

	animation_beginSaving();
	fprintf_P( &usi, PSTR("Saving config to EEPROM...") );
	config_update( &config );
	fprintf_P( &usi, PSTR("done!\n") );
	animation_endSaving();
}

#ifndef CHANNELSWITCHES_ENABLE
static void config_setBase( uint8_t offset )
{
	channelOffset = offset;
	fprintf_P( &usi, PSTR("Set base offset to %u!\n"), (unsigned)channelOffset );
}
#endif

static void setChannel( uint8_t channel, uint8_t value )
{
	channel -= channelOffset;
	switch( channel ) {
		case 0: red   = value; pwmLed_setRed( value );   break;
		case 1: green = value; pwmLed_setGreen( value ); break;
		case 2: blue  = value; pwmLed_setBlue( value );  break;
		case 3: uv    = value; softPwmLed_set( value );  break;
	}
}

static void setConfig( const char * key, uint8_t key_size, const uint8_t * values, uint8_t values_size )
{
	if( strncasecmp("MakeDefault", key, key_size) == 0 ) {
		if( values_size > 0 ) {
			fprintf_P( &usi, PSTR("Expected no values for \"MakeDefault\"!\n") );
			return;
		}
		config_makeDefault();
		return;
#ifndef CHANNELSWITCHES_ENABLE
	} else if( strncasecmp("Base", key, key_size) == 0 ) {
		if( values_size != 1 ) {
			fprintf_P( &usi, PSTR("Expected 1 value for \"Base\"!\n") );
			return;
		}
		config_setBase( values[0] );
		return;
#endif
	} else {
		fprintf_P( &usi, PSTR("Unknown configuration key!\n") );
		return;
	}
}


////////////////////////////////////////////////////////////////
////////////////////////////  Main  ////////////////////////////

ISR( ADC_vect )
{
	softPwmLed_update();
}

int main(void)
{
	////////////////////////////////////////////////////////////////
	//////////////////////// Initialisation ////////////////////////

	// disable pin change interrupt by default
	PCMSK0 = 0;
	PCMSK1 = 0;

	usiserial_init();

	pwmLed_init();
	softPwmLed_init();

	// Abuse ADC interrupt in free running mode for software PWM
	ADMUX = 0
	      | 9 // Single-Ended Input ADC9
	      ;
	ADCSRA = 0
	       | _BV(ADEN)
	       | _BV(ADSC)
	       | _BV(ADATE)
	       | _BV(ADIE)
//	       | _BV(ADPS2)
	       | _BV(ADPS1)
	       | _BV(ADPS0)
	       ;
	ADCSRB = 0 // Free running mode
	       ;

	// Use PA3 to PA7 as channel offset input switches
	DDRA  &= ~( _BV(PA3)|_BV(PA4)|_BV(PA5)|_BV(PA6)|_BV(PA7) ); // as input
	PORTA |=    _BV(PA3)|_BV(PA4)|_BV(PA5)|_BV(PA6)|_BV(PA7);   // enable pullup

	// enable interrupts
	sei();
	////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////
	////////////////////////   Main loop    ////////////////////////
	fprintf_P( &usi, PSTR("Illuminatir-Minion\n") );

	config_t config;
	bool eeprom_config_ok = config_get( &config, NULL );
	if( !eeprom_config_ok ) {
		fprintf_P( &usi, PSTR("EEPROM config CRC failed - using defaults!\n") );
	} else {
		OSCCAL = config.oscCal;
		fprintf_P( &usi, PSTR("EEPROM config loaded.\n") );
	}
	channelOffset = config.channelOffset;
	red   = config.red;
	green = config.green;
	blue  = config.blue;
	uv    = config.uv;
	pwmLed_setRed  ( red   );
	pwmLed_setGreen( green );
	pwmLed_setBlue ( blue  );
	softPwmLed_set ( uv    );

	while( true ) {
		static uint8_t cobsPacket[ILLUMINATIR_COBS_PACKET_MAXSIZE];
		static uint8_t cobsPacket_pos = 0;

#ifdef CHANNELSWITCHES_ENABLE
		channelOffset = 0
#	ifdef CHANNELSWITCHES_INVERT
		              | ((!(PINA & _BV(PA3)))?4:0)
		              | ((!(PINA & _BV(PA4)))?8:0)
		              | ((!(PINA & _BV(PA5)))?16:0)
		              | ((!(PINA & _BV(PA6)))?32:0)
		              | ((!(PINA & _BV(PA7)))?64:0)
#	else
		              | ((PINA & _BV(PA3))?4:0)
		              | ((PINA & _BV(PA4))?8:0)
		              | ((PINA & _BV(PA5))?16:0)
		              | ((PINA & _BV(PA6))?32:0)
		              | ((PINA & _BV(PA7))?64:0)
#	endif
		              ;
#endif

		uint8_t rec = usiserial_getByte();
		if( rec == 0 ) {
			illuminatir_error_t err = illuminatir_rand_cobs_parse( cobsPacket, cobsPacket_pos, setChannel, setConfig );
			if( err != ILLUMINATIR_ERROR_NONE ) {
				fprintf_P( &usi, PSTR("Could not parse packet: %S\n"), illuminatir_error_toString_P(err) );
			}

			cobsPacket_pos = 0;
		} else {
			if( cobsPacket_pos < sizeof(cobsPacket) ) {
				cobsPacket[cobsPacket_pos] = rec;
				cobsPacket_pos++;
			}
		}
	}
	////////////////////////////////////////////////////////////////

	return 0;
}
////////////////////////////////////////////////////////////////
