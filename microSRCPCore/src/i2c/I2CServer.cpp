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

#include <Arduino.h>
#include <Streaming.h>
#include "I2CServer.h"
#include "../srcp/SRCPDeviceManager.h"

extern "C"
{
#include <twi.h>
}

// Preinstantiate Objects //////////////////////////////////////////////////////

i2c::I2CServer WireServer = i2c::I2CServer();

namespace i2c
{

/**
 * Empfange Daten von I2C Master - Abhandlung SRCP SET fuer alle anderen nur abstellen der Argumente
 */
void I2CServer::slaveRxEvent( uint8_t* rxBuf, int size )
{
	WireServer.dispatch( rxBuf, size );
}

/**
 * Sende Daten an I2C Master - Abhandlung SRCP GET (mit Argumenten aus WireServer.dispatch)
 */
void I2CServer::slaveTxEvent()
{
	int len = WireServer.dispatchTx();

	if	( len > 0 )
	{
#if	( DEBUG_SCOPE > 2 )
		Serial << "send " << WireServer.cmd.cmd << ", addr " << WireServer.cmd.addr << ", dev " << WireServer.cmd.device
				<< ", rc " << (int) WireServer.cmd.args[0] << endl;
#endif
		twi_transmit( (uint8_t*) WireServer.cmd.args, len );
	}
}

void I2CServer::begin( int addr )
{
	// initialize the wire device and register event
	twi_init();
	twi_attachSlaveRxEvent( I2CServer::slaveRxEvent );
	twi_attachSlaveTxEvent( I2CServer::slaveTxEvent );
	twi_setAddress( addr );
}

void I2CServer::dispatch(uint8_t* args, int size )
{
	cmd.device = (srcp::devices) ((args[0]));
	cmd.cmd = (srcp::commands) ((args[1]));
	memcpy( &cmd.addr, &args[2], 2 );

	// FB und Power Devices haben keine Argumente
	if ( cmd.device != srcp::FB && cmd.device != srcp::POWER )
		for ( int i = 4; i < size; i++ )
			cmd.values[i - 4] = (int) ((args[i]));

#if	( DEBUG_SCOPE > 2 )
	Serial << "recv " << cmd.cmd << ", addr " << cmd.addr << ", dev " << cmd.device << endl;
#endif

	switch (cmd.cmd)
	{
		case srcp::SET:
			switch (cmd.device)
			{
				case srcp::POWER:
					DeviceManager.setPower( cmd.addr );
					break;

				case srcp::GA:
					cmd.values[0] = DeviceManager.setGA( cmd.addr, cmd.values[0], cmd.values[1], cmd.values[2] );
					break;

				case srcp::GL:
					cmd.values[0] = DeviceManager.setGL( cmd.addr, cmd.values[0], cmd.values[1], cmd.values[2], cmd.values );
					break;

				case srcp::SM:
					DeviceManager.setSM( cmd.values[0], cmd.addr, cmd.values[1], cmd.values[2], cmd.values[3] );
					break;

				default:
					break;
			}
			break;
		default:
			break;
	}
}

int I2CServer::dispatchTx()
{
	switch (cmd.cmd)
	{
		case srcp::GET:
			switch (cmd.device)
			{
				case srcp::FB:
					cmd.args[0] = DeviceManager.getFB( cmd.addr );
					return	( 1 );

				case srcp::SM:
					cmd.args[0] = DeviceManager.getSM( cmd.values[0], cmd.addr, cmd.values[1], cmd.values[2] );
					return ( 1 );

				case srcp::DESCRIPTION:
				{
					int size = DeviceManager.getDescription( cmd.values[0], cmd.addr, cmd.values[1], cmd.values );
#if	( DEBUG_SCOPE > 2 )
	Serial << "description: fb " << cmd.values[0] << "-" << cmd.values[1] << ", ga " << cmd.values[2]
	       << "-" << cmd.values[3] << ", gl " << cmd.values[4] << "-" << cmd.values[5] << endl;
#endif
					return	( size );
				}
				default:
					break;
			}
		default:
			break;
	}
	return	( 0 );
}

}
