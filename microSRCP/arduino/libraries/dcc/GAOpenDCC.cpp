/*
	GAOpenDCC - Steuerung von Zubehoer wie Weichen, Signale
	anhand des DCC Signals.

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

#include "GAOpenDCC.h"

namespace dcc
{

GAOpenDCC::GAOpenDCC( int startAddr, int endAddr )
{
	this->startAddr = startAddr;
	this->endAddr = endAddr;
}

int GAOpenDCC::set( int addr, int port, int value, int delay )
{
	// TODO wie ist es mit an und Abschalten des Powers?
/*	do_accessory( addr, 0, 0 );
	do_accessory( addr, 1, 0 );
	do_accessory( addr, port, 1 );*/

	return	( 200 );
}

}
