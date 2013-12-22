#include "math.h"

int STATUS_LED_PIN = 13;
int STATION_PIN = A1;  // pin to control radio stations from given source
int VOLUME_PIN = A0;  // pin with cable for volume potentiometer
int MAX_VOLUME = 100;

int DEVICE_STATUS = -1;
int DEVICE_STATUS_WAIT = 0;  // means status led should blink. it's 1 if it's fine and up
int DEVICE_STATUS_UP = 1;    // led is on

int lastVolume = 70;
int currentVolume = 70;  //TODO this should be set in setup from the pi

int lastStation = 0;
int currentStation = 0;  // TODO should be set

String piCommand = "";

// taken from http://hacking.majenko.co.uk/reading-serial-on-the-arduino
int readSerialLine(int readch, char *buffer, int len) {
  static int pos = 0;
  int rpos;
  piCommand = "";
  if ( readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}

void readSerial() {
  static char buffer[ 128 ];
  if ( readSerialLine ( Serial.read(), buffer, 128 ) > 0 ) {
    piCommand = buffer;
    Serial.print( "Your wish: " );
    Serial.println( piCommand );
  }
}

void processSerial() {
  if ( piCommand.startsWith( "CMD SET VOLUME" ) ) {
    currentVolume = piCommand.substring( 16, piCommand.length() - 1 ).toInt();
    lastVolume = currentVolume;
  }
}

void readVolume() {
  float vol = analogRead( VOLUME_PIN );
  currentVolume = floor( vol / 1023.0 * 100 );  // convert to percentage
  currentVolume = currentVolume - ( currentVolume % 10 ); // round down to next 10-step
}

void readStation() {
  float vol = analogRead( STATION_PIN );
  if ( vol < 384 )
    currentStation = -1;
  else if ( vol >= 384 && vol <= 640 )
    currentStation = 0;
  else
    currentStation = 1;
}

void setup() {
  // start serial
  Serial.begin( 9600 );
  // set pin modes
  pinMode( STATUS_LED_PIN, OUTPUT );
  DEVICE_STATUS = DEVICE_STATUS_WAIT;
  
  Serial.println( "Running internal setup" );
  // do some stuff?
  Serial.println( "Finished internal setup" );
  
  digitalWrite( STATUS_LED_PIN, HIGH );
}

void loop() {
  readSerial();
  
  if ( piCommand == "CMD WAIT" ) {
    DEVICE_STATUS = DEVICE_STATUS_WAIT;
  }
  if ( piCommand == "CMD RUN" ) {
    DEVICE_STATUS = DEVICE_STATUS_UP;
  }
  
  // do nothing if no okay-dokey from pi arrived yet
  if ( DEVICE_STATUS != DEVICE_STATUS_UP ) {
    // just blink the status led
    digitalWrite( STATUS_LED_PIN, LOW );
    delay( 1000 );
    digitalWrite( STATUS_LED_PIN, HIGH );
    delay( 1000 );
    return;
  }
  processSerial();
  
  readVolume();
  if ( currentVolume != lastVolume ) {
    // someone actually changed the volume!
    Serial.print( "CMD SET VOLUME " );
    Serial.println( currentVolume );
    lastVolume = currentVolume;
  }
  readStation();
  if ( currentStation != lastStation ) {
    if ( currentStation == 1 )
      Serial.println( "CMD SET STATION NEXT" );
    else if ( currentStation == -1 )
      Serial.println( "CMD SET STATION PREVIOUS" );
    lastStation = currentStation;
  }
}

