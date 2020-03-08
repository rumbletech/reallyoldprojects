/*
 * NRF24L01.C
 *
 * Created: 7/2/2019 2:17:19 PM
 *  Author: Mohamed yousry 
 * This file contains basic API to configure , send and receive from the NRF2401+ chips , it uses my_spi.h
 */

#include "NRF24L01.h"
#include "uart_debug.h"



//Write and verify register , this function will
//Write a n words ( up to 5 bytes ) to a register , and will read that register and check it against the written word
//returns the LSB of the Written word if successful 
//returns -1 if it fails 




int wrv_register ( unsigned char reg_address , unsigned char* word_ptr , unsigned char byte_num  ){
	
	unsigned char temp , i  ; 
	
	CS_LOW ; // HIGH TO LOW TRANSITION

	spi_master_write( MASK_W_REGISTER | reg_address ) ; // Send write command to register 
	
	for( i  = 0 ; i <= byte_num -1 ; i++ ){ 
	spi_master_write ( *( word_ptr + i ) );
	
	}

	CS_HIGH ; // LOW TO HIGHT END OF COMMAND 
	
	CS_LOW ; // NEW COMMAND 
	
	spi_master_write ( MASK_R_REGISTER | reg_address ); // Send a read command 
	
	for ( i = 0 ; i <= byte_num -1 ; ++i  ){
		
	 // read the register
		uart_print("\n");
		uart_printb(temp = spi_master_receive() );
		uart_print("\n");
	if ( temp != *( word_ptr + i ) ) { return -1 ; } // error 
	
	}
	CS_HIGH ; // END OF COMMAND 

	return *(word_ptr) ;  // Returns LSB of the byte array
			
}



void w_register ( uint8_t word , uint8_t addr ){
	
	CS_LOW ;                                      // CS_LOW
	spi_master_write( addr|MASK_W_REGISTER );     // WRITE REGISTER COMMAND
	spi_master_write( word );                     // WRITE 8- BIT WORD 
	CS_HIGH;	                                  // CS_HIGH
	
}



uint8_t r_register ( uint8_t addr ){
	
	CS_LOW ;                                             // CS_LOW                              
	spi_master_write ( MASK_R_REGISTER | addr );		 // SEND READ COMMAND 
	addr = spi_master_receive() ;						 // READ IN THE REGISTER  
	CS_HIGH;                                             // CS_HIGH 
	return addr ;										 // RETURN READ WORD 
	
}



void nrf_configure ( nrf_config config_struct ){

uint8_t temp ; 
	
//Start with the Configuration register

temp = ( config_struct.crc << CRC_LENGTH ) | ( config_struct.int_mask << MASK_MAX_INT )| ( config_struct.txrx_mode << PRIM_RX_TX  ) | ( config_struct.module_state << PWR_UP )	;

w_register ( temp , REGISTER_CONFIG );

	
// Now to the auto-re-transmit register

temp = ( config_struct.ard << ARD0 ) | ( config_struct.arc << ARC0 ) ;

w_register( temp , REGISTER_RETR );
	
//Now to the rf setup register

temp = ( config_struct.transmit_power << RF_PWR0 ) | ( config_struct.cont_wave_transmit << CONT_WAVE ) | ( config_struct.data_rate << RF_DR_HIGH );

w_register( temp , REGISTER_RF_SETUP ) ;
	
//	RF_CH

temp = config_struct.rf_ch ;

w_register( temp , REGISTER_RF_CH );

// FEATURE AND DYNAMIC PAYLOAD HERE

temp = config_struct.feature ;
w_register( temp , REGISTER_FEATURE );

//	DEFAULT IS TX // NO RECEIVERS

w_register( 0 , REGISTER_EN_AA);

w_register( 0 , REGISTER_EN_RXADDR);
	
}





uint8_t set_addr ( Pipe reg_addr , uint64_t addr , uint8_t addr_len ){
	
	if( addr_len > 5 ) { return 0 ; }                           //ADDRESS LENGTH HAS A MAX OF 5 BYTES
		
	w_register( addr_len - 2 , REGISTER_SETUP_AW ) ;            // WRITE ADDRESS WIDTH TO AW REGISTER
	
	CS_LOW ;                                                    // CS_LOW NEW COMMAND 
	
	spi_master_write ( reg_addr | MASK_W_REGISTER ) ;           // WRITE TO SPECIFIED ADDRESS REGISTER , PIP0 , PIPE1 , ...
	
	for ( uint8_t i = 0 ; i <= addr_len -1 ; i++ ){		               
		spi_master_write( addr & ( 0B11111111 ) );
		addr >>= 8 ; 		
												   }
	CS_HIGH ;                                                   // CS_HIGH
	
	return 1 ;                          

	
}


uint8_t nrf_power_down( void ){
	
	uint8_t temp ;

	temp = r_register( REGISTER_CONFIG ) ;				 // READ CONFIG REGISTER
	if ( !( temp & ( 1 << PWR_UP ) ) ){ return 0 ; }     // ALREADY IN POWERDOWN MODE
	temp &= ~( 1 << PWR_UP ) ;							 // CLEAR PWR_UP BIT 
	w_register( temp , REGISTER_CONFIG ) ;				 // WRITE NEW CONFIGURATION
	return 1 ;                                                  

}

uint8_t nrf_power_up ( void ){
	
	uint8_t temp ;
	
	temp = r_register( REGISTER_CONFIG ) ;          // READ CONFIG REGISTER
	if ( temp & ( 1 << PWR_UP ) ){ return 0 ; }     // ALREADY IN STANDBY MODE 1 
	temp |= ( 1 << PWR_UP ) ;                      // SET TO POWER UP 
	w_register( temp , REGISTER_CONFIG ) ;          // WRITE PWR_UP BIT TO REGISTER 
	_delay_ms(PWR_UP_SETTLE_TIME);					// POWER UP SETTLE TIME 

	return 1 ;                                      // POWERUP DONE 
	
}


void nrf_tx_write ( uint8_t * data_ptr , uint8_t num_bytes , uint8_t ack ){
	
	CS_LOW ;																	// HIGH TO LOW TRANSITION

	spi_master_write( ack ? W_TX_PAYLOAD : W_TX_PAYLOAD_NOACK ) ;				// Send write command to register
	
	for( uint8_t i = 0 ; i <= num_bytes -1 ; i++ ){
		spi_master_write ( *( data_ptr + i ) );		
	}

	CS_HIGH ;																	// LOW TO HIGHT END OF COMMAND	
	CE_PULSE ;																	// PULSE CE 10 uS
	
}





void nrf_rx_read ( uint8_t * ptr , uint8_t num_bytes ){
	
	CE_LOW ;										// Leave RX_MODE
	CS_LOW ;										// HIGH TO LOW TRANSITION

	spi_master_write( R_RX_PAYLOAD ) ;				// Send write command to register
	
	for( uint8_t i  = 0 ; i <= num_bytes -1 ; i++ ){
		
	*( ptr + i ) = spi_master_receive();
	
	}
	
	CS_HIGH ;	
	
	w_register( ( 1 << RX_DR ), REGISTER_STATUS ) ;   // CLEAR RX INTERRUPT 

	CE_HIGH ;										  // CE HIGH

}


uint8_t pipe_setup (Pipe reg_addr , uint64_t addr , uint8_t addr_len , uint8_t ENACK , uint8_t ENRX , uint8_t plw ){
	
	if( set_addr( reg_addr , addr , addr_len ) == 0  ) { return 0 ; }         // SET PIPE ADDRESS
	if ( plw > 32 ){ return 0 ; }											  // MAX PAYLOAD IS 32 BYTES
	w_register( plw , reg_addr + 7 ) ;										  // SET PIPE PAYLOAD WIDTH
	if( ENACK ) w_register(  1 << ( reg_addr - 0x0A) , REGISTER_EN_AA );      // SET PIPE AACK SETTING
	if( ENRX  ) w_register(  1 << ( reg_addr - 0x0A) , REGISTER_EN_RXADDR );  // SET PIPE RX ENABLE 
	
	return 1 ; 	
}
















