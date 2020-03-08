/*
 * RX.c
 *
 * Created: 7/3/2019 5:38:41 PM
 *  Author: Mohamed yousry
 */ 



#include "wireless_speaker.h"


int main ( void ){
	
		unsigned char word = 0x55 ;
		
		uart_init();
		spi_master_init( SCK_1MHZ );
		
		
		_delay_ms(200);
		CE_INIT ;
		CS_INIT ;
		IRQ_INIT ;
		
		nrf_config my_config ;
		
		// Configuration
		
		my_config.arc = 0b0000 ;
		my_config.ard = 0b0000 ;
		my_config.cont_wave_transmit = NO_WAVE ;
		my_config.crc = CRC_1BYTE ;
		my_config.data_rate = RATE_1MBPS ;
		my_config.int_mask = TX_INT ;
		my_config.module_state = POWER_UP ;
		my_config.txrx_mode = TX ;
		my_config.transmit_power = POWER_0DBM ;
		my_config.rx_address_width = ADDRW5;
		my_config.feature = NO_DYNPL_NO_AACK;
		
		
		// REGISTERS
		my_config.en_rx_addr_reg = 0b00000 ;
		my_config.enack_reg = 0b00000 ;
		my_config.rf_ch_reg = 10 ;
		my_config.pipe0_plw = 1 ;
		my_config.pipe1_plw = 1 ;
		my_config.pipe2_plw = 1 ;
		my_config.pipe3_plw = 1 ;
		my_config.pipe4_plw = 1 ;
		my_config.pipe5_plw = 1 ;
		set_addr( my_config.tx_addr , 0X8723435212 ,5 ) ;
		set_addr( my_config.pipe0_addr , 0X8723435213 ,5 ) ;
		set_addr( my_config.pipe1_addr , 0X8723435214 ,5 ) ;
		my_config.pipe2_addr = 0x43 ;
		my_config.pipe3_addr = 0x23 ;
		my_config.pipe4_addr = 0x63 ;
		my_config.pipe5_addr = 0x44 ;
		
		
		
		
		
		nrf_configure ( &my_config );
		
		
		
		

		

		

		
		
		while (1)
		{
			
			nrf_power_up( my_config.config_reg );
			nrf_tx_write( &word , 1 );
			nrf_power_down( my_config.config_reg );
			IRQ_POLL ;
			uart_print("sent");
			_delay_ms(5000);

			
			
		}
		
	}
	
	
	
	

