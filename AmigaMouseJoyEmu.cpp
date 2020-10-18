#include "AmigaMouseJoyEmu.h"

#include "Arduino.h"

#define QUADRATURE_LENGTH_US 4000
#define QUADRATURE_SPEED_LIMIT 100

AmigaMouseJoyEmu::AmigaMouseJoyEmu(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin9){
  _pin1 = pin1;  // V-pulse          /Up       BTN3       n/c
  _pin2 = pin2;  // H-pulse          /Down     n/c        n/c
  _pin3 = pin3;  // VQ-pulse         /Left     BTN1       n/c
  _pin4 = pin4;  // HQ-pulse         /Right    BTN2       n/c
  _pin5 = pin5;  // BTN3 (m)         n/c       potX       PenPress      I/O
  _pin6 = pin6;  // BTN1 (l)         /BTN1     n/c        /BeamTrigger  I/O
  _pin9 = pin9;  // BTN2 (r)         BTN2      PotY       BTN2          I/O

  _remainingX = 0;
  _remainingY = 0;
  pinMode(_pin1, OUTPUT);
  pinMode(_pin2, OUTPUT);
  pinMode(_pin3, OUTPUT);
  pinMode(_pin4, OUTPUT);
  pinMode(_pin5, OUTPUT);
  pinMode(_pin6, OUTPUT);
  pinMode(_pin9, OUTPUT);

  _quadrature_length = QUADRATURE_LENGTH_US;
  _speed_limit = QUADRATURE_SPEED_LIMIT;
}

void AmigaMouseJoyEmu::setTimings(uint32_t quadrature_length_us, uint8_t speed_limit){
  _quadrature_length = quadrature_length_us;
  _speed_limit = speed_limit;
}

void AmigaMouseJoyEmu::setMouseButtons(bool left, bool right, bool middle){
  digitalWrite(_pin6, !left);
  digitalWrite(_pin9, !right);
  digitalWrite(_pin5, !middle);
}

void AmigaMouseJoyEmu::addMove(int movementX, int movementY){
  _remainingX += movementX;
  _remainingY += movementY;

}

inline bool AmigaMouseJoyEmu::isInPast(uint32_t timeUs){
  if ( (int32_t)(timeUs - micros()) < 0 )
    return true;
  else
    return false;
}

inline uint8_t AmigaMouseJoyEmu::nextPhase(uint8_t currentPhase, bool positive){
  //positive: 0 1 3 2
  //negative: 0 2 3 1
  if ( positive ){
    switch(currentPhase){
      case 0:
        return 1;
      case 1:
        return 3;
      case 3:
        return 2;
      default: //reset invalid states
        return 0;
    }
  } else {
    switch(currentPhase){
      case 0:
        return 2;
      case 2:
        return 3;
      case 3:
        return 1;
      default: //reset invalid states
        return 0;
    }
  }
}

void AmigaMouseJoyEmu::move(){
  if ( _remainingX == 0 && _remainingY == 0 )
    return;
  //remove sign
  bool xDirection = _remainingX > 0;
  bool yDirection = _remainingY > 0;
  uint16_t moveX, moveY;
  if ( !xDirection )
    moveX = -_remainingX;
  else
    moveX = _remainingX;
  if ( !yDirection )
    moveY = -_remainingY;
  else
    moveY = _remainingY;

  //limit the speed. Accuracy probably doesn't matter at this speed anyway
  if ( moveX > _speed_limit )
    moveX = _speed_limit;
  if ( moveY > _speed_limit )
    moveY = _speed_limit;


  int16_t xDelay, yDelay;
  if ( moveX > 0 )
    xDelay = (_quadrature_length/4) / moveX;
  else
    xDelay = 0;
  if ( moveY > 0 )
    yDelay = (_quadrature_length/4) / moveY;
  else
    yDelay = 0;
  uint8_t xPhase = digitalRead(_pin2) | (digitalRead(_pin4)<<1);
  uint8_t yPhase = digitalRead(_pin3) | (digitalRead(_pin1)<<1);

  uint32_t endTime = micros() + _quadrature_length;
  uint32_t nextXTime, nextYTime;
  if ( xDelay > 0 )
    nextXTime = micros() + xDelay;
  else
    nextXTime = endTime + _quadrature_length;
  if ( yDelay > 0 )
    nextYTime = micros() + yDelay;
  else
    nextYTime = endTime + _quadrature_length;


  while ( !isInPast(endTime) ){
    if ( isInPast(nextXTime) ){
      xPhase = nextPhase(xPhase, xDirection);
      digitalWrite(_pin2, xPhase & 0x01);
      digitalWrite(_pin4, xPhase & 0x02);
      nextXTime += xDelay;
    }
    if ( isInPast(nextYTime) ){
      yPhase = nextPhase(yPhase, yDirection);
      digitalWrite(_pin3, yPhase & 0x01);
      digitalWrite(_pin1, yPhase & 0x02);
      nextYTime += yDelay;
    }
  }

  _remainingX = 0;
  _remainingY = 0;
}
