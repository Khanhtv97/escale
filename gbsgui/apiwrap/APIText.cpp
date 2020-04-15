//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APIText.cpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#include "APIText.hpp"
#include "GDebug.hpp"

static TEXT_Parameters_t* API_TEXT_Parameters[TEXT_MAX_NUMBER_OF_TEXT_OBJECTS];

//TODO: need  Reset text parameters */
//////////////////////////////////////////////////////////////////////////////////////
void APITextInit()
{

#if defined(TEXT_MAX_NUMBER) && defined(GFX_LAYER_MAX_NUMBER) && defined(BLIT_MAX_NUMBER)

   for( int i = 0; i < TEXT_MAX_NUMBER_OF_TEXT_OBJECTS; i++ )
   {
      FreeTextHandle(i);
      API_TEXT_Parameters[i]=NULL;
   }

#endif

}

//////////////////////////////////////////////////////////////////////////////////////
int getNextFreeTextId()
{
   for( int i = 0; i < TEXT_MAX_NUMBER_OF_TEXT_OBJECTS; i++ )
   {
      if( API_TEXT_Parameters[i] == NULL )
      {
         return i;
      }
   }

   return -1;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL PrintText(int TextId, const char* TextString, TextParams_t TxtPms, int width, int height)
{
   ST_ErrorCode_t ec = ST_NO_ERROR;

   if( strlen(TextString) < 1 )
   {
      return 0;
   }

   STLAYER_ViewPortParams_t      LAYER_ViewPortParams;
   STGXOBJ_Bitmap_t              LAYER_GXOBJ_Bitmap;
   STGXOBJ_Palette_t             LAYER_GXOBJ_Palette;
   STLAYER_ViewPortSource_t      LAYER_ViewPortSource;

   if( API_TEXT_Parameters[TextId] != NULL )
   {
      _DTraceError("TxtObjID %d is already used!",TextId);
      return ST_ERROR_DEVICE_BUSY;
   }

   int Plane = TxtPms.planeId;

   //GetLayerInformation(Plane);
   //Get the layer informations
   memset(&LAYER_ViewPortParams, 0, sizeof(STLAYER_ViewPortParams_t));
   memset(&LAYER_ViewPortSource, 0, sizeof(STLAYER_ViewPortSource_t));
   memset(&LAYER_GXOBJ_Bitmap  , 0, sizeof(STGXOBJ_Bitmap_t));
   memset(&LAYER_GXOBJ_Palette,  0, sizeof(STGXOBJ_Palette_t));

   LAYER_ViewPortSource.Data.BitMap_p = &LAYER_GXOBJ_Bitmap;
   LAYER_ViewPortSource.Palette_p     = &LAYER_GXOBJ_Palette;
   LAYER_ViewPortParams.Source_p      = &LAYER_ViewPortSource;

   ec = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane], &LAYER_ViewPortParams);

   if( ec != ST_NO_ERROR )
   {
      _DTraceError("PrintText --> Unable to get the source layer parameters !\n");
      return(TRUE);
   }

   ec = TEXT_GetTextHandle(&API_TEXT_Parameters[TextId]);

   if( ec != ST_NO_ERROR )
   {
      print("--> Unable to get text index %d, it is probably already used !\n",TextId);
      return ST_ERROR_NO_FREE_HANDLES;
   }

   API_TEXT_Parameters[TextId]->FontHandle                = TxtPms.fontId;
   API_TEXT_Parameters[TextId]->GFX_LAYER_GXOBJ_Bitmap_p  = LAYER_ViewPortParams.Source_p->Data.BitMap_p;
   API_TEXT_Parameters[TextId]->GFX_LAYER_GXOBJ_Palette_p = LAYER_ViewPortParams.Source_p->Palette_p;
   API_TEXT_Parameters[TextId]->Container_XPos            = 0;
   API_TEXT_Parameters[TextId]->Container_YPos            = 0;
   API_TEXT_Parameters[TextId]->Container_Width           = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
   API_TEXT_Parameters[TextId]->Container_Height          = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
   API_TEXT_Parameters[TextId]->X_Pos                     = TxtPms.posX;
   API_TEXT_Parameters[TextId]->Y_Pos                     = TxtPms.posY;
   API_TEXT_Parameters[TextId]->WrapMode                  = TEXT_WRAP_CHARACTER;
   API_TEXT_Parameters[TextId]->Append                    = FALSE;
   API_TEXT_Parameters[TextId]->Blend                     = TRUE;
   API_TEXT_Parameters[TextId]->FixedWidthCharacterSize   = FALSE;
   API_TEXT_Parameters[TextId]->Color                     = (U32)TxtPms.color;
   API_TEXT_Parameters[TextId]->Alpha                     = TxtPms.alpha;

   if( width > 0 )
   {
      API_TEXT_Parameters[TextId]->TextFieldWidth = width;
   }

   ec = TEXT_PrintText(API_TEXT_Parameters[TextId], TextString);

   if( ec != ST_NO_ERROR )
   {
      TEXT_FreeTextHandle(API_TEXT_Parameters[TextId]);
      API_TEXT_Parameters[TextId]=NULL;
      print("--> Can't display the text:\"%s\"",TextString);
      return ST_ERROR_FEATURE_NOT_SUPPORTED;
   }

   return ST_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL PrintAlignedText(int TextId, const char* TextString, TextParams_t obj_TextParams, int TextWidth, int AlignType)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   STLAYER_ViewPortParams_t LAYER_ViewPortParams;
   STGXOBJ_Bitmap_t         LAYER_GXOBJ_Bitmap;
   STGXOBJ_Palette_t        LAYER_GXOBJ_Palette;
   STLAYER_ViewPortSource_t LAYER_ViewPortSource;
   int Plane = obj_TextParams.planeId;

   // Get the layer informations
   memset(&LAYER_ViewPortParams, 0, sizeof(STLAYER_ViewPortParams_t));
   memset(&LAYER_ViewPortSource, 0, sizeof(STLAYER_ViewPortSource_t));
   memset(&LAYER_GXOBJ_Bitmap  , 0, sizeof(STGXOBJ_Bitmap_t));
   memset(&LAYER_GXOBJ_Palette,  0, sizeof(STGXOBJ_Palette_t));

   LAYER_ViewPortSource.Data.BitMap_p = &LAYER_GXOBJ_Bitmap;
   LAYER_ViewPortSource.Palette_p     = &LAYER_GXOBJ_Palette;
   LAYER_ViewPortParams.Source_p      = &LAYER_ViewPortSource;

   ErrCode = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane], &LAYER_ViewPortParams);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("PrintText --> Unable to get the source layer parameters !\n");
      return(TRUE);
   }

   if( API_TEXT_Parameters[TextId] != NULL )
   {
      _DTraceError("Text object ID %d is already used!",TextId);
      return(TRUE);
   }

   // Get text handle
   ErrCode = TEXT_GetTextHandle( &API_TEXT_Parameters[TextId] );

   if( ErrCode != ST_NO_ERROR )
   {
      print("--> Unable to get text index %d, it is probably already used !\n",TextId);
      return(TRUE);
   }

   // Set text parameters
   API_TEXT_Parameters[TextId]->FontHandle                = obj_TextParams.fontId;
   API_TEXT_Parameters[TextId]->GFX_LAYER_GXOBJ_Bitmap_p  = LAYER_ViewPortParams.Source_p->Data.BitMap_p;
   API_TEXT_Parameters[TextId]->GFX_LAYER_GXOBJ_Palette_p = LAYER_ViewPortParams.Source_p->Palette_p;
   API_TEXT_Parameters[TextId]->Container_XPos            = 0;
   API_TEXT_Parameters[TextId]->Container_YPos            = 0;
   API_TEXT_Parameters[TextId]->Container_Width           = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
   API_TEXT_Parameters[TextId]->Container_Height          = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
   API_TEXT_Parameters[TextId]->X_Pos                     = obj_TextParams.posX;
   API_TEXT_Parameters[TextId]->Y_Pos                     = obj_TextParams.posY;
   API_TEXT_Parameters[TextId]->WrapMode                  = TEXT_WRAP_CHARACTER;
   API_TEXT_Parameters[TextId]->Append                    = FALSE;
   API_TEXT_Parameters[TextId]->Blend                     = TRUE;
   API_TEXT_Parameters[TextId]->FixedWidthCharacterSize   = FALSE;
   API_TEXT_Parameters[TextId]->Color                     = (U32)obj_TextParams.color;
   API_TEXT_Parameters[TextId]->Alpha                     = obj_TextParams.alpha;

   // Display text
   ErrCode = TEXT_PrintAlignedText(API_TEXT_Parameters[TextId], TextWidth, AlignType, TextString);

   if( ErrCode != ST_NO_ERROR )
   {
      TEXT_FreeTextHandle(API_TEXT_Parameters[TextId]);
      API_TEXT_Parameters[TextId]=NULL;
      print("--> Can't display the text:\"%s\"",TextString);
      return(TRUE);
   }

   // Return no errors
   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL PrintLines( int startTextId, TextParams_t obj_TextParams, int hSpace)
{
   STLAYER_ViewPortParams_t LAYER_ViewPortParams;
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 TextObjectIndex;

   int Plane = obj_TextParams.planeId;

   // GetLayerInformation(Plane);
   ErrCode = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane], &LAYER_ViewPortParams);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to get the source layer parameters !\n");
      return(TRUE);
   }

   int X_Out = obj_TextParams.posX;
   int Y_Out = obj_TextParams.posY;

   char lines[20][30];
   int j = 0, k = 0, i = 0;
   U8* listChannelStr = (U8*)"VINH LONG\nBINH DUONG\nCINEMAX\nAN VIEN\nLET\'S VIET\nHA NOI\nTHE THAO\nGIAI TRI\nPHIM TRUYEN\nSAC MAU CUOC SONG\nVTV3\nHA TAY";
   int n = strlen((char*)listChannelStr);

   while(i < n)
   {
      if( listChannelStr[i] == 10 )
      {
         lines[j][k] = '\0';
         //print("lines[%d] = %s",j,lines[j]);
         j++;
         k = 0;
      }
      else
      {
         lines[j][k] = listChannelStr[i];
         k++;
      }
      i++;
   }

   lines[j][k] = '\0';
   startTextId = abs(startTextId%4);

   int lineNum = j;

   for( int i = 0; i < 8; i++ )
   {
      TextObjectIndex = i;
      // Get text handle
      ErrCode = TEXT_GetTextHandle(&API_TEXT_Parameters[TextObjectIndex]);
      // Set text parameters
      API_TEXT_Parameters[TextObjectIndex]->FontHandle = obj_TextParams.fontId;
      API_TEXT_Parameters[TextObjectIndex]->GFX_LAYER_GXOBJ_Bitmap_p = LAYER_ViewPortParams.Source_p->Data.BitMap_p;
      API_TEXT_Parameters[TextObjectIndex]->GFX_LAYER_GXOBJ_Palette_p = LAYER_ViewPortParams.Source_p->Palette_p;
      API_TEXT_Parameters[TextObjectIndex]->Container_XPos = 0;
      API_TEXT_Parameters[TextObjectIndex]->Container_YPos = 0;
      API_TEXT_Parameters[TextObjectIndex]->Container_Width = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
      API_TEXT_Parameters[TextObjectIndex]->Container_Height = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
      API_TEXT_Parameters[TextObjectIndex]->X_Pos = X_Out;
      API_TEXT_Parameters[TextObjectIndex]->Y_Pos = Y_Out + hSpace * i;
      API_TEXT_Parameters[TextObjectIndex]->WrapMode = TEXT_WRAP_CHARACTER;
      API_TEXT_Parameters[TextObjectIndex]->Append = FALSE;
      API_TEXT_Parameters[TextObjectIndex]->Blend = TRUE;
      API_TEXT_Parameters[TextObjectIndex]->FixedWidthCharacterSize = FALSE;
      API_TEXT_Parameters[TextObjectIndex]->Color = (U32)obj_TextParams.color;

      if( i%8 != 2 )
      {
         API_TEXT_Parameters[TextObjectIndex]->Alpha = 0x60;
      }
      else
      {
         API_TEXT_Parameters[TextObjectIndex]->Alpha = 0x80;
      }

      ErrCode = TEXT_PrintText(API_TEXT_Parameters[TextObjectIndex], lines[i+startTextId]);

      if( ErrCode != ST_NO_ERROR )
      {
         TEXT_FreeTextHandle(API_TEXT_Parameters[TextObjectIndex]);
         API_TEXT_Parameters[TextObjectIndex]=NULL;
         return(TRUE);
         break;
      }
      else
      {
         TextObjectIndex++;
      }
   }

   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t DeleteText(int TextId)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   if( API_TEXT_Parameters[TextId] == NULL )
   {
      _DTraceInfo("Text handle index %d not yet used\n",TextId);
      return ErrCode;
   }

   // Clear the text
   ErrCode = TEXT_ClearText(API_TEXT_Parameters[TextId]);

   if( ErrCode != ST_NO_ERROR )
   {
      print("--> Can't clear the text index %d\n",TextId);
      return(ErrCode);
   }

   // Free the text
   ErrCode = TEXT_FreeTextHandle(API_TEXT_Parameters[TextId]);

   if( ErrCode != ST_NO_ERROR )
   {
      print("--> Can't free the text index %d\n",TextId);
      return(ErrCode);
   }

   // Clear text parameter
   API_TEXT_Parameters[TextId] = NULL;

   // Return no errors
   return(ErrCode);
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t FreeTextHandle(int TextId)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   if( API_TEXT_Parameters[TextId] == NULL )
   {
      //_DTraceInfo("Text handle index %d not yet used\n",TextId);
      return ErrCode;
   }

   // Free the text
   ErrCode = TEXT_FreeTextHandle(API_TEXT_Parameters[TextId]);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("Can't free the text index %d\n",TextId);
      return(ErrCode);
   }

   // Clear text parameter
   API_TEXT_Parameters[TextId] = NULL;

   // Return no errors
   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
