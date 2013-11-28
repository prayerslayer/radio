#include "math.h"

const int VOLUME_PIN = A0;  // pin with cable for volume potentiometer
const int MAX_VOLUME = 100;

int lastVolume = 70;
int currentVolume = 70;  //TODO this should be set in setup from the pi

void setup() {
  // start serial
  Serial.begin( 9600 );
  
  Serial.println( "Running setup" );
  
  //TODO here we would read some initialization values from le pi
  
  Serial.println( "Finished setup" );
}

void readVolume() {
  float vol = analogRead( VOLUME_PIN );
  currentVolume = floor( vol / 1023.0 * 100 );  // convert to percentage
  currentVolume = currentVolume - ( currentVolume % 10 ); // round down to next 5-step
}

void loop() {
  readVolume();
  if ( currentVolume != lastVolume ) {
    // someone actually changed the volume!
    Serial.print( "CMD SET VOLUME " );
    Serial.println( currentVolume );
    delay( 100 );
    lastVolume = currentVolume;
  }
}

