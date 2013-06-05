/*
	GAOpenDCC - Steuerung von Zubehoer wie Weichen, Signale
	anhand des DCC Signals.

	Copyright (c) 2010 Marcel Bernet.  All right reserved.

	Der Code wurde ermoeglicht durch das OpenDCC Projekt und
	die Zurverfuegungstellung der Sourcen von Wolfgang Kufer.
	Besten Dank!

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

#ifndef GAOPENDCC_H_
#define GAOPENDCC_H_

#include "../srcp/SRCPGenericAccessoire.h"

namespace odcc
{

class GAOpenDCC : public srcp::SRCPGenericAccessoire
{
private:
	int startAddr;
	int endAddr;
public:
	GAOpenDCC( int startAddr, int endAddr );
	int set( int addr, int port, int value, int delay );
	int checkAddr( int addr ) { return ( addr >= startAddr && addr <= endAddr); }
	int	getStartAddr() { return( this->startAddr ); };
	int getEndAddr() { return( this->endAddr ); }
};

}

#endif /* GAOPENDCC_H_ */
