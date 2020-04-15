//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2016 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GImageDecode.cpp                                                           //
//////////////////////////////////////////////////////////////////////////////////////

#include "GImageDecode.hpp"
#include "GDebug.hpp"
#include <iostream>

#include "lodepng.h"
#include "nanojpeg.h"
#include "nanobmp.h"

//////////////////////////////////////////////////////////////////////////////////////
GImageDecode& GImageDecode::getInstance()
{
   static GImageDecode instance;
   return instance;
}

//////////////////////////////////////////////////////////////////////////////////////
GImageDecode::GImageDecode()
{
   m_pGamLoadObject = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
GImageDecode::~GImageDecode()
{

}

//////////////////////////////////////////////////////////////////////////////////////
U16 GImageDecode::RGBAtoARGB1555(U8 r, U8 g, U8 b,U8 a)
{
   U16 a1,r1,g1,b1;
   a1 = (U16)(a & 0x80) >> 7;
   r1 = (U16)(r & 0xF8) >> 3;
   g1 = (U16)(g & 0xF8) >> 3;
   b1 = (U16)(b & 0xF8) >> 3;
   return (U16)((a1 << 15 )| (r1<<10 ) | (g1 <<5) | (b1));
}

//////////////////////////////////////////////////////////////////////////////////////
U16 GImageDecode::RGBtoARGB1555(U8 r, U8 g, U8 b)
{
   U16 a1,r1,g1,b1;
   a1 = 1;
   r1 = (U16)(r & 0xF8) >> 3;
   g1 = (U16)(g & 0xF8) >> 3;
   b1 = (U16)(b & 0xF8) >> 3;
   return (U16)((a1 << 15 )| (r1<<10 ) | (g1 <<5) | (b1));
}

//////////////////////////////////////////////////////////////////////////////////////
GAMLOAD_Object_t* GImageDecode::ImagetoGAMObject(U8* image, U16 width, U16 height, Image_ColorType_t colorType)
{
   GError_t ErrCode = GERROR_NONE;

   STGXOBJ_Bitmap_t         GXOBJ_Bitmap,LAYER_GXOBJ_Bitmap;
   STGXOBJ_Palette_t        GXOBJ_Palette,LAYER_GXOBJ_Palette;
   STGXOBJ_BitmapAllocParams_t  GXOBJ_BitmapAllocParams1;
   STGXOBJ_BitmapAllocParams_t  GXOBJ_BitmapAllocParams2;
   STGXOBJ_PaletteAllocParams_t GXOBJ_PaletteAllocParams;
   U32 i,j,size;

   if( m_pGamLoadObject != NULL )
   {
      FreeGamObject(m_pGamLoadObject);
   }

   m_pGamLoadObject = new GAMLOAD_Object_t();
   if( m_pGamLoadObject == NULL )
   {
      return NULL;
   }

   m_pGamLoadObject->Bitmap  = new STGXOBJ_Bitmap_t();
   if( m_pGamLoadObject->Bitmap == NULL )
   {
      delete m_pGamLoadObject;
      m_pGamLoadObject = NULL;
      return NULL;
   }

   m_pGamLoadObject->Palette = new STGXOBJ_Palette_t();
   if(m_pGamLoadObject->Palette == NULL)
   {
      delete m_pGamLoadObject->Bitmap;
      m_pGamLoadObject->Bitmap = NULL;
      delete m_pGamLoadObject;
      m_pGamLoadObject = NULL;
      return NULL;
   }

   // Clear the Bitmap/Palette structure
   memset(m_pGamLoadObject->Bitmap, 0, sizeof(STGXOBJ_Bitmap_t));
   memset(m_pGamLoadObject->Palette, 0, sizeof(STGXOBJ_Palette_t));

   // Allocate the bitmap for the picture
   m_pGamLoadObject->Bitmap->Width = width;
   m_pGamLoadObject->Bitmap->Height = height;
   m_pGamLoadObject->Bitmap->Size1 = 0;
   m_pGamLoadObject->Bitmap->Size2 = 0;
   m_pGamLoadObject->Bitmap->ColorType = STGXOBJ_COLOR_TYPE_ARGB1555;
   m_pGamLoadObject->Bitmap->BitmapType = (STGXOBJ_BitmapType_t) 0;

   // Set statical
   m_pGamLoadObject->Bitmap->PreMultipliedColor   = FALSE;
   m_pGamLoadObject->Bitmap->ColorSpaceConversion = STGXOBJ_ITU_R_BT601;
   m_pGamLoadObject->Bitmap->AspectRatio          = STGXOBJ_ASPECT_RATIO_4TO3;
   m_pGamLoadObject->Bitmap->BigNotLittle         = FALSE;
   m_pGamLoadObject->Bitmap->Offset               = 0;
   m_pGamLoadObject->Bitmap->Data1_p = NULL;
   m_pGamLoadObject->Bitmap->Data1_Cp = NULL;
   m_pGamLoadObject->Bitmap->Data1_NCp = NULL;
   m_pGamLoadObject->Bitmap->Data2_p = NULL;
   m_pGamLoadObject->Bitmap->Data2_Cp = NULL;
   m_pGamLoadObject->Bitmap->Size2 = 0;

   //Compute picture parameters
   ErrCode = STGXOBJ_GetBitmapAllocParams(m_pGamLoadObject->Bitmap,STGXOBJ_NO_HARD, &GXOBJ_BitmapAllocParams1, &GXOBJ_BitmapAllocParams2);

   if( ErrCode != ST_NO_ERROR )
   {
      if( m_pGamLoadObject->Bitmap != NULL )
      {
         delete m_pGamLoadObject->Bitmap;
         m_pGamLoadObject->Bitmap = NULL;
      }

      if( m_pGamLoadObject->Palette != NULL )
      {
         delete m_pGamLoadObject->Palette;
         m_pGamLoadObject->Palette = NULL;
      }

      if( m_pGamLoadObject != NULL )
      {
         delete m_pGamLoadObject;
         m_pGamLoadObject = NULL;
      }

      return NULL;
   }

   m_pGamLoadObject->Bitmap->Offset = GXOBJ_BitmapAllocParams1.Offset;
   m_pGamLoadObject->Bitmap->Pitch  = GXOBJ_BitmapAllocParams1.Pitch;
   m_pGamLoadObject->Bitmap->Size1  = GXOBJ_BitmapAllocParams1.AllocBlockParams.Size;
   m_pGamLoadObject->Bitmap->Size2  = GXOBJ_BitmapAllocParams2.AllocBlockParams.Size;
   m_pGamLoadObject->Bitmap->SubByteFormat = (STGXOBJ_SubByteFormat_t) 1;

   if( m_pGamLoadObject->Bitmap->Size1 != 0 )
   {
      m_pGamLoadObject->Bitmap->Data1_NCp = (void *)SYS_Memory_Allocate(SYS_MEMORY_CACHED, m_pGamLoadObject->Bitmap->Size1, GXOBJ_BitmapAllocParams1.AllocBlockParams.Alignment);
      m_pGamLoadObject->Bitmap->Data1_Cp = m_pGamLoadObject->Bitmap->Data1_NCp;

      if( m_pGamLoadObject->Bitmap->Data1_Cp == NULL )
      {
         if( m_pGamLoadObject->Palette->Data_Cp != NULL )
         {
            SYS_Memory_Deallocate(SYS_MEMORY_CACHED, (U32)m_pGamLoadObject->Palette->Data_Cp);
         }

         if( m_pGamLoadObject->Bitmap != NULL )
         {
            delete m_pGamLoadObject->Bitmap;
            m_pGamLoadObject->Bitmap = NULL;
         }

         if( m_pGamLoadObject->Palette != NULL )
         {
            delete m_pGamLoadObject->Palette;
            m_pGamLoadObject->Palette = NULL;
         }

         if( m_pGamLoadObject != NULL )
         {
            delete m_pGamLoadObject;
            m_pGamLoadObject = NULL;
         }

         return NULL;
      }

      m_pGamLoadObject->Bitmap->Data1_p = (void *)SYS_Memory_UserToDevice(SYS_MEMORY_CACHED,(U32)m_pGamLoadObject->Bitmap->Data1_Cp);

      switch( colorType )
      {
      case COLOR_TYPE_RGBA:
         size = width*height*4;
         _DTraceError("Start copy data size = %d", size);
         for( i=0, j=0; i< size; i+=4 )
         {
            ((U16 *)m_pGamLoadObject->Bitmap->Data1_NCp)[j++] = RGBAtoARGB1555(image[i], image[i+1], image[i+2], image[i+3]);
         }
         _DTraceError("End copy");
         break;

      case COLOR_TYPE_RGB:
         size = width*height*3;
         for( i=0, j=0; i< size; i+=3 )
         {
            ((U16 *)m_pGamLoadObject->Bitmap->Data1_NCp)[j++] = RGBtoARGB1555(image[i], image[i+1], image[i+2]);
         }
         break;

      default:
         break;
      }
   }

   return m_pGamLoadObject;
}

//////////////////////////////////////////////////////////////////////////////////////
GAMLOAD_Object_t* GImageDecode::DecodePng(const char* FileName)
{
   GError_t ErrCode;
   void* FileHandle;
   GAMLOAD_Object_t* pObject = NULL;

   _DTraceError("DecodePng Start \n");

   unsigned char* image = NULL;
   unsigned width, height,outsize;
   unsigned error = lodepng_decode_file(&image, &width, &height, &outsize, FileName, LCT_RGBA, 8);

   _DTraceError("Width = %d, Height = %d, outsize = %d\n", width, height, outsize);

   if( error != 0 )
   {
      _DTraceError("DecodePng error %d \n", error);
      if( image != NULL )
      {
         free(image);
         image = NULL;
      }

      return NULL;
   }

   pObject = ImagetoGAMObject(image, width, height, COLOR_TYPE_RGBA);

   if( image != NULL )
   {
      free(image);
      image = NULL;
   }

   _DTraceError("DecodePng End \n");
   return pObject;
}

//////////////////////////////////////////////////////////////////////////////////////
GAMLOAD_Object_t* GImageDecode::DecodeJPG(const char* FileName)
{
   GError_t ErrCode;
   int size;
   unsigned char* buf = NULL;
   unsigned char* image = NULL;
   void* file = NULL;
   unsigned width, height, imageSize;
   GAMLOAD_Object_t* pObject = NULL;

   file = SYS_FOpen(FileName, "rb");
   if( file == NULL )
   {
      return NULL;
   }

   if( SYS_FSeek(file, 0, SEEK_END) != 0 )
   {
      SYS_FClose(file);
      return NULL;
   }

   size = SYS_FTell(file);

   buf = (unsigned char*)malloc(size);

   if( buf == NULL )
   {
      SYS_FClose(file);
      return NULL;
   }

   SYS_FSeek(file, 0, SEEK_SET);
   size = (int) SYS_FRead(buf, 1, size, file);

   // It may give LONG_MAX as directory size, this is invalid for us.

   if( size == LONG_MAX || size <= 0 )
   {
      size = -1;
      if( buf != NULL )
      {
         free(buf);
         buf = NULL;
      }
      SYS_FClose(file);
      return NULL;
   }

   SYS_FClose(file);

   njInit();

   if( njDecode(buf, size) )
   {
      _DTraceError("Error decoding the input file.\n");
      if( buf != NULL )
      {
         free(buf);
         buf = NULL;
      }
      njDone();
      return NULL;
   }

   width = njGetWidth();
   height = njGetHeight();
   image = njGetImage();
   imageSize = njGetImageSize();

   pObject = ImagetoGAMObject(image, width, height, COLOR_TYPE_RGB);

   njDone();

   if( buf != NULL )
   {
      free(buf);
      buf = NULL;
   }

   return pObject;
}

//////////////////////////////////////////////////////////////////////////////////////
GAMLOAD_Object_t* GImageDecode::DecodeBMP(const char *FileName)
{
   GAMLOAD_Object_t* pObject = NULL;
   bmpread_t bitmap;

   // Allow read Any size, read top down bitmap
   if( !bmpread(FileName, BMPREAD_ANY_SIZE | BMPREAD_TOP_DOWN, &bitmap) )
   {
      _DTraceError("%s: Error loading bitmap file\n", FileName);
      return NULL;
   }

   pObject = ImagetoGAMObject(bitmap.rgb_data, bitmap.width, bitmap.height, COLOR_TYPE_RGB);

   bmpread_free(&bitmap);

   return pObject;
}

//////////////////////////////////////////////////////////////////////////////////////
void GImageDecode::FreeGamObject(GAMLOAD_Object_t* pGamObject)
{
   _DTraceError("");
   // Free memory
   if( pGamObject == NULL )
   {
      pGamObject = m_pGamLoadObject;
      m_pGamLoadObject = NULL;;
   }

   if( pGamObject == NULL )
   {
      _DTraceError("pGamObject == NULL");
      return;
   }

   if( pGamObject->Palette != NULL )
   {
      if( pGamObject->Palette->Data_Cp != NULL )
      {
         SYS_Memory_Deallocate(SYS_MEMORY_CACHED, (U32)pGamObject->Palette->Data_Cp);
         pGamObject->Palette->Data_Cp = NULL;
      }

      delete pGamObject->Palette;
      pGamObject->Palette = NULL;
   }

   if( pGamObject->Bitmap != NULL )
   {
      if( pGamObject->Bitmap->Data2_Cp != NULL )
      {
         SYS_Memory_Deallocate(SYS_MEMORY_CACHED, (U32)pGamObject->Bitmap->Data2_Cp);
         pGamObject->Bitmap->Data2_Cp = NULL;
      }

      if( pGamObject->Bitmap->Data1_Cp != NULL )
      {
         SYS_Memory_Deallocate(SYS_MEMORY_CACHED, (U32)pGamObject->Bitmap->Data1_Cp);
         pGamObject->Bitmap->Data1_Cp = NULL;
      }

      delete pGamObject->Bitmap;
      pGamObject->Bitmap = NULL;
   }

   delete pGamObject;
   pGamObject = NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
