/**
 * AmigaMouseJoyEmu.h - Library to emulate Amiga Mouse or joystick
 * Created by Andras Biro, October 18, 2020
 * https://github.com/andrasbiro/AmigaMouseJoyEmu
 */
#ifndef __AMIGAMOUSEJOYEMU_H__
#define __AMIGAMOUSEJOYEMU_H__
#include "Arduino.h"

/**
 * \class AmigaMouseJoyEmu
 * 
 * \brief Library to emulate Amiga Mouse or joystick
 * 
 * This is small library that can emulate Amiga's DB9 connected mouse. It has
 * been tested with an Amiga 1200 and a PS2 mouse on an Arduino Pro Mini
 * 5V/16MHz.
 * 
 * \version 1.0 $
 * \date 2020/10/18 $
 */
class AmigaMouseJoyEmu
{
  private:
    //DB9 pins     mouse            joy       paddle     lightpen
    int _pin1;  // V-pulse          /Up       BTN3       n/c
    int _pin2;  // H-pulse          /Down     n/c        n/c
    int _pin3;  // VQ-pulse         /Left     BTN1       n/c
    int _pin4;  // HQ-pulse         /Right    BTN2       n/c
    int _pin5;  // BTN3 (m)         n/c       potX       PenPress      I/O
    int _pin6;  // BTN1 (l)         /BTN1     n/c        /BeamTrigger  I/O
    int _pin9;  // BTN2 (r)         BTN2      PotY       BTN2          I/O
    //pin7 - 5V; pin8 - GND

    int _remainingX;
    int _remainingY;

    uint32_t _quadrature_length;
    uint8_t _speed_limit;

    inline bool isInPast(uint32_t timeUs);
    inline uint8_t nextPhase(uint8_t currentPhase, bool positive);
  public:
    /**
     * \brief Constructor configures DB9 pins to output
     * \param [in] pin1 of the DB9 port
     * \param [in] pin2 of the DB9 port
     * \param [in] pin3 of the DB9 port
     * \param [in] pin4 of the DB9 port
     * \param [in] pin5 of the DB9 port
     * \param [in] pin6 of the DB9 port
     * \param [in] pin9 of the DB9 port
     */
    AmigaMouseJoyEmu(int pin1, int pin2, int pin3, int pin4, int pin5, int pin6, int pin9);
    
    /**
     * \brief Sets the timings of the emulation
     * \param [in] quadrature_length_us 
     *    The length of one emulation cycle in us. As the emulation will block
     *    execution while running, this sets the polling period, and also 
     *    the slowest possible speed per emulation cycle. Default is 4000us.
     *    Higher value means slower poll/speed.
     * \param [in] speed_limit
     *    The maximum amount of movement allowed by the \ref move() function
     *    Default is 100. Mainly controls the highest speed of the mouse.
     *    Higher value means higher speed.
     */
    void setTimings(uint32_t quadrature_length_us, uint8_t speed_limit);

    /**
     * \brief Sets the state of the mouse buttons
     * \param [in] left Left mouse button. true means pressed
     * \param [in] right Right mouse button. true means pressed
     * \param [in] middle Middle mouse button. true means pressed
     */
    void setMouseButtons(bool left, bool right, bool middle);

    /**
     * \brief Adds mouse movement. It won't actually perform the movement,
     *   call \ref move() for that. If called multiple times, the movement
     *   will be summed.
     * \param [in] movementX X direction movement (signed)
     * \param [in] movementY Y direction movement (signed)
     */
    void addMove(int movementX, int movementY);

    /**
     * \brief Moves the mouse. The function blocks while emulating the movement
     * 
     * If any movements needs to be performed, this function will do it in
     * the length set by \ref quadrature_length_us on \ref setTimings().
     * It will generate N number of quadratures, where N is the amount of
     * movement collected by \ref addMove() calls. X and Y movements are
     * performed at the same time.
     */
    void move();
};

#endif
