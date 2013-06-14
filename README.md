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
- ZIP Datei entpacken und die Dateien aus Verzeichnis microSRCP/arduino in den vorher notierte Sketchbook Speicherort kopieren.
- Arduino Entwicklungsumgebung starten - unter -> Datei -> Sketchbook sollten die microSRCP Sketches und unter -> Sketch -> Library importieren, die Libraries lan, srcp, dev verfügbar sein. Wenn nicht Arduino Entwicklungsumgebung beenden und das ganze nochmals versuchen.
- PC und Aurduino mittels USB Kabel verbinden
- Unter Tools -> entsprechendes Board und Serielle Schnittstelle einstellen
- Compilieren und Uploaden des Sketches...
- RocRail einrichten (siehe Beispiele in microSRCP/RocRail/...) und starten

Welche Arduino Boards und Shields benötigt werden steht in den entsprechenden Sketches. 

Für eine Detailierte Beschreibung siehe der Wiki Button (oben).




