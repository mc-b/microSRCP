//----------------------------------------------------------------
//
// OpenDCC
//
// Copyright (c) 2006, 2007 Kufer
//
// This source file is subject of the GNU general public license 2,
// that is available at the world-wide-web at
// http://www.gnu.org/licenses/gpl.txt
//
//-----------------------------------------------------------------
//
// file:      organizer.c
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// history:   2006-04-13 V0.01 started
//            2006-05-02 V0.02 bugfix in search_repeatbuffer, added queue_hp
//            2006-05-04 V0.03 bugfix in speed handling; simulation shows
//                             correct queue management.
//            2006-06-20 V0.04 added EEPROM routines to store locoformats
//            2006-09-30 V0.05 added speed converter routines
//                             func_to_locobuffer extended
//            2006-10-24 V0.06 EEPROM locations goes to own section
//            2006-10-31 V0.07 Added virtual decoder: instead of putting an
//                             accessory command to the tracks, the corresponding
//                             DMX routine is called.
//            2006-11-19 V0.08 Bugfix in search_loco_buffer; there was always DCC_idle :-(
//            2006-11-23 V0.09 added extended accessory command (only build_..)
//            2006-12-04 V0.10 added queue handling to queue_prog_is_empty();
//            2007-01-10 V0.11 corrected addr offset of 1 in build_nrma_accessory
//            2007-01-28 V0.12 read dcc_acc_repeat from eeprom
//            2007-02-04 V0.13 group mapping bei functions korrigiert und message fixed.
//            2007-03-12 V0.14 added code for POM (do_pom_....)
//                             in set_next_message now the type is evaluated and
//                             eventually the immediate repeat for prog_commands is set
//            2007-03-19 V0.15 save_turnout only if command is with activate == 1.
//            2007-03-21 V0.16 message.type is now forwarded to dccout (for feedback)
//            2008-01-07 V0.17 Direction change handled correct
//                             repeat_buffer is cleared every time a new command for
//                             a certain loco arrives
//                             read dcc_pom_repeat, dcc_func_repeat and speed from eeprom
//            2008-07-21 V0.18 addr_inquiry_locobuffer added,
//            2008-08-01 V0.19 changed: find_in_queue (now only for speed commands)
//                             organizer_halt_state added
//            2008-08-03 V0.20 unified return values
//            2009-06-23 V0.21 added build_dcc_fast_clock();
//			  2010-01-24 Angepasst an Arduinoboard mit ATmega328P, Marcel Bernet
//
// 2do:       Functions 13-28
//            function status flags
//
// tests:     2007-02-04 28 Speed Steps and light
//                       Turnouts and virtual Decoder
//                       Locobuffer
//            2007-03-15 POM, Programming Queue
//            2007-03-20 functions, 128 Steps
//--------------------------------------------------------------------------
//
// purpose:   lowcost central station for dcc (c-code for Atmel AVR)
// content:   queue for dcc messages
//            memory for actual loco states
//            
//            
// This module receives the parsed command as dcc-message, checks for the
// type of message and builds up the sequence of DCC messages
// send to DCCOUT (only for normal run mode)
//
//==============================================================================
//
// Table of Content, Chapters
//
//  1. Data and support routines
//  2. Build routines for DCC messages
//  3. Turnout control (position memory)
//  4. Routines for Locobuffer (memory for actual loco states)
//  5. Command Organizer (queues, engines for repeat and refresh of dcc messages)
//  6. Upstream Interface (to be called by parser)
//
//==============================================================================
//
// How to interface?
//
// Upstream:  init_organizer(void)  // set up the queue structures
//            run_organizer(void)   // multitask replacement, must be called
//                                  // every 2ms (approx)
//
//            bool do_locospeed(loco, speed)
//                                  // change speed of this loco (including speed converting)
//            bool do_***           // see Chapter 6 for all command entries
//            organizer_ready(void) // check, whether a command can be accepted
//
// downstream:
//            uses "next_message..." flags to interact with dccout
//
//------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>         // put var to program memory
#include <avr/eeprom.h>
#include <string.h>

#include "config.h"                // general structures and definitions
#include "parser.h"
#include "dccout.h"

#include "organizer.h" 

// mega32:   2kByte SRAM, 1kByte EEPROM
// mega644:  4kByte SRAM, 2kByte EEPROM

//------------------------------------------------------------------------
// define a structure for DCC messages
//------------------------------------------------------------------------

//-------------------------------------- programming command buffer (fifo)

t_message queue_prog[SIZE_QUEUE_PROG];

//-------------------------------------- high priority command buffer (fifo)

t_message queue_hp[SIZE_QUEUE_HP];

//-------------------------------------- low priority command buffer (fifo)

t_message queue_lp[SIZE_QUEUE_LP];

//-------------------------------------- repeat buffer

t_message repeatbuffer[SIZE_REPEATBUFFER];

struct locomem locobuffer[SIZE_LOCOBUFFER];


//-------------------------------------- memory to store turnout positions

#if (SIZE_TURNOUTBUFFER > 0)       // no of Turnouts / 8 (64 = 512 Turnouts)
unsigned char turnoutbuffer[SIZE_TURNOUTBUFFER];   
#endif
// ---------------------------------------------------------------------
// predefined messages
// stored in bss, copied at start to sram
//                       {repeat, size, type, data} 
t_message DCC_Reset    = {1,  {{ 2,  is_void}}, {0x00, 0x00}};    // DCC-Reset-Paket
t_message DCC_Idle     = {1,  {{ 2,  is_void}}, {0xFF, 0x00}};    // DCC-Idle-Paket
t_message DCC_BC_Stop  = {1,  {{ 2,  is_stop}}, {0x00, 0x71}};    // Broadcast Motor off:
                                                                    // 01DC000S :D=x, C=1 (ignore D)
t_message DCC_BC_Brake = {1,  {{ 2,  is_stop}}, {0x00, 0x70}};    // Broadcast Slow down
                                                                    // if S=0: slow down

unsigned char organizer_halt_state = 0; // if 0: no halt
                                        // if 1: send STOP (speed = 0)
                                        // if 2 send BRAKE (speed = emergency stop)

unsigned char dcc_acc_repeat;           // dcc accessory commands are repeated this time
                                        // this value is read from eeprom

unsigned char dcc_pom_repeat;           // dcc pom commands are repeated this time
                                        // this value is read from eeprom

unsigned char dcc_speed_repeat;         // dcc speed commands are repeated this time
                                        // this value is read from eeprom

unsigned char dcc_func_repeat;          // dcc func commands are repeated this time
                                        // this value is read from eeprom

t_format dcc_default_format;            // dcc default: 0=DCC14, 2=DCC28, 3=DCC128
                                        // this value is read from eeprom


//-------------------------------------------------------------------------------------------------

//--------------------------------- routines to convert speed from and to DCC14 and DCC28


unsigned char convert_speed_to_rail(unsigned char speed128, t_format format)
  {
    unsigned char retval, myspeed, direction;
    retval = speed128;             // by default
    myspeed = speed128 & 0x7F;    // mask direction
    direction = speed128 & 0x80;
    
    switch(format)
      {
        case DCC14:
            if (myspeed > 1) 
              {
                retval = (myspeed - 2) / 9 + 2;
                retval |= direction;
              } 
            break;
        case DCC27: // !!! not implemented -> same as DCC28.
        case DCC28:
            if (myspeed > 1)
              {
                retval = (myspeed - 2) * 2 / 9 + 2;
                retval |= direction;
              } 
            break;
        case DCC128:
            break;
      }
    return(retval);
  }


unsigned char convert_speed_from_rail(unsigned char speed, t_format format)
  {
    unsigned char retval, myspeed, direction;
    retval = speed;             // by default
    myspeed = speed & 0x7F;    // mask direction
    direction = speed & 0x80;

    switch(format)
      {
        case DCC14:
            if (myspeed > 1) 
              {
                retval = (myspeed - 2) * 9 + 2;
                retval |= direction;
              } 
            break;
        case DCC27: // !!! not implemented -> same as DCC28.
        case DCC28:
            if (myspeed > 1)
              {
                retval = ((myspeed - 2) * 9 + 1 )/ 2 + 2;
                retval |= direction;
              } 
            break;
        case DCC128:
            break;
      }
    return(retval);
  }

#ifdef TEST_CONVERT
void test_convert_speed(void)
  {
    int i;

    for (i=0; i<128; i++)
      {
        PORTA = convert_speed_to_rail(i, DCC28);
      }
    
    for (i=0; i<128; i++)
      {
        PORTA = convert_speed_to_rail(i, DCC14);
      }
    
    for (i=0; i<29; i++)
      {
        PORTA = convert_speed_from_rail(i, DCC28);
      }
    
    for (i=0; i<15; i++)
      {
        PORTA = convert_speed_from_rail(i, DCC14);
      }    
    PORTA = 0xF0;
  }
#endif

//=====================================================================================
//
// 2. routines to build DCC messages - see NMRA RP 9.2.1
//
//=====================================================================================

/// build short address and 14 speed steps; neg. speed = forward, pos speed = revers
/// 0AAAAAAA 01DUSSSS
///
void build_loko_7a14s(unsigned int nr, signed char speed, t_message *new_message)
  {
    // build short address and 14 speed steps; neg. speed = forward, pos speed = revers
        // 0AAAAAAA 01DUSSSS
    unsigned char mydata;

    new_message->repeat = dcc_speed_repeat;
    new_message->type = is_loco;
    new_message->size = 2;
    new_message->dcc[0] = (nr & 0x7F);
    // build up data: -> 01DUSSSS
    mydata = speed & 0x0F;
    mydata |= (speed & 0x80)>>2;
	mydata |= 0b01000000;                     // mark command
    new_message->dcc[1] = mydata;
}

/// build short address and 28 speed steps; neg. speed = forward, pos speed = revers
/// 0AAAAAAA 01DCSSSS

void build_loko_7a28s(unsigned int nr, signed char speed, t_message *new_message)
  {
    // build short address and 28 speed steps; neg. speed = forward, pos speed = revers
        // 0AAAAAAA 01DCSSSS
    unsigned char mydata;

    new_message->repeat = dcc_speed_repeat;
    new_message->type = is_loco;
    new_message->size = 2;
    new_message->dcc[0] = (nr & 0x7F);
    // build up data: -> 01DCSSSS
	if ((speed & 0x1F) == 0) mydata = 0;
	else
	  {
	    if ((speed & 0x1F) == 1) mydata = 1;    // emergency stop
		else
		  {
		    mydata = (((speed & 0x1F) + 2) >> 1) | ((speed & 0x01) << 4);    // beim LSB kein +2, ist egal
                                                                           // intern ist speed 1 = nothalt
		  }
	  }
    mydata |= (speed & 0x80)>>2;
	mydata |= 0b01000000;                     // mark command
    new_message->dcc[1] = mydata;
}

void build_loko_7a128s(unsigned int nr, signed char speed, t_message *new_message)
  {
    // message 0AAAAAAA 00111111 DSSSSSSS    ; Speed comes msb first

    new_message->repeat = dcc_speed_repeat;
    new_message->type = is_loco;
    new_message->size = 3;
    new_message->dcc[0] = (nr & 0x7F);
    new_message->dcc[1] = 0b00111111;
    // build up data: -> DSSSSSSS
    new_message->dcc[2] = speed;
  }

// Note: DCC legal for long address is 1 ... 10239
void build_loko_14a14s(unsigned int nr, signed char speed, t_message *new_message)
  {
    // build long address and 14 speed steps; neg. speed = forward, pos speed = revers
        // 0AAAAAAA 01DUSSSS
    unsigned char mydata;

    new_message->repeat = dcc_speed_repeat;
    new_message->type = is_loco;
    new_message->size = 3;
    new_message->dcc[0] = 0xC0 | ( (unsigned char)(nr / 256) & 0x3F);
    new_message->dcc[1] = (char)(nr & 0xFF);

    // build up data: -> 01DUSSSS
    mydata = speed & 0x0F;
    mydata |= (speed & 0x80)>>2;
	mydata |= 0b01000000;                     // mark command
    new_message->dcc[2] = mydata;
}


void build_loko_14a28s(int nr, int speed, t_message *new_message)
  {
    // Message: 11AAAAAA AAAAAAAA

    unsigned char mydata;

    new_message->repeat = dcc_speed_repeat;
    new_message->type = is_loco;
    new_message->size = 3;
    new_message->dcc[0] = 0xC0 | ( (unsigned char)(nr / 256) & 0x3F);
    new_message->dcc[1] = (char)(nr & 0xFF);

    // build up data: -> 01DCSSSS
	if ((speed & 0x1F) == 0) mydata = 0;
	else
	  {
	    if ((speed & 0x1F) == 1) mydata = 1;    // emergency stop
		else
		  {
		    mydata = (((speed & 0x1F) + 2) >> 1) | ((speed & 0x01) << 4);    // beim LSB kein +2, ist egal
		  }
	  }
    mydata |= (speed & 0x80)>>2;
    mydata |= 0b01000000;                     // mark command

    new_message->dcc[2] = mydata;

}

void build_loko_14a128s(int nr, int speed, t_message *new_message)
  {
    // message 11AAAAAA AAAAAAAA 00111111 DSSSSSSS    ; Speed comes msb first

    new_message->repeat = dcc_speed_repeat;
    new_message->type = is_loco;
    new_message->size = 4;
    new_message->dcc[0] = 0xC0 | ( (unsigned char)(nr / 256) & 0x3F);
    new_message->dcc[1] = (char)(nr & 0xFF);
    new_message->dcc[2] = 0b00111111;
    // build up data: -> DSSSSSSS
    new_message->dcc[3] = speed;
  }


void build_nmra_basic_accessory(unsigned int nr, char output, char activate, t_message *new_message)
  {
    // Message: 10AAAAAA 1aaaBCCC
    // parameters: nr: turnout [0000-4095]
    //             output: coil (red, green) [0,1]
    //             activate: on off, [0,1]; note: intellibox only sends on, never off :-o
    //
    // Notes:   10111111 1000BCCC is broadcast
    //          aaa is bit 7 to 9 of address, but is transmitted inverted!

    unsigned int address   = 0;     // of the decoder
    unsigned char pairnr   = 0;     // decoders have pair of outputs, range [0-3]

   // calc real address of the decoder and the pairnr of the switch

    address = ((nr) / 4) + 1;  /* valid decoder addresses: 1..1023 */
    pairnr  = (nr) % 4;             // was nr-1

    new_message->repeat = dcc_acc_repeat;
    #if (TURNOUT_FEEDBACK_ENABLED == 1)
        if (activate) new_message->type = is_acc;
        else new_message->type = is_feedback;
    #else
        new_message->type = is_acc;
    #endif
    new_message->size = 2;
    new_message->dcc[0] = 0x80 | (address & 0x3F);
    new_message->dcc[1] = 0x80 | ( ((address / 0x40) ^ 0x07) * 0x10 );    // shift down, invert, shift up
    new_message->dcc[1] = new_message->dcc[1] | ((activate & 0x01) * 0x08);   // add B
    new_message->dcc[1] = new_message->dcc[1] | (pairnr * 2) | (output & 0x01);
  }


void build_nmra_extended_accessory(unsigned int nr, char aspect, t_message *new_message)
  {
    // Message: 10AAAAAA 0aaa0AA1 000sssss
    // parameters: nr: turnout [0001-4096]
    //             aspect: signal or state to execute (5 bits)
    //
    // Notes:   10111111 10000111 000sssss is broadcast
    //          aaa is msb address, but is transmitted inverted!

    unsigned int address   = 0;     // of the decoder
    unsigned char pairnr   = 0;     // decoders have pair of outputs, range [0-3]

   // calc real address of the decoder and the pairnr of the switch

    address = ((nr-1) / 4) + 1;  /* valid decoder addresses: 1..1023 */
    pairnr  = (nr-1) % 4;

    new_message->repeat = dcc_acc_repeat;
    new_message->type = is_acc;
    new_message->size = 3;
    new_message->dcc[0] = 0x80 | (address & 0x3F);
    new_message->dcc[1] = 0x80 | ( ((address / 0x40) ^ 0x07) * 0x10 );    // shift down, invert, shift up
    new_message->dcc[1] = new_message->dcc[1] | (pairnr * 2) | (0x01);
    new_message->dcc[2] = aspect;
  }


void build_function_7a_grp1(int nr, unsigned char func, t_message *new_message)
  {
    // Message: 0AAAAAAA 100FFFFF
    // FFFFF => FL, F4, F3, F2, F1

    new_message->repeat = dcc_func_repeat;
    new_message->type = is_void;
    new_message->size = 2;
    new_message->dcc[0] = (nr & 0x7F);
    // build up data: -> 100FFFFF
    new_message->dcc[1] = 0b10000000 | (func & 0x1F);
  }

void build_function_7a_grp2(int nr, unsigned char func, t_message *new_message)
  {
    // Message: 0AAAAAAA 1011FFFF
    // FFFFF => F8, F7, F6, F5

    new_message->repeat = dcc_func_repeat;
    new_message->type = is_void;
    new_message->size = 2;
    new_message->dcc[0] = (nr & 0x7F);
    // build up data: -> 1011FFFF
    new_message->dcc[1] = 0b10110000 | (func & 0x0F);
  }

void build_function_7a_grp3(int nr, unsigned char func, t_message *new_message)
  {
    // Message: 0AAAAAAA 1010FFFF
    // FFFF => F12, F11, F10, F9

    new_message->repeat = dcc_func_repeat;
    new_message->type = is_void;
    new_message->size = 2;
    new_message->dcc[0] = (nr & 0x7F);
    // build up data: -> 1010FFFF
    new_message->dcc[1] = 0b10100000 | (func & 0x0F);
  }


void build_function_14a_grp1(int nr, unsigned char func, t_message *new_message)
  {
    // Message: 11AAAAAA AAAAAAAA 100FFFFF
    // FFFFF => FL, F4, F3, F2, F1

    new_message->repeat = dcc_func_repeat;
    new_message->type = is_void;
    new_message->size = 3;
    new_message->dcc[0] = 0xC0 | ( (unsigned char)(nr / 256) & 0x3F);
    new_message->dcc[1] = (char)(nr & 0xFF);
    // build up data: -> 100FFFFF
    new_message->dcc[2] = 0b10000000 | (func & 0x1F);
  }

void build_function_14a_grp2(int nr, unsigned char func, t_message *new_message)
  {
    // Message: 11AAAAAA AAAAAAAA 1011FFFF
    // FFFFF => F8, F7, F6, F5

    new_message->repeat = dcc_func_repeat;
    new_message->type = is_void;
    new_message->size = 3;
    new_message->dcc[0] = 0xC0 | ( (unsigned char)(nr / 256) & 0x3F);
    new_message->dcc[1] = (char)(nr & 0xFF);
    // build up data: -> 1011FFFF
    new_message->dcc[2] = 0b10110000 | (func & 0x0F);
  }

void build_function_14a_grp3(int nr, unsigned char func, t_message *new_message)
  {
    // Message: 11AAAAAA AAAAAAAA 1010FFFF
    // FFFF => F12, F11, F10, F9

    new_message->repeat = dcc_func_repeat;
    new_message->type = is_void;
    new_message->size = 3;
    new_message->dcc[0] = 0xC0 | ( (unsigned char)(nr / 256) & 0x3F);
    new_message->dcc[1] = (char)(nr & 0xFF);
    // build up data: -> 1010FFFF
    new_message->dcc[2] = 0b10100000 | (func & 0x0F);
  }


void build_pom_14a(int nr, unsigned int cv, unsigned char data, t_message *new_message)
  {
    // message 11AAAAAA AAAAAAAA 111xxxxx xxxxxxxx 
    // short form:               1111CCCC DDDDDDDD
    //                               0000 -------- unused
    //                               0010 xxxxxxxx = Acceleration Value (CV#23)
    //                               0011 xxxxxxxx = Deceleration Value (CV#24)
    //
    // long form:                1110CCAA AAAAAAAA DDDDDDDD (like in programmer)
    //                               CC=00 Reserved for future use
    //                               CC=01 Verify byte
    //                               CC=11 Write byte
    //                               CC=10 Bit manipulation
    // here: only long form, only write byte
    //
    unsigned int cv_adr;

    cv_adr = cv-1;
     
    new_message->repeat = dcc_pom_repeat;
    new_message->type = is_prog;
    new_message->size = 5;
    new_message->dcc[0] = 0xC0 | ( (unsigned char)(nr / 256) & 0x3F);
    new_message->dcc[1] = (char)(nr & 0xFF);
    // build up data: -> 1110CCAA
    new_message->dcc[2] = 0b11100000 | 0b00001100 | (unsigned char)((cv_adr >> 8) & 0b11);
    new_message->dcc[3] = (unsigned char)(cv_adr & 0xFF);
    new_message->dcc[4] = data;
  }


void build_pom_7a(int nr, unsigned int cv, unsigned char data, t_message *new_message)
  {
    // message 0AAAAAAA 111xxxxx xxxxxxxx 
    // short form:      1111CCCC DDDDDDDD
    //                  0000 -------- unused
    //                  0010 xxxxxxxx = Acceleration Value (CV#23)
    //                  0011 xxxxxxxx = Deceleration Value (CV#24)
    //
    // long form:       1110CCAA AAAAAAAA DDDDDDDD (like in programmer)
    //                      CC=00 Reserved for future use
    //                      CC=01 Verify byte
    //                      CC=11 Write byte
    //                      CC=10 Bit manipulation
    // here: only long form, only write byte
    //
    unsigned int cv_adr;

    cv_adr = cv-1;
     
    new_message->repeat = dcc_pom_repeat;
    new_message->type = is_prog;
    new_message->size = 4;
    new_message->dcc[0] = (nr & 0x7F);
    // build up data: -> 1110CCAA
    new_message->dcc[1] = 0b11100000 | 0b00001100 | (unsigned char)((cv_adr >> 8) & 0b11);
    new_message->dcc[2] = (unsigned char)(cv_adr & 0xFF);
    new_message->dcc[3] = data;
  }

void build_pom_accessory(int nr, unsigned int cv, unsigned char data, t_message *new_message)
  {
    // message 10AAAAAA 1aaaCDDD 111xxxxx xxxxxxxx 
    //                      0000= whole decoder  1xxx -> output xxx
    // long form:                1110CCAA AAAAAAAA DDDDDDDD (like in programmer)
    //                               CC=00 Reserved for future use
    //                               CC=01 Verify byte
    //                               CC=11 Write byte
    //                               CC=10 Bit manipulation
    // here: only long form, only write byte
    //
    unsigned int cv_adr;

    cv_adr = cv-1;
     
    new_message->repeat = dcc_pom_repeat;
    new_message->type = is_prog;
    new_message->size = 5;

    new_message->dcc[0] = 0x80 | (nr & 0x3F);
    new_message->dcc[1] = 0x80 | ( ((nr / 0x40) ^ 0x07) * 0x10 );    // shift down, invert, shift up
    // build up data: -> 1110CCAA
    new_message->dcc[2] = 0b01110000 | 0b00001100 | (unsigned char)((cv_adr >> 8) & 0b11);
    new_message->dcc[3] = (unsigned char)(cv_adr & 0xFF);
    new_message->dcc[4] = data;
  }

#if (DCC_FAST_CLOCK == 1)
void build_dcc_fast_clock(t_fast_clock *my_clock, t_message *new_message)
  {
    // message 00000000 11000001 [TCODE0] {[TCODE1] [TCODE2] [TCODE3]}
    // TCODE: CC = 00:	DDDDDD = mmmmmm, this denotes the minute, range 0..59.
    // TCODE: CC = 10:	DDDDDD = 0HHHHHH, this denotes the hour, range 0..23
    // TCODE: CC = 01: 	DDDDDD = 000WWW, this denotes the day of week, 
    //                               0=Monday, 1=Tuesday, 2=Wednesday, 3=Thursday, 4=Friday, 5=Saturday, 6=Sunday.
    // TCODE: CC = 11:	DDDDDD = 00FFFFF, this denotes the acceleration factor, range 0..31; 
    //

    new_message->repeat = 0;
    new_message->type = is_void;
    new_message->size = 6;
    #if (6 > MAX_DCC_SIZE)
      #error wrong config, MAX_DCC_SIZE too small
    #endif


    new_message->dcc[0] = 0;
    new_message->dcc[1] = 0xC1;
    new_message->dcc[2] = 0x00 | my_clock->minute;
    new_message->dcc[3] = 0x80 | my_clock->hour;
    new_message->dcc[4] = 0x40 | my_clock->day_of_week;
    new_message->dcc[5] = 0xC0 | my_clock->ratio;
  }
#endif

//============================================================================
//
// 3. Routines for turnoutbuffer
//
//============================================================================
//
// purpose:   stores and reload positions of turnouts
//
// note:      if memory is limited, only the turnouts in this memory are stored
//            and recovered, all other are reported as 0 (=green).
//
// addressing:   Turnout: 1 .... 8, 9 .... 16, ...
//               Group:   |--1---|, |--2----|, ...
//               addr:    0 .... 7

#if (SIZE_TURNOUTBUFFER > 0) 

void save_turnout(unsigned int addr, unsigned char output)
  {
    unsigned int index;
    unsigned char mask;

    index = (addr) / 8;
    mask = (unsigned char) (addr) & 0x7;
    mask = 1 << mask;
    if (index < SIZE_TURNOUTBUFFER)
      {  
        if (output)
            turnoutbuffer[index] |= mask;
        else
            turnoutbuffer[index] &= ~mask;
      }
  }

unsigned char recall_turnout(unsigned int addr)
  {
    unsigned int index;
    unsigned char mask;

    index = addr / 8;
    mask = (unsigned char) (addr) & 0x7;
    mask = 1 << mask;

    if (index < SIZE_TURNOUTBUFFER)
      {
        if ((turnoutbuffer[index] & mask) == 0) return(0);
        else return(1);
      }
    return(0);  // as default, if buffer is not large enough
  }

unsigned char recall_turnout_group(unsigned int group_addr)
  {
    unsigned int index;
    
    index = group_addr - 1;

    if (index < SIZE_TURNOUTBUFFER)
      {
        return(turnoutbuffer[index]);
      }
    return(0);  // as default, if buffer is not large enough
  }
 
#else  // SIZE_TURNOUTBUFFER

void save_turnout(unsigned int addr, unsigned char output)
  {
  }

unsigned char recall_turnout(unsigned int addr)
  {
    return(0);
  }

unsigned char recall_turnout_group(unsigned int group_addr)
  {
    return(0);
  }

#endif // SIZE_TURNOUTBUFFER


//============================================================================
//
// 4. Routines for locobuffer
//
//============================================================================
//
// purpose:   creates a flexible refresh of loco speeds
//
// how:       every speed command is entered to locobuffer.
//            search_locobuffer return a message of the loco
//            to be refreshed.
//            "younger" locos are refreshed more often.
//
// interface: set_loco_mode (int addr, format) -> this is done automatically
//            enter_loco (int addr, char speed)
//            init_locobuffer ()
//            get_loco_format(unsigned int addr)
//            store_loco_format(unsigned int addr, t_format format)
//
//------------------------------------------------------------------------
// LOCO FORMATS are stored in EEPROM
//------------------------------------------------------------------------
//
// The default format of OpenDCC is defined in config.h (DCC_DEFAULT_FORMAT)
// and stored in EEPROM (CV24)
//
// Rules: if a loco runs with the default format, it is not stored.
//        if a loco runs at a different format, the address and the format
//        is stored in EEPROM as 16 bit value, which is defined as follows:
//        - upper 2 bits contain the loco format;
//        - lower 14 bits contain the loco address.
//        - if entry == 0x0000, the entry is void.
//  
// 
// Up to 64 locos may have different format (ESIZE_LOCO_FORMAT)

// We allocate this storage to a fixed address to have access from the
// parser with a constant offset to eeprom
// (parser commands: read and write Special Option)
//

// EEPROM Interface
// EEPROM byteweise lesen -> geht schneller
//
/// get_loco_format returns the stored loco format, if loco was never
//  used with a format different from default it is not stored.
//
t_format get_loco_format(unsigned int addr)
  {
    return(dcc_default_format);     // not found - default format
  }

//
// see also: http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial#EEPROM
// is it default format?
// -> no:   search loco, if found: replace it
//          if not found: search empty and store it
//          if no empty found: error too many locos with extra format -> unhandled - Code 0
// -> yes:  search loco, if found: clear entry


unsigned char store_loco_format(unsigned int addr, t_format format)
  {
    return(1);
  }

// local static var to locobuffer

unsigned char cur_i;             // this locobuffer entry is currently used
unsigned char cur_ref_level;     // level = 0

unsigned char lb_index;

t_message loco_search;
t_message *loco_search_ptr;

t_message locobuff_mes;
t_message *locobuff_mes_ptr;


void init_locobuffer(void)
  {
    unsigned char j;

    cur_i = 0;                                           // refresh index
	cur_ref_level = 0;
    loco_search_ptr = &loco_search;
    locobuff_mes_ptr = &locobuff_mes;

    for (j=0; j<SIZE_LOCOBUFFER; j++)
      {
        locobuffer[j].address = 0;
      }
  }

unsigned char last_locobuffer_index(void)
  {
    return lb_index;
  }


//-----------------------------------------------------------------------------------
// neue Addr, neue Speed und neues Format in Locobuffer eintragen
// return:  char: Bit 0 (LB_SLOW_DOWN)  1 falls neue Speed kleiner alte Speed
//                                      1 falls Richtungswechsel
//                                      0 falls neue Speed >= alte Speed.
//                Bit 1 (LB_STOLEN)     1 Falls owner changed
// damit kann der Caller den Befehl zum Bremsen in einer anderen queue ablegen.

unsigned char enter_speed_f_to_locobuffer(unsigned int addr, unsigned char speed, t_format format) 
  {
    unsigned char i, found_i, found_r;
    unsigned char retval = 0;

    // find same entry
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].address == addr)
          {
            // replace it

            locobuffer[i].address = addr;
            if (locobuffer[i].format != format)           // got new format -> store it
              {
                store_loco_format(addr, format);          // !!! unhandled, if store fails!
              }
            locobuffer[i].refresh = 0;
            if ((speed ^ locobuffer[i].speed) & 0x80) retval = (1 << ORGZ_SLOW_DOWN);      // dir changed
            if ((speed & 0x7F) < (locobuffer[i].speed & 0x7F)) retval = (1 << ORGZ_SLOW_DOWN);   // brake
            
            locobuffer[i].speed = speed;
            lb_index = i;
            return(retval);
         }
      }
    // does not yet exist -> find either empty entry or replace oldest one
    // find an empty entry
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].address == 0)
          {
            // fill it
            locobuffer[i].address = addr;
            locobuffer[i].refresh = 0;
            locobuffer[i].format = format;
            store_loco_format(addr, format);          // !!! unhandled, if store fails!
            locobuffer[i].speed = speed;            
            lb_index = i;
            return(retval);
          }
      }
    // find oldest entry
    found_i = 0; found_r = 0;
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].refresh > found_r)
          {
            found_i = i; found_r = locobuffer[i].refresh;
          }
      }
    locobuffer[found_i].address = addr;
    locobuffer[found_i].refresh = 0;
    locobuffer[found_i].format = format;
    store_loco_format(addr, format);         // !!! unhandled, if store fails!
    locobuffer[found_i].speed = speed;
    lb_index = found_i;
    retval = (1 << ORGZ_STOLEN);
    return(retval);
  }


//-----------------------------------------------------------------------------------
// neue Addr, neue Speed in Locobuffer eintragen (format bleibt erhalten)
// return:  Integer: low byte:  Index, wo eingetragen wurde
//                   high byte: 1 falls neue Speed kleiner alte Speed
//                              0 falls neue Speed >= alte Speed.
// damit kann der Caller den Befehl zum Bremsen in einer anderen queue ablegen.

unsigned char enter_speed_to_locobuffer(unsigned int addr, unsigned char speed)    // returns index where entered
  {
    unsigned char i, found_i, found_r;
    unsigned char retval = 0;

    // find same entry
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].address == addr)
          {
            // replace it

            locobuffer[i].address = addr;
            locobuffer[i].refresh = 0;
            
            if ((speed ^ locobuffer[i].speed) & 0x80) retval = (1 << ORGZ_SLOW_DOWN);  // dir changed
            if ((speed & 0x7F) < (locobuffer[i].speed & 0x7F))  retval = (1 << ORGZ_SLOW_DOWN);   // brake
            
            locobuffer[i].speed = speed;
            lb_index = i;
            return(retval);
         }
      }
    // does not yet exist -> find either empty entry or replace oldest one
    // we noramlly use DCC_DEFAULT_FORMAT (see get_loco_format())
    // find an empty entry
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].address == 0)
          {
            // fill it
            locobuffer[i].address = addr;
            locobuffer[i].refresh = 0;
            locobuffer[i].format = get_loco_format(addr);
            locobuffer[i].speed = speed;
            lb_index = i;
            return(0);
          }
      }
    // find oldest entry
    found_i = 0; found_r = 0;
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].refresh > found_r)
          {
            found_i = i; found_r = locobuffer[i].refresh;
          }
      }
    locobuffer[found_i].address = addr;
    locobuffer[found_i].refresh = 0;
    locobuffer[found_i].format = get_loco_format(addr);
    locobuffer[found_i].speed = speed;
    lb_index = found_i;
    retval = (1 << ORGZ_STOLEN);
    return(retval);
  }


//-----------------------------------------------------------------------------------
// neue Addr und Function in Locobuffer eintragen
// addr: loco address
// funct: 4 bits (lsbs) of function
// grp:   0 = light (only one bit, lsb)
//        1 = f1 - f4
//        2 = f5 - f8
//        3 = f9 - f12
// return:  byte:  Errorcode - (stolen...)

unsigned char enter_func_to_locobuffer(unsigned int addr, unsigned char funct, unsigned char grp)
  {
    unsigned char i, found_i, found_r;
    // find same entry
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].address == addr)
          {
            // replace it
            locobuffer[i].address = addr;
            if (grp == 0)
              {
                locobuffer[i].fl = funct & 0x01 ;
              }
            if (grp == 1)
              {
                locobuffer[i].f4_f1 = funct & 0x0F;           
              }
            if (grp == 2)
              {
                locobuffer[i].f8_f5 = funct & 0x0F;
              }
            if (grp == 3)
              {
                locobuffer[i].f12_f9 = funct & 0x0F;
              }
            lb_index = i;
            return(0);
         }
      }
    // find an empty entry
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].address == 0)
          {
            // fill it
            locobuffer[i].address = addr;
            locobuffer[i].refresh = 0;
            locobuffer[i].speed = 0;
            locobuffer[i].format = get_loco_format(addr);
            if (grp == 0)
              {
                locobuffer[i].fl = funct & 0x01;
              }
            if (grp == 1)
              {
                locobuffer[i].f4_f1 = funct & 0x0F;
              }
            if (grp == 2)
              {
                locobuffer[i].f8_f5 = funct & 0x0F;
              }
            if (grp == 3)
              {
                locobuffer[i].f12_f9 = funct & 0x0F;
              }
            lb_index = i;
            return(0);
          }
      }
    // find oldest entry
    found_i = 0; found_r = 0;
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].refresh > found_r)
          {
            found_i = i; found_r = locobuffer[i].refresh;
          }
      }
    locobuffer[found_i].address = addr;
    locobuffer[found_i].refresh = 0;
    locobuffer[found_i].speed = 0;
    locobuffer[found_i].format = get_loco_format(addr);
    
    if (grp == 0)
      {
        locobuffer[found_i].fl = funct & 0x01;
      }
    if (grp == 1)
      {
        locobuffer[found_i].f4_f1 = funct & 0x0F;
      }
    if (grp == 2)
      {
        locobuffer[found_i].f8_f5 = funct & 0x0F;
      }
    if (grp == 3)
      {
        locobuffer[found_i].f12_f9 = funct & 0x0F;
      }
    lb_index = found_i;
    return(1 << ORGZ_STOLEN);
  }


// scan locobuffer and return the next address; 0 if not found
// note: we do not return the locobuffer directly, but we sort the answer
//       according to the loco address.
//       dir=1: scan forward - returns the next higher loco addr;
//              if there is not higher addr - return the same.
//       dir=0: scan backward - returns the next lower.

unsigned int addr_inquiry_locobuffer(unsigned int addr, unsigned char dir)    //
  {
    unsigned char i;
    unsigned int next_addr;

    if (dir)
      {                                 // forward
        next_addr = 0xffff;    
        for (i=0; i<SIZE_LOCOBUFFER; i++)
          {
            if (locobuffer[i].address > addr)
              {
                if (locobuffer[i].address < next_addr)
                  {
                     next_addr = locobuffer[i].address;
                  }
              }
          }
        if (next_addr == 0xffff) next_addr = 0;
      }
    else
      {                                // revers
        next_addr = 0;    
        for (i=0; i<SIZE_LOCOBUFFER; i++)
          {
            if (locobuffer[i].address < addr)
              {
                if (locobuffer[i].address > next_addr)
                  {
                     next_addr = locobuffer[i].address;
                  }
              }
          }
        if (next_addr == 0) next_addr = 0;
      }
    return(next_addr);
  }


void delete_from_locobuffer(unsigned int addr)    
  {
    unsigned char i;
    
    for (i=0; i<SIZE_LOCOBUFFER; i++)
      {
        if (locobuffer[i].address == addr)
          {
            locobuffer[i].address = 0;
          }
      }
  }

//--------------------------------------------------------------------------------------------
// This routine creates DCC messages from the data stored in locobuffer
// and returns a pointer to the message biult
//
// Notes: Address handling: 1...DCC_SHORT_ADDR_LIMIT:          DCC short address
//                          DCC_SHORT_ADDR_LIMIT+1 ... 10239: long address


// Note on speed handling:

t_message * build_speed_message_from_locobuffer(unsigned char i)
  {
    unsigned char format, speed;

    loco_search_ptr = &DCC_Idle;  // default = idle
    format = locobuffer[i].format;
    speed = convert_speed_to_rail(locobuffer[i].speed, format);

    switch(format)
      {
        case DCC128:
            if (locobuffer[i].address > DCC_SHORT_ADDR_LIMIT)
              {
                build_loko_14a128s(locobuffer[i].address, speed, locobuff_mes_ptr);
              }
            else
              {
                build_loko_7a128s(locobuffer[i].address, speed, locobuff_mes_ptr);
              }
            return (locobuff_mes_ptr);
            break;
        case DCC27: // Implmentierungslücke: DCC27 wird nicht unterstützt !!! dann halt idle ...
        case DCC28:
            if (locobuffer[i].address > DCC_SHORT_ADDR_LIMIT)
               {
                 build_loko_14a28s(locobuffer[i].address, speed, locobuff_mes_ptr);
               }
            else
               {
                 build_loko_7a28s(locobuffer[i].address, speed, locobuff_mes_ptr);
               }
            return (locobuff_mes_ptr);
            break;
        case DCC14:
            if (locobuffer[i].address > DCC_SHORT_ADDR_LIMIT)
              {
                build_loko_14a14s(locobuffer[i].address, speed, locobuff_mes_ptr);
                // bei DCC14 muss noch das Lichtbit in den Befehl gemogelt werden (so ein Rucksack...)
                if (locobuffer[i].fl)
                  {
                    locobuff_mes_ptr->dcc[2] |= 0x10;
                  }
              }
            else
              {
                build_loko_7a14s(locobuffer[i].address, speed, locobuff_mes_ptr);
                // bei DCC14 muss noch das Lichtbit in den Befehl gemogelt werden (so ein Rucksack...)
                if (locobuffer[i].fl)
                  {
                    locobuff_mes_ptr->dcc[1] |= 0x10;
                  }
              }
            return (locobuff_mes_ptr);
            break;
      }
     return (loco_search_ptr);
  }


t_message * build_f1_message_from_locobuffer(unsigned char i)
  {
    if (locobuffer[i].address > DCC_SHORT_ADDR_LIMIT)
      {
        build_function_14a_grp1(locobuffer[i].address, locobuffer[i].fl<<4 | locobuffer[i].f4_f1, locobuff_mes_ptr);
      }
    else
      {
        build_function_7a_grp1(locobuffer[i].address, locobuffer[i].fl<<4 | locobuffer[i].f4_f1, locobuff_mes_ptr);
      }
    return (locobuff_mes_ptr);
  }

t_message * build_f2_message_from_locobuffer(unsigned char i)
  {
    if (locobuffer[i].address > DCC_SHORT_ADDR_LIMIT)

      {
        build_function_14a_grp2(locobuffer[i].address, locobuffer[i].f8_f5, locobuff_mes_ptr);
      }
    else
      {
        build_function_7a_grp2(locobuffer[i].address, locobuffer[i].f8_f5, locobuff_mes_ptr);
      }
    return (locobuff_mes_ptr);
  }

t_message * build_f3_message_from_locobuffer(unsigned char i)
  {
    if (locobuffer[i].address > DCC_SHORT_ADDR_LIMIT)
      {
        build_function_14a_grp3(locobuffer[i].address, locobuffer[i].f12_f9, locobuff_mes_ptr);
      }
    else
      {
        build_function_7a_grp3(locobuffer[i].address, locobuffer[i].f12_f9, locobuff_mes_ptr);
      }
    return (locobuff_mes_ptr);
  }



void incr_cur_i (void)    // hilfsroutine
  {
    unsigned char j; unsigned char temp;
    cur_i += 1;
    if (cur_i == SIZE_LOCOBUFFER)
      {
        cur_i = 0;         // next run
        cur_ref_level++;   // with higher level
        if (cur_ref_level == 10)
          {
            // level has reached top: now fade out all refresh levels.
            cur_ref_level = 0;
            for (j=0; j<SIZE_LOCOBUFFER; j++)
              {
                temp = locobuffer[j].refresh + 1;         // nicht elegant, aber schnell
                if (temp > 100) temp = 100;
                locobuffer[j].refresh = temp;
              }
          }
      }

  }

///-----------------------------------------------------------------------------------
// search_locobuffer returns pointer to dcc message
// The search result depends on the current searchlevel:
// level 0: refresh only new locos -> speed
// level 1: refresh only new and fresh messages
// level 2..3: refresh new, fresh and older
// level 4: refresh all locos -> speed
// level 5: refresh all locos -> func grp 1
// level 6: refresh all locos -> speed
// level 7: refresh all locos -> func grp 2
// level 8: refresh all locos -> speed
// level 9: refresh all locos -> func grp 3
//
//
t_message * search_locobuffer(void)    
  {
    loco_search_ptr = &loco_search;
    while(1)
      {
        incr_cur_i();                          // current index ++

        if (locobuffer[cur_i].address != 0)
		  {
     		switch (cur_ref_level)
	    	  {
		        case 0:
				case 1:
				case 2:
				case 3:
				    if (locobuffer[cur_i].refresh <= cur_ref_level)
	                  {
                        // target found
                        return(build_speed_message_from_locobuffer(cur_i));
                      }
                    break;
				case 4:
				    return(build_speed_message_from_locobuffer(cur_i));
                    break;
				case 5:
				    if ((locobuffer[cur_i].fl != 0) || (locobuffer[cur_i].f4_f1 != 0))
					  {
                        // target found
                        return(build_f1_message_from_locobuffer(cur_i));
                      }
                    break;
				case 6:
				    return(build_speed_message_from_locobuffer(cur_i));
                    break;
				case 7:
				    if (locobuffer[cur_i].f8_f5 != 0)
					  {
                        // target found
                        return(build_f2_message_from_locobuffer(cur_i));
                      }
                    break;
				case 8:
				    return(build_speed_message_from_locobuffer(cur_i));
                    break;
				case 9:
				    if (locobuffer[cur_i].f12_f9 != 0)
					  {
                        // target found
                        return(build_f3_message_from_locobuffer(cur_i));
                      }
                    break;
		      }
          }
        if (cur_i == 0)
          {
            loco_search_ptr = &DCC_Idle;
            return (loco_search_ptr);           // completed whole level in locobuffer, nothing found
          }                                     // emergency return;
      }
    loco_search_ptr = &DCC_Idle;
    return (loco_search_ptr);                   // void
  }






//==========================================================================================
//
// 5. COMMAND_ORGANIZER
//
//==========================================================================================
//
// purpose:   scans the queues and buffer and determines which command is the next
//
// how:       there are 5 queues/buffers:
//              queue_hp: new messages, high priority
//              queue_lp: new messages, low priority
//              queue_prog: this queue is only used during programming;
//              repeatbuffer: messages to be repeated
//              locobuffer: messages to be refreshed (like speed messages)
//
//            every new command is put to queue_lp (and if necessary to queue_hp),
//            if queue_lp is full, an error will be created.
//            If the command already exists in any of the queues,
//            the existing command is replaced.
//            If the new command is a loco command, it will be also stored in
//            locobuffer.
//            when a new signal can be accepted by DCC_OUT, the next command
//            is selected according the following rules:
//
//            if (queue_hp not empty) && (message(queue_hp) != previous_message)
//                 next_message = read(queue_hp)
//            elseif (queue_lp not empty) && (message(queue_lp) != previous_message)
//                 next_message = read(queue_lp)
//                 update_repeatbuffer(next_message)
//            elseif (repeatbuffer not empty) && (message(repeatbuffer) != previous_message)
//                 next_message = read(repeatbuffer)
//            elseif (message(locobuffer) != previous_message)
//                 next_message = read(locobuffer)
//            else
//                 next_message = idle_message
//
//            update_repeatbuffer(next_message):
//               search for speed command to same addr; if found replace;
//               search for message with lowest repeat count and replace
//
//
//             exceptions: broadcast and stop; these command replace the
//             current next_message immediately.
//

unsigned char prog_read;          // points to next read
unsigned char prog_write;         // points to next write
                                  // rd = wr: queue empty
                                  // rd = wr + 1: queue full

unsigned char hp_read;            // points to next read
unsigned char hp_write;           // points to next write
                                  // rd = wr: queue empty
                                  // rd = wr + 1: queue full

unsigned char lp_read;            // points to next read
unsigned char lp_write;           // points to next write
                                  // rd = wr: queue empty
                                  // rd = wr + 1: queue full
// !!! unsigned char repeat_filled;

void init_organizer(void)
  {
    unsigned char i;
    hp_read = 0;
    hp_write = 0;
    lp_read = 0;
    lp_write = 0;
    organizer_halt_state = 0;

    for (i=0; i<SIZE_REPEATBUFFER; i++)
      {
        repeatbuffer[i].repeat = 0;
        repeatbuffer[i].type   = is_void;
      }

    init_locobuffer();

    dcc_default_format = DCC_DEFAULT_FORMAT;

    dcc_acc_repeat = NUM_DCC_ACC_REPEAT;
    dcc_pom_repeat = NUM_DCC_POM_REPEAT;
    dcc_func_repeat = NUM_DCC_FUNC_REPEAT;
    dcc_speed_repeat = NUM_DCC_SPEED_REPEAT;

  }


// return TRUE if found and replaced, return false, if not found
bool find_in_queue_lp(t_message *new_message)
  {
    unsigned char my_i;

    my_i = lp_read;
    while (my_i != lp_write)
      {
        if (queue_lp[my_i].dcc[0] == new_message->dcc[0])
          {
            // same adr found, is it short / long? and is it speed command?
            // if yes, replace it and return
            if (queue_lp[my_i].dcc[0] < 112)                // short adr.
              {
                if ( (queue_lp[my_i].dcc[1] == 0x3F)  &&   // (128 Speed Steps)
                     (new_message->dcc[1] == 0x3F) ) 
                  {
                    queue_lp[my_i].dcc[2] = new_message->dcc[2]; 
                    return(1);
                  }
                else if ( ((queue_lp[my_i].dcc[1] & 0x40) == 0x40) &&   // (28 Speed Steps)
                          ((new_message->dcc[1] & 0x40) == 0x40) )
                  {
                    queue_lp[my_i].dcc[1] = new_message->dcc[1];
                    return(1);
                  }
              }
            else if ( (queue_lp[my_i].dcc[0] >= 192) &&
                     (queue_lp[my_i].dcc[0] < 232)  )
              {
                if (queue_lp[my_i].dcc[1] == new_message->dcc[1])  // long addr
                  {
                    if ( (queue_lp[my_i].dcc[2] == 0x3F)  &&   // (128 Speed Steps)
                         (new_message->dcc[2] == 0x3F) ) 
                      {
                        queue_lp[my_i].dcc[3] = new_message->dcc[3]; 
                        return(1);
                      }
                    else if ( ((queue_lp[my_i].dcc[2] & 0x40) == 0x40) &&   // (28 Speed Steps)
                              ((new_message->dcc[2] & 0x40) == 0x40) )
                      {
                        queue_lp[my_i].dcc[2] = new_message->dcc[2];
                        return(1);
                      }
                  }
              }
          }
        my_i++;
        if (my_i == SIZE_QUEUE_LP) my_i = 0;
      }
    return(0);
  }

// put this message in the queues, returns 1 if there is still space
// if message is writen despite fifo full - a total flush will occur!
// so be sure to ask organizer_ready() before.

bool put_in_queue_lp(t_message *new_message)
  {
    unsigned char i;
    // check for same message in queue_lp
    // scan queue_lp from read_i up to write_i
    if (find_in_queue_lp(new_message)) return(0);

    // now feed in queue_lp
    // queue_lp[lp_write].repeat = new_message->repeat;
    // queue_lp[lp_write].qualifier = new_message->qualifier;   // both type and size
    // memcpy(queue_lp[lp_write].dcc, new_message->dcc, new_message->size);
    memcpy(&queue_lp[lp_write], new_message, sizeof(t_message));


    lp_write++;
    if (lp_write == SIZE_QUEUE_LP) lp_write = 0;

    // check for full (with reserved entry)
    i = lp_write+1;
    if (i == SIZE_QUEUE_LP) i = 0;
    if (i == lp_read) return(1 << ORGZ_FULL);         // totally full
    i = i+1;
    if (i == SIZE_QUEUE_LP) i = 0;
    if (i == lp_read) return(1 << ORGZ_FULL);         // one left -> say full, keep one extra

    return(0);
  }

// return TRUE if found, return false, if not found
/*
bool old_code_to_be_deleted_find_in_queue_hp(t_message *new_message)
  {
    unsigned char my_i;

    my_i = hp_read;
    while (my_i != hp_write)
      {
        if (queue_hp[my_i].dcc[0] == new_message->dcc[0])
          {
            // same adr found, is it short / long? and is it speed command?
            // if yes, replace it and return
            if (((queue_hp[my_i].dcc[0] & 0xC0) == 0xC0)  &&  // long adr.
                (queue_hp[my_i].dcc[1] == new_message->dcc[1])  &&
                ((queue_hp[my_i].dcc[2] & 0x40) == 0x40) )
              {
                 // long adr and speed command
                 // queue_hp[my_i].repeat = new_message->repeat;
                 // queue_hp[my_i].qualifier = new_message->qualifier;
                 // memcpy(queue_hp[my_i].dcc, new_message->dcc, new_message->size);
                 memcpy(&queue_hp[my_i], new_message, sizeof(t_message));
                 return(1);
              }
            if (((queue_hp[my_i].dcc[0] & 0x80) == 0x00)  &&  // short adr.
                ((queue_hp[my_i].dcc[1] & 0x40) == 0x40) )
              {
                 // short adr and speed command
                 // queue_hp[my_i].repeat = new_message->repeat;
                 // queue_hp[my_i].qualifier = new_message->qualifier;
                 // memcpy(queue_hp[my_i].dcc, new_message->dcc, new_message->size);
                 memcpy(&queue_hp[my_i], new_message, sizeof(t_message));
                 return(1);
              }
          }
        my_i++;
        if (my_i == SIZE_QUEUE_HP) my_i = 0;
      }
    return(0);
  } */


// return TRUE if found and replaced, return false, if not found
bool find_in_queue_hp(t_message *new_message)
  {
    unsigned char my_i;

    my_i = hp_read;
    while (my_i != hp_write)
      {
        if (queue_hp[my_i].dcc[0] == new_message->dcc[0])
          {
            // same adr found, is it short / long? and is it speed command?
            // if yes, replace it and return
            if (queue_hp[my_i].dcc[0] < 112)                // short adr.
              {
                if ( (queue_hp[my_i].dcc[1] == 0x3F)  &&   // (128 Speed Steps)
                     (new_message->dcc[1] == 0x3F) ) 
                  {
                    queue_hp[my_i].dcc[2] = new_message->dcc[2]; 
                    return(1);
                  }
                else if ( ((queue_hp[my_i].dcc[1] & 0x40) == 0x40) &&   // (28 Speed Steps)
                          ((new_message->dcc[1] & 0x40) == 0x40) )
                  {
                    queue_hp[my_i].dcc[1] = new_message->dcc[1];
                    return(1);
                  }
              }
            else if ( (queue_hp[my_i].dcc[0] >= 192) &&
                     (queue_hp[my_i].dcc[0] < 232)  )
              {
                if (queue_hp[my_i].dcc[1] == new_message->dcc[1])  // long addr
                  {
                    if ( (queue_hp[my_i].dcc[2] == 0x3F)  &&   // (128 Speed Steps)
                         (new_message->dcc[2] == 0x3F) ) 
                      {
                        queue_hp[my_i].dcc[3] = new_message->dcc[3]; 
                        return(1);
                      }
                    else if ( ((queue_hp[my_i].dcc[2] & 0x40) == 0x40) &&   // (28 Speed Steps)
                              ((new_message->dcc[2] & 0x40) == 0x40) )
                      {
                        queue_hp[my_i].dcc[2] = new_message->dcc[2];
                        return(1);
                      }
                  }
              }
          }
        my_i++;
        if (my_i == SIZE_QUEUE_HP) my_i = 0;
      }
    return(0);
  }

// put this message in the queues, returns error if full
// bei write trotz full gibt es einen gnadenlosen fifo-flush,
// also vorher organizer_ready() anfragen.

unsigned char put_in_queue_hp(t_message *new_message)
  {
    unsigned char i;
    // check for same message in queue_hp
    // scan queue_hp from read_i up to write_i

    if (find_in_queue_hp(new_message)) return(0);

    // now feed in queue hp (high priority)
    // queue_hp[hp_write].repeat = new_message->repeat;
    // queue_hp[hp_write].qualifier = new_message->qualifier;
    // memcpy(queue_hp[hp_write].dcc, new_message->dcc, new_message->size);
    memcpy(&queue_hp[hp_write], new_message, sizeof(t_message));

    hp_write++;
    if (hp_write == SIZE_QUEUE_HP) hp_write = 0;

    // check for full (with reserved entry)
    i = hp_write + 1;
    if (i == SIZE_QUEUE_HP) i = 0;
    if (i == hp_read) return(1<<ORGZ_FULL);         // totally full
    i = i + 1;
    if (i == SIZE_QUEUE_HP) i = 0;
    if (i == hp_read) return(1<<ORGZ_FULL);         // one left -> say full, keep one extra

    return(0);
  }


// Ersatz für Organizer (temporär, später soll das mit run_organizer gemacht werden,
// um Tastendruck und Kurzschlußüberwachung zu haben!!!!!
bool queue_prog_is_empty(void)
  {
    if (prog_write == prog_read) return(1);
	else 
      {
        run_organizer();
      }
    return(0);
  }



// put this message in the queues, returns error if full
// bei write trotz full gibt es einen gnadenlosen fifo-flush,
// also vorher organizer_ready() anfragen.

unsigned char put_in_queue_prog(t_message *new_message)
  {
    unsigned char i;

    // now feed in queue_prog
    // queue_prog[prog_write].repeat = new_message->repeat;
    // queue_prog[prog_write].qualifier = new_message->qualifier;
    // memcpy(queue_prog[prog_write].dcc, new_message->dcc, new_message->size);
    memcpy(&queue_prog[prog_write], new_message, sizeof(t_message));

    prog_write++;
    if (prog_write == SIZE_QUEUE_PROG) prog_write = 0;

    // check for full (with reserved entry)
    i = prog_write + 1;
    if (i == SIZE_QUEUE_PROG) i = 0;
    if (i == prog_read) return(1 << ORGZ_FULL);         // totally full
    i = i + 1;
    if (i == SIZE_QUEUE_PROG) i = 0;
    if (i == prog_read) return(1 << ORGZ_FULL);         // one left -> say full, keep one extra

    return(0);
  }


unsigned char put_in_queue_low(t_message *new_message)
  {
    bool retval = 0;

    switch(opendcc_state)
      {
        case RUN_OKAY:             // DCC running
        case RUN_STOP:             // DCC Running, all Engines Emergency Stop
        case RUN_OFF:              // Output disabled (2*Taste, PC)
        case RUN_SHORT:            // Kurzschluß
        case RUN_PAUSE:            // DCC Running, all Engines Speed 0

            retval = put_in_queue_lp(new_message);
            break;

        case PROG_OKAY:
        case PROG_SHORT:           //
        case PROG_OFF:
        case PROG_ERROR:
            // FIXME if (!programmer_busy())
              {
                retval = put_in_queue_prog(new_message);
              }
            break;
      }
    return(retval);
  }

//-----------------------------------------------------------------------------------
// search_repeatbuffer: scans the repeat queue for the message with the highest repeat req.
// returns this entry in *mysearch and the repeat count as value;
// if return==0 then repeatbuffer is empty

unsigned char search_repeatbuffer(t_message *mysearch)
  {
  unsigned char run_repeat = 0;
  unsigned char i; unsigned char found_i=0;

    for (i=0; i<SIZE_REPEATBUFFER; i++)
      {
        if (repeatbuffer[i].repeat > run_repeat)
          {
            run_repeat = repeatbuffer[i].repeat;
            found_i = i;
          }
      }
    if (run_repeat > 0)
      {
        repeatbuffer[found_i].repeat--;
        mysearch->qualifier = repeatbuffer[found_i].qualifier;  // both type and size
        memcpy(mysearch->dcc, repeatbuffer[found_i].dcc, repeatbuffer[found_i].size);
      }
    return(run_repeat);
  }


void update_repeatbuffer(t_message *new_message)
  {
  unsigned char run_repeat = 255;
  unsigned char i; unsigned char found_i=0;

    if (new_message->repeat == 0) return;      // der will gar keinen repeat haben
    if (new_message->type == is_prog) return;  // der soll keinen repeat haben (nur immediate)
    for (i=0; i<SIZE_REPEATBUFFER; i++)
      {
        if (repeatbuffer[i].dcc[0] == new_message->dcc[0])
          {
            // same adr found, is it short / long? and is it speed command?
            // if yes, replace it and return
            if (repeatbuffer[i].dcc[0] < 112)                   // short adr.
              {
                if (repeatbuffer[i].dcc[1] == 0x3F)             // (128 Speed Steps)
                  {
                    memcpy(&repeatbuffer[i], new_message, sizeof(t_message));
                    return;
                  }
                else if (repeatbuffer[i].dcc[1] & 0x40)         // (28 Speed Steps)
                  {
                    memcpy(&repeatbuffer[i], new_message, sizeof(t_message));
                    return;
                  }
              }
            else if (repeatbuffer[i].dcc[0] < 128)
              {
              }
            else if (repeatbuffer[i].dcc[0] < 192)
              {
                unsigned char test;
                test = repeatbuffer[i].dcc[1] ^ new_message->dcc[1];
                if (!(test & 0x76))  // upper addr equal 
                  {
                    memcpy(&repeatbuffer[i], new_message, sizeof(t_message));
                    return;
                  } 
              } 
            else if (repeatbuffer[i].dcc[0] < 232)              // long addr
              {
                if (repeatbuffer[i].dcc[1] == new_message->dcc[1])  // long addr
                  {
                    if (repeatbuffer[i].dcc[2] == 0x3F)             // (128 Speed Steps)
                      {
                        memcpy(&repeatbuffer[i], new_message, sizeof(t_message));
                        return;
                      }
                    else if (repeatbuffer[i].dcc[2] & 0x40)         // (28 Speed Steps)
                      {
                        memcpy(&repeatbuffer[i], new_message, sizeof(t_message));
                        return;
                      }
                  }
              }
          }
      }

    // no command found to be replaced, so search for lowest refresh
    for (i=0; i<SIZE_REPEATBUFFER; i++)
      {
        if (repeatbuffer[i].repeat < run_repeat)
          {
            found_i = i;
            run_repeat = repeatbuffer[i].repeat;
          }
      }

    // repeatbuffer[found_i].repeat = new_message->repeat;
    // repeatbuffer[found_i].qualifier = new_message->qualifier;   // both type and size
    // memcpy(repeatbuffer[found_i].dcc, new_message->dcc, new_message->size);
    memcpy(&repeatbuffer[found_i], new_message, sizeof(t_message));
  }

// remove commands to this loco from repeatbuffer
// to get rid of old settings in case the loco is updated
/* old!
void clear_from_repeatbuffer(t_message *new_message)
  {
    unsigned char i;

    for (i=0; i<SIZE_REPEATBUFFER; i++)
      {
        if (repeatbuffer[i].dcc[0] == new_message->dcc[0])
          {
            // same adr found, is it short / long? and is it speed command?
            // if yes, replace it and return
            if (((repeatbuffer[i].dcc[0] & 0xC0) == 0xC0)  &&  // long adr.
                 (repeatbuffer[i].dcc[1] == new_message->dcc[1])  &&
                 ((repeatbuffer[i].dcc[2] & 0x40) == 0x40) )
              {
                 // long adr and speed command
                 repeatbuffer[i].repeat = 0;
                 return;
              }
            if (((repeatbuffer[i].dcc[0] & 0x80) == 0x00)  &&  // short adr.
                ((repeatbuffer[i].dcc[1] & 0x40) == 0x40) )
              {
                 // short adr and speed command
                 repeatbuffer[i].repeat = 0;
                 return;
              }
          }
      }
  } */

// only loco commands
void clear_from_repeatbuffer(t_message *new_message)
  {
    unsigned char i;

    for (i=0; i<SIZE_REPEATBUFFER; i++)
      {
        if (repeatbuffer[i].dcc[0] == new_message->dcc[0])
          {
            // same adr found, is it short / long? and is it speed command?
            // if yes, replace it and return
            if (repeatbuffer[i].dcc[0] < 112)                   // short adr.
              {
                if (repeatbuffer[i].dcc[1] == 0x3F)             // (128 Speed Steps)
                  {
                    repeatbuffer[i].repeat = 0;
                    return;
                  }
                else if (repeatbuffer[i].dcc[1] & 0x40)         // (28 Speed Steps)
                  {
                    repeatbuffer[i].repeat = 0;
                    return;
                  }
              }
            else if ( (repeatbuffer[i].dcc[0] >= 192) &&
                     (repeatbuffer[i].dcc[0] < 232)  )
              {
                if (repeatbuffer[i].dcc[1] == new_message->dcc[1])  // long addr
                  {
                    if (repeatbuffer[i].dcc[2] == 0x3F)             // (128 Speed Steps)
                      {
                        repeatbuffer[i].repeat = 0;
                        return;
                      }
                    else if (repeatbuffer[i].dcc[2] & 0x40)         // (28 Speed Steps)
                      {
                        repeatbuffer[i].repeat = 0;
                        return;
                      }
                  }
              }
          }
      }
  }


//----------------------------------------------------------------------------------
// downstream
//----------------------------------------------------------------------------------
//
// set_next_message: forward the current message to dccout.c
// next_message_count is the flag of DCCOUT - a new message has arrived

void set_next_message (t_message *new)
  {
    unsigned char my_repeat;

    memcpy(next_message.dcc, new->dcc, new->size);

    // now scan this message for speed command and replaces the speed value depending
    // on organizer_halt_state

    if (organizer_halt_state > 0)
      {
        if ( (next_message.dcc[0] > 0) &&
             (next_message.dcc[0] < 112) ) // short adr.
          {
            if (next_message.dcc[1] == 0x3F )  // (128 Speed Steps)
              {
                // next_message.dcc[2] (msb=dir, 7 bit=speed, 0=stop, 1=e-stop)
                next_message.dcc[2] &= 0x80; // keep dir
              }
            else if ((next_message.dcc[1] & 0x40) == 0x40 )
              {
                next_message.dcc[1] &= 0xF0; // keep dir
              }
          }
        if ((next_message.dcc[0] >= 192)  && // long adr. 
            (next_message.dcc[0] < 232) )
          {
            if (next_message.dcc[2] == 0x3F )  // (128 Speed Steps)
              {
                // next_message.dcc[3] (msb=dir, 7 bit=speed, 0=stop, 1=e-stop)
                next_message.dcc[3] &= 0x80; // keep dir
              }
            else if ((next_message.dcc[2] & 0x40) == 0x40 )
              {
                next_message.dcc[2] &= 0xF0; // keep dir
              }
          }
      }

    next_message.size = new->size;
    next_message.type = new->type;

    if (new->type == is_prog)
      {                                      // prog commands keep their repeat (like PoM)
        my_repeat = new->repeat;             // immediate repeat
        if (my_repeat == 0) my_repeat = 1;   // at least once
        next_message_count = my_repeat;
      }
    else
      {
        next_message_count = 1;              // all other command have no repeat
                                             // -> repeat is done with repeat_buffer.
      }
  }
  
void set_next_message_and_repeat (t_message *new)
  {
    unsigned char my_repeat;

    memcpy(next_message.dcc, new->dcc, new->size);

    next_message.size = new->size;
    next_message.type = new->type;

    my_repeat = new->repeat;             //immediate repeat
    if (my_repeat == 0) my_repeat = 1;
    next_message_count = my_repeat;
  }

//---------------------------------------------------------------------------------
// run_organizer: select the next message to put on the rail
//
// depending on the opendcc_state (fehlt noch!!!):
//
// RUN_OKAY:    run all track queues and refreshes
//
//xRUN_STOP:    only queue_hp, only queue_lp,                  // DCC Running, all Engines Emergency Stop
//              no repeatbuffer, no locobuffer        
//
// RUN_OFF:     only idle (we dont want to loose a command)    // Output disabled (2*Taste, PC)
//
// RUN_SHORT:   only idle (we dont want to loose a command)    // Kurzschluss;
//
// RUN_PAUSE:   run queues and refresh, but set speed 0        // DCC Running, all Engines Speed 0
//
// PROG_OKAY:   only queue_prog,
//              no repeatbuffer, no locobuffer                 // Prog-Mode running
//
// PROG_SHORT:  only idle                                      // Prog-Mode + Kurzschluss
//
// PROG_OFF:    only idle                                      // Prog-Mode, abgeschaltet
//
// PROG_ERROR:  only queue_prog                                // Prog-Mode, Fehler beim Programmieren
//              no repeatbuffer, no locobuffer        

// Achtung: organizer läuft zur Zeit bei RUN_OKAY


void run_organizer(void)
  {
    t_message *my_search_ptr;
    t_message *next_mess_ptr;
    t_message search_message;

    my_search_ptr = &search_message;

    // is DCC_OUT ready?
    if (next_message_count != 0) return;

    // we can now put the next message on the tracks

    switch(opendcc_state)
	  {
	    case RUN_OKAY:      // all runnung
        case RUN_PAUSE:     // slow down
        case RUN_STOP:      // speed 0		

        	//Serial_println( "run_okay " );
            // check queue_hp
            if ((hp_write != hp_read) &&
                (queue_hp[hp_read].dcc[0] != next_message.dcc[0]))
              {
                // read message from queue_hp
                next_mess_ptr = &queue_hp[hp_read];
                set_next_message(next_mess_ptr);
                hp_read++;
                if (hp_read == SIZE_QUEUE_HP) hp_read = 0;   // advance pointer

                // put this message to repeatbuffer
                update_repeatbuffer(next_mess_ptr);
              }
            else
              {// check queue_lp
                if ((lp_write != lp_read) &&
                    (queue_lp[lp_read].dcc[0] != next_message.dcc[0]))
                  {
                    // read message from queue_lp
                    next_mess_ptr = &queue_lp[lp_read];
                    set_next_message(next_mess_ptr);
                    lp_read++;
                    if (lp_read == SIZE_QUEUE_LP) lp_read = 0;   // advance pointer

                    // put this message to repeatbuffer
                    update_repeatbuffer(next_mess_ptr);
                  }
                else
                  {
                    if (search_repeatbuffer(my_search_ptr) &&
                        (search_message.dcc[0] != next_message.dcc[0]) )
                      {
                        // read this message from repeatbuffer
                        set_next_message(my_search_ptr);
                      }
                    else
                      {
                        my_search_ptr = search_locobuffer();
                        if (my_search_ptr->dcc[0] != next_message.dcc[0] )
                          {
                            // read this message from locobuffer
                            set_next_message(my_search_ptr);
                          }
                        else
                          { // nichts gefunden, dann halt idle
                            my_search_ptr = &DCC_Idle;
                            set_next_message(my_search_ptr);
                          }
                      }
                  }
              }
		    break;
	
        case RUN_OFF:
		    // outputs are off, we do nothing: keep dccout alive with idle
		    // my_search_ptr = &DCC_Idle;
            // set_next_message(my_search_ptr);
			break;
	
        case RUN_SHORT:
		    // outputs are off, we do nothing; keep dccout alive with idle
		    // my_search_ptr = &DCC_Idle;
            // set_next_message(my_search_ptr);
			
		    break;
	
        case PROG_OKAY:
        case PROG_SHORT:
        case PROG_OFF:
        case PROG_ERROR:
		    // run prog queue
			if (prog_write != prog_read)
              {
                // read message from queue_prog
                next_mess_ptr = &queue_prog[prog_read];
                set_next_message_and_repeat(next_mess_ptr);        // repeat as often as in message
                prog_read++;
                if (prog_read == SIZE_QUEUE_PROG) prog_read = 0;   // advance pointer
              }
            else
              { // nichts gefunden, dann halt idle
                my_search_ptr = &DCC_Idle;
                set_next_message(my_search_ptr);
              }
		    break;
	  }
  }



//=======================================================================================
//
//  6. Upstream Interface
//
//=======================================================================================
// 
//  Notes: 
//      prior to any command to the organizer, init_organizer() must be called
//      all commands return true, if there is still space in the queues
//
//  Summary of commands to organizer:
//      init_organizer(void)
//      organizer_ready(void)
//      do_loco_speed_f(addr, speed, format)   set speed and format for a loco
//      do_loco_speed(addr, speed)             set speed for a loco
//      do_loco_func_grp0(addr, funct)         light
//      do_loco_func_grp1(addr, funct)         f1-f4
//      do_loco_func_grp2(addr, funct)         f5-f8
//      do_loco_func_grp3(addr, funct)         f9-f12
//      do_accessory(addr, output, activate)   turnout
//      do_pom_loco(addr, cv, data)            program on the main
//      do_pom_accessory(addr, cv, data)       program on the main
//      do_all_stop(void)                      Halt all locos
//      do_fast_clock(min. hour, day, ratio)
//
// All commands share the same return bits - could just be or'ed.
//
//---------------------------------------------------------------------------------------
//
// say full, even if there one space left; this allows broadcast to be sent without asking.
// returns 1 if there is space for commands
bool organizer_ready(void)
  {
    unsigned char i;
    // check for full (with reserved entry)
    i = hp_write + 1;
    if (i == SIZE_QUEUE_HP) i = 0;
    if (i == hp_read) return(0);         // totally full
    i = i + 1;
    if (i == SIZE_QUEUE_HP) i = 0;
    if (i == hp_read) return(0);         // one left -> say full, keep one extra

    i = lp_write + 1;
    if (i == SIZE_QUEUE_LP) i = 0;
    if (i == lp_read) return(0);         // totally full
    i = i + 1;
    if (i == SIZE_QUEUE_LP) i = 0;
    if (i == lp_read) return(0);         // one left -> say full, keep one extra

    return(1);                            // both queues have space
  }


// Speed einstellen: dieses Kommando geht auch in die high priority queue falls gebremst wird;
// immer in low priority queue, von dort wird es nach dem Ausgeben in repeatbuffer
// übernommen.
// parameter: addr:   0...16383
//            speed:  0..127 (independent from format)
//            format: new format
//
// return:    false, if full; return 1 if there is still space
// 
unsigned char do_loco_speed_f(unsigned int addr, unsigned char speed, t_format format)
  {
    unsigned char index, retval;
    t_message *my_message;

    retval = enter_speed_f_to_locobuffer(addr, speed, format);
    index = last_locobuffer_index();
    if (retval & (1 << ORGZ_SLOW_DOWN) )
      {  // slow down or direction change
        my_message = build_speed_message_from_locobuffer(index);
        retval |= put_in_queue_hp(my_message);
        retval |= put_in_queue_low(my_message);
      }
    else
      {  // speed up
        my_message = build_speed_message_from_locobuffer(index);
        retval |= put_in_queue_low(my_message);
      }

    clear_from_repeatbuffer(my_message);   // neu 2008-01-07
    return(retval);
  }


// Speed einstellen: dieses Kommando geht auch in die high priority queue falls gebremst wird;
// immer in low priority queue, von dort wird es nach dem Ausgeben in repeatbuffer
// übernommen.
// return false, if full; return 1 if there is still space


unsigned char do_loco_speed(unsigned int addr, unsigned char speed)
  {
    unsigned char index, retval;
    t_message *my_message;

    // does this loco exist?
    retval = enter_speed_to_locobuffer(addr, speed);
    index = last_locobuffer_index();
    if (retval & (1 << ORGZ_SLOW_DOWN) )
      {  // slow down or direction change
        my_message = build_speed_message_from_locobuffer(index);
        retval |= put_in_queue_hp(my_message);
        retval |= put_in_queue_low(my_message);
      }
    else
      {  // speed up
        my_message = build_speed_message_from_locobuffer(index);
        retval |= put_in_queue_low(my_message);
      }

    clear_from_repeatbuffer(my_message);   // neu 2008-01-07
    return(retval);
  }



// Loco function einstellen

unsigned char do_loco_func_grp0(unsigned int addr, unsigned char funct)
  {
    unsigned char index, retval;

    retval = enter_func_to_locobuffer(addr, funct, 0);
    index = last_locobuffer_index();
    retval |= put_in_queue_low(build_f1_message_from_locobuffer(index));   // grp 0 = light
    return(retval);
  }


unsigned char do_loco_func_grp1(unsigned int addr, unsigned char funct)
  {
    unsigned char index, retval;

    retval = enter_func_to_locobuffer(addr, funct, 1);
    index = last_locobuffer_index();
    retval |= put_in_queue_low(build_f1_message_from_locobuffer(index));   // grp 1
    return(retval);
  }

unsigned char do_loco_func_grp2(unsigned int addr, unsigned char funct)
  {
    unsigned char index, retval;

    retval = enter_func_to_locobuffer(addr, funct, 2);
    index = last_locobuffer_index();
    retval |= put_in_queue_low(build_f2_message_from_locobuffer(index));   // grp 2
    return(retval);

  }
unsigned char do_loco_func_grp3(unsigned int addr, unsigned char funct)
  {
    unsigned char index, retval;

    retval = enter_func_to_locobuffer(addr, funct, 3);
    index = last_locobuffer_index();
    retval |= put_in_queue_low(build_f3_message_from_locobuffer(index));   // grp 3
    return(retval);
  }


// 
// parameters: addr:     turnout decoder [0000-4095]
//             output:   coil (red, green) [0,1]
//             activate: off, on = [0,1]; 
// special:    if addr >= virtual_decoder_offset, then remap the call to dmx

bool do_accessory(unsigned int addr, unsigned char output, unsigned char activate)      // turnout
  {
    unsigned char retval;

    #if (DMX_ENABLED==1) 
        if (addr >= virtual_decoder_offset)
          {
            do_dmx((addr - virtual_decoder_offset)*2 + output);
            return(0);   // queues not affected, therefore always return without error code
          }
    #endif
    if (activate) save_turnout(addr, output);
    build_nmra_basic_accessory(addr, output, activate, locobuff_mes_ptr);
    retval = put_in_queue_low(locobuff_mes_ptr);
    return(retval);
  }

// programming on the main (locos)
//
// parameters: addr:     loco
//             cv:       config var in this loco
//             data:     value to be written

bool do_pom_loco(unsigned int addr, unsigned int cv, unsigned char data)
  {
    unsigned char retval;

    if (addr < 128)
      {
        build_pom_7a(addr, cv, data, locobuff_mes_ptr);
      }
    else
      {
        build_pom_14a(addr, cv, data, locobuff_mes_ptr);
      }
    retval = put_in_queue_low(locobuff_mes_ptr);
    return(retval);
  }


// programming on the main (accessory)
//
// parameters: addr:     accessory decoder addr (not turnout!)
//             cv:       config var in this accessory
//             data:     value to be written 

bool do_pom_accessory(unsigned int addr, unsigned int cv, unsigned char data)
  {
    unsigned char retval;

    build_pom_accessory(addr, cv, data, locobuff_mes_ptr);
    retval = put_in_queue_low(locobuff_mes_ptr);
    return(retval);
  }


#if (DCC_FAST_CLOCK == 1)
bool do_fast_clock(t_fast_clock* my_clock)
  {
    unsigned char retval;
    
    build_dcc_fast_clock(my_clock, locobuff_mes_ptr);
    retval = put_in_queue_low(locobuff_mes_ptr);
    return(retval);
  }
#endif


void do_all_stop(void)
  {

    // future idea: we could also do a soft stop.

    DCC_BC_Brake.repeat = 10;

    put_in_queue_hp(&DCC_BC_Brake);

    organizer_halt_state = 2;
  }


