#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>
#include <argp.h>

#include "config.h"
#include "illuminatir.h"

#define EEPROM_SIZE (512)

#define ARG_VERBOSE 'v'
#define ARG_OSCCAL  'c'
#define ARG_RED     'r'
#define ARG_GREEN   'g'
#define ARG_BLUE    'b'
#define ARG_UV      'u'
#define ARG_OFFSET  'o'
#define ARG_DEFAULT 'd'


static char doc[] = "Modifies IlluminatIR-Minion EEPROM config files.";
static char args_doc[] = "<OutputFile>";

static struct argp_option options[] = {
	{"verbose",  ARG_VERBOSE,  0,             0, "Verbose messages", 0 },
	{"oscCal",   ARG_OSCCAL,   "INT",         0, "Calibration value for the internal RC oscillator", 0 },
	{"red",      ARG_RED,      "INT",         0, "Default Red channel (0-255)", 0 },
	{"green",    ARG_GREEN,    "INT",         0, "Default Green channel (0-255)", 0 },
	{"blue",     ARG_BLUE,     "INT",         0, "Default Blue channel (0-255)", 0 },
	{"uv",       ARG_UV,       "INT",         0, "Default UV channel (0-255)", 0 },
	{"offset",   ARG_OFFSET,   "INT",         0, "Channel offset (0-255)", 0 },
	{"default",  ARG_DEFAULT,  "FILE",         0, "Read defaults from existing EEPROM image file", 0 },
	{ 0,         0,            0,             0, 0, 0}
};

struct arguments
{
	char * defaultFile;
	char * outputFile;
	unsigned verbosity;
	int oscCal;
	int red;
	int green;
	int blue;
	int uv;
	int offset;
};

static error_t parse_opt_uint8( int key, char * arg, struct argp_state * state, int * i )
{
	(void)key;
	
	char * end;
	long l = strtol( arg, &end, 0 );
	if( arg == end ) {
		fprintf( state->err_stream, "Argument not a valid integer!\n" );
		return ARGP_ERR_UNKNOWN;
	}
	if( l < 0 || l > 255 ) {
		fprintf( state->err_stream, "Argument out of range!\n" );
		return ARGP_ERR_UNKNOWN;
	}
	if( i ) {
		*i = (int)l;
	}
	return 0;
}

static error_t parse_opt( int key, char * arg, struct argp_state * state )
{
	struct arguments * arguments = (struct arguments *) state->input;

	switch( key ) {
		case ARG_VERBOSE: {
			arguments->verbosity++;
			break;
		}
		case ARG_OSCCAL: return parse_opt_uint8( key, arg, state, &arguments->oscCal );
		case ARG_RED:    return parse_opt_uint8( key, arg, state, &arguments->red );
		case ARG_GREEN:  return parse_opt_uint8( key, arg, state, &arguments->green );
		case ARG_BLUE:   return parse_opt_uint8( key, arg, state, &arguments->blue );
		case ARG_UV:     return parse_opt_uint8( key, arg, state, &arguments->uv );
		case ARG_OFFSET: return parse_opt_uint8( key, arg, state, &arguments->offset );
		case ARG_DEFAULT: {
			if( arguments->defaultFile ) {
				return ARGP_ERR_UNKNOWN;
			}
			arguments->defaultFile = arg;
			break;
		}
		case ARGP_KEY_ARG: {
			if( arguments->outputFile ) {
				return ARGP_ERR_UNKNOWN;
			}
			arguments->outputFile = arg;
			break;
		}
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}


static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };


int main( int argc, char ** argv )
{
	struct arguments arguments;

	// default values
	arguments.defaultFile = NULL;
	arguments.outputFile = NULL;
	arguments.verbosity = 2;
	arguments.oscCal     = -1;
	arguments.red       = -1;
	arguments.green     = -1;
	arguments.blue      = -1;
	arguments.uv        = -1;
	arguments.offset    = -1;

	argp_parse( &argp, argc, argv, 0, 0, &arguments );

	config_t config;

	if( arguments.defaultFile ) {
		if( arguments.verbosity > 0 ) {
			printf( "Loading defaults from \"%s\".\n", arguments.defaultFile );
		}
		FILE * file = fopen( arguments.defaultFile, "r" );
		if( !file ) {
			fprintf( stderr, "Could not open \"%s\": %s\n", arguments.defaultFile, strerror(errno) );
			return 1;
		}
		
		fseek( file , 0 , SEEK_END);
		long file_size = ftell( file );
		if( file_size < 0 ) {
			fprintf( stderr, "Could not get size of \"%s\": %s\n", arguments.defaultFile, strerror(errno) );
			fclose( file );
			return 1;
		}
		rewind( file );
		
		if( (size_t)file_size < sizeof(config_t) ) {
			fprintf( stderr, "Expected at least %zu bytes of EEPROM data, but only got %zu!\n", sizeof(config_t), file_size );
			fclose( file );
			return 1;
		}
		
		size_t bytes_read = fread( (void*)&config, 1, sizeof(config), file );
		if( bytes_read != sizeof(config) ) {
			fprintf( stderr, "Failed to read %zu bytes of EEPROM data!\n", sizeof(config) );
			fclose( file );
			return 1;
		}
		fclose( file );
		
		uint8_t crc = illuminatir_crc8( (const void*)&config, sizeof(config)-1, ILLUMINATIR_CRC8_INITIAL_SEED );
		if( crc != config.crc ) {
			fprintf( stderr, "Warning: CRC check of provided default EEPROM data failed: Expected 0x%"PRIx8" but got 0x%"PRIx8".\n", crc, config.crc );
		}
	} else {
		config = (config_t){
			.oscCal = 120,
			.red    = 64,
			.green  = 64,
			.blue   = 64,
			.uv     = 0,
			.channelOffset = 0,
		};
	}

	if( arguments.oscCal >= 0 ) {
		config.oscCal = arguments.oscCal;
	}
	if( arguments.red >= 0 ) {
		config.red = arguments.red;
	}
	if( arguments.green >= 0 ) {
		config.green = arguments.green;
	}
	if( arguments.blue >= 0 ) {
		config.blue = arguments.blue;
	}
	if( arguments.uv >= 0 ) {
		config.uv = arguments.uv;
	}
	if( arguments.offset >= 0 ) {
		config.channelOffset = arguments.offset;
	}
	
	config.crc = illuminatir_crc8( (const void*)&config, sizeof(config)-1, ILLUMINATIR_CRC8_INITIAL_SEED );

	printf("Configuration: 0x");
	for( size_t i = 0; i < sizeof(config); i++ ) {
		printf("%02"PRIx8" ", ((uint8_t*)&config)[i] );
	}
	printf("\n");

	if( arguments.outputFile ) {
		if( arguments.verbosity > 0 ) {
			printf( "Writing configuration to \"%s\".\n", arguments.outputFile );
		}
		FILE * file = fopen( arguments.outputFile, "wx" );
		if( !file ) {
			fprintf( stderr, "Could not open \"%s\": %s\n", arguments.outputFile, strerror(errno) );
			return 1;
		}
		size_t bytes_written = fwrite( (void*)&config, 1, sizeof(config), file );
		if( bytes_written != sizeof(config) ) {
			fprintf( stderr, "Failed to write %zu bytes of EEPROM data!\n", sizeof(config) );
			fclose( file );
			return 1;
		}
		fclose( file );
	}

	return 0;
}
