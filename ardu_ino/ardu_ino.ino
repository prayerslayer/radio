const int STATUS_LED_PIN = 13;
const int VOLUME_UP_PIN = 3;
const int VOLUME_DN_PIN = 2;
const int CHANNEL_PINS[] = {8, 9};

int currentChannel = -1;
int currentVolume = 0;

int DEVICE_STATUS = 0;
int DEVICE_STATUS_WAIT = 0;  // means status led should blink. it's 1 if it's fine and up
int DEVICE_STATUS_UP = 1;    // led is on

// volume variables
volatile int vol_seqstore = 0;
volatile int vol_encoded = 0b11;
volatile int vol_lastEncoded = 0;
volatile long vol_encoderValue = 0;
volatile long vol_lastEncoderValue = 0;
volatile int vol_MSB = 1;
volatile int vol_LSB = 1;

String piCommand = "";

int getPressedButton() {
  for (int i = 0; i < (sizeof(CHANNEL_PINS)/sizeof(int)); i++) {
    int state = digitalRead(CHANNEL_PINS[i]);
    if (state == HIGH) {
      return i;
    }
  }
  return -1;
}

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

void readVolume() {
  currentVolume = vol_encoderValue;
  vol_encoderValue = 0;
}

int readChannel() {
  int btn = getPressedButton();
  return btn >= 0 ? btn : -1;
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
  for (int i = 0; i < sizeof(CHANNEL_PINS) - 1; i++) {
    pinMode(CHANNEL_PINS[i], INPUT);
  }
  
  // ints
  attachInterrupt( digitalPinToInterrupt(VOLUME_UP_PIN), updateVolume, CHANGE );
  attachInterrupt( digitalPinToInterrupt(VOLUME_DN_PIN), updateVolume, CHANGE );

  Serial.println( "OK" );
  digitalWrite( STATUS_LED_PIN, HIGH );
}

void updateVolume() {
  vol_MSB = digitalRead( VOLUME_UP_PIN );
  vol_LSB = digitalRead( VOLUME_DN_PIN );

  vol_encoded = ( vol_MSB << 1 ) | vol_LSB; //converting the 2 pin value to single number
  if ( ( vol_seqstore & 0x3 ) != vol_encoded ) {
    // at least one of the bits has changed compared to last stable state
    // (interrupt might bounce )
    vol_seqstore = vol_seqstore << 2; //shift the next sequence step
    vol_seqstore |= vol_encoded; // add encoded value
    vol_seqstore = vol_seqstore & 0b1111; // only keep last 4 bits
    
    vol_lastEncoderValue = vol_encoderValue;

    //            <==  |  ==> 
    // grey code: 00 01 11 10
    
    // to the left
    if ( vol_seqstore == 0b0100 ||
         vol_seqstore == 0b1101 ||
         vol_seqstore == 0b1011 ||
         vol_seqstore == 0b0010) {
        vol_encoderValue = -1;
    }
    
    // to the right
    if ( vol_seqstore == 0b0001 ||
         vol_seqstore == 0b0111 ||
         vol_seqstore == 0b1110 ||
         vol_seqstore == 0b1000) {
        vol_encoderValue = 1;
    }
  }
}

void blink(int n) {
  digitalWrite( STATUS_LED_PIN, LOW);
  delay(n);
  digitalWrite( STATUS_LED_PIN, HIGH);
  delay(n);
}

void loop() {
  readSerial();

  if ( piCommand == "OK" ) {
    DEVICE_STATUS = DEVICE_STATUS_UP;
  }
  
  // do nothing if no okay-dokey from pi arrived yet
  if ( DEVICE_STATUS != DEVICE_STATUS_UP ) {
    // just blink the status led
    blink(1000);
    return;
  }
  
  readVolume();
  if ( currentVolume != 0 ) {
    // someone actually changed the volume!
    blink(100);
    Serial.print( "V" );
    Serial.println( currentVolume > 0 ? "+" : "-");
  }
  int channel = readChannel();
  if ( channel != currentChannel && channel > -1 ) {
    blink(100);
    Serial.print( "C");
    Serial.println(channel);
    currentChannel = channel;
  }
}

