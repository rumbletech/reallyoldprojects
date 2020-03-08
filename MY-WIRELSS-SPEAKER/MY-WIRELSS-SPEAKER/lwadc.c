/*
 * lwadc.c
 *
 * Created: 7/7/2019 9:27:45 PM
 *  Author: Mohamed yousry 
 */ 



#include "lwadch.h"


// INIT NO INTERRUPT 
void adc_init( uint8_t adclk , uint8_t adref ){
	
	ADMUX  = ( adref << REFS0 ) ;                    // SET MUX REGISTER , ADC0 IS SELECTED BY DEFAULT 
	ADCSRA = ( adclk << ADPS0 ) | ( 1 << ADEN ) ;
	  
}


// INIT WITH INTERRUPT
void adc_init_int( uint8_t adclk , uint8_t adref ){
	
	ADMUX  = ( adref << REFS0 ) ;                    // SET MUX REGISTER , ADC0 IS SELECTED BY DEFAULT
	ADCSRA = ( adclk << ADPS0 ) | ( 1 << ADEN ) | ( 1 << ADIE ) ;
	
}

// SELECTS CHANNEL 

void adc_channel_select( uint8_t adc_channel ){
	
	ADMUX|= ( adc_channel << MUX0 ) ;

	
}


// ADC CONVERT NO INTERRUPT --- AKA BLOCKING

uint16_t adc_sample( void ){
	
	uint16_t converresult ; 
	
	ADCSRA |= ( 1 << ADSC ) ;
	while ( ADCSRA & ( 1 << ADSC ) );
	
	converresult = ADCL ;
	converresult|= (ADCH << 8 ) ;
		
	return converresult ; 
	
}
