/*
	SRCPParser - Parst ASCII SRCP Messages

	Siehe auch: http://srcpd.sourceforge.net/srcp/

	Copyright (c) 2013 Marcel Bernet.  All right reserved.

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

#include "SRCPParser.h"

namespace srcp
{

/**
 * Parst den I/O Buffer und stellt das Ergebniss in globaler cmd Struct ab.
 */
void SRCPParser::parse( command_t& cmd, char* args )
{
	char d[10];
	memset( cmd.values, 0, sizeof(cmd.values) );

	cmd.cmd = UNKNOWN;
	cmd.device = NA;
	cmd.args[0] = '\0';

	if (strncasecmp(args, "SET CONNECTIONMODE SRCP", 23) == 0  )
	{
		cmd.cmd = CONNECTIONMODE;
		cmd.values[0] = COMMAND;
		strcpy( d, args+24 );
		if	( strncasecmp(d, "INFO", 4) == 0 )
			cmd.values[0] = INFO;
	}
	else if ( strncasecmp( args, "SET PROTOCOL SRCP", 17) == 0 )
	{
		cmd.cmd = PROTOCOL;
		strcpy( cmd.args, args+18 );
	}
	else if (strncasecmp(args, "SET", 3) == 0)
	{
		// GL hat max. 12 (15 - 3) Funktionen, siehe SRCP_MAX_ARGS
		cmd.cmd = SET;
		// Bus, Devices, Adresse sind immer vorhanden.
		sscanf( args, "%*s %d %s %d", &cmd.bus, d, &cmd.addr );
		cmd.device = getDevice( d );

		switch	( cmd.device )
		{
			case	POWER:
				sscanf( args,  "%*s %*d %*s %s", cmd.args );
				if	( strncasecmp( cmd.args, "ON", 2) == 0 )
					cmd.values[0] = ON;
				else
					cmd.values[0] = OFF;
				break;

			case	SM:
				sscanf( args, "%*s %*d %*s %*d %s %d %d", d, &cmd.values[1], &cmd.values[2] );
				cmd.values[0] = getDevice( d );		// Device an welche der SM Befehle geschickt wird.
				break;

			// GL, GA
			default:
				sscanf( args, "%*s %*d %*s %*d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &cmd.values[0], &cmd.values[1], &cmd.values[2],
						&cmd.values[3], &cmd.values[4], &cmd.values[5], &cmd.values[6], &cmd.values[7], &cmd.values[8], &cmd.values[9],
						&cmd.values[10], &cmd.values[11], &cmd.values[12], &cmd.values[13], &cmd.values[14] );
				break;
		}
	}
	else if (strncasecmp(args, "GET", 3) == 0)
	{
		cmd.cmd = GET;
		// Bus, Devices, Adresse sind immer vorhanden.
		sscanf( args, "%*s %d %s %d", &cmd.bus, d, &cmd.addr );
		cmd.device = getDevice( d );

		switch	( cmd.device )
		{
			case SM:
				sscanf( args, "%*s %*d %*s %*d %s %d", d, &cmd.values[1] );
				cmd.values[0] = getDevice( d ); // betrifft Device bzw. SM Befehl fuer Device
				break;
		}
	}
	else if (strncasecmp(args, "WAIT", 4) == 0)
		cmd.cmd = WAIT;
	else if (strncasecmp(args, "INIT", 4) == 0)
		cmd.cmd = INIT;
	else if (strncasecmp(args, "TERM", 4) == 0)
		cmd.cmd = TERM;
	else if (strncasecmp(args, "GO", 2) == 0)
		cmd.cmd = GO;
	else
		cmd.cmd = UNKNOWN;
}

devices SRCPParser::getDevice( char* device )
{
	if	( strncasecmp(device, "POWER", 5) == 0 )
		return	( POWER );
	else if	( strncasecmp(device, "FB", 2) == 0 )
		return	( FB );
	else if	( strncasecmp(device, "GA", 2) == 0 )
		return	( GA );
	else if	( strncasecmp(device, "GL", 2) == 0 )
		return	( GL );
	else if	( strncasecmp(device, "SM", 2) == 0 )
		return	( SM );
	// Pseudo Device - Konfigurationsvariablen abfragen
	else if	( strncasecmp(device, "CV", 2) == 0 )
		return	( CV );

	return	( NA );
}

} /* namespace srcp */

