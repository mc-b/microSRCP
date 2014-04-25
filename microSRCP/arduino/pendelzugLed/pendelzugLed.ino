int dirpin1 = 12; // Fahrrichtung für Treiber 2, Pin 12 + 13
int dirpin2 = 13;
int speedpin = 11; // Geschwindigkeit für Treiber 2, Pin 11

int rot = 7; // LED Lichtsignal
int gruen = 6;

int sensorA = A0; // Analog Pin 0 - Sensor Haltepunkt A
int sensorB = A1; // Analog Pin 1 - Sensor Haltepunkt B
int speed = 120;

void setup()
{
  pinMode(dirpin1, OUTPUT);   // steuert die Fahrtrichtung
  pinMode(dirpin2, OUTPUT);   
  pinMode( sensorA, INPUT_PULLUP ); // Sensoren A und B als Input
  pinMode( sensorB, INPUT_PULLUP ); // und Interne Widerstände setzen

  // verwende die nachfolgenden Pins als Output: 
  pinMode(rot, OUTPUT); 
  pinMode(gruen, OUTPUT); 
}
void loop()
{
  digitalWrite(dirpin1,HIGH); // Fahrrichtung vorwaerts
  digitalWrite(dirpin2,LOW);
  analogWrite(speedpin, speed); // Geschwindigkeit 
  while ( digitalRead( sensorA) != 0 ) // Loop bis SensorA aktiviert wird
    delay( 10 );

  analogWrite(speedpin, 0); // Stop 
  digitalWrite(gruen, HIGH); // freie Fahrt 
  digitalWrite(rot, LOW); 
  delay(1000); // 1 Sekunden Wartezeit

  digitalWrite(dirpin1,LOW); // Fahrrichtung rueckwaerts
  digitalWrite(dirpin2,HIGH);
  analogWrite(speedpin, speed); // Geschwindigkeit 
  while ( digitalRead( sensorB) != 0  ) // Loop bis SensorA aktiviert wird
    delay( 10 );
    
  analogWrite(speedpin, 0); // Stop 
  digitalWrite(gruen, LOW); // Stop 
  digitalWrite(rot, HIGH); 
  delay(1000); // 1 Sekunden Wartezeit
}


