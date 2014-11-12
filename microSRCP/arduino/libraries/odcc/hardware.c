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

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>         // put var to program memory
#include <avr/eeprom.h>
#include <string.h>

#include "config.h"                // general structures and definitions
#include "hardware.h"
#include "parser.h"
#include "dccout.h"

#include "organizer.h"

void init_main( void )
{

#if (__AVR_ATmega328P__ || __AVR_ATmega1280__ || __AVR_ATmega2560__ || __AVR_ATmega32U4__)
	PORTB = (0 << NDCC) // out
			| (1 << DCC) // out
			| (0 << ENA_PROG) // out, disabled
			| (0 << ENA_MAIN); // out, disabled

	DDRB = (1 << NDCC) // out
			| (1 << DCC) // out
			| (1 << ENA_PROG) // out
			| (1 << ENA_MAIN); // out
#else
	// Input/Output Ports initialization
	// Port D
	PORTD = (0 << NDCC) // out
			| (1 << DCC) // out
			| (0 << ENA_PROG) // out, disabled
			| (0 << ENA_MAIN); // out, disabled

	DDRD = (1 << NDCC) // out
			| (1 << DCC) // out
			| (1 << ENA_PROG) // out
			| (1 << ENA_MAIN); // out
#endif

}

