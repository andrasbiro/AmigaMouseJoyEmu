/**
 * This example converts PS2 mouse movement and 3 buttons into
 * Amiga mouse. It depends on Kris Chambers' excellent PS2 library
 * from https://github.com/kristopher/PS2-Mouse-Arduino
 */

#include "PS2Mouse.h"
#include "AmigaMouseJoyEmu.h"
#include <Arduino.h>

PS2Mouse mouse(A2, A3); //PS2 pins on the analog ports
AmigaMouseJoyEmu amigaMJE(2, 3, 4, 5, 6, 7, 8); //DB9 connected to digital port

void setup()
{
  // Serial.begin(115200);
  mouse.initialize();
}

int buttonData;
void loop()
{
  int data[2];
  mouse.report(data); //read the data from PS2
  if ( buttonData != data[0] ){ //if button values did change, update it on the Amiga
    amigaMJE.setMouseButtons(data[0] & 0x01, data[0] & 0x02, data[0] & 0x04);
    // Print button states on Serial
    // Serial.print("LRM:");
    // Serial.print(!(data[0] & 0x01));
    // Serial.print(!(data[0] & 0x02));
    // Serial.print(!(data[0] & 0x04));
    // Serial.println();
    buttonData = data[0];
  }
  amigaMJE.addMove(data[1], data[2]); //set the movement for the Amiga
  amigaMJE.move(); //move the mouse on the Amiga
}
