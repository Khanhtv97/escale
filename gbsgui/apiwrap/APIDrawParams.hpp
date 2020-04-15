/*
 * APIDrawParams.hpp
 *
 *  Created on: Apr 14, 2012
 *      Author: seta
 */

#ifndef APIDRAWPARAMS_HPP_
#define APIDRAWPARAMS_HPP_

#if defined(GFX_LAYER_MAX_NUMBER)
#define GFX_SCALE_DEFAULT 0
#define GFX_SCALE_AUTO    1
#define GFX_SCALE_MANUAL  2
#endif

class APIDrawParams
{
public:
   int planeId;
   int posX;
   int posY;
   int width;
   int height;
   int alpha; // from 0 - 128
   BOOL IsAntiAliasingEnabled; // TRUE || FALSE
   BOOL IsAntiFlickerEnabled;  // TRUE || FALSE
   BOOL IsAntiFlutterEnabled;  // TRUE || FALSE

   APIDrawParams()
   {
      planeId = 0;
      posX = 0;
      posY = 0;
      width = 0;
      height = 0;
      alpha = 128;
      IsAntiAliasingEnabled = TRUE;
      IsAntiFlickerEnabled = TRUE;
      IsAntiFlutterEnabled = TRUE;
   }

   ~APIDrawParams()
   {
   }
};

#endif /* APIDRAWPARAMS_HPP_ */
