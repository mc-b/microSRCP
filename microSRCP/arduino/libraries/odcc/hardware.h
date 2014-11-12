//----------------------------------------------------------------
//
// OpenDCC
//
// Copyright (c) 2006 Kufer
//
// This source file is subject of the GNU general public license 2,
// that is available at the world-wide-web at
// http://www.gnu.org/licenses/gpl.txt
//
//-----------------------------------------------------------------
//
// file:      hardware.h
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// history:   2007-03-27 V0.01 copied from config.h,
//            all removed but hardware accesses.
//			  2010-01-24 Angepasst an Arduinoboard mit ATmega328P, Marcel Bernet
//
//-----------------------------------------------------------------
//
// purpose:   lowcost central station for dcc
// content:   central definitions used in the project
//            all hardware project settings are done here!!
//            
//            1.   Prozessor, Timing
//                 (change here, if OpenDCC is run on a different uP)
//            2.   IOs
//                 (change here, if OpenDCC is run on a different board)
//
//-----------------------------------------------------------------
#ifndef __HARDWARE_H__
#define __HARDWARE_H__

//========================================================================
// 1. Processor Definitions
//========================================================================
//

#if (__AVR_ATmega32__ || __AVR_ATmega328P__ || __AVR_ATmega32U4__)
  // atmega32:   2kByte SRAM, 1kByte EEPROM
  #define SRAM_SIZE    2048
  #define EEPROM_SIZE  1024
  #define EEPROM_BASE  0x810000L
#endif

#if (__AVR_ATmega644__)
  // atmega644:   4kByte SRAM, 2kByte EEPROM
  #define SRAM_SIZE    4096
  #define EEPROM_SIZE  2048
  #define EEPROM_BASE  0x810000L
#endif

#if (__AVR_ATmega644P__)
  // atmega644:   4kByte SRAM, 2kByte EEPROM
  #define SRAM_SIZE    4096
  #define EEPROM_SIZE  2048
  #define EEPROM_BASE  0x810000L
#endif
#if (__AVR_ATmega1280__ || __AVR_ATmega2560__)
  // atmega644:   4kByte SRAM, 2kByte EEPROM
  #define SRAM_SIZE    8096
  #define EEPROM_SIZE  4096
  #define EEPROM_BASE  0x810000L
#endif

//========================================================================
// 2. Port Definitions
//========================================================================

// Port C
//#define SHORT_MAIN 0     // in
//#define SHORT_PROG 1     // in

#if (__AVR_ATmega328P__ || __AVR_ATmega32U4__)
	// Port B
	#define NDCC       2     // out
	#define DCC        1     // out
	#define ENA_PROG   4     // out  high = enable programming
	#define ENA_MAIN   5     // out  high = enable main
#elif (__AVR_ATmega1280__ || __AVR_ATmega2560__)
	// Port B
	#define NDCC       6     // out
	#define DCC        5     // out
	#define ENA_PROG   4    // out  high = enable programming
	#define ENA_MAIN   7     // out  high = enable main
#else // ATmega32, 644, 644P
	// Port D
	#define NDCC       4     // out
	#define DCC        5     // out
	#define ENA_PROG   6     // out  high = enable programming
	#define ENA_MAIN   7     // out  high = enable main
#endif

//------------------------------------------------------------------------
// LED-Control and IO-Macros


// this results after compile: sbi(PORTB,MY_CTS)

#if (__AVR_ATmega328P__ || __AVR_ATmega1280__ || __AVR_ATmega2560__ || __AVR_ATmega32U4__)
	#define MAIN_TRACK_ON    PORTB |= (1<<ENA_MAIN)
	#define MAIN_TRACK_OFF   PORTB &= ~(1<<ENA_MAIN)
	#define MAIN_TRACK_STATE (PINB & (1<<ENA_MAIN)))
	#define PROG_TRACK_ON    PORTB |= (1<<ENA_PROG)
	#define PROG_TRACK_OFF   PORTB &= ~(1<<ENA_PROG)
	#define PROG_TRACK_STATE (PINB & (1<<ENA_PROG))
#else
	#define MAIN_TRACK_ON    PORTD |= (1<<ENA_MAIN)
	#define MAIN_TRACK_OFF   PORTD &= ~(1<<ENA_MAIN)
	#define MAIN_TRACK_STATE (PIND & (1<<ENA_MAIN)))
	#define PROG_TRACK_ON    PORTD |= (1<<ENA_PROG)
	#define PROG_TRACK_OFF   PORTD &= ~(1<<ENA_PROG)
	#define PROG_TRACK_STATE (PIND & (1<<ENA_PROG))
#endif


// Abfragen:
//#define MAIN_IS_SHORT    (!(PINC & (1<<SHORT_MAIN)))
//#define PROG_IS_SHORT    (!(PINC & (1<<SHORT_PROG)))

void init_main(void);
void init_interrupt( void );

#endif   // hardware.h
