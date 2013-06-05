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
// file:      organizer.h
// author:    Wolfgang Kufer
// contact:   kufer@gmx.de
// history:   2006-04-13 V0.1 started
//            2006-10-17 V0.2 added turnout buffer
//            2007-03-12 V0.3 added pom
//            2007-11-20 V0.4 type mismatch in store_loco_format.
//			  2010-01-24 Angepasst an Arduinoboard mit ATmega328P, Marcel Bernet
//
//-----------------------------------------------------------------
//
// purpose:   lowcost central station for dcc
// content:   queue for dcc messages
//            memory for actual loco states
//            engines for repeat and refresh of dcc messages
//
// interface upstream: 
//            init_organizer(void)  // set up the queue structures
//            run_organizer(void)   // multitask replacement, must be called
//                                  // every 2ms (approx)
//
//            bool dodcc(message*)  // put this message in the queues, returns
//                                  // error if full
//            bool do_loco_speed(loco, speed)
//                                  // change speed of this loco
//
// interface downstream: 
//            uses "next_message..." flags to interact with dccout
//
//-----------------------------------------------------------------


//------------------------------------------------------------------------
// define the structures for DCC messages
//------------------------------------------------------------------------
// SIZE_... : see config.h
//-------------------------------------- primary command buffer (fifo)

extern t_message queue_lp[SIZE_QUEUE_LP];          // low priority
 
extern t_message queue_hp[SIZE_QUEUE_HP];          // high priority

extern t_message repeatbuffer[SIZE_REPEATBUFFER];  // instant repeat

extern struct locomem locobuffer[SIZE_LOCOBUFFER];      // refresh

// ----------------------------------------------------------------
// predefined messages
//
// stored in bss, copied at start to sram

extern t_message DCC_Reset;    // DCC-Reset-Paket
extern t_message DCC_Idle    ;    // DCC-Idle-Paket

// extern t_message DCC_BC_Stop ;    // Broadcast Motor off: // 01DC000S :D=x, C=1 (ignore D)
// extern t_message DCC_BC_Brake ;    // Broadcast Slow down // if S=0: slow down

//------------------------------------------------------------------
// Upstream Interface for parser
//------------------------------------------------------------------

void init_organizer(void);                                      // must be called once at program start

void run_organizer(void);                                       // must be called in a loop!

extern unsigned char organizer_halt_state;                      // if != 0: speed commands are locally forced to zero


// -- routines for command entry
bool organizer_ready(void);                                     // true if command can be accepted

unsigned char convert_speed_to_rail(unsigned char speed128, t_format format);
unsigned char convert_speed_from_rail(unsigned char speed, t_format format);

unsigned char do_loco_speed(unsigned int addr, unsigned char speed);     // eine Lok eintragen (speed 1-127), will be converted to format
unsigned char do_loco_speed_f(unsigned int addr, unsigned char speed, t_format format);      // eine Lok eintragen (incl. format)
unsigned char do_loco_func_grp0(unsigned int addr, unsigned char funct); 
unsigned char do_loco_func_grp1(unsigned int addr, unsigned char funct); 
unsigned char do_loco_func_grp2(unsigned int addr, unsigned char funct); 
unsigned char do_loco_func_grp3(unsigned int addr, unsigned char funct); 

void do_all_stop(void);
bool do_accessory(unsigned int addr, unsigned char output, unsigned char activate);      // turnout

bool do_pom_loco(unsigned int addr, unsigned int cv, unsigned char data);                     // program on the main

bool do_pom_accessory(unsigned int addr, unsigned int cv, unsigned char data);                // program on the main

#if (DCC_FAST_CLOCK == 1)
 bool do_fast_clock(t_fast_clock* my_clock);
#endif


t_format get_loco_format(unsigned int addr);

unsigned char store_loco_format(unsigned int addr, t_format format);

unsigned int addr_inquiry_locobuffer(unsigned int addr, unsigned char dir);    // returns next addr in buffer

void delete_from_locobuffer(unsigned int addr);

//------------------------------------------------------------------

void save_turnout(unsigned int addr, unsigned char output);

unsigned char recall_turnout(unsigned int addr);  // addr 0.. 

unsigned char recall_turnout_group(unsigned int group_addr);

//------------------------------------------------------------------
// Upstream Interface for programmer
//------------------------------------------------------------------


bool queue_prog_is_empty(void);

unsigned char put_in_queue_prog(t_message *new_message);


//-------------------------------------------------------------------------------------------------
// the following links only for debugging!!
//------------------------------------------------------------------

void init_locobuffer(void);

void build_loko_7a28s(unsigned int nr, signed char speed, t_message *new_message);
  
void build_loko_7a128s(unsigned int nr, signed char speed, t_message *new_message);
 
void build_loko_14a28s(int nr, int speed, t_message *new_message);
 
void build_loko_14a128s(int nr, int speed, t_message *new_message);

void build_nmra_basic_accessory(unsigned int nr, char output, char activate, t_message *new_message);

void build_nmra_extended_accessory(unsigned int nr, char aspect, t_message *new_message);

void build_function_short_grp1(int nr, unsigned char func, t_message *new_message);



//--------------------------------------------------------------------------
// Routines for locobuffer
//--------------------------------------------------------------------------
//
// purpose:   creates a flexible refresh of loco speeds
//
// how:       every speed command is entered to locobuffer.
//            search_locobuffer return a message of the loco
//            to be refreshed.
//            "younger" locos are refreshed more often.
//
// interface: set_loco_mode (int addr, format)
//            init_locobuffer ()


#define ORGZ_SLOW_DOWN  0    // Bit 0: last entry to locobuffer slowed down
#define ORGZ_STOLEN     1    // Bit 1: locomotive has been stolen
#define ORGZ_FULL       7    // Bit 7: organizer fully loaded



unsigned char enter_speed_to_locobuffer(unsigned int addr, unsigned char speed);                      // returns result
 
unsigned char enter_speed_f_to_locobuffer(unsigned int addr, unsigned char speed, t_format format);   // returns result

unsigned char enter_func_to_locobuffer(unsigned int addr, unsigned char funct, unsigned char grp);   // returns result

unsigned char last_locobuffer_index(void);     // returns index of last enter

 
t_message * search_locobuffer(void);   // returns pointer to dcc message

bool put_in_queue_lp(t_message *new_message);
 
void set_next_message (t_message *next);


