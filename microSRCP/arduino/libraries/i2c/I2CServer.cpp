/*
	I2CServer - SRCP Server fuer den I2C Bus.
	Aufbau wie SRCPSession empfaengt die Signale vom I2C Bus
	und leitet sie an die lokalen Geraete weiter, fragt Sensoren
	ab, etc.

	Im Sketch als WireServer ansprechbar.

	ACHTUNG: lange Debuggingausgaben in den Events koennen
	den gesamten Arduino blockieren!!!

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
#include <Wire.h>
#include "I2CServer.h"
#include "../srcp/SRCPDeviceManager.h"


// Preinstantiate Objects //////////////////////////////////////////////////////

i2c::I2CServer WireServer = i2c::I2CServer();

namespace i2c
{
// Global Commando Buffer, sonst ist keine Uebergabe moeglich.
srcp::command_t global_cmd;

/**
 * Empfange Daten von I2C Master - Abhandlung SRCP SET fuer alle anderen nur abstellen der Argumente
 */
void I2CServer::slaveRxEvent( int size )
{
	// CMD Struktur loeschen bzw. alle Werte welchen nicht immer gesetzt werden (z.B. Bus welche nicht uebertragen wird)
	memset( global_cmd.values, 0, sizeof(global_cmd.values) );
	global_cmd.bus = 0;

	if	( size >= 4 )
	{
		global_cmd.device 	= (srcp::devices) Wire.read();
		global_cmd.cmd 	= (srcp::commands) Wire.read();
		char buf[2];
		Wire.readBytes( buf, 2 );
		memcpy( &global_cmd.addr, buf, 2 );
	}
	else
		for	( int i = 0; i < size; i++ )
		{
			int d = Wire.read();
#if	( DEBUG_SCOPE > 2 )
	Serial3 << "unknown: " << d << endl;
#endif
			return;
		}

	// Restliche Parameter abstellen, falls vorhanden
	for	( int i = 4; i < size; i++ )
		global_cmd.values[i-4] = Wire.read();


#if	( DEBUG_SCOPE > 2 )
	Serial3 << "recv: " << global_cmd.cmd << ":" << global_cmd.bus << ":" << global_cmd.device << ":" << global_cmd.addr << " ";
	for	( int i = 0; i < SRCP_MAX_ARGS; i++ )
		Serial3 << ":" << global_cmd.values[i];
	Serial3.println();
#endif
	onReceive( global_cmd );
}

/**
 * Sende Daten an I2C Master - Abhandlung SRCP GET (mit Argumenten aus WireServer.dispatch)
 */
void I2CServer::slaveTxEvent()
{
#if	( DEBUG_SCOPE > 2 )
		Serial3 << "send: ";
#endif

	int len = onRequest( global_cmd );

	if	( len > 0 )
	{
#if	( DEBUG_SCOPE > 2 )
		Serial3 << global_cmd.cmd << ", addr " << global_cmd.addr << ", dev " << global_cmd.device << ",size " << len;
		for	( int i = 0; i < len; i++ )
			Serial3 << ":" << (int) global_cmd.args[i];
		Serial3.println();
#endif
		Wire.write( (uint8_t*) global_cmd.args, len );
	}
	// Error
	else
	{
#if	( DEBUG_SCOPE > 2 )
		Serial3.println( "error");
#endif
		Wire.write( -1 );
	}

}

void I2CServer::begin( int addr )
{
	// initialize the wire device and register event
	Wire.onReceive( I2CServer::slaveRxEvent );
	Wire.onRequest( I2CServer::slaveTxEvent );

	Wire.begin( addr );
}

void I2CServer::onReceive( srcp::command_t& cmd )
{
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

int I2CServer::onRequest( srcp::command_t& cmd )
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
					return	( DeviceManager.getDescription( cmd.values[0], cmd.addr, cmd.values[1], cmd.values ) );
				default:
					break;
			}
		default:
			break;
	}
	return	( 1 );
}

}
