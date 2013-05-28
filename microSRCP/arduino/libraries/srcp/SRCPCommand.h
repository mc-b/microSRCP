/*
	SRCPCommand - Hilfsklasse definiert alle wichtigen Konstanten
	und Strukturen fuer die SRCP Kommunikation.

	Siehe auch: http://srcpd.sourceforge.net/srcp/

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

#ifndef SRCPCOMMAND_H_
#define SRCPCOMMAND_H_

#include <inttypes.h>

#define SRCP_MAX_ARGS 15

namespace srcp
{

/**
 * Folgende Kommandos sind definiert:
    GET Abfrage von Werten
    SET Setzen von Werten
    CHECK Prüft einen Befehl
    WAIT Wartet auf Werte
    INIT Initialisierung von Elementen
    TERM Beendet von mit INIT initialisierte Elemente
    RESET Reinitialisiert ein Element
    VERIFY Überprüft die Einstellungen eines Elements
 */

	enum commands { GET, SET, GO, CONNECTIONMODE, CHECK, WAIT, INIT, TERM, RESET, VERIFY, PROTOCOL, HELP, UNKNOWN };
	enum devices { POWER, FB, GA, GL, SM, DESCRIPTION, CV, LAN, NA };

	struct command_t
	{
		commands cmd;
		int bus;
		devices device;
		int addr;
		union
		{
			int values[SRCP_MAX_ARGS];
			char args[SRCP_MAX_ARGS*2];
		};
	};

	// Protokoll zustaende
	enum { UNDEFINED, HANDSHAKE, COMMAND, INFO };
	// Power
	enum power_enum { ON, OFF };
}

// Muss im main() definiert werden, fuer alle anderen extern
extern srcp::command_t global_cmd;

#endif /* SRCPCOMMAND_H_ */
