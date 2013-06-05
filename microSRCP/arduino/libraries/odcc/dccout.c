//----------------------------------------------------------------
//
// OpenDCC
//
// Copyright (c) 2006-2008 Kufer
//
// This source file is subject of the GNU general public license 2,
// that is available at the world-wide-web at
// http://www.gnu.org/licenses/gpl.txt
//
//-----------------------------------------------------------------
//
// file:      dccout.c
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// history:   2006-04-13 V0.1 started
//            2006-05-19 V0.2 long preambles if PROG_TRACK_STATE = ON
//            2006-06-13 V0.3 Bugfix: TC1R no usable as state
//            2006-09-30 V0.4 added check for F_CPU, added code proposal
//                            for Märklin MM1 and MM2 (not tested)
//            2007-03-19 V0.5 added code for FEEDBACK
//            2007-03-27 V0.6 added check for hardware.h
//            2008-07-09 V0.7 interrupt processor dependant
//                            (Atmega32, Atmega644P)
//            2009-06-23 V0.9 DCC message size imported from config.h (MAX_DCC_SIZE)
//			  2010-01-24 Angepasst an Arduinoboard mit ATmega328P, Marcel Bernet
//
//-----------------------------------------------------------------
//
// purpose:   lowcost central station for dcc
// content:   ISR for DCC-OUT
//
//-----------------------------------------------------------------


#define DEBUG_SCOPE  0              // 0 no DEBUG SCOPE output
                                    // 1 Generate Preamble Pulse on DMX 
                                    // 2 Generate Interrupt Pulse on DMX

#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>           // put var to program memory
#include <avr/eeprom.h>
#include <string.h>

#include "config.h"                 // general structures and definitions
#include "dccout.h"                 // import own header

#ifndef __HARDWARE_H__
 #warning: please define a target hardware
 #warning: I need:  DCC: the output pin (readback of current phase) 
 #warning: I need:  PROG_TRACK_STATE: the enable pin for programming (readback, deciding 14 or 20 preamble) 
#endif

#if (DEBUG_SCOPE)
 #warning DCCOUT.C: DEBUG_SCOPE enabled - are you sure?
#endif  
//-----------------------------------------------------------------
//------ message formats
// DCC Baseline Packet 3 bytes (address data xor) form 42 bits
// 11111111111111 0 0AAAAAAA 0 01DCSSSS 0 EEEEEEEE 1
// built as follows:
// bit:   value     defines
// 0-14   1         preamble
// 15     0         packet start bit  = > allways 0xFF, 0xFE at start
// 16-23  address   dcc address
// 24     0         data start bit
// 25-32  data      data
// 33-40  xor       error checking
// 41     1         packet end bit

// DCC Extended Packets (see NMRA 9.2.1):
// Addr:            used for:
// 0       00000000 broadcast
// 1-127   0AAAAAAA 7-bit addressed loco decoder
// 128-191 10AAAAAA accessory decoder (either 9 bit or 11 bit)
// 192-231 11000000-11100111 14-bit addressed loco decoders
// 232-254 11101000-11111110 reserved, future use
// 255     11111111 idle
//
// following one to three data bytes
// Data:   CCCDDDDD [0 DDDDDDDD 0 DDDDDDDD]
// CCC defines type:
// 000     Funktionen des Dekoders steuern
//         00000000           decoder reset
//         0001CCCC 0AAAAAAA  consist control
// 001     Erweiterte Funktionen des Dekoders steuern
//         00111111 DDDDDDDD  speed step control
// 010     Geschwindigkeit für Rückwärtsfahrt
// 011     Geschwindigkeit f¨ur Vorw¨artsfahrt
// 100     Hilfsfunktion aus Gruppe 1 steuern
//         100DDDDD           DDDDD = FL, F4, F3, F2, F1
// 101     Hilfsfunktion aus Gruppe 2 steuern
//         two modes:
//         1011DDDD           DDDD = F8, F7, F6, F5
//         1010DDDD           DDDD = F12, F11, F10, F9
// 110     Reserviert für zukünftige Erweiterungen
// 111     Zugriff auf Konfguration-Variablen
//         1110CCAA AAAAAAAA DDDDDDDD
//         CC = 00 (reserved), 01=Verify, 11=write, 10 bit set
//         AA AAAAAAAA 10 bit cv-address, msb first (cv = address+1)
//         bit set: DDDDDDDD = 111CDAAA; C=1 -> write, C=0 verify, D=Bit, AAA=bit pos.
//         write requires two packets !
//
// accesory messages
//  9-bit addresses: 10AAAAAA  1AAACDDD
// 11-bit addresses: 10AAAAAA  0AAA0AA1 000XXXXX
//
//=====================================================================
//
// DCC_OUT
//
// purpose:   receives the next message and puts on the DCC-Pins,
//            using the PWM engine of the ATmega.
//
// how:       the pwm timer is programmed to clear on Compare value;
//            each compare hit of the timer generates a INT,
//            Output is toggled in the Timer and the Timer
//            is reprogrammed for the next bit duration.
//
//            Every second INT checks for the settings of next bit and
//            advances the state engine. Both compares run in parallel
//            to generate both DCC and nDCC for the output driver.
//
//            The ISR uses an state engine (doi.state) to track the
//            actual position inside a DCC message. preamble and
//            XOR checksum are managed by the ISR.
//
// interface: new messages (only the payload) are copied from
//            global char array next_message. After copying, the
//            global int next_message_count is decremented. The
//            interfacing routine has to stop interrupts when writing
//            to next_message.
//            next_message: first char = size of message (valid 2...5),
//                          following chars = message (payload);
//
//            if no next_message is given, dccout will keep alive
//            and will send all 1;
//
//-----------------------------------------------------------------
//------ message formats
// DCC Baseline Packet 3 bytes (address data xor) form 42 bits
// 11111111111111 0 0AAAAAAA 0 01DCSSSS 0 EEEEEEEE 1
// built as follows:
// bit:   value     defines
// 0-14   1         preamble
// 15     0         packet start bit  = > allways 0xFF, 0xFE at start
// 16-23  address   dcc address
// 24     0         data start bit
// 25-32  data      data
// 33-40  xor       error checking
// 41     1         packet end bit (may be next preamble)


//-------------------------------------------------------------------------------
// generate bit sequence through PWM timing
// prescaler = 1
// f(clk) = 16 MHz
// t(period_0) = 232 us, t(period_1) = 116 us
//
// calculate: TOP     = f(clk) * t(period) / prescaler - 1
//            COMPARE = f(clk) * t(period) / 2 * prescaler - 1
//
// at 16MHz it lasts aprox. 4us (wc) until new timervals are set;
// Interrupts must not be blocked more then 50us.
// DCCOUT-ISR must have the highest priority in the system.
//
// If an interrupt is missed, the following occurs:
// The counter will wrap at 0xffff, thus putting out a stretched bit of
// 4,096ms duration; annoying, but not dead ;-)
//
//-------------------------------------------------------------------------------


/// This are timing definitions from NMRA
#define PERIOD_1   116L                  // 116us for DCC 1 pulse - do not change
#define PERIOD_0   232L                  // 232us for DCC 0 pulse - do not change
#define CUTOUT_GAP  30L                  // 30us gap after last bit of xor

//-----------------------------------------------------------------
//
// Timer 1 overflow interrupt service routine
//
// purpose:  - create dcc activity
//           - set new CTC (clear timer on compare) values
//             depending on next bit.
//           - advance state engine
//-----------------------------------------------------------------
//

// upstream interface for messages:

struct next_message_s next_message;    // see dccout.h

volatile unsigned char next_message_count;



// upstream interface for turnout feedback:

#if (TURNOUT_FEEDBACK_ENABLED == 1)
   
  volatile unsigned int feedbacked_accessory; // this is the current turnout under query
                                              // this includes the coil address!
  volatile unsigned char feedback_required;   // 1: make a query at end of next preamble
  volatile unsigned char feedback_ready;      // MSB: if 1: the query is done (communication flag)
                                              // LSB: if 1: there was no feedback
                                              //            == position error
#endif

//----------------------------------------------------------------------------------------
// Timing for feedback
//
// next_message  AAAAA   ...... BBBBBBBBBBBBBBBBBBBBB
//
// doi                AAAAAAAAAAAAAAAAAAAAAAAAAA     BBBBBBBBBBBBBBBBBBBBBBBBBB
//
// DCC            |  PRE  |   A1  |   A2  |  XOR  |  PRE  |   B1  |  B2  |  XOR  |
//
// external feedback pulse              _________^^^^^^^^____________________
//
// feedback_required    ________________________^^^^^^^^^^_____________________^^^^^^^^^^______
//                                                      ? 
//
// feedback_ready              __________________________^^^^^^^^^^^^^^^^^^^^^^_____
//                            ------------------aaaaaaaaaAAAAAAAAAAAAAAAAAAAAAAbbbbbbb ...
//
//
// Step:    Action
// 1        at beginning of preamble:
//          copy dcc message and type from next_message to local variable
// 2        put out this message
// 3        at end of checksum - test for feedback and set actual turnout addr and
//          a flag (feedback_required).
// 4        at end of next preamble - query feedback line and set message


// security check against 2exp32 (4294967296)
#if ((F_CPU / 1024 * PERIOD_0) > 4177000)
#warning: Overflow in calculation of constant
// if this warning is issued, please split the "/1000000L" into two parts
#endif

//----------------------------------------------------------------------------------------
// Optimierter Code
// folgende Tricks:
//  a) doi.phase wird direkt von PIND zurückgelesen, damit entfällt der Memoryzugriff
//  b) doi.state wird in MY_STATE_REG[7..5] abgelegt - schnell abzufragen
//  c) kein switch für doi.state, sondern if else
//  d) doi.bits_in_state wird in MY_STATE_REG[4...0] abgelegt. MY_STATE_REG ist
//     damit *die* zentrale globale Variable, die den Zustand der ISR abbildet.
//  e) do_send nicht als call, sondern direkt als inline call
//  f) Mit einem #define DCCOUT_STATE_REG auf ein IO-Register des Prozessors
//     kann die ISR noch weiter beschleunigt werden -> dieses IO-Register
//     wird dann als globale Variable für MY_STATE_REG misbraucht. 
//
// Optimizimg code
// following tricks
// a) read back of doi.phase from PIND - shorter then memory access
// b) doi.state is allocated at MY_STATE_REG[7..5] - fast access
// c) use of if-then-else instead of switch().
// d) doi.bits_in_state are allocated at MY_STATE_REG[4..0] - fast access
//    MY_STATE_REG is *the* central variable!
// e) do_send() is not called, but compiled as inline code
// f) MY_STATE_REG is mapped to an unused IO port of the atmel
//    -> #define DCCOUT_STATE_REG 
//----------------------------------------------------------------------------------------


static inline void do_send(bool myout) __attribute__((always_inline));
void do_send(bool myout)
  {
    if (myout == 0)
      {                                     // 0 - make a long pwm pulse
        TCCR1A = (1<<COM1A1) | (0<<COM1A0)  //  clear OC1A (=DCC) on compare match
               | (1<<COM1B1) | (1<<COM1B0)  //  set   OC1B (=NDCC) on compare match
               | (0<<WGM11)  | (0<<WGM10);  //  CTC (together with WGM12 and WGM13)
        OCR1A = F_CPU * PERIOD_0 / 2 / 1000000L;               //1856
        OCR1B = F_CPU * PERIOD_0 / 2 / 1000000L;               //1856
      }
    else
      {                                     // 1 - make a short pwm puls
        TCCR1A = (1<<COM1A1) | (0<<COM1A0)  //  clear OC1A (=DCC) on compare match
               | (1<<COM1B1) | (1<<COM1B0)  //  set   OC1B (=NDCC) on compare match
               | (0<<WGM11)  | (0<<WGM10);  //  CTC (together with WGM12 and WGM13)
        OCR1A = F_CPU * PERIOD_1 / 1000000L / 2;               //928
        OCR1B = F_CPU * PERIOD_1 / 1000000L / 2;               //928
      }
  }

// this is the code for cutout - lead_in
static inline void do_send_no_B(bool myout) __attribute__((always_inline));
void do_send_no_B(bool myout)
  {
    if (myout == 0)
      {                                     // 0 - make a long pwm pulse
        TCCR1A = (1<<COM1A1) | (0<<COM1A0)  //  clear OC1A (=DCC) on compare match
               | (1<<COM1B1) | (1<<COM1B0)  //  set   OC1B (=NDCC) on compare match
               | (0<<WGM11)  | (0<<WGM10);  //  CTC (together with WGM12 and WGM13)
        OCR1A = F_CPU * PERIOD_0 / 2 / 1000000L;               //1856 (for 16MHz)
        OCR1B = F_CPU / 1000000L * 4 * PERIOD_0 / 2 ;          // extended (cutout starts after OCR1A)
      }
    else
      {                                     // 1 - make a short pwm puls
        TCCR1A = (1<<COM1A1) | (0<<COM1A0)  //  clear OC1A (=DCC) on compare match
               | (1<<COM1B1) | (1<<COM1B0)  //  set   OC1B (=NDCC) on compare match
               | (0<<WGM11)  | (0<<WGM10);  //  CTC (together with WGM12 and WGM13)
        // OCR1A = F_CPU * PERIOD_1  / 2 / 1000000L ;            //928
        OCR1A = F_CPU * CUTOUT_GAP / 1000000L ;            //928
        OCR1B = F_CPU / 1000000L * 8 * PERIOD_1 / 2;          // extended (cutout starts after OCR1A)
      }
  }


// define some handy names for the states of the ISR
#define DOI_IDLE     (0 << 5)
#define DOI_PREAMBLE (1 << 5)
#define DOI_BSTART   (2 << 5)
#define DOI_BYTE     (3 << 5)
#define DOI_XOR      (4 << 5)
#define DOI_END_BIT  (5 << 5)
#define DOI_CUTOUT_1 (6 << 5)
#define DOI_CUTOUT_2 (7 << 5)
#define DOI_CNTMASK  0x1F

#ifdef DCCOUT_STATE_REG
    struct
      {
	                                                    // state in IO-space -> #define
        unsigned char ibyte;                            // current index of byte in message
        unsigned char cur_byte;                         // current byte
        unsigned char xor_byte;                         // actual check
        unsigned char current_dcc[MAX_DCC_SIZE];        // current message in output processing
        unsigned char bytes_in_message;                 // current size of message (decremented)
        t_msg_type type;                                // type (for feedback)
      } doi;
    #define MY_STATE_REG DCCOUT_STATE_REG
#else
    struct
      {
        unsigned char state;                            // current state
        unsigned char ibyte;                            // current index of byte in message
        unsigned char cur_byte;                         // current byte
        unsigned char xor_byte;                         // actual check
        unsigned char current_dcc[MAX_DCC_SIZE];        // current message in output processing
        unsigned char bytes_in_message;                 // current size of message (decremented)
        t_msg_type type;                                // type (for feedback)
      } doi;
    #define MY_STATE_REG doi.state
#endif


ISR(TIMER1_COMPA_vect)
  {
    #if (DEBUG_SCOPE == 2)
	  DMX_OUT_LOW;
	#endif

    register unsigned char state = MY_STATE_REG & ~DOI_CNTMASK;    // take only 3 upper bits

    // two phases: phase 0: just repeat same duration, but invert output.
    //             phase 1: create new bit.
	// we use back read of PIND instead of phase
#if (__AVR_ATmega328P__ || __AVR_ATmega1280__ || __AVR_ATmega2560__)
    if (!(PINB & (1<<DCC)))  //was: (doi.phase == 0)
#else
    if (!(PIND & (1<<DCC)))  //was: (doi.phase == 0)
#endif
	  {
	    if ((state == DOI_CUTOUT_2) )
          {
            TCCR1A = (1<<COM1A1) | (1<<COM1A0)  //  set   OC1A (=DCC) on compare match
                   | (1<<COM1B1) | (1<<COM1B0)  //  set   OC1B (=NDCC) on compare match
                   | (0<<WGM11)  | (0<<WGM10);  //  CTC (together with WGM12 and WGM13)
            #if (DEBUG_SCOPE == 2)
    	       DMX_OUT_HIGH;
    	    #endif
            OCR1A = (F_CPU / 1000000L * 4 * PERIOD_1)
                    - (F_CPU / 1000000L * CUTOUT_GAP);      // create extended timing: 4 * PERIOD_1 for DCC - GAP
    		OCR1B = (F_CPU / 1000000L * 9 * PERIOD_1 / 2)   //                         4.5 * PERIOD_1 for NDCC - GAP
                    - (F_CPU / 1000000L * CUTOUT_GAP);
    		return;  
    	  }
        else
          {
            TCCR1A = (1<<COM1A1) | (1<<COM1A0)  //  set   OC1A (=DCC) on compare match
                   | (1<<COM1B1) | (0<<COM1B0)  //  clear OC1B (=NDCC) on compare match
                   | (0<<WGM11)  | (0<<WGM10);  //  CTC (together with WGM12 and WGM13)
            //was: doi.phase = 1;
            #if (DEBUG_SCOPE == 2)
    	       DMX_OUT_HIGH;
    	    #endif
    		return;  
    	  }
      }
    //was: doi.phase = 0;

//     register unsigned char state = MY_STATE_REG & ~DOI_CNTMASK;    // take only 3 upper bits
        #if (DEBUG_SCOPE == 2)
	       DMX_OUT_HIGH;
	    #endif

    if (state == DOI_IDLE) 
      {
        #if (DEBUG_SCOPE == 1)
          DMX_OUT_HIGH;
        #endif
        do_send(1);

        if (next_message_count > 0)
          {
            memcpy(doi.current_dcc, next_message.dcc, sizeof(doi.current_dcc));
            doi.bytes_in_message = next_message.size;
            // no size checking - if (doi.cur_size > MAX_DCC_SIZE) doi.cur_size = MAX_DCC_SIZE;
            doi.ibyte = 0;
            doi.xor_byte = 0;
            doi.type = next_message.type;   // remember type in case feedback is required

            next_message_count--;
    
            if (PROG_TRACK_STATE) MY_STATE_REG = DOI_PREAMBLE+(20-3);   // long preamble if service mode
            else 				MY_STATE_REG = DOI_PREAMBLE+(14-3);     // 14 preamble bits
                                                                        // doi.bits_in_state = 14;  doi.state = dos_send_preamble;
          }
        return;
      }
    if (state == DOI_PREAMBLE)
      {
        do_send(1);
        MY_STATE_REG--;
        if ((MY_STATE_REG & DOI_CNTMASK) == 0)
          {
            MY_STATE_REG = DOI_BSTART;          // doi.state = dos_send_bstart;
            #if (DEBUG_SCOPE == 1)
                DMX_OUT_LOW;
		    #endif
            #if (TURNOUT_FEEDBACK_ENABLED == 1)
            if (feedback_required)
              {
                if (EXT_STOP_PRESSED) 
                  {
                    // feedback received -> yes, turnout has this position
                    feedback_ready = (1 << FB_READY) | (1 << FB_OKAY);
                  }
                else
                  {
                    // no feedback: 
                    feedback_ready = (1 << FB_READY);
                  }
                feedback_required = 0;
              }
            #endif // feedback
          }
		return;
      }
    if (state == DOI_BSTART)
      {
            #if (DEBUG_SCOPE == 1)
			  DMX_OUT_LOW;
			#endif
			do_send(0);     // trennende 0
            if (doi.bytes_in_message == 0)
              { // message done, goto xor
                doi.cur_byte = doi.xor_byte;
                MY_STATE_REG = DOI_XOR+8;  // doi.state = dos_send_xor; doi.bits_in_state = 8;
              }
            else
              { // get next addr or data
                doi.bytes_in_message--;
                doi.cur_byte = doi.current_dcc[doi.ibyte++];
                doi.xor_byte ^= doi.cur_byte;
              MY_STATE_REG = DOI_BYTE+8;  // doi.state = dos_send_byte; doi.bits_in_state = 8;
              }
            return;
      }
    if (state == DOI_BYTE)
      {
            if (doi.cur_byte & 0x80) {do_send(1);}
            else                     {do_send(0);}
            doi.cur_byte <<= 1;
            MY_STATE_REG--;
            if ((MY_STATE_REG & DOI_CNTMASK) == 0)
              {
              MY_STATE_REG = DOI_BSTART+8;  // doi.state = dos_send_bstart;
              }
            return;
      }
    if (state == DOI_XOR)    // ev. else absichern
      {
            if (doi.cur_byte & 0x80) {do_send(1);}
            else                     {do_send(0);}
            doi.cur_byte <<= 1;
            MY_STATE_REG--;
            if ((MY_STATE_REG & DOI_CNTMASK) == 0)                  // bitcounter lower 5 bits
              {
                MY_STATE_REG = DOI_END_BIT;  // doi.state = dos_idle;
                #if (TURNOUT_FEEDBACK_ENABLED == 1)
                if (doi.type == is_feedback)
                  {
                    // message1 message0 -> ...||......|
                    // -aaa-ccc --AAAAAA => aaaAAAAAAccc
                    feedbacked_accessory = ((doi.current_dcc[0] & 0b00111111) << 3)   // addr low
                                       | ((~doi.current_dcc[1] & 0b01110000) << 5)    // addr high
                                       | (doi.current_dcc[1] & 0b00000111);           // output
                    feedback_ready = 0;
                    feedback_required = 1;
                  }
                #endif // feedback
              }
            return;
     }
    if (state == DOI_END_BIT)
      {
        do_send(1);
        MY_STATE_REG = DOI_CUTOUT_1;
        return;
      } 
    if (state == DOI_CUTOUT_1) 
      {
        #if (DEBUG_SCOPE == 1)
          DMX_OUT_HIGH;
        #endif
        do_send(1);

        MY_STATE_REG = DOI_CUTOUT_2;
        return;
      }
    if (state == DOI_CUTOUT_2) 
      {
        #if (DEBUG_SCOPE == 1)
          DMX_OUT_HIGH;
        #endif
        do_send(1);
        MY_STATE_REG = DOI_IDLE;
        return;
      }

  }

void init_dccout(void)
  {
    MY_STATE_REG = DOI_IDLE; // doi.state = dos_idle;
    next_message_count = 0;
    next_message.size = 2;
    next_message.dcc[0] = 0;
    next_message.dcc[1] = 0;

    #if (TURNOUT_FEEDBACK_ENABLED == 1)
      feedback_ready = 0;
      feedback_required = 0;
    #endif

    do_send(1);                         // init COMP regs.

    // setup timer 1

    TCNT1 = 0;    // no prescaler

     // note: DDR for Port D4 and D5 must be enabled
    TCCR1A = (1<<COM1A1) | (0<<COM1A0)  //  clear OC1A (=DCC) on compare match
           | (1<<COM1B1) | (1<<COM1B0)  //  set   OC1B (=NDCC) on compare match
           | (0<<WGM11)  | (0<<WGM10);  //  CTC (together with WGM12 and WGM13)
                                          //  TOP is OCR1A

    TCCR1B = (0<<ICNC1)  | (0<<ICES1)   // Noise Canceler: Off
           | (0<<WGM13)  | (1<<WGM12)
           | (0<<CS12)   | (0<<CS11) | (1<<CS10);  // no prescaler, source = sys_clk


    #if (__AVR_ATmega644P__ || __AVR_ATmega328P__ || __AVR_ATmega1280__ || __AVR_ATmega2560__)
        TIMSK1 |= (1<<OCIE1A);          // Output Compare A
    #elif (__AVR_ATmega644__)
        #error:  undefinded - read manual!
    #elif (__AVR_ATmega32__)
        TIMSK |= (1<<OCIE1A);          // Output Compare A
    #else
      #error:  undefinded
    #endif
  }
