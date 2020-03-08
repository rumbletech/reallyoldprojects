/*
 * lwadch.h
 *
 * Created: 7/7/2019 9:27:34 PM
 *  Author: Mohamed yousry 
 */ 


#define F_CPU 8000000UL

#ifndef LWADCH_H_
#define LWADCH_H_

//INCLUDES

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>
#include "uart_debug.h"


#if ( F_CPU == 8000000 )

#define ADCLK_250K  0B101
#define ADCLK_125K  0B110
#define ADCLK_62P5K 0B111

#endif

#define EXT_REF   0B00
#define INT_REF   0B11
#define AVCC_REF  0B01



uint16_t adc_sample( void );
void adc_channel_select( uint8_t adc_channel );
void adc_init( uint8_t adclk , uint8_t adref );
void adc_init_int( uint8_t adclk , uint8_t adref );


  

#endif /* LWADCH_H_ */