/*
 * my_spi.h
 *
 * Created: 7/1/2019 4:18:32 PM
 * Author : Mohamed yousry 
 */ 




#ifndef MY_SPI_H_
#define MY_SPI_H_


#include <avr/io.h>
#include <avr/interrupt.h>


/* This small library uses standard positive sck polarity and sample on rising edge of the clock , 
   Data order is MSB First 
   No interrupt routine
   Intended as Master transmitter and/or Master receiver only 
*/


#ifndef F_CPU 
#error " F_CPU Is not defined for SPI Library " 
#endif 


#define SPI_DDR DDRB
#define SPI_MOSI_PIN PB3
#define SPI_MISO_PIN PB4
#define SPI_SCK_PIN  PB5
#define SPI_SS_PIN PB2

// FSCK Frequency bit-rate [SCK RATES FOR 8 MHZ ]
 
#if ( F_CPU == 8000000)

#define SCK_4MHZ  0B100
#define SCK_2MHZ  0B000
#define SCK_1MHZ  0B101
#define SCK_500K  0B001
#define SCK_250K  0B110
#define SCK_125K  0B010
#define SCK_62P5K 0B011

#endif



#define DUMMY_BYTE 0XFF



void spi_master_init ( unsigned char clk_freq ) ;
void spi_slave_init ( unsigned char clk_freq ) ;
unsigned char spi_master_write ( unsigned char spi_byte  ) ;
unsigned char spi_master_receive( void );



	





#endif /* MY_SPI_H_ */