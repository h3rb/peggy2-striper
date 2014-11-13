/* Peggy2 Stripe Morpher
 Written for 25 parallel strips of RGBW...R configuration (vertically homogenous RGBW x-aligned)
 Works fine with RGBW (square "superpixel") or monochrome or other off-spec configurations
 Based on Qix Peggy2Lib demo.
 Makes your Peggy2 reminiscent of a 1970s TV sci-fi panel.

 Author: 2014 (c) Herbert Elwood Gilliland III
 Arduino sketch code for Peggy 2.0, using the Peggy2 library, version 0.2.  
 Includes some basic button controls to toggle colors (maybe overridden by the morpher AI)
 
 Based in Peggy2_Qix:
 Create a light that bounces off Paggy's "walls", and leaves a tail of 4 lights behind it of decreasing brightness
 Copyright (c) 2009 David S. Kauffman
 inspired by Peggy 2.0 example code from Windell Oskay
 
 This example is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 */


#include <Peggy2.h>

boolean reds=true, greens=true, blues=true, whites=true;
boolean _color_restrict=false;
boolean _color_pairing=false;
boolean _constrain_x=false;
boolean _constrain_y=false;

// TAIL determines how long the echoes behind hte bouncing ball are.
// Since there are only 16 gray levels..
#define MAXTAILLENGTH 12
#define TAIL MAXTAILLENGTH

#define WH        25
#define XCOMP     4
#define MAXX      (WH-1) // (WH/XCOMP)
#define MAXY      (WH-1)
#define XRGB(x,c) (x-(x%XCOMP)+c)

#define QIXES 24
int active=QIXES;
#define FRAMEBUFFERS 4
Peggy2 frames[FRAMEBUFFERS];

int DELAY=1;
int CLEAR_DELAY=5;

// xpos and ypos are arrays that keep track of the last few locations of the ball, so it can draw the tail
short xpos[QIXES][TAIL];
short ypos[QIXES][TAIL];
short xdir[QIXES];
short ydir[QIXES];
short xcol[QIXES];

// these variable control whether the ball is moving up, down, left or right.

void setup()                    // run once, when the sketch starts
{
  unsigned short i;
  frames[0].HardwareInit();   // Call this once to init the hardware. 
  // (Only needed once, even if you've got lots of frames.)
  for ( i=0; i<QIXES; i++ ) Init_Kix(i);
   for (i = 0; i < FRAMEBUFFERS; i++) { frames[i].Clear(); }
  buttoninit();
  attachInterrupt(0, platformReadInput, CHANGE);
}  // End void setup()  


// Other Misc: Handy Peggy2 button definitions:
#define selectButton     (0)
#define anyButton        (1)
#define leftButton       (2)
#define downButton       (3)
#define upButton         (4)
#define rightButton      (5)
#define TRUE             (1)
#define FALSE            (0)
#define KEYCHECK_DELAY (150)
unsigned long lastTime;
void platformReadInput() {

  unsigned long timeNow, timeDelta;

  // time delay on button check so we don't oversample the buttons and capture them muliple times.
  timeNow = millis(); 
  timeDelta = timeNow - lastTime;
  if (timeDelta <= KEYCHECK_DELAY) {
    return; // sampling the button press too fast, come back a bit later
  }   
  else {  // sufficent time has passed, process the buttons again...
  lastTime=timeNow;
   buttoncommands();
  }
}

void RGBW( unsigned short f, 
 unsigned short x, unsigned short y,
 unsigned short r, unsigned short g,
 unsigned short b, unsigned short w ) {
 if ( y < MAXY && x < MAXX-4 ) {
  if ( r > 0 ) frames[f].SetPoint(x,   y); else frames[f].ClearPoint(x,  y);
  if ( g > 0 ) frames[f].SetPoint(x+1, y); else frames[f].ClearPoint(x+1,y);
  if ( b > 0 ) frames[f].SetPoint(x+2, y); else frames[f].ClearPoint(x+2,y);
  if ( w > 0 ) frames[f].SetPoint(x+3, y); else frames[f].ClearPoint(x+3,y);
 }
}

void buttoncommands() {
  if ( buttonDown(rightButton) ) reds=!reds;
  if ( buttonDown(leftButton) ) greens=!greens;
  if ( buttonDown(upButton) ) blues=!blues;
  if ( buttonDown(downButton) ) whites=!whites;
}

void Init_Kix( int q ) {
  xcol[q] = random(XCOMP);
  
  xdir[q] = 1;
  ydir[q] = 1;
  
  // pick a random starting spot
  xpos[q][0] = random (WH);
  ypos[q][0] = random (WH);
  // set the directions
  xdir[q] = 1;
  ydir[q] = 1;

  // fill in some initial values of the tail so we don't just light up the (0,0) lights.
  short i=0;
  for (i = 1; i < TAIL; i++) {
    xpos[q][i] = xpos[q][(i-1)] + xdir[q];
    ypos[q][i] = ypos[q][(i-1)] + ydir[q];
  }
}

void Step_Kix( int q ) {
  short i=0;
  for (i = TAIL-1; i > 0; i--) {
    xpos[q][i] = xpos[q][(i-1)];
    ypos[q][i] = ypos[q][(i-1)];
  }
  
  if ( !_constrain_x ) {
   xpos[q][0] += xdir[q];              // stay the course
   if (xpos[q][0] > MAXX) {            // off the right edge
    xdir[q]= -1;    // pick a new random velocity to the left
    xpos[q][0] = MAXX;     // stay on board
   } else if (xpos[q][0] < 0) {            // off the left edge
    xdir[q]= 1;
    xpos[q][0] = 0;
   }
  }
  
  if ( !_constrain_y ) {
   ypos[q][0] += ydir[q];              // stay the course
    
   if (ypos[q][0] > MAXY) {
    ydir[q]= - (random (2) + 1);
    ypos[q][0] = MAXY;
   } else if (ypos[q][0] < 0) {
    ydir[q]= random (2) + 1;
    ypos[q][0] = 0;
   }
  }

  // Set the new ball location, and its "echoes" behind it of where it has been
  if ( _color_restrict ) {
   for (i = 0; i < TAIL; i++) {
    int col = TAIL - i - 1;
    int xp = xpos[q][i];
    int yp = ypos[q][i];
    // add this point to 1, 2, 3 or 4 frame buffers to give us up to 4 brighness levels
    if ( (xcol[q] == 0 && reds)
      || (xcol[q] == 1 && greens)
      || (xcol[q] == 2 && blues)
      || (xcol[q] == 3 && whites) ) {
     int xp_c=XRGB(xp,xcol[q]);
     if (col & 8) { frames[0].SetPoint(xp_c, yp); if ( xp_c > 0 && _color_pairing ) frames[0].SetPoint(xp_c-1,yp); }
     if (col & 4) { frames[1].SetPoint(xp_c, yp); if ( xp_c > 0 && _color_pairing ) frames[1].SetPoint(xp_c-1,yp); }
     if (col & 2) { frames[2].SetPoint(xp_c, yp); if ( xp_c > 0 && _color_pairing ) frames[2].SetPoint(xp_c-1,yp); }
     if (col & 1) frames[3].SetPoint(xp_c, yp); 
    }
   }
  } else {
   for (i = 0; i < TAIL; i++) {
    int col = TAIL - i - 1;
    int xp = xpos[q][i];
    int yp = ypos[q][i];
    // add this point to 1, 2, 3 or 4 frame buffers to give us up to 4 brighness levels
    if (col & 8) { frames[0].SetPoint(xp, yp); if ( xp > 0 && _color_pairing ) frames[0].SetPoint(xp-1,yp); }
    if (col & 4) { frames[1].SetPoint(xp, yp); if ( xp > 0 && _color_pairing ) frames[1].SetPoint(xp-1,yp); }
    if (col & 2) { frames[2].SetPoint(xp, yp); if ( xp > 0 && _color_pairing ) frames[2].SetPoint(xp-1,yp); }
    if (col & 1) frames[3].SetPoint(xp, yp);
   }
  }
}

int COUNTDOWN=0;
int CLEAR_COUNTDOWN;
unsigned long FRAME=0;

void loop()                     // run over and over again
{ 
  short i, j;
  short reps;
  
  FRAME++;
  if ( FRAME > 1000 ) FRAME=1;
  if ( FRAME == 1 || random(1,1000) == 69 ) {
   do {
    if ( random(1,10) > 7 ) {
     CLEAR_DELAY=random(0,200);
     DELAY=random(0,60);
    } else {
     CLEAR_DELAY=random(0,10);
     DELAY=random(0,5);
    }
    active=random(1,QIXES);
    if ( random(1,10) < 5 ) {
      _constrain_x=false;
      _constrain_y=false;
    } else {
     _constrain_x=random(1,10) > 4;
     _constrain_y=random(1,10) > 4;
    }
    _color_pairing=(random(1,10) < 4);
    _color_restrict=(random(1,10) > 2);
    reds  =random(1,10) < 5 ? false : true;
    greens=random(1,10) < 5 ? false : true;
    blues =random(1,10) < 5 ? false : true;
    whites=random(1,10) < 5 ? false : true;
   } while ( !reds && !greens && !blues && !whites );
  }
  
  if ( COUNTDOWN <= 0 ) {
 // Clear all the frame buffers
   if ( CLEAR_COUNTDOWN <= 0 ) {
    for (i = 0; i < FRAMEBUFFERS; i++) { frames[i].Clear(); }
    CLEAR_COUNTDOWN=CLEAR_DELAY;
   } else CLEAR_COUNTDOWN -= 1;
   for ( i =0; i < active; i++ ) Step_Kix(i);
   COUNTDOWN=DELAY;
  } else COUNTDOWN -= 1;
 
  // Display the ball and its tail. Refresh the frontmost balls more than the later tail
//  for (i = 0; i < FRAMEBUFFERS; i++) {
    frames[0].RefreshAll(8);   //Draw frame buffer, more for the lead ball, less for the tail
    frames[1].RefreshAll(4);
    frames[2].RefreshAll(2);
    frames[3].RefreshAll(1);
//  }
}


// Hardware initialization for full button set:
void buttoninit(void)
{
  DDRB &= 0xFE;    // PORTB pin 0 is button s2, set it up as an input
  PORTB |= 0x01;   // PORTB pin 0 pull up
  DDRC &= 0xE0;    // PORTC pins 0 - 4 set up as inputs corresponding to buttons 1 - 5
  PORTC |= 0x1F;   // PORTC pins 0 - 4 pull ups 
}

unsigned short buttonDown(unsigned short ButtonNumber)
{
  switch(ButtonNumber)
  {
  case 0:
    return ((PINB & 1) == 0);		// Button s2
    break; 
  case 1:
    return ((PINC & 1) == 0);		// Button b1
    break;
  case 2:
    return ((PINC & 2) == 0);		// Button b2
    break;			
  case 3:
    return ((PINC & 4) == 0);		// Button b3
    break;
  case 4:
    return ((PINC & 8) == 0);		// Button b4
    break;
  case 5:
    return ((PINC & 16) == 0);		// Button b5
    break;
  default:
    return false;	// invalid button number 
  }		
}
