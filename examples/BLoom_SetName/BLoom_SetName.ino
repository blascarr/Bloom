#include <Bloom.h>
#include <SoftwareSerial.h>

#define pinRESET 4
unsigned long bps = 38400;

SoftwareSerial BLSerial( 2, 3 );

Bloom BLController( BLSerial , pinRESET, bps );

void setup() {
  Serial.begin( bps );

  BLController.begin();
  BLController.readConfig();
  BLController.search();
  BLController.setName("Bloom");
  Serial.println( BLController.getName() );
  
}

void loop() {

}
