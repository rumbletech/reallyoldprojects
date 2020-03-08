



/*
 * my_spi.c
 *
 * Created: 7/1/2019 4:18:55 PM
 * Author: Mohamed yousry 
 * SPI_SMALL_LIBRARY
 */ 

// INCLUDES



#define F_CPU 8000000UL // Define here F_CPU FOR SPI HEADER 

#include "my_spi.h"





void spi_master_init ( unsigned char clk_freq ) {
	
	SPI_DDR |= ( 1 << SPI_MOSI_PIN) | ( 1<< SPI_SCK_PIN ) | ( 1 << SPI_SS_PIN );
	SPI_DDR &= ~ ( 1 << SPI_MISO_PIN ) ;
	
	SPCR = ( 1 << SPE ) | ( clk_freq & 0b011 ) | ( 1 << MSTR ); // SET SPR0 SPR1 ACCORDING TO CLK_FREQ AND SET SPE
	
	SPSR = ( clk_freq >> 2 ) ;  // SET OR CLEAR SPI2X
	
}

void spi_slave_init ( unsigned char clk_freq ){
	
	SPCR = ( 1 << SPE )|( clk_freq & 0b11 ) ; // SET SPR0 SPR1 ACCORDING TO CLK_FREQ AND SET SPE
	
	SPSR = ( clk_freq >> 2 ) ;  // SET OR CLEAR SPI2X
}



unsigned char spi_master_write ( unsigned char spi_byte  ){
	
	SPDR = spi_byte ;
	
	while ( !(SPSR & ( 1 << SPIF) ) ) ;
	
	return SPDR ;
}

// This function will write a dummy byte to prouduce 8 clks to receive a byte from Device Dummy_byte is 0xff
// if another dummy is needed it can be changed by changing DUMMBY_BYTE MACRO
// Or using spi_master_write( ) with the right parameter

unsigned char spi_master_receive( void ){
	
	SPDR = DUMMY_BYTE ;
	
	while( ! ( SPSR & ( 1 << SPIF )) );
	
	return SPDR ;
}










