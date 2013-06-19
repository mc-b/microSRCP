/*
	SRCPDeviceManager leitet die SRCP Befehle an die Geraete weiter

	Siehe auch: http://srcpd.sourceforge.net/srcp/

	Copyright (c) 2012 Marcel Bernet.  All right reserved.

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

#include "SRCPDeviceManager.h"

#include "SRCPGenericAccessoire.h"
#include "SRCPDevice.h"

namespace srcp
{

SRCPDeviceManager::SRCPDeviceManager()
{
	firstGA = (srcp::SRCPGenericAccessoire*) 0;
	firstGL = (srcp::SRCPGenericLoco*) 0;
	firstFB = (srcp::SRCPFeedback*) 0;
}

int SRCPDeviceManager::setGA( int addr, int port, int value, int delay )
{
	SRCPGenericAccessoire* next = (srcp::SRCPGenericAccessoire*) 0;
	int rc = 200;

	for	( next = firstGA; next != (srcp::SRCPGenericAccessoire*) 0; next = next->nextElement() )
	{
		if	( next->checkAddr( addr ) == 0 )
			continue;
		rc = next->set( addr, port, value, delay );
	}
	return	( rc );

}
int SRCPDeviceManager::setGL( int addr, int drivemode, int v, int v_max, int fn[] )
{
	SRCPGenericLoco* next = (srcp::SRCPGenericLoco*) 0;
	int rc = 200;

	for	( next = firstGL; next != (srcp::SRCPGenericLoco*) 0; next = next->nextElement() )
	{
		if	( next->checkAddr( addr ) == 0 )
			continue;
		rc = next->set( addr, drivemode, v, v_max, fn );
	}
	return	( rc );
}

void SRCPDeviceManager::setPower( int on )
{
	SRCPGenericAccessoire* ga = (srcp::SRCPGenericAccessoire*) 0;
	for	( ga = firstGA; ga != (srcp::SRCPGenericAccessoire*) 0; ga = ga->nextElement() )
		ga->setPower( on );

	SRCPGenericLoco* gl = (srcp::SRCPGenericLoco*) 0;
	for	( gl = firstGL; gl != (srcp::SRCPGenericLoco*) 0; gl = gl->nextElement() )
		gl->setPower( on );
}

void SRCPDeviceManager::refresh()
{
	for	( SRCPFeedback* next = firstFB; next != (srcp::SRCPFeedback*) 0; next = next->nextElement() )
			next->refresh();
	SRCPGenericAccessoire* ga = (srcp::SRCPGenericAccessoire*) 0;
	for	( ga = firstGA; ga != (srcp::SRCPGenericAccessoire*) 0; ga = ga->nextElement() )
		ga->refresh();

	SRCPGenericLoco* gl = (srcp::SRCPGenericLoco*) 0;
	for	( gl = firstGL; gl != (srcp::SRCPGenericLoco*) 0; gl = gl->nextElement() )
		gl->refresh();

}

int SRCPDeviceManager::getFB( int addr )
{
	SRCPFeedback* fb = (srcp::SRCPFeedback*) 0;
	for	( fb = firstFB; fb != (srcp::SRCPFeedback*) 0; fb = fb->nextElement() )
	{
		if	( fb->checkAddr( addr ) == 0 )
			continue;
		return	( fb->get( addr ) );
	}
	return	( 0 );
}

int SRCPDeviceManager::getDescription( int bus, int addr, int device, int rc[] )
{
	if	( addr == 0 && bus == 0 && device == LAN )
	{
		rc[0] = 0;
		rc[1] = 0;
		SRCPFeedback* fb = (srcp::SRCPFeedback*) 0;
		for	( fb = firstFB; fb != (srcp::SRCPFeedback*) 0; fb = fb->nextElement() )
		{
			// das 1. Mal
			if	( rc[0] == 0 )
			{
				rc[0] = fb->getStartAddr();
				rc[1] = fb->getEndAddr();
				continue;
			}
			rc[0] = (fb->getStartAddr() < rc[0]) ? fb->getStartAddr() : rc[0];
			rc[1] = (fb->getEndAddr()   > rc[1]) ? fb->getEndAddr()   : rc[1];
		}
		rc[2] = 0;
		rc[3] = 0;
		SRCPGenericAccessoire* ga = (srcp::SRCPGenericAccessoire*) 0;
		for	( ga = firstGA; ga != (srcp::SRCPGenericAccessoire*) 0; ga = ga->nextElement() )
		{
			// das 1. Mal
			if	( rc[2] == 0 )
			{
				rc[2] = ga->getStartAddr();
				rc[3] = ga->getEndAddr();
				continue;
			}
			rc[2] = (ga->getStartAddr() < rc[2]) ? ga->getStartAddr() : rc[2];
			rc[3] = (ga->getEndAddr()   > rc[3]) ? ga->getEndAddr()   : rc[3];
		}
		rc[4] = 0;
		rc[5] = 0;
		SRCPGenericLoco* gl = (srcp::SRCPGenericLoco*) 0;
		for	( gl = firstGL; gl != (srcp::SRCPGenericLoco*) 0; gl = gl->nextElement() )
		{
			// das 1. Mal
			if	( rc[4] == 0 )
			{
				rc[4] = gl->getStartAddr();
				rc[5] = gl->getEndAddr();
				continue;
			}
			rc[4] = (gl->getStartAddr() < rc[4]) ? gl->getStartAddr() : rc[4];
			rc[5] = (gl->getEndAddr()   > rc[5]) ? gl->getEndAddr()   : rc[5];
		}
	}
	// 3 x 2 Integer
	return	( 12 );
}

} /* namespace srcp */

/** Return the number of bytes currently free in RAM. */
int freeRAM( void )
{
	extern int __bss_end;
	extern int *__brkval;
	int free_memory;
	if ( (int) __brkval == 0 )
	{
		// if no heap use from end of bss section
		free_memory = ((int) &free_memory) - ((int) &__bss_end);
	}
	else
	{
		// use from top of stack to heap
		free_memory = ((int) &free_memory) - ((int) __brkval);
	}
	return free_memory;
}

/** Globaler Device Manager */
srcp::SRCPDeviceManager DeviceManager = srcp::SRCPDeviceManager();

