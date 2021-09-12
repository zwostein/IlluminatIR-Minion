#include "usiserial.h"

#include <stdbool.h>

#include <avr/interrupt.h>

#include <util/atomic.h>

#define USISERIAL_DATA_BITS                8
#define USISERIAL_START_BIT                1
#define USISERIAL_STOP_BIT                 1
#define USISERIAL_HALF_FRAME               5

#define USISERIAL_TIMER0_PRESCALER         1
#define USISERIAL_TIMER0_CYCLES_PER_BIT    (F_CPU / (USISERIAL_BAUDRATE * USISERIAL_TIMER0_PRESCALER ))
#define USISERIAL_TIMER0_SEED              (0xFFFFUL - USISERIAL_TIMER0_CYCLES_PER_BIT)

#define USISERIAL_INTERRUPT_STARTUP_DELAY  (0x11UL / USISERIAL_TIMER0_PRESCALER)

#if ((USISERIAL_TIMER0_CYCLES_PER_BIT*3)/2) > (0xFFFFUL - USISERIAL_INTERRUPT_STARTUP_DELAY)
    #define USISERIAL_INITIAL_TIMER0_SEED  (0xFFFFUL - (USISERIAL_TIMER0_CYCLES_PER_BIT/2))
    #define USISERIAL_COUNTER_SEED_RECEIVE (16UL - (USISERIAL_START_BIT + USISERIAL_DATA_BITS) )
#else
    #define USISERIAL_INITIAL_TIMER0_SEED  (0xFFFFUL - ((USISERIAL_TIMER0_CYCLES_PER_BIT*3)/2) )
    #define USISERIAL_COUNTER_SEED_RECEIVE (16UL - USISERIAL_DATA_BITS)
#endif

#define USISERIAL_COUNTER_SEED_TRANSMIT    (16UL - USISERIAL_HALF_FRAME)


#define USISERIAL_RX_BUFFER_MASK ( USISERIAL_RX_BUFFER_SIZE - 1 )
#if ( USISERIAL_RX_BUFFER_SIZE & USISERIAL_RX_BUFFER_MASK )
#	error RX buffer size is not a power of 2
#endif

#define USISERIAL_TX_BUFFER_MASK ( USISERIAL_TX_BUFFER_SIZE - 1 )
#if ( USISERIAL_TX_BUFFER_SIZE & USISERIAL_TX_BUFFER_MASK )
#	error TX buffer size is not a power of 2
#endif


static uint8_t          rxBuf[USISERIAL_RX_BUFFER_SIZE] = { 0 };
static volatile uint8_t rxHead = 0;
static volatile uint8_t rxTail = 0;
static uint8_t          txBuf[USISERIAL_TX_BUFFER_SIZE] = { 0 };
static volatile uint8_t txHead = 0;
static volatile uint8_t txTail = 0;
static uint8_t          txData = 0;

static volatile union status
{
	uint8_t status;
	struct
	{
		bool ongoing_Transmission_From_Buffer:1;
		bool ongoing_Transmission_Of_Package:1;
		bool ongoing_Reception_Of_Package:1;
		bool reception_Buffer_Overflow:1;
		bool flag4:1;
		bool flag5:1;
		bool flag6:1;
		bool flag7:1;
	};
} status = { 0 };


void usiserial_clearBuffers( void )
{
	rxTail = 0;
	rxHead = 0;
	txTail = 0;
	txHead = 0;
}


static inline uint8_t reverse_byte( uint8_t x )
{
	x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
	x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
	x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
	return x;
}


static inline void usiserial_startTimer0( uint16_t tcnt0 )
{
	TCNT0H = tcnt0 >> 8;
	TCNT0L = tcnt0 & 0xff;
	TCCR0B = 0
	       | _BV(PSR0)
#if USISERIAL_TIMER0_PRESCALER == 1
//	       | _BV(CS02)
//	       | _BV(CS01)
	       | _BV(CS00)
#elif USISERIAL_TIMER0_PRESCALER == 8
//	       | _BV(CS02)
	       | _BV(CS01)
//	       | _BV(CS00)
#elif USISERIAL_TIMER0_PRESCALER == 64
//	       | _BV(CS02)
	       | _BV(CS01)
	       | _BV(CS00)
#else
#	error "Unsupported Timer0 prescaler"
#endif
	       ;
	TIMSK |= _BV(TOIE0); // Enable Timer0 OVF interrupt.
}

static inline void usiserial_stopTimer0( void )
{
	TCCR0B = 0;
}


static inline void usiserial_setUsiCount( uint8_t count )
{
	USISR  = 0xF0 // Clear all USI interrupt flags.
	       | (count & 0x0F)
	       ;
}

static inline void usiserial_startUsi( uint8_t count )
{
	usiserial_setUsiCount( count );
	USICR  = 0
//	       | _BV(USISIE)
	       | _BV(USIOIE) // Enable USI Counter OVF interrupt.
//	       | _BV(USIWM1)
	       | _BV(USIWM0) // Select Three Wire mode.
//	       | _BV(USICS1)
	       | _BV(USICS0) // Select Timer0 OVER as USI Clock source.
//	       | _BV(USICLK)
//	       | _BV(USITC)
	       ;
}

static inline void usiserial_startUsiRx( uint8_t count )
{
	usiserial_startUsi( count );
}

static inline uint8_t usiserial_getUsiRx( void )
{
	return USIDR;
}

static inline void usiserial_startUsiTx( uint8_t data, uint8_t count )
{
	usiserial_startUsiRx( count );
	USIDR  = data;
}

static inline void usiserial_continueUsiTx( uint8_t data, uint8_t count )
{
	usiserial_setUsiCount( count );
	USIDR  = data;
}

static inline void usiserial_stopUsi( void )
{
	USICR  = 0
//	       | _BV(USISIE)
//	       | _BV(USIOIE) // Enable USI Counter OVF interrupt.
//	       | _BV(USIWM1)
	       | _BV(USIWM0) // Select Three Wire mode.
//	       | _BV(USICS1)
//	       | _BV(USICS0) // Select Timer0 OVER as USI Clock source.
//	       | _BV(USICLK)
//	       | _BV(USITC)
	       ;
}


static inline void usiserial_switch2Tx( void )
{
	usiserial_startTimer0( 0 );
	usiserial_startUsiTx( 0xff, 0x0F );
	status.ongoing_Transmission_From_Buffer = true;
	USISERIAL_DDR |= _BV(USISERIAL_DO_P);  // Output.
}


static inline void usiserial_switch2Rx( void )
{
	USISERIAL_DDR &= ~_BV(USISERIAL_DO_P);   // Do not drive DO during RX.
	usiserial_stopTimer0();
	usiserial_stopUsi();
	USISERIAL_PCMSK |= _BV(USISERIAL_PCINT); // Enable Pin Change interrupt on DI pin.
	status.ongoing_Transmission_From_Buffer = false;
}


void usiserial_init( void )
{
	// Timer configuration
	TCCR0A = 0
	       | _BV(TCW0)
	       ;

	// USI configuration
	USIPP  = 0
#ifdef USISERIAL_PORTA
	       | _BV(USIPOS) // Use USI on PA[2:0].
#endif
	       ;

	// Pin configuration
	USISERIAL_DDR  &= ~( _BV(USISERIAL_DO_P) | _BV(USISERIAL_DI_P) ); // Input.
	USISERIAL_PORT |=    _BV(USISERIAL_DO_P) | _BV(USISERIAL_DI_P);   // Enable pullups.
	GIMSK |= _BV(USISERIAL_PCIE); // Enable pin change interrupts.

	// Wait for incoming data.
	usiserial_switch2Rx();
}


void usiserial_putByte( uint8_t data )
{
	uint8_t tmphead = (txHead + 1) & USISERIAL_TX_BUFFER_MASK; // Calculate buffer index.
	while( tmphead == txTail );                                // Wait for free space in buffer.
	txBuf[tmphead] = reverse_byte( data );                     // Reverse the order of the bits in the data byte and store data in buffer.
	txHead = tmphead;                                          // Store new index.

	if ( !status.ongoing_Transmission_From_Buffer ) {          // Start transmission from buffer (if not already started).
		while( status.ongoing_Reception_Of_Package );          //  Wait for USI to finish reading incoming data.
		usiserial_switch2Tx();
	}
}


uint8_t usiserial_getByte( void )
{
	while( rxHead == rxTail );                                 // Wait for incoming data.
	uint8_t tmptail = (rxTail + 1) & USISERIAL_RX_BUFFER_MASK; // Calculate buffer index.
	rxTail = tmptail;                                          // Store new index.
	return reverse_byte( rxBuf[tmptail] );                     // Reverse the order of the bits in the data byte before it returns data from the buffer.
}


uint8_t usiserial_rxAvailable( void )
{
	return (rxHead - rxTail) & USISERIAL_RX_BUFFER_MASK;
}


uint8_t usiserial_txAvailable( void )
{
	return (txTail - txHead - 1) & USISERIAL_TX_BUFFER_MASK;
}


ISR( PCINT_vect )
{
	if( !(USISERIAL_PIN & _BV(USISERIAL_DI_P)) ) {
		USISERIAL_PCMSK &= ~_BV(USISERIAL_PCINT); // Disable Pin Change interrupt on DI pin.
		status.ongoing_Reception_Of_Package = true;

		usiserial_startTimer0( USISERIAL_INTERRUPT_STARTUP_DELAY + USISERIAL_INITIAL_TIMER0_SEED );
		usiserial_startUsi( USISERIAL_COUNTER_SEED_RECEIVE );
	}
}

ISR( USI_OVF_vect )
{
	if( status.ongoing_Transmission_From_Buffer ) {                                           // Check if we are running in Transmit mode.
		if( status.ongoing_Transmission_Of_Package ) {                                        //  If ongoing transmission, then send second half of transmit data.
			status.ongoing_Transmission_Of_Package = false;                                   //   Clear on-going package transmission flag.
			usiserial_continueUsiTx( (txData<<3)|0x07, USISERIAL_COUNTER_SEED_TRANSMIT );     //   Rest of the data and a stop-bit.
		} else {                                                                              //  Else start sending more data or leave transmit mode.
			if( txHead != txTail ) {                                                          //   If there is data in the transmit buffer, then send first half of data.
				status.ongoing_Transmission_Of_Package = true;                                //    Set on-going package transmission flag.
				uint8_t tmptail = (txTail+1) & USISERIAL_TX_BUFFER_MASK;                      //    Calculate buffer index.
				txTail = tmptail;                                                             //    Store new index.
				txData = txBuf[tmptail];                                                      //    Read out the data that is to be sent. Note that the data must be bit reversed before sent.
				                                                                              //    The bit reversing is moved to the application section to save time within the interrupt.
				usiserial_continueUsiTx( (txData>>2)|0x80, USISERIAL_COUNTER_SEED_TRANSMIT ); //    Copy (initial high state,) start-bit and 6 LSB of original data (6 MSB of bit of bit reversed data).
			} else {                                                                          //   Else enter receive mode.
				usiserial_switch2Rx();
			}
		}
	} else {                                                                                  // Else running in receive mode.
		status.ongoing_Reception_Of_Package = false;
		uint8_t tmphead = (rxHead+1) & USISERIAL_RX_BUFFER_MASK;                              //  Calculate buffer index.
		if( tmphead == rxTail ) {                                                             //  If buffer is full trash data and set buffer full flag.
			status.reception_Buffer_Overflow = true;                                          //   Store status to take actions elsewhere in the application code
		} else {                                                                              //  If there is space in the buffer then store the data.
			rxHead = tmphead;                                                                 //   Store new index.
			rxBuf[tmphead] = usiserial_getUsiRx();                                            //   Store received data in buffer. Note that the data must be bit reversed before used.
		}                                                                                     //  The bit reversing is moved to the application section to save time within the interrupt.
		usiserial_switch2Rx();
	}
}

// Timer0 Overflow interrupt is used to trigger the sampling of signals on the USI ports.
ISR( TIMER0_OVF_vect )
{
	// Reload the timer, current count is added for timing correction.
	uint16_t count = TCNT0L;
	count |= (uint16_t)TCNT0H << 8;

	count += USISERIAL_TIMER0_SEED;

	TCNT0H = count >> 8;
	TCNT0L = count;
}
