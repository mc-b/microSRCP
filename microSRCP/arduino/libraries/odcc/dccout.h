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
// file:      dccout.h
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// history:   2006-04-13 V0.1 started
//            2006-06-10 V0.2 added volatile, damit der gnu nicht auf
//                            register optimiert.
//            2009-06-23 V0.4 MAX_DCC_SIZE
//			  2010-01-24 Angepasst an Arduinoboard mit ATmega328P, Marcel Bernet
//
//-----------------------------------------------------------------
//
// purpose:   lowcost central station for dcc
// content:   ISR for DCC-OUT
//
//-----------------------------------------------------------------


struct next_message_s
  {
    unsigned char size;
    t_msg_type    type;
    unsigned char dcc[MAX_DCC_SIZE];
  };

extern struct next_message_s next_message;

volatile extern unsigned char next_message_count;     // load message and set count
                                               // if > 1 -> output next_message
											   // if = 0 -> ready for next_message

#if (TURNOUT_FEEDBACK_ENABLED == 1)
// upstream interface for turnout feedback:

// bits
#define FB_READY 7
#define FB_OKAY  1

extern volatile unsigned int feedbacked_accessory; // this is the current turnout under query
                                                   // this includes the coil address!
extern volatile unsigned char feedback_required;   // 1: make a query at end of next preamble
extern volatile unsigned char feedback_ready;      // MSB: if 1: the query is done (communication flag)
                                                   // LSB: if 1: there was no feedback
                                                   //            == position error
#endif


//extern ISR(TIMER1_COMPA_vect);

void timer1_simu(void);                       // call this in a loop during simulation

void init_dccout(void);                      // call once at boot up


