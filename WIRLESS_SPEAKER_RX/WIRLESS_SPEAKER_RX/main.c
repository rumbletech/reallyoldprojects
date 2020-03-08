/*
 * WIRLESS_SPEAKER_RX.c
 *
 * Created: 7/3/2019 5:46:33 PM
 * Author :mohamed yousry RXXXXXXXXXX
 */ 


#include "wireless_speaker.h"

#define CLEAR_OC0_ON_COMPARE_MATCH 0B10
#define PRESCALER                  0B011



int main(void){
	
	uint8_t word[5] ;
	uint16_t analog_val ; 
	
	
	word[0] = 0 ;
	word[1] = 0 ;
	word[2] = 0 ;
	word[3] = 0 ;
	word[4] = 0 ;
	
	
	uart_init();
	spi_master_init( SCK_4MHZ );
	
	
	_delay_ms(200);
	CE_INIT ;
	CS_INIT ;
	IRQ_INIT ;
	DDRB |= ( 1 << PB3 );
	DDRA = 0XFF ; 
	
	nrf_config my_config ;
	
	//START FAST PWM 
	
	OCR0 = 0 ; 
	TCCR0 = ( PRESCALER << CS00 ) | ( 1 << WGM00 )| ( 1 << WGM01 )| ( CLEAR_OC0_ON_COMPARE_MATCH << COM00 ) ;
	
	// Configuration
	
my_config.arc = 0 ;                               
my_config.ard = 0 ;                               
my_config.cont_wave_transmit = NO_WAVE ;
my_config.crc = CRC_1BYTE ;        
my_config.data_rate = RATE_1MBPS ;         
my_config.int_mask = RX_INT ;              
my_config.module_state = POWER_UP ;           
my_config.txrx_mode = RX ;                        
my_config.feature = NO_DYNPL_NO_AACK_NOCMD ;   
my_config.rf_ch = 10 ;     
my_config.transmit_power = 0 ;
           
nrf_configure ( my_config );   

pipe_setup( PIPE0 , 0X8723435212 , 5 , 0 , 1 , 3 )   ;    // RECEIVING PIPE EN_RX 1 , SHOCKBURST AACK PIPE ENACK 1 


	CE_HIGH; 
	while (1)
	{

		
		IRQ_POLL ;								// wait for interrupt request
		nrf_rx_read( word , 3 ) ;               // READ THE PACKET
		analog_val = 0 ;
		analog_val = word[1];
		analog_val <<= 8 ; 
		analog_val |= word[0];
		OCR0 = analog_val / 4 ;

	}
}




// CRYSTAL CHECK
/*	
	PORTA = 0XFF ;
	_delay_ms(1000);
	PORTA = 0X00;
	_delay_ms(1000);
*/