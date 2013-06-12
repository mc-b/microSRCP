
/*
	microSRCPGL - Arduino Wave mit Adafruit Wave Shield

	Fuer weitere Details siehe https://github.com/mc-b/microSRCP/wiki
	und http://www.ladyada.net/make/waveshield/

	Copyright (c) 2010 - 2013 Marcel Bernet.  All right reserved.

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

// Debugging > 0 == ON
#define DEBUG_SCOPE 0

#include <Arduino.h>

#include <SRCPCommand.h>
#include <SRCPDevice.h>
#include <SRCPDeviceManager.h>
#include <SRCPFeedback.h>
#include <SRCPGenericAccessoire.h>
#include <SRCPGenericLoco.h>
#include <SRCPMessages.h>
#include <SRCPParser.h>
#include <SRCPServerSerial.h>
#include <SRCPSession.h>


#include <Wire.h>
#include <I2CDeviceManager.h>
#include <I2CFBProxy.h>
#include <I2CGAProxy.h>
#include <I2CGLProxy.h>
#include <I2CServer.h>

#include <ArduinoPins.h>
#include <FatReader.h>
#include <FatStructs.h>
#include <GAWave.h>
#include <mcpDac.h>
#include <SdInfo.h>
#include <SdReader.h>
#include <WaveDeviceManager.h>
#include <WaveHC.h>
#include <Wavemainpage.h>
#include <WavePinDefs.h>
#include <WaveUtil.h>

// SRCP I2C - Slave
i2c::I2CServer server = WireServer;

//////////////////////////////////////////////////////////////////////////////////////////
// Konfiguration I2C
#define I2C_ADDR		9	// Eigene I2C Adresse - muss pro I2C Board angepasst werden!
#define I2C_OFFSET		16	// Offset, d.h. wieviele Adressen pro Board reserviert werden
#define I2C_ENABLED		0
#define ADDR(x)			((I2C_ADDR * I2C_OFFSET) + x)	// Berechnung effektive Adresse
/**
 * Initialisierung - Protokoll, Geraete etc.
 */
void setup()
{
#if	( DEBUG_SCOPE > 1 )
	// Start each software serial port
	Serial.begin( 9600 );
#endif

	WaveDevManager.init();

        // ACHTUNG: zu viele Sounds fuehren zu einem Out of Memory!
	DeviceManager.addAccessoire( new wav::GAWave( ADDR(1), "GLA1") );
	DeviceManager.addAccessoire( new wav::GAWave( ADDR(2), "GLA2") );	
	//DeviceManager.addAccessoire( new wav::GAWave( ADDR(3), "S3") );
	//DeviceManager.addAccessoire( new wav::GAWave( ADDR(4), "S4") );	
	//DeviceManager.addAccessoire( new wav::GAWave( ADDR(5), "S5") );
	//DeviceManager.addAccessoire( new wav::GAWave( ADDR(6), "S6") );	

	// initialize I2C - Slave
	WireServer.begin( I2C_ADDR );

#if	( DEBUG_SCOPE > 1 )
	Serial.print ( "Server listen " );
	Serial.println( FreeRam() );
#endif
}

/**
 * Endlosloop
 */
void loop()
{
	// Host Meldungen verarbeiten
	server.dispatch();

	// Refresh der Sensoren bzw. Abfragen ob Aenderungen stattgefunden haben
	DeviceManager.refresh();

  	// Sound abspielen, welche mit setSound eingetragen wurden.
	WaveDevManager.play();
}
