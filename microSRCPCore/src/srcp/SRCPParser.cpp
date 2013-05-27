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

void SRCPParser::parse( char* args, int length )
{
	char d[10];
	memset( global_cmd.values, 0, sizeof(global_cmd.values) );

	global_cmd.cmd = UNKNOWN;
	global_cmd.device = NA;
	global_cmd.args[0] = '\0';

	if (strncasecmp(args, "SET CONNECTIONMODE SRCP", 23) == 0  )
	{
		global_cmd.cmd = CONNECTIONMODE;
		global_cmd.values[0] = COMMAND;
		strcpy( d, args+24 );
		if	( strncasecmp(d, "INFO", 4) == 0 )
			global_cmd.values[0] = INFO;
	}
	else if ( strncasecmp( args, "SET PROTOCOL SRCP", 17) == 0 )
	{
		global_cmd.cmd = PROTOCOL;
		strcpy( global_cmd.args, args+18 );
	}
	else if (strncasecmp(args, "SET", 3) == 0)
	{
		// GL hat max. 12 (15 - 3) Funktionen, siehe SRCP_MAX_ARGS
		global_cmd.cmd = SET;
		// Bus, Devices, Adresse sind immer vorhanden.
		sscanf( args, "%*s %d %s %d", &global_cmd.bus, d, &global_cmd.addr );
		global_cmd.device = getDevice( d );

		switch	( global_cmd.device )
		{
			case	POWER:
				sscanf( args,  "%*s %*d %*s %s", global_cmd.args );
				if	( strncasecmp( global_cmd.args, "ON", 2) == 0 )
					global_cmd.values[0] = ON;
				else
					global_cmd.values[0] = OFF;
				break;

			case	SM:
				sscanf( args, "%*s %*d %*s %*d %s %d %d", d, &global_cmd.values[1], &global_cmd.values[2] );
				global_cmd.values[0] = getDevice( d );		// Device an welche der SM Befehle geschickt wird.
				break;

			// GL, GA
			default:
				sscanf( args, "%*s %*d %*s %*d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &global_cmd.values[0], &global_cmd.values[1], &global_cmd.values[2],
						&global_cmd.values[3], &global_cmd.values[4], &global_cmd.values[5], &global_cmd.values[6], &global_cmd.values[7], &global_cmd.values[8], &global_cmd.values[9],
						&global_cmd.values[10], &global_cmd.values[11], &global_cmd.values[12], &global_cmd.values[13], &global_cmd.values[14] );
				break;
		}
	}
	else if (strncasecmp(args, "GET", 3) == 0)
	{
		global_cmd.cmd = GET;
		// Bus, Devices, Adresse sind immer vorhanden.
		sscanf( args, "%*s %d %s %d", &global_cmd.bus, d, &global_cmd.addr );
		global_cmd.device = getDevice( d );

		switch	( global_cmd.device )
		{
			case SM:
				sscanf( args, "%*s %*d %*s %*d %s %d", d, &global_cmd.values[1] );
				global_cmd.values[0] = getDevice( d ); // betrifft Device bzw. SM Befehl fuer Device
				break;
		}
	}
	else if (strncasecmp(args, "WAIT", 4) == 0)
		global_cmd.cmd = WAIT;
	else if (strncasecmp(args, "INIT", 4) == 0)
		global_cmd.cmd = INIT;
	else if (strncasecmp(args, "TERM", 4) == 0)
		global_cmd.cmd = TERM;
	else if (strncasecmp(args, "GO", 2) == 0)
		global_cmd.cmd = GO;
	else
		global_cmd.cmd = UNKNOWN;
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

