/*
	parser - enum als XXX_parser.h welche nicht mehr
	benoetigt wird.

	Copyright (c) 2010 Marcel Bernet.  All right reserved.

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PARSER_H_
#define PARSER_H_

extern t_opendcc_state opendcc_state;            // this is the current state of the box

// -------- broadcast messages
typedef enum {ALL_OFF,        // Nothalt
              ALL_ON,         // ein
		      ALL_LOCO_OFF,   // speed 0
		      PROGMODE}       // Programmiermode
			  t_BC_message;

// FIXME unsigned char programmer_busy(void);


#endif /* PARSER_H_ */
