microSRCP
=========

microSRCP Projekt, Steuerung einer Modelleisenbahn auf Basis der Arduino Plattform

Dieses Projekt löst das microSRCP Projekt auf berlios.de ab.

Kurzinstallation:
- Arduino Entwicklungsumgebung - http://arduino.cc/en/Main/Software, ab Version 1.0.5 installieren
- RocRail - http://wiki.rocrail.net/doku.php, zur Ansteuerung der Modelleisenbahn und der Arduino's, installieren
- Arduino Entwicklungsumgebung starten und Verzeichnis aus Menu -> Datei -> Einstellungen -> Sketchbook Speicherort notieren
- Entwicklungsumgebung beenden
- gewünschte ZIP/tar-Datei von https://github.com/mc-b/microSRCP/tags auswählen und downloaden
- ZIP/tar Datei entpacken und die Dateien aus Verzeichnis .../microSRCP/arduino in den vorher notierten Sketchbook Speicherort kopieren.
- Arduino Entwicklungsumgebung starten - unter -> Datei -> Sketchbook -> microSRCPServer wählen
- PC und Arduino mittels USB Kabel verbinden, das Betriebssystem richtet eine neue Serielle Schnittstelle ein (bei Problemen siehe www.arduino.cc)
- Unter Tools -> entsprechendes Board und Serielle Schnittstelle einstellen
- Compilieren und Uploaden des Sketches...
- RocRail starten und unter -> RocRail Eigenschaften neue srcp Zentrale mit Sub Bibliothek RS232 und gleicher Serieller Schnittstelle wie bei der Arduino Entwicklungsumgebung einrichten

Welche Arduino Boards und Shields benötigt werden steht in den entsprechenden Sketches. 

Für eine Detailierte Beschreibung, siehe https://github.com/mc-b/microSRCP/wiki.

Copyright
---------
Die Software untersteht der GNU GPL, siehe entsprechende Copyrights in 
den Sourcen. Da bedeutet u.a., sie kann frei verwendet werden, Aenderungen
und Erweiterungen muessen aber wieder frei zugaenglich sein.
Kommerzielle Produkte, ohne Offenlegung des Codes, sind damit ausgeschlossen!

Haftung
-------
Bei den Bauanleitungen bzw. irgendwelcher Software gibt es keine Haftung fuer 
irgendwelche Schaeden oder Funktionsgarantie, bitte immer nur als Anregung auffassen.
Ich hafte nicht fuer Schaeden, die der Anwender oder Dritte durch die Verwendung der 
Software oder Hardware verursachen oder erleiden. In keinem Fall hafte ich fuer 
entgangenen Umsatz oder Gewinn oder sonstige Vermoegensschaeden die bei der Verwendung 
oder durch die Verwendung dieser Programme oder Anleitungen entstehen koennen.
Und wer mit Strom umgeht, soll sich bitte bei hoeheren Spannung und Stroemen der 
Gefahren bewusst sein. Modellbahn gehoert VDE-technisch zur Kategorie Spielzeug, 
dementsprechend streng sind die Vorschriften. Siehe hierzu auch die 
Sicherheitshinweise des Fremo. 

Die Schaltungen und die Software werden als Anregung und Hilfe unter Modellbahnern
veroeffentlicht. Sie sind auf Grund von Beispielen aus dem Netz bzw. eigenen Ideen 
entstanden. Natuerlich sind sinnvolle Anregungen, Fehlermeldungen und 
Verbesserungen zu den Schaltung immer willkommen. Allerdings moechte ich aus 
gegebenen Anlass darauf hinweisen, dass ich leider keine Zeit fuer langwierige 
Diskussionen des Typs "Ich habe die Schaltung nachgebaut, warum funktioniert 
sie bei mir nicht" habe. Auch die Frage "Ich habe noch diesen oder jenen Chip 
in der Schublade, kann ich den auch verwenden", moege sich bitte jeder selbst 
beantworten. Kontaktaufnahme bitte per mail und nicht per Telefon. 
Bitte bei Problemen moeglichst exakt schildern, was wie schief geht, 
am besten mit logfiles bzw. exakter Fehlermeldung. 



