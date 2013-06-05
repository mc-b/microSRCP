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
// file:      config.c
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// history:   2006-10-23 V0.1 started
//            2007-01-27 V0.2 shift with DMX Macro 
//            2007-10-16 V0.3 TURNOUT_FEEDBACK_ACTIVATED
//            2008-01-08 V0.4 serial_id and short_turnoff_time added to eeprom
//                            dcc_default_format added.
//                            num_of_*** added (for each command group) 
//            2008-02-02 V0.5 feedback_size added 
//            2008-06-18 V0.6 external Stop  
//            2009-03-15 V0.10 ext_stop_deadtime added
//			  2010-01-24 Angepasst an Arduinoboard mit ATmega328P, Marcel Bernet
//
//-----------------------------------------------------------------
//
// purpose:   central station for dcc
// content:   EEPROM configuration
//            
//-----------------------------------------------------------------

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>                 // using sscanf and sprintf increases prog. by 4k!
#include <inttypes.h>
#include <avr/pgmspace.h>          // put var to program memory
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

#include <string.h>

#include "config.h"                // general structures and definitions

//======================================================================
// some globals used throughout OpenDCC

unsigned char invert_accessory = 0;  // Uhlenbrock seems to invert red and green
                                     // with accessory commands - Lenz not
                                     // we do the same!
                                     // so we have to decide in a mixed environment
                                     // which side should be inverted
                                     // Bit 1: invert IB, Bit 0: invert Lenz
                                     // read from CV12

const unsigned char opendcc_version PROGMEM = OPENDCC_VERSION;

//-----------------------------------------------------------------------
// Generelle Variablen und Defines
//
// OpenDCC_Version      Byte, ist dezimal zu interpretieren z.B. 0x04 = Version 0.04

// baudrate             Byte, Wertebereich siehe RS232

// OpenDCC_Mode         Byte, 0; z.Z. reserved

// virtual_decoder      Word, An dieser Stelle beginnt das remapping von Schaltbefehlen
//                            auf DMX
//                            0xffff: abgeschaltet
//                            DMX_MACRO_ADDR ist in config.h definiert

// s88_mode             Bit 0: 0: no hw reading
//                             1: Standard s88 reading
//                      Bit 1: 0: no turnout feedback
//                             1: turnout feedback enabled

// s88_size1            Byte  Anzahl der Bytes auf dem S88-Strang 1
//                            Size is given as bytes;  16 means 8 modules 
// s88_size2            Byte  Anzahl der Bytes auf dem S88-Strang 2
// s88_size3            Byte  Anzahl der Bytes auf dem S88-Strang 3

// invert_accessory     Bit   if 1: Invertiere bei Lenz-Protokoll rot-grün
//                                  bei accessory commands

// startmode_ibox       Byte  0: = Standard Start Mode (=p50-Mode)
//                            1: = P50X Mode

// dcc_acc_repeat       Byte  0...16: so oft wird ein DCC Accessory command wiederholt

// feedback_s88_offset  Byte  Offset in bytes, where feedback messages go to s88

// feedback_s88_type    EAN   0: Ack-Feedback
//                            1: Position Feedback  

