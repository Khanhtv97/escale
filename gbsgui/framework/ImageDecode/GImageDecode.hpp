//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2016 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GImageDecode.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GIMAGEDECODE_HPP_
#define GIMAGEDECODE_HPP_

#include "stapp_main.h"
#include "GDebug.hpp"
#include "GError.hpp"

typedef enum Image_ColorType_e
{
   COLOR_TYPE_RGB,
   COLOR_TYPE_RGBA
}
Image_ColorType_t;

class GImageDecode
{
private:
   GImageDecode();
   ~GImageDecode();
   U16 RGBAtoARGB1555(U8 a,U8 r, U8 g, U8 b);
   U16 RGBtoARGB1555(U8 r, U8 g, U8 b);
   GAMLOAD_Object_t* ImagetoGAMObject(U8* image, U16 width, U16 height, Image_ColorType_t colorType);

   GAMLOAD_Object_t* m_pGamLoadObject;

public:
   static GImageDecode& getInstance();

   GAMLOAD_Object_t* DecodePng(const char *FileName);
   GAMLOAD_Object_t* DecodeJPG(const char *FileName);
   GAMLOAD_Object_t* DecodeBMP(const char *FileName);

   void FreeGamObject(GAMLOAD_Object_t* pGamObject = NULL);
};
#endif /* GIMAGEDECODE_HPP_ */
