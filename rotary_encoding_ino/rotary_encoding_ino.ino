int LEFT_LED = 13;
int RIGHT_LED = 12;
int ROT_LEFT = 3;
int ROT_RIGHT = 2;

volatile int seqstore = 0;
volatile int encoded = 0b11;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
volatile long lastEncoderValue = 0;
volatile int MSB = 1;
volatile int LSB = 1;

void setup() {
  Serial.begin( 9600 );
  pinMode( LEFT_LED, OUTPUT);
  pinMode( RIGHT_LED, OUTPUT );
  
  pinMode( ROT_LEFT, INPUT );
  pinMode( ROT_RIGHT, INPUT );
  digitalWrite( ROT_LEFT, HIGH );
  digitalWrite( ROT_RIGHT, HIGH );
  
  attachInterrupt( 0, updateEncoder, CHANGE );
  attachInterrupt( 1, updateEncoder, CHANGE );
}


/*
*  Apparently the encoder I use produces 11 01 00 10 for one step.
*  So I have to check for 11 01 00 10 or 01 00 10 11 pattern
*/
void updateEncoder() {
  MSB = digitalRead( ROT_LEFT );
  LSB = digitalRead( ROT_RIGHT );
  
  encoded = (MSB << 1) | LSB; //converting the 2 pin value to single number

  if ( ( seqstore & 0x3 ) != encoded ) { 
    // at least one of the bits has changed compared to last stable state
    // (interrupt might bounce )
    seqstore = seqstore << 2; //shift the next sequence step
    seqstore |= encoded; // add encoded value
    seqstore = seqstore & 0b11111111; // only keep last 8 bits
    
    lastEncoderValue = encoderValue;
    
    // counter-clockwise code is 10 00 01 11 (135) and all its permutations:
    // 11 10 00 01 = 225
    // 01 11 10 00 = 120
    // 00 01 11 10 =  30
    if ( seqstore == 135 || seqstore == 225 || seqstore == 120 || seqstore == 30  ) {
      encoderValue--;
    }
    // clockwise code is 01 00 10 11 (75) and all its permutations:
    // 00 10 11 01 =  45
    // 10 11 01 00 = 180
    // 11 01 00 10 = 210
    if ( seqstore == 75 || seqstore == 45 || seqstore == 180 || seqstore == 210 ) {
      encoderValue++;
    }
  }
}

void loop() {
  /*
  digitalWrite( LEFT_LED, HIGH );
  delay( 100 );
  digitalWrite( LEFT_LED, LOW );
  digitalWrite( RIGHT_LED, HIGH );
  delay( 100 );
  digitalWrite( RIGHT_LED, LOW );
  */
  if ( lastEncoderValue < encoderValue ) {
    Serial.println( "RIGHT" );
    digitalWrite( RIGHT_LED, HIGH );
    digitalWrite( LEFT_LED, LOW );
  } else if ( lastEncoderValue > encoderValue ) {
    Serial.println( "LEFT" );
    digitalWrite( LEFT_LED, HIGH );
    digitalWrite( RIGHT_LED, LOW );
  } else {
    Serial.println( "NONE" );
    digitalWrite( LEFT_LED, LOW );
    digitalWrite( RIGHT_LED, LOW );
  }
  delay( 100 );
  //Serial.println( lastEncoderValue );
  //Serial.print( "Seqstore " ); Serial.println( seqstore );
  //Serial.print( "Matches CCW " ); Serial.println( seqstore == PATTERN_CCW );
  //Serial.print( "Matches CW " ); Serial.println( seqstore == PATTERN_CW );
  Serial.print( "Last " ); Serial.println( lastEncoderValue );
  Serial.print( "Value " ); Serial.println( encoderValue );
  Serial.println( "===== " );
}
