/*
 * uart_debug.h
 *
 * Created: 7/1/2019 7:21:31 PM
 *  Author: Mohamed yousry
 * Simple 38.4K uart debugger api functions
 */ 


#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_


#include <avr/io.h>

// 3 simple api's 

//==============================================

// INITIALIZE UART AT 38400 BAUD 
void uart_init( void );
// SENDS A VALUE 
void uart_send( unsigned char uart_byte );
//PRINTS A STRING POINTED TO BY STRPTR 
void uart_print( char * strptr );
// PRINTS BINARY VALUE OF THE ARG
void uart_printb( unsigned char byte );


//================================================


#endif /* UART_DEBUG_H_ */