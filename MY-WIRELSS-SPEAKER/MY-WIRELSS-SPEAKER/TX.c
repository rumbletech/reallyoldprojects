/*
 * MY-WIRELSS-SPEAKER.c
 *
 * Created: 7/1/2019 4:01:05 PM
 * Author : Mohamed yousry mohamed ..
 * 
 * This is the main source file for the project ( Wireless Speaker ) 
 *
 */ 



#include "wireless_speaker.h"

#define PRESCALER_8KHZ 0B010
#define PRESCALEER_VAL 125

#define TIMER0_START     TCCR0 = ( PRESCALER_8KHZ << CS00 )
#define TIMER0_STOP      TCCR0 = 0 
#define TIMER0_RELOAD(X) TCNT0 = 0XFF - X + 1  


// GLOBOS

volatile uint8_t timer_flag = 0 ; 


// PROTOS

static __inline__ void timer0_init( void );


ISR( TIMER0_OVF_vect ){
	
	TIMER0_STOP ;
	TIMER0_RELOAD( PRESCALEER_VAL );
	timer_flag = 1 ;
	TIMER0_START; 
	
	
}

int main(void){
	
	uint8_t data_packet[3];
	uint16_t adc_val  ; 
	
	data_packet[0]= 0 ;
	data_packet[1]= 0 ;
	data_packet[2]= 0 ; 
	
	adc_val = 0 ;
	 
    uart_init();
	spi_master_init( SCK_4MHZ );
	_delay_ms(200);
	CE_INIT ;
	CS_INIT ;
	IRQ_INIT ; 
	adc_init( ADCLK_125K , AVCC_REF );            // USE AVCC AS REFERENCE , 125KHZ ADC CLOCK 
	adc_channel_select( 0 );                       // SELECT CHANNEL 0
	adc_sample();								  // DO THE FIRST SAMPLING 
	
	
	
	
	nrf_config my_config ; 
	
	// Configuration 
	
	my_config.arc = 0 ;
	my_config.ard = 10 ;
	my_config.cont_wave_transmit = NO_WAVE ;
	my_config.crc = CRC_1BYTE ;
	my_config.data_rate = RATE_1MBPS ;
	my_config.int_mask = TX_INT ;
	my_config.module_state = POWER_UP ;
	my_config.txrx_mode = TX ;
	my_config.transmit_power = POWER_0DBM ;
	my_config.feature = NO_DYNPL_NO_AACK_CMD ;
	my_config.rf_ch = 10 ;
	
	
	nrf_configure ( my_config );                 // CONFIGURE THE NRF24L01+
	
	set_addr(PTX , 0X8723435212 , 5 );           // SET TX BYTE , ADDRESS WIDTH 5 BYTE , 0X8723435212 ADDRESS
	
	nrf_power_up();	
	
	
	timer0_init();
	sei();
	
	
    while (1) 
    {
		
		if ( timer_flag ){
		uart_print("1");	
		adc_val = adc_sample();
		data_packet[0] = ( adc_val & 0B11111111 );
		data_packet[1] = ( adc_val >> 8 );
		++data_packet[2];
		nrf_tx_write( data_packet , 3 , 0 ) ;               
		IRQ_POLL ; 
		CLEAR_TX_INT ;	
		timer_flag = 0 ;
		
		}

		
    }
}


/*
adc_sample();

nrf_tx_write( &word , 1 , 0 ) ;
IRQ_POLL ;
CLEAR_TX_INT ;
TX_FLUSH;
nrf_power_down();

if ( word == 0x55 ){ word = 0xD5 ;}
else { word = 0x55 ; }
uart_print("sent");
_delay_ms(1000);
*/


static __inline__ void timer0_init( void ){
	
	TIMSK = ( 1 << TOIE0 );
	TCCR0 = ( PRESCALER_8KHZ << CS00 ) ;
	TCNT0 = 0XFF - PRESCALEER_VAL + 1 ;
	
}

