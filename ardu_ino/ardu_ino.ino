#include "math.h"

int STATUS_LED_PIN = 13;
int VOLUME_UP_PIN = 4;
int VOLUME_DN_PIN = 6;
int STATION_UP_PIN = 2;
int STATION_DN_PIN = 3;

int MIN_STATION = 0;
int MAX_STATION = 7;
int STATION_OFFSET = 8; // rotations until station is switched
int MIN_VOLUME = 0;
int MAX_VOLUME = 100;

int DEVICE_STATUS = -1;
int DEVICE_STATUS_WAIT = 0;  // means status led should blink. it's 1 if it's fine and up
int DEVICE_STATUS_UP = 1;    // led is on


// station variables
volatile int stat_seqstore = 0;
volatile int stat_encoded = 0b11;
volatile int stat_lastEncoded = 0;
volatile long stat_encoderValue = 0;
volatile long stat_lastEncoderValue = 0;
volatile int stat_MSB = 1;
volatile int stat_LSB = 1;

// volume variables
volatile int vol_seqstore = 0;
volatile int vol_encoded = 0b11;
volatile int vol_lastEncoded = 0;
volatile long vol_encoderValue = 0;
volatile long vol_lastEncoderValue = 0;
volatile int vol_MSB = 1;
volatile int vol_LSB = 1;

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
  }
}

void processSerial() {
  if ( piCommand.startsWith( "CMD SET VOLUME" ) ) {
    currentVolume = piCommand.substring( 16, piCommand.length() - 1 ).toInt();
    lastVolume = currentVolume;
  }
}

void readVolume() {
  if ( currentVolume == vol_encoderValue ) {
    return;
  }
  currentVolume = vol_encoderValue;
}

void readStation() {
  if ( currentStation == stat_encoderValue ) {
    return;
  }
  currentStation = stat_encoderValue / STATION_OFFSET;
}

void setup() {
  // start serial
  Serial.begin( 9600 );
  // set pin modes
  pinMode( STATUS_LED_PIN, OUTPUT );
  pinMode( VOLUME_UP_PIN, INPUT );
  pinMode( VOLUME_DN_PIN, INPUT );
  digitalWrite( VOLUME_DN_PIN, HIGH );
  digitalWrite( VOLUME_UP_PIN, HIGH );
  pinMode( STATION_UP_PIN, INPUT );
  pinMode( STATION_DN_PIN, INPUT );
  digitalWrite( STATION_UP_PIN, HIGH );
  digitalWrite( STATION_DN_PIN, HIGH );
  
  // ints
  attachInterrupt( 0, updateStation, CHANGE );
  attachInterrupt( 1, updateStation, CHANGE );
  
  DEVICE_STATUS = DEVICE_STATUS_WAIT;
  
  
  Serial.println( "Running internal setup" );
  // do some stuff?
  Serial.println( "Finished internal setup" );
  
  digitalWrite( STATUS_LED_PIN, HIGH );
}

/*
*  Apparently the encoder I use produces 11 01 00 10 for one step.
*  So I have to check for 11 01 00 10 or 01 00 10 11 pattern
*/
void updateVolume() {
  vol_MSB = digitalRead( VOLUME_UP_PIN );
  vol_LSB = digitalRead( VOLUME_DN_PIN );
  
  vol_encoded = ( vol_MSB << 1 ) | vol_LSB; //converting the 2 pin value to single number

  if ( ( vol_seqstore & 0x3 ) != vol_encoded ) { 
    // at least one of the bits has changed compared to last stable state
    // (interrupt might bounce )
    vol_seqstore = vol_seqstore << 2; //shift the next sequence step
    vol_seqstore |= vol_encoded; // add encoded value
    vol_seqstore = vol_seqstore & 0b11111111; // only keep last 8 bits
    
    vol_lastEncoderValue = vol_encoderValue;
    
    // counter-clockwise code is 10 00 01 11 (135) and all its permutations:
    // 11 10 00 01 = 225
    // 01 11 10 00 = 120
    // 00 01 11 10 =  30
    if ( vol_seqstore == 135 || vol_seqstore == 225 || vol_seqstore == 120 || vol_seqstore == 30  ) {
      // decremenet only if we're not going under minimum volume
      if ( vol_encoderValue >= MIN_VOLUME )
        vol_encoderValue--;
    }
    // clockwise code is 01 00 10 11 (75) and all its permutations:
    // 00 10 11 01 =  45
    // 10 11 01 00 = 180
    // 11 01 00 10 = 210
    if ( vol_seqstore == 75 || vol_seqstore == 45 || vol_seqstore == 180 || vol_seqstore == 210 ) {
      // increment only if we're not going over maximum volume
      if ( vol_encoderValue <= MAX_VOLUME )
        vol_encoderValue++;
    }
  }
}

void updateStation() {
  stat_MSB = digitalRead( STATION_UP_PIN );
  stat_LSB = digitalRead( STATION_DN_PIN );
  
  stat_encoded = ( stat_MSB << 1 ) | stat_LSB; //converting the 2 pin value to single number

  if ( ( stat_seqstore & 0x3 ) != stat_encoded ) { 
    // at least one of the bits has changed compared to last stable state
    // (interrupt might bounce )
    stat_seqstore = stat_seqstore << 2; //shift the next sequence step
    stat_seqstore |= stat_encoded; // add encoded value
    stat_seqstore = stat_seqstore & 0b11111111; // only keep last 8 bits
    
    stat_lastEncoderValue = stat_encoderValue;
    
    // counter-clockwise code is 10 00 01 11 (135) and all its permutations:
    // 11 10 00 01 = 225
    // 01 11 10 00 = 120
    // 00 01 11 10 =  30
    if ( stat_seqstore == 135 || stat_seqstore == 225 || stat_seqstore == 120 || stat_seqstore == 30  ) {
      // decremenet only if we're not going under minimum volume
      if ( stat_encoderValue >= 0 )
        stat_encoderValue--;
    }
    // clockwise code is 01 00 10 11 (75) and all its permutations:
    // 00 10 11 01 =  45
    // 10 11 01 00 = 180
    // 11 01 00 10 = 210
    if ( stat_seqstore == 75 || stat_seqstore == 45 || stat_seqstore == 180 || stat_seqstore == 210 ) {
      // increment only if we're not going over maximum volume
      if ( stat_encoderValue <= MAX_STATION * STATION_OFFSET )
        stat_encoderValue++;
    }
  }
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
    int diff = currentStation - lastStation;
    if ( diff > 0 )
      Serial.println( "CMD SET STATION NEXT" );
    else if ( diff < 0 )
      Serial.println( "CMD SET STATION PREVIOUS" );
    lastStation = currentStation;
  }
}

