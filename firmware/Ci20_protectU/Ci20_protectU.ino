// For MSP430G2-Ci20 motor control
#include <Servo.h>

// COMMANDS LIST
const unsigned char CMD_FORWARD   = 0x10;
const unsigned char CMD_BACKWARD  = 0x20;
const unsigned char CMD_STOP      = 0x30;
const unsigned char CMD_BRAKE     = 0x40;
const unsigned char CMD_RTLFT     = 0x50;  // Rotate left
const unsigned char CMD_RTRGT     = 0x60;  // Rotate Right
const unsigned char CMD_PING      = 0xF1;  // r u thr?

// RETURN CODES
const byte RET_SUCCESS      = 0x50;
const byte RET_INVALID_CMD  = 0x51;
const byte RET_INVALID_OPT  = 0x52;
const byte RET_INIT_OK      = 0x53;


/* Left Motor */
const int motorR_pinA  = P2_0;
const int motorR_pinB  = P1_5;
const int motorR_pwm   = P2_4;

/* Right Motor */
const int motorL_pinA  = P1_4;
const int motorL_pinB  = P1_3;
const int motorL_pwm   = P2_1;

/* Servo Motor */
const int servoPin     = P1_6;

const int updatePeriod = 2000;

// ************** BASIC MOTOR CONTROL ***************
void leftMotor_fwd( void ) {
  digitalWrite( motorL_pinA, HIGH );
  digitalWrite( motorL_pinB,  LOW );
}

void leftMotor_bck( void ) {
  digitalWrite( motorL_pinA,  LOW );
  digitalWrite( motorL_pinB, HIGH );
}

void leftMotor_stop( void ) {
  digitalWrite( motorL_pinA,  LOW );
  digitalWrite( motorL_pinB,  LOW );
}

void leftMotor_brake( void ) {
  digitalWrite( motorL_pinA, HIGH );
  digitalWrite( motorL_pinB, HIGH );
}

void rightMotor_fwd( void ) {
  digitalWrite( motorR_pinA, HIGH );
  digitalWrite( motorR_pinB,  LOW );
}

void rightMotor_bck( void ) {
  digitalWrite( motorR_pinA,  LOW );
  digitalWrite( motorR_pinB, HIGH );
}

void rightMotor_stop( void ) {
  digitalWrite( motorR_pinA,  LOW );
  digitalWrite( motorR_pinB,  LOW );
}

void rightMotor_brake( void ) {
  digitalWrite( motorR_pinA, HIGH );
  digitalWrite( motorR_pinB, HIGH );
}

// ************** HIGH LEVEL CONTROL *************
void turnLeft( unsigned t = 0 ) {
  leftMotor_bck();
  rightMotor_fwd();
  delay( t );
}

void turnRight( unsigned t = 0 ) {
  leftMotor_fwd();
  rightMotor_bck();
  delay( t );
}

void forward() {
  leftMotor_fwd();
  rightMotor_fwd();
}

void backward() {
  leftMotor_bck();
  rightMotor_bck();
}

void bot_stop() {
  leftMotor_stop();
  rightMotor_stop();
}

void bot_brake() {
  leftMotor_brake();
  rightMotor_brake();
}
// ************************************************ 

uint8_t start   = 0;
int counter     = 0;

// Motor PWMs
const uint8_t lPwmDefault = 200;
const uint8_t rPwmDefault = 200;
uint8_t lmPwmVal  = lPwmDefault;
uint8_t rmPwmVal  = rPwmDefault;

// Servo camServo;
int pos = 0;

void setup()
{
  pinMode( P1_1,  INPUT );
  pinMode( P1_2, OUTPUT );
  
  pinMode( motorL_pinA, OUTPUT );
  pinMode( motorL_pinB, OUTPUT );
  pinMode( motorL_pwm,  OUTPUT );
  pinMode( motorR_pinA, OUTPUT );
  pinMode( motorR_pinB, OUTPUT );
  pinMode( motorR_pwm,  OUTPUT );
  
  // Stop the bot  
  bot_stop();
  
  analogWrite( motorL_pwm, 70 );
  analogWrite( motorR_pwm, 140 );
  forward();  delay( 2500 );  bot_stop();
  
  analogWrite( motorL_pwm, 140 );
  analogWrite( motorR_pwm, 70 );
  forward();  delay( 2500 );  bot_stop();
  
  analogWrite( motorL_pwm, lmPwmVal );
  analogWrite( motorR_pwm, rmPwmVal );
  
  start = 0;
  Serial.begin( 115200 );
  
  // camServo.attach( servoPin );
  
}

void loop()
{
   // Serial event will take care
}

/* Serial Commands */
void serialEvent() {
  
  /*If waiting for start command - 8 0xFF in a row*/
  if( !start ) {
    while( Serial.available() ) {
      char inChar = (char) Serial.read();
      if( inChar == (char) 0xF1 ) {
        counter++;
      } else {
        counter--;
      }
      if( counter < 0 ) {
        counter = 0;
      }
    }
    if( counter >= 8 ) {
      start = 1;
      Serial.println( "BOT STARTED" );
      Serial.flush();    // Flush any additional bytes recieved
    }
  } else {
    while( Serial.available() ) {
      uint8_t inCmd = (uint8_t) Serial.read();
//      Serial.print( inCmd, HEX );
//      Serial.print( " -> " ); 
//      Serial.println( (inCmd&0x0F), HEX );
      switch( (inCmd & 0xF0) ) {
        case CMD_FORWARD  : 
                            if( (inCmd&0x0F) == 0x00 ) {
                              Serial.write( RET_SUCCESS );
                            } else if( (inCmd&0x0F) == 0x01 ) {
                              while( Serial.available() < 1 );
                              uint8_t val = (uint8_t) Serial.read();
                              lmPwmVal = val;  
                              rmPwmVal = val;  
                              Serial.write( RET_SUCCESS ); 
                            } else if( (inCmd&0x0F) == 0x02 ) {
                              while( Serial.available() < 2 );
                              uint8_t valL = (uint8_t) Serial.read();
                              uint8_t valR = (uint8_t) Serial.read();
                              lmPwmVal = valL;
                              rmPwmVal = valR;
                              Serial.write( RET_SUCCESS ); 
                            } else {
                              Serial.write( RET_INVALID_OPT );
                            }
                            Serial.flush();
                            analogWrite( motorL_pwm, lmPwmVal );
                            analogWrite( motorR_pwm, rmPwmVal );
                            forward();  
                            break;
        case CMD_BACKWARD :
                            if( (inCmd&0x0F) == 0x00 ) {
                              Serial.write( RET_SUCCESS );
                            } else if( (inCmd&0x0F) == 0x01 ) {
                              while( Serial.available() < 1 );
                              uint8_t val = (uint8_t) Serial.read();
                              lmPwmVal = val;
                              rmPwmVal = val;
                              Serial.write( RET_SUCCESS ); 
                            } else if( (inCmd&0x0F) == 0x02 ) {
                              while( Serial.available() < 2 );
                              uint8_t valL = (uint8_t) Serial.read();
                              uint8_t valR = (uint8_t) Serial.read();
                              lmPwmVal = valL;
                              rmPwmVal = valR;
                              Serial.write( RET_SUCCESS ); 
                            } else {
                              Serial.write( RET_INVALID_OPT );
                            }
                            Serial.flush();
                            analogWrite( motorL_pwm, lmPwmVal );
                            analogWrite( motorR_pwm, rmPwmVal );
                            backward();  
                            break;
        case CMD_STOP     : 
                            if( (inCmd&0x0F) == 0x00 ) {
                              Serial.write( RET_SUCCESS );
                            } else {
                              Serial.write( RET_INVALID_OPT );
                            }
                            Serial.flush();
                            bot_stop();  
                            break;
        case CMD_BRAKE    : 
                            if( (inCmd&0x0F) == 0x00 ) {
                              Serial.write( RET_SUCCESS );
                            } else {
                              Serial.write( RET_INVALID_OPT );
                            }
                            Serial.flush();
                            bot_brake();  
                            break;
        case CMD_RTLFT    : if( (inCmd&0x0F) == 0x00 ) {
                              lmPwmVal = lPwmDefault;  //
                              rmPwmVal = rPwmDefault;
                              Serial.write( RET_SUCCESS );
                            } else if( (inCmd&0x0F) == 0x01 ) {
                              while( Serial.available() < 1 );
                              uint8_t val = (uint8_t) Serial.read();
                              lmPwmVal = val;
                              rmPwmVal = val;
                              Serial.write( RET_SUCCESS ); 
                            } else {
                              Serial.write( RET_INVALID_OPT );
                            }
                            Serial.flush();
                            analogWrite( motorL_pwm, lmPwmVal );
                            analogWrite( motorR_pwm, rmPwmVal );
                            turnLeft();
                            break;
        case CMD_RTRGT    : 
                            if( (inCmd&0x0F) == 0x00 ) {
                              lmPwmVal = lPwmDefault;  //
                              rmPwmVal = rPwmDefault;
                              Serial.write( RET_SUCCESS );
                            } else if( (inCmd&0x0F) == 0x01 ) {
                              while( Serial.available() < 1 );
                              uint8_t val = (uint8_t) Serial.read();
                              lmPwmVal = val;
                              rmPwmVal = val;
                              Serial.write( RET_SUCCESS ); 
                            } else {
                              Serial.write( RET_INVALID_OPT );
                            }
                            Serial.flush();
                            analogWrite( motorL_pwm, lmPwmVal );
                            analogWrite( motorR_pwm, rmPwmVal );
                            turnRight();
                            break;
        case CMD_PING    : 
                            Serial.write( RET_SUCCESS );
        default:  
                            Serial.write( RET_INVALID_CMD );
      }
    }
  }
}

