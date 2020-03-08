/*
 * uart_debug.c
 *
 * Created: 7/1/2019 7:21:10 PM
 *  Author: Mohamed yousry
 */ 


#include "uart_debug.h"



void uart_init( void ){
	
	UCSRB = ( 1 << RXEN ) | ( 1 << TXEN ) ; 
	UCSRC = ( 1 << URSEL ) | ( 0B11 << UCSZ0 ) ;
	UBRRL = 12 ; // 38400 BAUD 
}


void uart_send ( unsigned char uart_byte ){
	
	UDR = uart_byte ;
	
	while ( !(UCSRA & ( 1 << UDRE ) ) ); // Wait for the buffer to be ready to be written again ;;;
	 
}


// Prints a string ... pointed to by strptr 

void uart_print( char * strptr ){
	
	while ( *(strptr) != 0 ){
		
		uart_send( *(strptr) ); 
		++strptr ; 
	}
	
}


// PRINTS BINARY VALUE OF THE ARG

void uart_printb( unsigned char byte ){
	signed char i ; 
	for ( i = 0 ; i <=7  ; i++ ){
		
		if ( byte & ( 0b10000000 ) ){
			uart_send('1');
		}
		else{
			uart_send('0');
			
		}
		byte <<= 1  ; 
	}
}






	
