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
// file:      config.h
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// history:   2006-04-13 V0.01 started
//            2006-06-08 V0.02 added SHORT_TIMEOUT
//            2006-09-30 V0.03 Speed always stored as 0..127
//            2006-10-17 V0.04 added Turnoutbuffer 
//            2006-10-24 V0.05 added EEPROM support 
//            2006-10-24 V0.06 new release to web, EEPROM Sections
//                             back to default :-(
//            2006-11-19 V0.07 removed bug in organizer.c
//            2006-12-10 V0.08 programmer in 0.7 got errors, now removed
//            2007-01-18 V0.09 inverting accessory commands when emulating
//                             lenz, added intellibox support,
//                             added virtual decoder support
//            2007-01-19 V0.10 Intellibox - remapping of SO1
//            2007-01-28 V0.11 Intellibox - bugfix with dmx
//                             Intellibox, bugfix with TC (2 RS232 Stopbits)
//                             DCC Accessory repeat count is configurable
//                             in eeprom
//            2007-02-05 V0.12 EEPROM Extension for railware and queries to SO
//                             switch-over to gcc 4.1.1 (important)
//            2007-03-10 V0.13 debugging of Trainprogrammer together with IB mode,
//                             repeat counter bei prog command corrected
//                             POM added
//            2007-03-19 V0.14 added turnout feedback (started and tested)
//                             added extended timing var. for programming
//            2008-01-07 V0.15 added type for message
//                             added CV for Prog turnoff and s88 timing
//            2008-02-02       feedback_size added          
//            2008-06-29       changed from bool to unsigned char (new WinAVR)
//            2008-07-18       CV for external Stop
//            2008-07-25       S88 timing now based on timer2 - no more busy waiting
//                             changed feedback for accessory for lenz parser
//            2008-08-20 V0.16 Bugfix in return values from organizer (ibox_parser)
//            2009-03-15       ext_stop_deadtime added
//            2009-06-23 V0.17 MY_TICK_PERIOD added, compile-switch DCC_FAST_CLOCK
//                             MAX_DCC_SIZE changed to 6
//			  2010-01-24 Angepasst an Arduinoboard mit ATmega328P, Marcel Bernet
//
//-----------------------------------------------------------------
//
// purpose:   lowcost central station for dcc
// content:   central definitions used in the project
//            all major project settings are done here!!
//            
//            1.   Prozessor, Timing
//                 (change here, if OpenDCC is run on a different uP)
//            2.   IOs
//                 (change here, if OpenDCC is run on a different board)
//            3.   System Defintions and default baudrate
//  -->            (select here, what system to build (Intellibox, Lenz, S88, DMX)
//            4.   DCC Definitions (do not change)
//            4.a) defines for handling of DCC
//                 (user definitions like loco formats and buffer sizes)
//            4.b) defines for handling of S88
//            4.c) variable definitions
//            5.   Usage of Memory, EEROM and Registers
//                 (change here for number of locos, turnouts, virtual decoders...)
//
//-----------------------------------------------------------------
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define OPENDCC_VERSION   0x11
#define OPENDCC_SERIAL    0x02     // This is a serial number used to identify different boxes.
#define BUILD_YEAR        0x10
#define BUILD_MONTH       0x01
#define BUILD_DAY         0x22
 

// Debugswitches:  0: no debugging, interrupts running -> alive!
//                 1: no ints, some lowlevel test
//                 2: command_organizer tests
//                 3: IB or Lenz parser tests, see main.c


#define WATCH_S88_SIZE1   0         // 1=test

#define DEBUG  0

#define EEPROM_FIXED_ADDR 0         // if 1: use indivual EEPROM section for Config
                                    // Locos and DMX
                                    // Note: keep at 0, there is bug in AVR Studio
                                    // Atmel is still working on the fix 


#include "hardware.h"               // right now, this includes the definition
                                    // for the hardware on www.opendcc.de


//------------------------------------------------------------------------
// Timing Definitions (all values in us)
//------------------------------------------------------------------------
//

/// This is the timer tick for timeouts, LEDs, key-debounce ...

#define TICK_PERIOD          5000L       // 5ms = 5000us (valid range: 2000-8000)
                                         // if changed: see all .c files and
                                         // check possible range overflows in definitions!

#define MY_TICK_PERIOD       1000L       // 1ms - used as base sysclock from 23.06.2009
                                         // if changed: see all .c files and
                                         // check possible range overflows in definitions!


#define TIMER2_TICK_PERIOD   4L          // 4us


#define SHORT_TURNOFF_TIME    15000L     // wait 15ms before turning off power after
                                         // a short is detected on outputs
                                         // this is the default - it goes to CV34.

#define PROG_SHORT_TURNOFF_TIME 40000L     // wait 40ms before turning off power after
                                         // a short is detected on outputs
                                         // this is the default - it goes to CV35.
#if (SHORT_TURNOFF_TIME < TICK_PERIOD)
 #warning: Configuration mismatch - SHORT_TURNOFF_TIME must be larger then TICK_PERIOD
#endif





//========================================================================
// 3. System Definitions
//========================================================================
// Here You find the defines what system to build
//      (may be altered by user)
//------------------------------------------------------------------------

#define LOCO_DATABASE       UNNAMED     // NAMED    containing Addr, Format, Names (atmega32)
                                        // UNNAMED  containing Addr, Format, (no Names)

#define DMX_ENABLED         0           // 0: disabled
                                        // 1: if enabled, add code for dmx-control

#define DMX_MACRO_ADDR      899         // Turnout address from this address on are
                                        // on put to track, but remapped as dmx-out
                                        // ADDR in this context is counted from 0
                                        // This setting is only active, if dmx is enabled 

#define S88_ENABLED         0           // 1: if enabled, add code for s88-control

#define XPRESSNET_ENABLED   0           // 0: classical OpenDCC
                                        // 1: if enabled, add code for Xpressnet (Requires Atmega644P)
                                        //    see xpnet.c for a detailed list of supported commands.
                                        //    (added here just for compatibility - keep at 0!)

#if (PARSER == LENZ)
  #define DEFAULT_BAUD      BAUD_19200  // supported: 2400, 4800, 9600, 19200, 38400, 57600, 11500         
#endif

#if (PARSER == INTELLIBOX)
  //#define DEFAULT_BAUD      BAUD_2400  // like org. IBOX       
  #define DEFAULT_BAUD      BAUD_19200        
#endif

#define STORE_TURNOUT_POSITIONS     0   // if enabled, add code and sram memory for
                                        // local stored switch positions

#define TURNOUT_FEEDBACK_ENABLED    0   // if enabled, add code for turnout feedback

#define TURNOUT_FEEDBACK_ACTIVATED  0   // if enabled,
                                        // the corresponding mode-flags in EEPROM are set.
                                        // external S88-reading is disabled (see also config.c)

#define DCC_FAST_CLOCK     1            // 0: standard DCC
                                        // 1: add commands for DCC fast clock



//========================================================================
// 4. DCC Definitions
//========================================================================
// 4.a) defines for handling of DCC
//      (may be altered by user)
//------------------------------------------------------------------------

#define DCC_DEFAULT_FORMAT     DCC28    // may be DCC14, DCC28, DCC128               (--> CV)
                                        // This Format is reported if loco was never used
                                        // before; is loco was used once, the previous
                                        // format is reported.
                                        // In case of IB-Control:
                                        // This Format is used, when a speed command is given
                                        // to a loco never used before.

#define DCC_F13_F28            0        // 1: add code for functions F13 up to F28

#define DCC_SHORT_ADDR_LIMIT   112      // This is the maximum number for short addressing mode on DCC
#define XP_SHORT_ADDR_LIMIT    99       // This is the maximum number for short addressing mode on Xpressnet

#define NUM_DCC_SPEED_REPEAT   3        // Speed commands are repeated this number   (--> CV)
                                        // + one extra repetition if speed is decreased

#define NUM_DCC_ACC_REPEAT     2        // Accessory Commands are repeated this number (--> CV)

#define NUM_DCC_FUNC_REPEAT    0        // Function Commands are repeated this number (--> CV)

#define NUM_DCC_POM_REPEAT     3        // Program on the main are repeated this number (--> CV)

// note: in addition, there is the locobuffer, where all commands are refreshed
//       this locobuffer does not apply to accessory commands nor pom-commands

//-----------------------------------------------------------------------
// Sizes of Queues and Buffers -> see section 5, memory usage
//


//------------------------------------------------------------------------
// 4.b) variable defines for the state engine
//------------------------------------------------------------------------

typedef enum {DISCONNECTED, CONNECTED, RUNNING, STOPPED} t_rs232_led_state;

typedef enum {RUN_OKAY,     // DCC Running
              RUN_STOP,     // DCC Running, all Engines Emergency Stop
              RUN_OFF,      // Output disabled (2*Taste, PC)
              RUN_SHORT,    // Kurzschluss;
              RUN_PAUSE,    // DCC Running, all Engines Speed 0
              PROG_OKAY,    // Prog-Mode running 
              PROG_SHORT,   // Prog-Mode + Kurzschluss
              PROG_OFF,     // Prog-Mode, abgeschaltet
              PROG_ERROR    // Prog-Mode, Fehler beim Programmieren
              } t_opendcc_state;


//------------------------------------------------------------------------
// define a structure for DCC messages inside OpenDCC
//------------------------------------------------------------------------
#define   MAX_DCC_SIZE  6

// This enum defines the type of message put to the tracks.

typedef enum {is_void,      // message with no special handling (like functions)
              is_stop,      // broadcast
              is_loco,      // standard dcc speed command
              is_acc,       // accessory command
              is_feedback,  // accessory command with feedback
              is_prog,      // service mode - longer preambles
              is_prog_ack}  t_msg_type;


typedef struct
  {
    unsigned char repeat;             // counter for repeat or refresh (depending)
    union
     {
       struct
        {
         unsigned char size: 4;            // 2 .. 5
         t_msg_type    type: 4;            // enum: isvoid, isloco, accessory, ...
        } ;
       unsigned char qualifier;
     } ;
    unsigned char dcc[MAX_DCC_SIZE];  // the dcc content
  } t_message;



// define a structure for the loco memeory (6 bytes)

typedef enum {DCC14, DCC27, DCC28, DCC128} t_format;

struct locomem
  {
    unsigned int address;           // address (either 7 or 14 bits)
    unsigned char speed;            // this is in effect a bitfield:
                                    // msb = direction (1 = forward, 0=revers)
                                    // else used as integer, speed 1 ist NOTHALT
                                    // this is i.e. for 28 speed steps:
                                    // 0=stop
                                    // 1=emergency stop
                                    // 2..127: speed steps 1..126
                                    // speed is always stored as 128 speed steps
                                    // and only converted to the according format
                                    // when put on the rails (30.09.2006) 
    t_format format: 2;             // 00 = 14, 01=27, 10=28, 11=128 speed steps.
                                    // DCC27 is not supported
    unsigned char res1: 1;          // bit is reserved
    unsigned char fl: 1;            // function light
    unsigned char f4_f1: 4;         // function 4 downto 1
    unsigned char f8_f5: 4;         // function 8 downto 5
    unsigned char f12_f9: 4;        // function 12 downto 9
    unsigned char refresh;          // refresh is used as level: 0 -> refreshed often
  };

// Note on speed coding:
//
// Speed is always stored as 0..127.
// Speedentries from IBOX are handled directly.
// Speedentries from LENZ are converted (speed_from_rail) when they are put to
//                                      (speed_to_rail) or read from locobuffer.
// When a message is put on the rails, speed is converted according to format.


// define a structure for programming results

typedef enum 
  {
    PR_VOID     = 0x00, 
    PR_READY    = 0x01,     // 0x11, command station is ready
    PR_BUSY     = 0x02,     // 0x1f, busy
    PR_REGMODE  = 0x03,     // 0x10, register + values
    PR_CVMODE   = 0x04,     // 0x14, Last command was CV + data
    PR_SHORT    = 0x05,     // 0x12, short detected
    PR_NOTFOUND = 0x06,     // 0x13, no found
  } t_prog_summary;

// old: typedef enum {PR_VOID, PR_READY, PR_BUSY, PR_REGMODE, PR_CVMODE, PR_SHORT} t_lenz_result;

typedef struct
  {
    unsigned char minute; 
    unsigned char hour;
    unsigned char day_of_week;
    unsigned char ratio;
  } t_fast_clock;



//========================================================================
// 5. Usage of Memory, EEROM and Registers
//========================================================================
// Globals
//
extern const unsigned char opendcc_version PROGMEM;
extern unsigned char invert_accessory;

// 5.1. Registers
//------------------------------------------------------------------------
// unused IO-Regs, which may be used as speedup for Global Vars
//
// TWDR, TWAR, TWBR, SPDR, ADMUX
//

#define DCCOUT_STATE_REG    TWBR      // if nothing defined, dccout will use a global var. in sram


//------------------------------------------------------------------------
// 5.2. Interrupts
//------------------------------------------------------------------------
// Interrupt:     Prio  GIE   Module
// TIMER1_COMPA:  high  no    DCC-Signal Generator
// TIMER0_COMP:   ..    yes   Timetick every 5ms (defined by TICK_PERIOD)
// SIG_UART_RECV: ..    yes   Receive from Host
// SIG_UART_DATA: low   yes   Send to Host
//
// Note: with DMX turned on, the Interrupt System may be blocked for 44us,
// so be careful with the 58us Rate of DCC

//------------------------------------------------------------------------
// 5.3. Memory Usage - RAM
//------------------------------------------------------------------------
//
// RAM:   Size:     Usage
//         170      General
//          64      RS232 Rx
//          64      RS232 Tx
//         600      Locobuffer (Size * 6)
//                  DMX-Control
//                  S88-Buffer

#define SIZE_QUEUE_PROG       8       // programming queue (7 bytes each entry)
#define SIZE_QUEUE_LP        16       // low priority queue (7 bytes each entry)
#define SIZE_QUEUE_HP         8       // high priority queue (7 bytes each entry)
#define SIZE_REPEATBUFFER    16       // immediate repeat (7 bytes each entry)
#define SIZE_LOCOBUFFER      32       // no of simult. active locos (6 bytes each entry)
#if (STORE_TURNOUT_POSITIONS == 1)    
#define SIZE_TURNOUTBUFFER  128       // no of Turnouts / 8 (64 = 512 Turnouts)
#endif
#ifndef SIZE_TURNOUTBUFFER
#define SIZE_TURNOUTBUFFER    0       // not stored
#endif


// This union allows to access 16 bits as word or as two bytes.
// This approach is (probably) more efficient than shifting.
typedef union
{
    uint16_t as_uint16;
    uint8_t  as_uint8[2];
} t_data16;

#endif   // config.h
