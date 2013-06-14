microSRCP
=========

microSRCP Projekt, Steuerung einer Modelleisenbahn auf Basis der Arduino Plattform

Dieses Projekt löst das microSRCP Projekt auf berlios.de ab.

Kurzinstallation:
- Arduino Entwicklungsumgebung - http://arduino.cc/en/Main/Software, ab Version 1.0.5 installieren
- RocRail - http://wiki.rocrail.net/doku.php, zur Ansteuerung der Modelleisenbahn und der Arduino's, installieren
- Arduino Entwicklungsumgebung starten und Verzeichnis aus Menu -> Datei -> Einstellungen -> Sketchbook Speicherort notieren
- Entwicklungsumgebung beenden
- microSRCP Sourcen mittels ZIP Button (oben) downloaden
- ZIP Datei entpacken und die Dateien aus Verzeichnis microSRCP/arduino in den vorher notierten Sketchbook Speicherort kopieren.
- Arduino Entwicklungsumgebung starten - unter -> Datei -> Sketchbook -> microSRCPServer wählen
- PC und Aurduino mittels USB Kabel verbinden, das Betriebssystem richtet eine neue Serielle Schnittstelle ein (bei Problemen siehe www.arduino.cc)
- Unter Tools -> entsprechendes Board und Serielle Schnittstelle einstellen
- Compilieren und Uploaden des Sketches...
- RocRail starten und unter -> RocRail Eigenschaften neue srcp Zentrale mit Sub Bibliothek RS232 und gleicher Serieller Schnittstelle wie bei der Arduino Entwicklungsumgebung einrichten

Welche Arduino Boards und Shields benötigt werden steht in den entsprechenden Sketches. 

Für eine Detailierte Beschreibung siehe der Wiki Button (oben).




