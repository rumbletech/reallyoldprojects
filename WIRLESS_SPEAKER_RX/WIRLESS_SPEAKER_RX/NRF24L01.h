/*
 * NRF24L01.h
 *
 * Created: 7/2/2019 2:17:19 PM
 *  Author: Mohamed yousry 
 * This file contains basic API to configure , send and receive from the NRF2401+ chips , it uses my_spi.h
 */ 

#define F_CPU 16000000UL

#ifndef NRF24L01_H_
#define NRF24L01_H_



/* Example TX *********************************************************************************************************

nrf_config my_config ;                            // CONFIGURATION STRUCT
spi_master_init( SCK_1MHZ );                      // INIT SPI PERIPHERAL
_delay_ms(200);                                   // 100MS ~ 200 MS POWER UP DELAY
CE_INIT ;                                         // CE PIN INIT
CS_INIT ;                                         // CS PIN INIT
IRQ_INIT ;                                        // IRQ PIN INIT , ACTIVE LOW INTERRUPT
my_config.arc = 2 ;                               // HOW MANY TIMES YOU WANT THE MODULE TO RETRANSMIT IF A PACKET IS LOST
my_config.ard = 2 ;                               // HOW MUCH TIME DOES HE WAIT FOR ACK BEFORE RETRANSMITTING PACKETS
my_config.cont_wave_transmit = NO_WAVE ;          // CONTINOUS WAVE MODE ( DUNNO DONT CARE )
my_config.crc = CRC_1BYTE ;                       // CRC CONFIG 
my_config.data_rate = RATE_1MBPS ;                // AIR DATA  RATE
my_config.int_mask = TX_INT ;                     // INTERRUPT MASKS
my_config.module_state = POWER_UP ;               // YOU WANT THE MODULE IN POWER UP OR POWER DOWN AFTER INIT ?
my_config.txrx_mode = TX ;                        // YOU WANT IT PRIMARY TX OR RX ?
my_config.transmit_power = POWER_0DBM ;           // TRANSMITTING POWER 
my_config.feature = NO_DYNPL_NO_AACK_NOCMD ;      // WORD = DYNAMIC PAYLOAD ENABLE \ AACK ENABLE \ NOACK CMD ENABLE 
my_config.rf_ch = 10 ;                            // CHANNEL FREQ , 10 >> 2.4 GHZ + 10 MHZ
nrf_configure ( my_config );                      // CONFIGURE THE NRF24L01+
set_addr(PTX , 0X8723435212 , 5 );                // SET TX BYTE , ADDRESS WIDTH 5 BYTE , 0X8723435212 ADDRESS

// IF YOU WANT TO SET RECEIVER PIPES , YOU CAN USE API >> pipe_setup()
	
nrf_power_up();									  // POWER UP MODULE IF IT IS IN POWER DOWN 
nrf_tx_write( &word , 1 , 0 ) ;                   // WRITE A SINGLE BYTE NO ACK
IRQ_POLL ;                                        // POLL THE IRQ LINE , NO ISR IS USED
CLEAR_TX_INT ;                                    // CLEAR THE FLAG TO ALLOW IRQ PIN TO GO HIGH AGAIN , MUST BE MANUALLY CLEARED LIKE THIS
TX_FLUSH;                                         // FLUSH TX BUFFER ( NOT NECESSARY ) 
nrf_power_down();                                 // POWER DOWN THE MODULE ( TX DONE )
	
************************************************************************************************************************/

/* RX EXAMPLE

my_config.arc = 0 ;												// AUTO RETRANSMIT COUNT 
my_config.ard = 0 ;												// AUTO RETRANSMIT DELAY , SET TO ZERO WHEN PRIMARY RX
my_config.cont_wave_transmit = NO_WAVE ;                        // NO CONTINOUS WAVE MODE 
my_config.crc = CRC_1BYTE ;										// 1 BYTE CRC ERROR CHECKING
my_config.data_rate = RATE_1MBPS ;								// AIR DATA RATE 1MBPS
my_config.int_mask = RX_INT ;									// RX_INT ONLY MASK
my_config.module_state = POWER_UP ;								// START IN POWER UP MODE
my_config.txrx_mode = RX ;										// PRIM RX OPERATION
my_config.feature = NO_DYNPL_NO_AACK_NOCMD ;                    // DON'T CARE YOU CAN SET TO A WANTED VALUE IF NOT NEEDED IT MUST BE SET TO ZERO
my_config.rf_ch = 10 ;										    // SAME CHANNEL AS TX
my_config.transmit_power = 0 ;									// TRANSMIT POWER NOT NEEDED HERE SET TO 0 

nrf_configure ( my_config );                                    // CONFIGURE THE DEVICE WITH THE SETTINGS IN THE STRUCT
pipe_setup( PIPE0 ,0X8723435212 , 5 , 0 , 1 , 1 )   ;			// SETUP PIPE0 TO RECEIVE 


CE_HIGH;														// ENABLE RX 
while (1)
{
	IRQ_POLL ;													// WAIT FOR INTERRUPT ( POLLING IS USED HERE ) 
	nrf_rx_read( &word , 1 ) ;								    // READ THE WORD 
	
}
*/	

/* CONFIG EXAMPLE EN_AA TX SIDE 

	set_addr(PTX , 0X8723435212 , 5 );						// SET TX BYTE , ADDRESS WIDTH 5 BYTE , 0X8723435212 ADDRESS
	pipe_setup( PIPE0 ,0X8723435212 , 5 , 1 , 1 , 1 );		// RX_PIPE EN_RX ENABLED , EN_AA ENALBED FOR PIPE0 ( MUST BE PIPE0 ) 
	
	
*/

/* CONFIG_EXAMPLE EN_AA RX SIDE 

pipe_setup( PIPE0 , 0X8723435212 , 5 , 1 , 1 , 1 )   ;    // RECEIVING PIPE EN_RX 1 , SHOCKBURST AACK PIPE ENACK 1 

*/


//Includes

#include "my_spi.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

//CE AND CSN PINS ARE DEFINED HERE

#define CE_PIN PD6      // PIN NUMBERS
#define CS_PIN PD5

#define CE_PORT PORTD    // USED PORT
#define CS_PORT PORTD

#define CE_DDR DDRD     // USED DDR 
#define CS_DDR DDRD

// IRQ PIN DEFINES

#define IRQ_PORT PORTD
#define IRQ_PIN  PIND
#define IRQ_DDR  DDRD
#define IRQ      PD2


//defines

#define MAX_PIPE_ADDR_LENGTH 5
#define PULSE_TIME 20
#define PWR_UP_SETTLE_TIME 1.5
#define MASK_CLEAR_INTERRUPT 0B01110000

//MACROS - END WITH ; 

#define CE_INIT  CE_DDR|= ( 1 << CE_PIN ); CE_PORT &= ~ ( 1 << CE_PIN )
#define CS_INIT  CS_DDR|= ( 1 << CS_PIN ); CS_PORT |=   ( 1 << CS_PIN )

#define CE_PULSE CE_DDR|= ( 1 << CE_PIN ) ; CE_PORT|= ( 1 << CE_PIN );_delay_us(PULSE_TIME);CE_PORT&=~( 1 << CE_PIN )

#define CS_HIGH CS_PORT|=  ( 1 << CS_PIN ) 
#define CS_LOW  CS_PORT&= ~( 1 << CS_PIN ) 
#define CE_LOW  CE_PORT&= ~( 1 << CE_PIN )
#define CE_HIGH CE_PORT|=  ( 1 << CE_PIN )


#define IRQ_INIT IRQ_DDR&=~( 1<< IRQ) ; IRQ_PORT&=~(1<<IRQ) 
#define IRQ_POLL while ( IRQ_PIN & ( 1 << IRQ ) )


#define TX_FLUSH	       CS_LOW;spi_master_write( FLUSH_TX );CS_HIGH 
#define RX_FLUSH	       CS_LOW;spi_master_write( FLUSH_RX );CS_HIGH
#define CLEAR_INTS         w_register( MASK_CLEAR_INTERRUPT , REGISTER_STATUS )
#define CLEAR_TX_INT       w_register( ( 1 << TX_DS ) , REGISTER_STATUS )
#define CLEAR_RX_INT       w_register( ( 1 << RX_DR ) , REGISTER_STATUS )
#define CLEAR_MAX_RT_INT   w_register( ( 1 << MAX_RT) , REGISTER_STATUS )






// REGISTER MAP AND COMMANDS

// SPI COMMANDS FOR THE NRF24L01+

#define MASK_R_REGISTER     0B00000000
#define MASK_W_REGISTER     0B00100000
#define R_RX_PAYLOAD        0B01100001
#define W_TX_PAYLOAD        0B10100000
#define FLUSH_TX            0B11100001
#define FLUSH_RX            0B11100010
#define REUSE_TX_PL         0B11100011
#define W_TX_PAYLOAD_NOACK  0B10110000
#define NOP                 0B11111111





// NRF24L01+ REGISTER MAP

#define REGISTER_CONFIG 0X00            // Configuration Register

// BITS
#define MASK_R_INT  6          // 0 unmasked , 1 masked
#define MASK_T_INT  5          // 0 unmasked , 1 masked
#define MASK_MAX_INT 4          // 0 unmasked , 1 masked
#define EN_CRC      3          // Enable CRC AUTO-GENERATION 1   , DISABLE 0
#define CRC_LENGTH  2          // 0 >> 1 BYTE CRC ,  1 >> 2 BYTE CRC
#define PWR_UP      1          // 1 POWER-UP , 0 POWER-DOWN
#define PRIM_RX_TX  0          // 1 RX , 0 TX


#define REGISTER_EN_AA  0X01            // ENABLE AUTO ACK FUNCTION REGISTER
//BITS

#define ENAA_P5 5         // ENABLES AACK ON PIPES 0 TO 5
#define ENAA_P4 4
#define ENAA_P3 3
#define ENAA_P2 2
#define ENAA_P1 1
#define ENAA_P0 0


#define REGISTER_EN_RXADDR 0X02          // ENABLES RX ADDRESSES ON PIPES 0 TO 5

//BITS

#define ERX_P5 5
#define ERX_P4 4
#define ERX_P3 3
#define ERX_P2 2
#define ERX_P1 1
#define ERX_P0 0

#define REGISTER_SETUP_AW 0X03                  //Selects WIDTH OF ADDRESS OF ALL PIPES ( COMMON WIDTH )

//BITS

#define AW1 1                    // ONLY 2 BITS  , 00 ILLEGAL , 01 3BYTE ADDR , 10 4 BYTE ADDR , 11 5BYTE ADDR
#define AW0 0

// MODES

#define AW_3BYTES 3
#define AW_4BYTES 4
#define AW_5BYTES 5


#define REGISTER_RETR  0X04          // SETUP FOR AUTO-RETRANSIMISSION FEATURE , ARD : Auto-retransmit delay , ARC : Auto-retransmitt count

//BITS

#define ARD3 7
#define ARD2 6
#define ARD1 5
#define ARD0 4

// Minimum delay for ARD is 250 uS at ARD = 0000 , 00001 = 500 US and so on till 4000 US AT 1111

#define ARC3 3
#define ARC2 2
#define ARC1 1
#define ARC0 0

// Minimum re-transmits is 0 , ARC =0000 >> RE-TRANSMIT DISABLED , ARC >> 1111 >> RETRANSMIT UP TO 15 TIMES

#define REGISTER_RF_CH  0X05         // Contains RF channel FREQ 128 CHANNEL 7 BITS from 0 >> 6

//BITS
#define RF_CH6 6
#define RF_CH5 5
#define RF_CH4 4
#define RF_CH3 3
#define RF_CH2 2
#define RF_CH1 1
#define RF_CH0 0


#define REGISTER_RF_SETUP 0X06
//BITS
#define CONT_WAVE  7  //ENABLES CONTINOUS CARRIER TRANSMISSION WHEN IT IS SET
#define RF_DR_LOW  5  //SETS THE DATA RATE TO 250KBPS
#define RF_DR_HIGH 3  // SELECTS HIGH SPEED DATA RATES 0 FOR 1 MBPS , 1 FOR 2MBPS GIVEN THAT RF_DR_LOW IS 0
#define RF_PWR1 2
#define RF_PWR0 1

//MODES

#define PWR_18DBM 0B00
#define PWR_12DBM 0B01
#define PWR_6DBM  0B10
#define PWR_0DBM  0B11

#define REGISTER_STATUS 0X07 //STATUS REGISTER

//BITS

#define RX_DR     6           // FLAG BIT < DATA READY > WRITE 1 TO CLEAR
#define TX_DS     5           // FLAG BIT < DATA SENT > WRITE 1 TO CLEAR
#define MAX_RT    4           // FLAG BIT < MAX NUMBER OF RE-TRANSMITTS OCCURED > WRITE 1 TO CLEAR
#define RX_P_NUM2 3           // RX_PIPE NUMBER THAT HAS AVAILABLE DATA
#define RX_P_NUM1 2           //( 111 MEANS EMPTY )
#define RX_P_NUM0 1           // ( 000 >> 101 ) THE NUMBERS OF THE SIX PIPES
#define TX_FULL   0           // FLAG BIT WHEN TX_FIFO IS FULL





#define REGISTER_RX_ADDR_P0 0X0A      // 40 BIT ADDRESS OF THE FIRST PIPE
#define REGISTER_RX_ADDR_P1 0X0B      // 40 BIT ADDRESS OF THE SECOND PIPE
#define REGISTER_RX_ADDR_P2 0X0C      // 40 BIT ADDRESS OF THE THIRD PIPE
#define REGISTER_RX_ADDR_P3 0X0D      // 40 BIT ADDRESS OF THE FOURTH PIPE
#define REGISTER_RX_ADDR_P4 0X0E      // 40 BIT ADDRESS OF THE FIFTH PIPE
#define REGISTER_RX_ADDR_P5 0X0F      // 40 BIT ADDRESS OF THE SIXTH PIPE


#define REGISTER_TX_ADDR 0X10          // 40 BIT TRANSMITTER ADDRESS

#define REGISTER_RX_PW_P0 0X11         // PAYLOAD WIDTH REGISTER FOR FIRST PIPE

//BITS

#define RX_PW_P0_5 5             // IT CAN GO FROM 0B00000001 TO 0B00010000 >> 32 BYTES MAX
#define RX_PW_P0_4 4
#define RX_PW_P0_3 3
#define RX_PW_P0_2 2
#define RX_PW_P0_1 1
#define RX_PW_P0_0 0

#define REGISTER_RX_PW_P1 0X12         // PAYLOAD WIDTH REGISTER FOR SECOND PIPE

//BITS

#define RX_PW_P1_5 5             // IT CAN GO FROM 0B00000001 TO 0B00010000 >> 32 BYTES MAX
#define RX_PW_P1_4 4
#define RX_PW_P1_3 3
#define RX_PW_P1_2 2
#define RX_PW_P1_1 1
#define RX_PW_P1_0 0

#define REGISTER_RX_PW_P2 0X13         // PAYLOAD WIDTH REGISTER FOR THIRD PIPE

//BITS

#define RX_PW_P2_5 5             // IT CAN GO FROM 0B00000001 TO 0B00010000 >> 32 BYTES MAX
#define RX_PW_P2_4 4
#define RX_PW_P2_3 3
#define RX_PW_P2_2 2
#define RX_PW_P2_1 1
#define RX_PW_P2_0 0

#define REGISTER_RX_PW_P3 0X14         // PAYLOAD WIDTH REGISTER FOR FOURTH PIPE

//BITS

#define RX_PW_P3_5 5             // IT CAN GO FROM 0B00000001 TO 0B00010000 >> 32 BYTES MAX
#define RX_PW_P3_4 4
#define RX_PW_P3_3 3
#define RX_PW_P3_2 2
#define RX_PW_P3_1 1
#define RX_PW_P3_0 0


#define REGISTER_RX_PW_P4 0X15         // PAYLOAD WIDTH REGISTER FOR FIFTH PIPE

//BITS

#define RX_PW_P4_5 5             // IT CAN GO FROM 0B00000001 TO 0B00010000 >> 32 BYTES MAX
#define RX_PW_P4_4 4
#define RX_PW_P4_3 3
#define RX_PW_P4_2 2
#define RX_PW_P4_1 1
#define RX_PW_P4_0 0


#define REGISTER_RX_PW_P5 0X16         // PAYLOAD WIDTH REGISTER FOR SIXTH PIPE

//BITS

#define RX_PW_P5_5 5             // IT CAN GO FROM 0B00000001 TO 0B00010000 >> 32 BYTES MAX
#define RX_PW_P5_4 4
#define RX_PW_P5_3 3
#define RX_PW_P5_2 2
#define RX_PW_P5_1 1
#define RX_PW_P5_0 0



#define REGISTER_DYNPD 0X1C // DYNAMIC PAYLOAD ENABLE REGISTER

//BITS

#define DPL5 5                    // ENABLES DYNAMIC PAYLOAD WIDTH ON THE SPECIFIED RX PIPE
#define DPL4 4
#define DPL3 3
#define DPL2 2
#define DPL1 1
#define DPL0 0



#define REGISTER_FEATURE 0X1D

//BITS

#define EN_DPL 2       // ENABLES DYNAMIC PAYLOAD
#define EN_ACK_PAY 1   // ENABLES PAYLOAD WITH ACK
#define EN_DYN_ACK 0   // ENABLES W_TX_NOACK COMMAND


typedef enum {
	
	POWER_UP   = 0B1 ,
	POWER_DOWN = 0B0 , 
	
	} Module_state;


typedef enum {
	RX = 0b1 ,
	TX = 0B0 ,
	} Txrx_mode ;
	
	
typedef enum {
	
	CRC_0BYTE = 0B00 ,     // NO CRC
	CRC_1BYTE = 0B10 ,
	CRC_2BYTE = 0B11 ,
	} Crc ;
	
typedef enum {
	RX_TX_MAX_INT = 0B000,
	RX_TX_INT     = 0B001 ,
	RX_INT        = 0B011 ,
	TX_INT        = 0B101 ,
	TX_MAX_INT    = 0B100 ,
	RX_MAX_INT    = 0B010 ,
	NO_INT	      =  0B111 ,
} Int_mask ;


typedef enum {
	
	 POWER_18DBM = PWR_18DBM ,
	 POWER_12DBM = PWR_12DBM ,
	 POWER_6DBM  = PWR_6DBM  ,
	 POWER_0DBM  = PWR_0DBM  ,
	} Transmit_power ;
	
typedef enum {
	
	RATE_250KBPS = 0B100 ,
	RATE_1MBPS   = 0B000 ,
	RATE_2MBPS   = 0B001 ,
	
	} Data_rate ;	
	
typedef enum {
	
	EN_WAVE = 1 ,
	NO_WAVE = 0 , 
	
	} Cont_wave_transmit ;	
	
	
	typedef enum {
		EN_DYNPL_EN_AACK_NOCMD = 0B110 ,
		NO_DYNPL_EN_AACK_NOCMD = 0B010 ,
		EN_DYNPL_NO_AACK_NOCMD = 0B100 ,
		NO_DYNPL_NO_AACK_NOCMD = 0B000 ,
		EN_DYNPL_EN_AACK_CMD   = 0B111 ,
		NO_DYNPL_EN_AACK_CMD   = 0B011 ,
		EN_DYNPL_NO_AACK_CMD   = 0B101 ,
		NO_DYNPL_NO_AACK_CMD   = 0B001 ,
		
		
		} Feature ;
		
		
	typedef enum {
		
		PTX     =   REGISTER_TX_ADDR    , 
		PIPE0   =   REGISTER_RX_ADDR_P0 ,
		PIPE1   =   REGISTER_RX_ADDR_P1 ,
		PIPE2   =   REGISTER_RX_ADDR_P2 ,
		PIPE3   =   REGISTER_RX_ADDR_P3 ,
		PIPE4   =   REGISTER_RX_ADDR_P4 ,
		PIPE5   =   REGISTER_RX_ADDR_P5 ,
		
		} Pipe ;	
	
	
	
	




	
	
	
	
	
typedef struct {
	
	Cont_wave_transmit cont_wave_transmit ;         // CONTINOUS WAVE TRANSMISSION , 1 ON , 0 OFF
	Data_rate data_rate ;							// DATA RATE
	Transmit_power transmit_power ;					// TRANSMISSION POWER
	Int_mask int_mask ;								// IRQ PIN INT MASK
	Crc crc ;										// CRC CONFIG
	Txrx_mode txrx_mode ;							// PRIM_RX , PRIM_TX
	Module_state module_state ;						// Module State POWERUP - POWERDOWN
	Feature feature ;								// Feature Register 
	uint8_t ard ;									// Auto-retransmit delay 
	uint8_t arc ;								    // Auto-retransmit number 
	uint8_t rf_ch;                                  // RF CHANNEL 
	
	} nrf_config , *nrf_configptr ;




//Prototypes

void w_register ( uint8_t word , uint8_t addr );
uint8_t r_register ( uint8_t addr ) ;
uint8_t set_addr ( Pipe reg_addr , uint64_t addr , uint8_t addr_len );
void nrf_configure ( nrf_config config_struct ) ; 
uint8_t nrf_power_up ( void );
uint8_t nrf_power_down( void );
void nrf_tx_write ( uint8_t  * data_ptr , uint8_t num_bytes , uint8_t ack );
void nrf_rx_read ( uint8_t * ptr , uint8_t num_bytes );
uint8_t pipe_setup (Pipe reg_addr , uint64_t addr , uint8_t addr_len , uint8_t ENACK , uint8_t ENRX , uint8_t plw );








#endif /* NRF24L01_H_ */