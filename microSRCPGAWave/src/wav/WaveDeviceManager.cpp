/*
 WaveDeviceManager - Erzeugt die Wave Geraete und stellt
 allgemeine Funktionen zum abspielen von Sounds zur
 Verfuegung.

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

#include <Arduino.h>
#if	( DEBUG_SCOPE > 0 )
#include <HardwareSerial.h>
#include <Streaming.h>
#endif
#include "WaveDeviceManager.h"
#include "GAWave.h"

namespace wav
{

// Preinstantiate Objects //////////////////////////////////////////////////////

char* WaveDeviceManager::nextSound = 0;

SdReader card; // This object holds the information for the card
FatVolume vol; // This holds the information for the partition on the card
FatReader root; // This holds the information for the volumes root directory
FatReader file; // This object represent the WAV file
WaveHC wave; // This is the only wave (audio) object, since we will only play one at a time

int WaveDeviceManager::init()
{
	if ( !card.init() )
		return	( 1 );

	// enable optimized read - some cards may timeout
	card.partialBlockRead( true );

	if ( !vol.init( card ) )
		return	( 2 );

	if ( !root.openRoot( vol ) )
		return	( 3 );

#if	( DEBUG_SCOPE > 0 )
	ls();
#endif
	return	( 0 );
}

int WaveDeviceManager::ls()
{
#if	( DEBUG_SCOPE > 0 )
	// Try to open the root directory
	if ( !root.openRoot( vol ) )
		return	( 1 );

	// Print out all of the files in all the directories.
	root.ls( LS_R | LS_FLAG_FRAGMENTED );
#endif
	return	( 0 );
}

int WaveDeviceManager::playfile( char *name )
{
	// open by name
	if ( !file.open( root, name ) )
		return	( 1 );

	// create and play Wave
	if ( !wave.create( file ) )
		return	( 2 );

	wave.play();
	return	( 0 );
}

void WaveDeviceManager::playcomplete( char *name )
{
	playfile( name );

	while ( wave.isplaying )
		delay( 500 );

	wave.stop();
	file.close();
}

void WaveDeviceManager::play()
{
	if ( nextSound != 0 )
	{
		char* n = nextSound;
		nextSound = 0;
		playcomplete( n );
	}
}

}

wav::WaveDeviceManager WaveDevManager = wav::WaveDeviceManager();
