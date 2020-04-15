//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: resource.cpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#include "stddefs.h"
#include "stapp_main.h"
#include "resource.hpp"
#include "GDebug.hpp"
#include "APIBitmap.hpp"
#include "images.hpp"

int FONT_OFFSETY[MAX_FONT_ID] = {23, 24, 27, 27};

GAMLOAD_Object_t* LoadPreDefinedGam( GAMLOAD_Object_t* Src )
{
   GAMLOAD_Object_t* Object = NULL;
   Object = new GAMLOAD_Object_t();
   //Object->Bitmap= new STGXOBJ_Bitmap_t();
   //Object->Palette= new STGXOBJ_Palette_t();

   if( Object != NULL )
   {
      if( InitializeGamObject(Src, Object) != ST_NO_ERROR )
      {
         delete Object;
         Object = NULL;
      }
   }

   return Object;
}
//////////////////////////////////////////////////////////////////////////////////////
