peggy2-striper
==============

Peggy2 Stripe Morpher for Evil Mad Scientist's Peggy 2.0


Description
-----------

Peggy2 Stripe Morpher

Makes your Peggy2 look like a 1970s TV sci-fi panel with ever-changing morphing Qix-based animations that have varying parameters and constraints.

- Written for 25 parallel strips of RGBW...R configuration (vertically homogenous RGBW x-aligned), I made my Peggy2 off-spec instead of making square pixels I made a line of pixels similar to actual phosphor CRT pixel configurations on classic "boob tube" television sets.  Same color is aligned along a vertical line creating about 9 pixels wide (9.25 to be precise as first and last LED columns are both red) and 25 pixels tall instead of the grid configuration found on most peggy2 RGBWs.
- Works fine with RGBW 2x2-square "superpixel" or monochrome or other off-spec configurations, however you will want to change the way it handles multiple colors by modifying XRGB() macro and some of the redraw logic to handle the 2x2-square superpixel configuration.  Specifically, you must make changes to the "color restrict" feature and the way the booleans "reds,greens,blues,whites" are interpreted in the redraw section.  These changes won't affect monochrome results, so don't bother doing that if you have only one color of LED installed.

 Author: 2014 (c) Herbert Elwood Gilliland III
 Arduino sketch code for Peggy 2.0, using the Peggy2 library, version 0.2.  
 License: Identical to Peggy2_Qix / Peggy2Lib
 
 Based in Peggy2_Qix
 Create a light that bounces off Paggy's "walls", and leaves a tail of 4 lights behind it of decreasing brightness
 Copyright (c) 2009 David S. Kauffman
 inspired by Peggy 2.0 example code from Windell Oskay
 
 This example is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
