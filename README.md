# AmigaMouseJoyEmu

This is a simple arduino/platformio library to emulate mouse (and maybe
joystick, in the future). I use it together with Kris Chambers' [excellent PS2
library](https://github.com/kristopher/PS2-Mouse-Arduino) on an Arduino Pro Mini
(5V/16MHz version). However, in theory, it is also possible to use it with a USB
HID library.

Currently it can only emulate a mouse - although I planned to add Joystick
support to it, I'm not sure if I ever will.

## Mouse emulation

### Buttons

The current version emulates a 3 button mouse. Although I wanted to add scroll
support, I couldn't find a clear description how
[FreeWheel](http://aminet.net/package/util/mouse/FreeWheel) expects that
information.

### Movement

Amiga mouse uses quadrature encoders which is sent directly to the mouse port's
pin 1/2/3/4. This library sends N number of 4 quadrature phases (usually a
"click" on a rotary encoder) on both X and Y axes. The emulated signal always
takes the same time, 4ms by default. Due to the blocking nature of the
emulation, this sets the polling period, as well as the slowest possible
movement in one emulation cycle. There is a maximum limit of movement for an
emulation cycle, 100 by default - resulting quadrature phase changes in 10us
period. I'm not sure of the limitation of the Arduino platform I used or the
Amiga, but this worked well on my 16MHz version.

### Emulation

The idea is that you should poll your input in the main loop, which should have
the 3 button state and the cumulated movement since the last poll. If the button
state is changed since the last call, it should be sent to the amiga, next the
emulation of the quadrature movement should take place.

This method works very well with more intelligent mouse protocols, like PS2 or
USB HID, or interrupt based inputs, but it won't work well with "dumber"
protocols as in input (such as the Amiga mouse protocol itself)