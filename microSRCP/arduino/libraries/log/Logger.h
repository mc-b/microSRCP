/*
	Logger - der Apache Library Log4j nachempfunden. Verschiedene
	LogLevel definieren, welche Meldungen angezeigt werden sollen.

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

#ifndef LOGGER_H_
#define LOGGER_H_

#include <Arduino.h>
#include <HardwareSerial.h>
#include <SoftwareSerial.h>

/////////////////////////////////////////////////////////////////////////////////
// Meldungen
#define	OFF_LEVEL		0		// keine Meldungen
#define	ERROR_LEVEL		1 		// Fehler in der Programmausfuehrung
#define	WARN_LEVEL		2		// Warnungen
#define	INFO_LEVEL		3		// Informationen, Start, Initialisierung i.o. etc.
#define	DEBUG_LEVEL		4		// Allgemeine Debug Infos
#define	TRACE_LEVEL		5		// Fein Detalierte Meldungen, nur in Ausnahmefaellen zu aktivieren

#if ( __AVR_ATmega1280__ || __AVR_ATmega2560__ )
#define	LOGGER_LEVEL	INFO_LEVEL
#else
#define LOGGER_LEVEL 	OFF_LEVEL
#endif

/**
 * Allgemeine Initialisierungen
 */
#if ( LOGGER_LEVEL > OFF_LEVEL)

#if ( __AVR_ATmega1280__ || __AVR_ATmega2560__ )
#define Logger	Serial3
#elif ( __AVR_ATmega328P__ )
extern	SoftwareSerial Logger;
#endif

#define BEGIN(x) 	Logger.begin( x );
#else
#define BEGIN(x)
#endif

/**
 * Error
 */
#if ( LOGGER_LEVEL >= ERROR_LEVEL )
#define ERROR(str) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.println('\"');
#else
#define ERROR(str)
#endif

/**
 * Warning
 */
#if ( LOGGER_LEVEL >= WARN_LEVEL )
#define WARN(str) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.println('\"');
#else
#define WARN(str)
#endif

/**
 * Info
 */
#if ( LOGGER_LEVEL >= INFO_LEVEL )
#define INFO(str) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.println('\"');
#else
#define INFO(str)
#endif

#if ( LOGGER_LEVEL >= INFO_LEVEL )
#define INFO2(str, v) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.print('\"'); \
		Logger.print( ":" ); \
		Logger.println( v );
#else
#define INFO2(str, v)
#endif

#if ( LOGGER_LEVEL >= INFO_LEVEL )
#define INFO3(str, v, x ) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.print('\"'); \
		Logger.print( ":" ); \
		Logger.println( v ); \
		Logger.print( ", " ); \
		Logger.println( x );
#else
#define INFO3(str, v, x)
#endif

/**
 * Debug
 */
#if ( LOGGER_LEVEL >= DEBUG_LEVEL )
#define DEBUG(str) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.println('\"');
#else
#define DEBUG(str)
#endif

#if ( LOGGER_LEVEL >= DEBUG_LEVEL )
#define DEBUG2(str, v) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.print('\"'); \
		Logger.print( ":" ); \
		Logger.println( v );
#else
#define DEBUG2(str, v)
#endif

#if ( LOGGER_LEVEL >= DEBUG_LEVEL )
#define DEBUG3(str, v, x ) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.print('\"'); \
		Logger.print( ":" ); \
		Logger.println( v ); \
		Logger.print( ", " ); \
		Logger.println( x );
#else
#define DEBUG3(str, v, x)
#endif

/**
 * Trage
 */
#if ( LOGGER_LEVEL >= TRACE_LEVEL )
#define TRACE(str) \
		Logger.print(millis());     \
		Logger.print(": ");    \
		Logger.print(__PRETTY_FUNCTION__); \
		Logger.print(' ');      \
		Logger.print(__FILE__);     \
		Logger.print(':');      \
		Logger.print(__LINE__);     \
		Logger.print(" \"");      \
		Logger.print(str); \
		Logger.println('\"');
#else
#define TRACE(str)
#endif


#endif /* LOGGER_H_ */
