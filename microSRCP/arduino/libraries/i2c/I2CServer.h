/*
	I2CServer - SRCP Server fuer den I2C Bus.
	Aufbau wie SRCPSession empfaengt die Signale vom I2C Bus
	und leitet sie an die lokalen Geraete weiter, fragt Sensoren
	ab, etc.

	Im Sketch als WireServer ansprechbar.

	Beispiel fuer eine Implementation sie SRCPGASlave oder
	SRCPGLSave.

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

#ifndef I2CSERVER_H_
#define I2CSERVER_H_

#include <Arduino.h>
#include "../srcp/SRCPCommand.h"

namespace i2c
{

class I2CServer
{
private:
	static void onReceive( srcp::command_t& cmd );
	static int onRequest( srcp::command_t& cmd );
	static void slaveRxEvent( int size );
	static void slaveTxEvent();
public:

	void begin( int addr = 1 );
	// ohne Funktion, send/receive erfolgt via Events
	void dispatch() {};
};

}
extern i2c::I2CServer WireServer;

#endif /* I2CSERVER_H_ */
