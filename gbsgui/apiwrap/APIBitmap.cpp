//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: APIBitmap.cpp                                                              //
//////////////////////////////////////////////////////////////////////////////////////

#include "APIBitmap.hpp"
#include "GDebug.hpp"

#define BMP_LAYER_ALPHA_MIN  18
#define BMP_LAYER_ALPHA_MAX 128

static APIBitmap_Context_t Bitmap_Context;
//////////////////////////////////////////////////////////////////////////////////////
void APIBitmapInit()
{
   GRAFIX_Enable(0);
   GRAFIX_Enable(1);
   GBLITCopyInit();
}

void APIBitmapTerm()
{
   GRAFIX_Disable(0);
   GRAFIX_Disable(1);
   GBLITCopyTerm();
}
//////////////////////////////////////////////////////////////////////////////////////
BOOL ClearPlane(int planeId)
{
   return GBLITfillRect(0, 0, -1, -1, 0, planeId);
}

//////////////////////////////////////////////////////////////////////////////////////
static void BLITi_Callback(STEVT_CallReason_t Reason,
                           const ST_DeviceName_t RegistrantName,
                           STEVT_EventConstant_t Event,
                           const void* EventData,
                           const void* SubscriberData_p);


//////////////////////////////////////////////////////////////////////////////////////
int toSTMAlpha(int aPercent)
{
   int ret = BMP_LAYER_ALPHA_MAX;

   if( aPercent < 10 )
   {
      //transparency off
      ret = BMP_LAYER_ALPHA_MAX;
   }
   else if( aPercent >= 90 )
   {
      //most transparency
      ret = BMP_LAYER_ALPHA_MIN;
   }
   else
   {
      //from 10-99%
      ret = (int)(BMP_LAYER_ALPHA_MAX - (aPercent*(BMP_LAYER_ALPHA_MAX-BMP_LAYER_ALPHA_MIN))/100);
   }

   return ret;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GFX_SetLayerAlpha(int planeId, int alpha)
{
   ST_ErrorCode_t ec = ST_NO_ERROR;           //int alphaA1=alpha;
   STLAYER_GlobalAlpha_t LAYER_GlobalAlpha;   // Check if viewport exists

   if( GFX_LAYER_ViewPortHandle[planeId] == 0 )
   {
      _DTraceError("--> This graphic layer %d is undefined on this hardware !\n",planeId);
      return FALSE;
   }

   if( alpha > BMP_LAYER_ALPHA_MAX || alpha < BMP_LAYER_ALPHA_MIN )
   {
      return FALSE;
   }

   LAYER_GlobalAlpha.A0 = alpha;
   LAYER_GlobalAlpha.A1 = alpha;

   ec = STLAYER_SetViewPortAlpha(GFX_LAYER_ViewPortHandle[planeId], &LAYER_GlobalAlpha);
   ec = STLAYER_GetViewPortAlpha(GFX_LAYER_ViewPortHandle[planeId], &LAYER_GlobalAlpha);

   if( ec != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to set the global alpha on this layer %d !\n",planeId);
      return FALSE;

   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GFX_SetAntiFlicker(int planeId, BOOL isEnable)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;

   if( planeId >= GFX_LAYER_MAX_NUMBER )
   {
      _DTraceError("--> Invalid graphic plane ID, should be [0,1,..] !\n");
      return FALSE;
   }

   if( GFX_LAYER_Handle[planeId] == 0 )
   {
      _DTraceError("--> This graphic layer %d is undefined on this hardware !\n", planeId);
      return FALSE;
   }

   if( TRUE == isEnable )
   {
      ErrCode = STLAYER_EnableViewPortFilter(GFX_LAYER_ViewPortHandle[planeId], GFX_LAYER_Handle[planeId]);
      if( ErrCode != ST_NO_ERROR )
      {
         _DTraceError("--> Unable to enable flicker filter for plane %d !\n", planeId);
         return FALSE;
      }
   }
   else
   {
      ErrCode = STLAYER_DisableViewPortFilter(GFX_LAYER_ViewPortHandle[planeId]);
      if(ErrCode != ST_NO_ERROR )
      {
         _DTraceError("--> Unable to disable flicker filter for plane %d !\n", planeId);
         return FALSE;
      }
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GFXalphaCur()
{
   STLAYER_GlobalAlpha_t ga;
   STLAYER_GetViewPortAlpha(GFX_LAYER_ViewPortHandle[0], &ga);
   return ga.A0;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GFXalphaSet(U8 a)
{
   STLAYER_GlobalAlpha_t ga;
   ga.A0 = a;
   ga.A1 = a;
   ST_ErrorCode_t err = ST_NO_ERROR;

   err = STLAYER_SetViewPortAlpha(GFX_LAYER_ViewPortHandle[0], &ga);
   err = STLAYER_SetViewPortAlpha(GFX_LAYER_ViewPortHandle[1], &ga);

//   _DTraceError("2013-05-02thu_th66:GFXalphaSet(a%u)e%u\n", a, err);

   return a;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 GFXalphaChg(int c)
{
   STLAYER_GlobalAlpha_t ga;
   U8 ao, an;
   ST_ErrorCode_t err = ST_NO_ERROR;

   err = STLAYER_GetViewPortAlpha(GFX_LAYER_ViewPortHandle[0], &ga);

   ao = ga.A0;
   an = ao + c;

   if( an <= 128 )
   {
      ga.A0 = an;
      ga.A1 = an;
   }

   err = STLAYER_SetViewPortAlpha(GFX_LAYER_ViewPortHandle[0], &ga);
   err = STLAYER_SetViewPortAlpha(GFX_LAYER_ViewPortHandle[1], &ga);
//   _DTraceError("2013-04-30tu_th66:GFXalphaChg(c%d)ao%u.an%u e%u\n", c, ao, an, err);

   return an;
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        DrawGam
// Description: Draw .gam file by .gam object (*source) and Draw Parameters (obj_APIDrawParams)
//////////////////////////////////////////////////////////////////////////////////////
BOOL DrawGam( const GAMLOAD_Object_t* source, APIDrawParams obj_APIDrawParams )
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   STLAYER_ViewPortParams_t LAYER_ViewPortParams;
   STGXOBJ_Bitmap_t         LAYER_GXOBJ_Bitmap;
   STGXOBJ_Palette_t        LAYER_GXOBJ_Palette;
   STLAYER_ViewPortSource_t LAYER_ViewPortSource;
   U32 XPos, YPos;

   int Plane = obj_APIDrawParams.planeId;
   XPos = obj_APIDrawParams.posX;
   YPos = obj_APIDrawParams.posY;

   // Get the layer informations
   memset(&LAYER_ViewPortParams, 0, sizeof(STLAYER_ViewPortParams_t));
   memset(&LAYER_ViewPortSource, 0, sizeof(STLAYER_ViewPortSource_t));
   memset(&LAYER_GXOBJ_Bitmap  , 0, sizeof(STGXOBJ_Bitmap_t));
   memset(&LAYER_GXOBJ_Palette, 0, sizeof(STGXOBJ_Palette_t));
   LAYER_ViewPortSource.Data.BitMap_p = &LAYER_GXOBJ_Bitmap;
   LAYER_ViewPortSource.Palette_p     = &LAYER_GXOBJ_Palette;
   LAYER_ViewPortParams.Source_p      = &LAYER_ViewPortSource;

   ErrCode = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane], &LAYER_ViewPortParams);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("DrawGam --> Unable to get the layer parameters !\n");
      return(TRUE);
   }

   // Convert with blitter
   STBLIT_Source_t BLIT_SrcBuffer;
   STBLIT_Destination_t BLIT_DstBuffer;
   STBLIT_BlitContext_t BLIT_Context;

   // Reset structure
   memset(&BLIT_SrcBuffer, 0, sizeof(STBLIT_Source_t));
   memset(&BLIT_DstBuffer, 0, sizeof(STBLIT_Destination_t));
   memset(&BLIT_Context , 0, sizeof(STBLIT_BlitContext_t));

   // Set blit context
   BLIT_Context.ColorKeyCopyMode = STBLIT_COLOR_KEY_MODE_NONE;

   //BLIT_Context.AluMode = STBLIT_ALU_COPY;//STBLIT_ALU_ALPHA_BLEND; // original
   BLIT_Context.AluMode = STBLIT_ALU_ALPHA_BLEND; // test
   BLIT_Context.EnableMaskWord = FALSE;
   BLIT_Context.MaskWord = 0;
   BLIT_Context.EnableMaskBitmap = FALSE;
   BLIT_Context.MaskBitmap_p = NULL;
   BLIT_Context.MaskRectangle.PositionX = 0;
   BLIT_Context.MaskRectangle.PositionY = 0;
   BLIT_Context.MaskRectangle.Width = 0;
   BLIT_Context.MaskRectangle.Height = 0;
   BLIT_Context.WorkBuffer_p = NULL;
   BLIT_Context.EnableColorCorrection = FALSE;
   BLIT_Context.ColorCorrectionTable_p = NULL;
   BLIT_Context.Trigger.EnableTrigger = FALSE;
   BLIT_Context.Trigger.ScanLine = 0;
   BLIT_Context.Trigger.Type = STBLIT_TRIGGER_TYPE_BOTTOM_VSYNC;//STBLIT_TRIGGER_TYPE_TOP_VSYNC;
   BLIT_Context.Trigger.VTGId = 0;
   BLIT_Context.GlobalAlpha = obj_APIDrawParams.alpha;
   BLIT_Context.EnableClipRectangle = FALSE;
   BLIT_Context.ClipRectangle.PositionX = 0;
   BLIT_Context.ClipRectangle.PositionY = 0;
   BLIT_Context.ClipRectangle.Width = 0;
   BLIT_Context.ClipRectangle.Height = 0;
   //BLIT_Context.WriteInsideClipRectangle = FALSE; // test
   BLIT_Context.WriteInsideClipRectangle = TRUE; // original
   //BLIT_Context.EnableFlickerFilter = obj_APIDrawParams.IsAntiFlickerEnabled; // original

#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
   BLIT_Context.EnableResizeFilter = FALSE;
#endif
   BLIT_Context.JobHandle = STBLIT_NO_JOB_HANDLE;
   BLIT_Context.UserTag_p = NULL;
   BLIT_Context.NotifyBlitCompletion = FALSE;
   BLIT_Context.EventSubscriberID = 0;

   // Set source
   BLIT_SrcBuffer.Type = STBLIT_SOURCE_TYPE_BITMAP;
   BLIT_SrcBuffer.Data.Bitmap_p = source->Bitmap;
   BLIT_SrcBuffer.Palette_p = source->Palette;

   BLIT_SrcBuffer.Rectangle.PositionX = 0;
   BLIT_SrcBuffer.Rectangle.PositionY = 0;
   BLIT_SrcBuffer.Rectangle.Width = source->Bitmap->Width;
   BLIT_SrcBuffer.Rectangle.Height = source->Bitmap->Height;

   // Set destination
   BLIT_DstBuffer.Bitmap_p = LAYER_ViewPortParams.Source_p->Data.BitMap_p;
   // BLIT_DstBuffer.Palette_p = LAYER_ViewPortSource[Plane].Palette_p;
   BLIT_DstBuffer.Palette_p = LAYER_ViewPortParams.Source_p->Palette_p;

   if( obj_APIDrawParams.width == 0 || obj_APIDrawParams.height == 0 )
   {
      BLIT_DstBuffer.Rectangle.PositionX = XPos;
      BLIT_DstBuffer.Rectangle.PositionY = YPos;
      BLIT_DstBuffer.Rectangle.Width = source->Bitmap->Width;
      BLIT_DstBuffer.Rectangle.Height = source->Bitmap->Height;
   }
   else
   {
      BLIT_DstBuffer.Rectangle.PositionX = XPos;
      BLIT_DstBuffer.Rectangle.PositionY = YPos;
      BLIT_DstBuffer.Rectangle.Width = obj_APIDrawParams.width;
      BLIT_DstBuffer.Rectangle.Height = obj_APIDrawParams.height;
   }

   // Perfom the blit operation
   ErrCode = STBLIT_Blit(BLIT_Handle[0], &BLIT_SrcBuffer, NULL, &BLIT_DstBuffer, &BLIT_Context);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to transform graphic format !\n");
      return(TRUE);
   }
   // Return no errors

   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
// Name:        ClearRect
// Description: Clear rect of grafix plane by drawing a "black" rectangle
//////////////////////////////////////////////////////////////////////////////////////
BOOL ClearRect(int Plane, int posX, int posY, int width, int height)
{
   ST_ErrorCode_t                ErrCode=ST_NO_ERROR;
   STGXOBJ_Rectangle_t           BLIT_Rectangle;
   STGXOBJ_Color_t               BLIT_Color;
   STBLIT_BlitContext_t          BLIT_Context;
   STEVT_Handle_t                BLIT_EVT_Handle;
   STEVT_OpenParams_t            EVT_OpenParams;
   STEVT_DeviceSubscribeParams_t EVT_SubcribeParams;
   STEVT_SubscriberID_t          EVT_SubscriberID;
   S32                           Id;
   STLAYER_ViewPortParams_t      LAYER_ViewPortParams;
   STGXOBJ_Bitmap_t              LAYER_GXOBJ_Bitmap;
   STGXOBJ_Palette_t             LAYER_GXOBJ_Palette;
   STLAYER_ViewPortSource_t      LAYER_ViewPortSource;

   // Get blitter identifier

   Id = 0;
   if( GFX_LAYER_Handle[Plane] == 0 )
   {
      _DTraceError("--> This graphic layer %d is undefined on this hardware !\n",Plane);
      return(TRUE);
   }

   // Get the layer informations
   memset(&LAYER_ViewPortParams,0,sizeof(STLAYER_ViewPortParams_t));
   memset(&LAYER_ViewPortSource,0,sizeof(STLAYER_ViewPortSource_t));
   memset(&LAYER_GXOBJ_Bitmap  ,0,sizeof(STGXOBJ_Bitmap_t));
   memset(&LAYER_GXOBJ_Palette,0,sizeof(STGXOBJ_Palette_t));
   LAYER_ViewPortSource.Data.BitMap_p = &LAYER_GXOBJ_Bitmap;
   LAYER_ViewPortSource.Palette_p     = &LAYER_GXOBJ_Palette;
   LAYER_ViewPortParams.Source_p      = &LAYER_ViewPortSource;
   ErrCode=STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane],&LAYER_ViewPortParams);

   // Create semaphore
   Semaphore_Init_Fifo(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT], 0);

   // Initialise STBLIT Event
   EVT_OpenParams.dummy = 0;

   ErrCode = STEVT_Open(EVT_DeviceName[0], &EVT_OpenParams, &BLIT_EVT_Handle);

   if( ErrCode != ST_NO_ERROR )
   {
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);
      _DTraceError("--> Unable to create an event handle !\n");
      return(TRUE);
   }

   // Register for the BLIT Events
   memset(&EVT_SubcribeParams, 0, sizeof(STEVT_DeviceSubscribeParams_t));

   EVT_SubcribeParams.NotifyCallback   = BLITi_Callback;
   EVT_SubcribeParams.SubscriberData_p = (void *)(0x00FAA100+BLIT_SEM_EVENT_CLEARRECT);
   ErrCode = STEVT_SubscribeDeviceEvent(BLIT_EVT_Handle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT,&EVT_SubcribeParams);

   if( ErrCode != ST_NO_ERROR )
   {
      STEVT_Close(BLIT_EVT_Handle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);
      _DTraceError("--> Unable to subscribe to blitter events !\n");
      return(TRUE);
   }

   ErrCode = STEVT_GetSubscriberID(BLIT_EVT_Handle, &EVT_SubscriberID);

   if( ErrCode != ST_NO_ERROR )
   {
      STEVT_UnsubscribeDeviceEvent(BLIT_EVT_Handle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
      STEVT_Close(BLIT_EVT_Handle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);
      _DTraceError("--> Unable to get the subscriber id !\n");
      return(TRUE);
   }

   // Setup the blitter context
   memset(&BLIT_Context, 0, sizeof(STBLIT_BlitContext_t));

   BLIT_Context.ColorKeyCopyMode         = STBLIT_COLOR_KEY_MODE_NONE;
   BLIT_Context.AluMode                  = STBLIT_ALU_COPY;
   BLIT_Context.EnableMaskWord           = FALSE;
   BLIT_Context.MaskWord                 = 0;
   BLIT_Context.EnableMaskBitmap         = FALSE;
   BLIT_Context.MaskBitmap_p             = NULL;
   BLIT_Context.MaskRectangle.PositionX  = 0;
   BLIT_Context.MaskRectangle.PositionY  = 0;
   BLIT_Context.MaskRectangle.Width      = 0;
   BLIT_Context.MaskRectangle.Height     = 0;
   BLIT_Context.WorkBuffer_p             = NULL;
   BLIT_Context.EnableColorCorrection    = FALSE;
   BLIT_Context.ColorCorrectionTable_p   = NULL;
   BLIT_Context.Trigger.EnableTrigger    = FALSE;
   BLIT_Context.Trigger.ScanLine         = 0;
   BLIT_Context.Trigger.Type             = STBLIT_TRIGGER_TYPE_TOP_VSYNC;
   BLIT_Context.Trigger.VTGId            = 0;
   BLIT_Context.GlobalAlpha              = 0;
   BLIT_Context.EnableClipRectangle      = FALSE;
   BLIT_Context.ClipRectangle.PositionX  = 0;
   BLIT_Context.ClipRectangle.PositionY  = 0;
   //BLIT_Context.ClipRectangle.Width      = LAYER_ViewPortSource[Plane].Data.BitMap_p->Width;
   BLIT_Context.ClipRectangle.Width      = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
   //BLIT_Context.ClipRectangle.Height     = LAYER_ViewPortSource[Plane].Data.BitMap_p->Height;
   BLIT_Context.ClipRectangle.Height     = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
   BLIT_Context.WriteInsideClipRectangle = FALSE;
   //BLIT_Context.WriteInsideClipRectangle = TRUE;
   BLIT_Context.EnableFlickerFilter      = FALSE;
#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
   BLIT_Context.EnableResizeFilter       = FALSE;
#endif
   BLIT_Context.JobHandle                = STBLIT_NO_JOB_HANDLE;
   BLIT_Context.UserTag_p                = NULL;
   BLIT_Context.NotifyBlitCompletion     = TRUE;
   BLIT_Context.EventSubscriberID        = EVT_SubscriberID;

   // Clear color
   switch( LAYER_ViewPortParams.Source_p->Data.BitMap_p->ColorType )
   {
   case STGXOBJ_COLOR_TYPE_ARGB8888:
      //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB8888 \n");
      BLIT_Color.Type                 = STGXOBJ_COLOR_TYPE_ARGB8888;
      BLIT_Color.Value.ARGB8888.Alpha = 0;
      BLIT_Color.Value.ARGB8888.R     = 0;
      BLIT_Color.Value.ARGB8888.G     = 0;
      BLIT_Color.Value.ARGB8888.B     = 0;
      break;

   case STGXOBJ_COLOR_TYPE_ARGB1555:
      //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB1555 \n");
      BLIT_Color.Type                 = STGXOBJ_COLOR_TYPE_ARGB1555;
      BLIT_Color.Value.ARGB1555.Alpha = 0;//0;//2013-04-29mo_th66;
      BLIT_Color.Value.ARGB1555.R     = 0;
      BLIT_Color.Value.ARGB1555.G     = 0;
      BLIT_Color.Value.ARGB1555.B     = 0;
      break;

   case STGXOBJ_COLOR_TYPE_ARGB4444:
      //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB4444 \n");
      BLIT_Color.Type                 = STGXOBJ_COLOR_TYPE_ARGB4444;
      BLIT_Color.Value.ARGB4444.Alpha = 0;//0;//2013-04-29mo_th66
      BLIT_Color.Value.ARGB4444.R     = 0;
      BLIT_Color.Value.ARGB4444.G     = 0;
      BLIT_Color.Value.ARGB4444.B     = 0;
      break;

   case STGXOBJ_COLOR_TYPE_ARGB8888_255:
      //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB8888_255 \n");
      BLIT_Color.Type                 = STGXOBJ_COLOR_TYPE_ARGB8888_255;
      BLIT_Color.Value.ARGB8888.Alpha = 0;//0;//2013-04-29mo_th66;
      BLIT_Color.Value.ARGB8888.R     = 0;
      BLIT_Color.Value.ARGB8888.G     = 0;
      BLIT_Color.Value.ARGB8888.B     = 0;
      break;

   default:
      //_DTraceError("default \n");
      STEVT_Close(BLIT_EVT_Handle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);
      return(TRUE);
      break;
   }

   BLIT_Rectangle.PositionX = posX;
   BLIT_Rectangle.PositionY = posY;

   if( width == -1 || height == -1 )
   {
      BLIT_Rectangle.Width  = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
      BLIT_Rectangle.Height = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
   }
   else
   {
      BLIT_Rectangle.Width  = width;
      BLIT_Rectangle.Height = height;
   }

   // Fill the rectangle
   ErrCode = STBLIT_FillRectangle(BLIT_Handle[Id],LAYER_ViewPortParams.Source_p->Data.BitMap_p,&BLIT_Rectangle,&BLIT_Color,&BLIT_Context);

   if( ErrCode != ST_NO_ERROR )
   {
      STEVT_UnsubscribeDeviceEvent(BLIT_EVT_Handle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
      STEVT_Close(BLIT_EVT_Handle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);
      _DTraceError("--> Unable to clear the graphic plane using the blitter !\n");
      return(TRUE);
   }

   // Wait end of operation
   // _DTraceError("Semaphore_Wait(BLIT_Sem_EventOccured[Id])");
   Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);

   // Close the event
   ErrCode = STEVT_UnsubscribeDeviceEvent(BLIT_EVT_Handle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);

   if( ErrCode != ST_NO_ERROR )
   {
      STEVT_Close(BLIT_EVT_Handle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);
      _DTraceError("-> Unable to unsubscribe from blitter events !\n");
      return(TRUE);
   }

   ErrCode = STEVT_Close(BLIT_EVT_Handle);

   if( ErrCode != ST_NO_ERROR )
   {
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);
      _DTraceError("-> Unable to close blitter event handle !\n");
      return(TRUE);
   }

   // Delete the semaphore
   Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_CLEARRECT]);

   // Return no errors
   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GBDrawRect(int posX, int posY, int width, int height, U32 argb, int Plane)
{
   STGXOBJ_Bitmap_t         LAYER_GXOBJ_Bitmap;
   STGXOBJ_Palette_t        LAYER_GXOBJ_Palette;
   STLAYER_ViewPortParams_t LAYER_ViewPortParams;
   STLAYER_ViewPortSource_t LAYER_ViewPortSource;
   U8 alpha,R,G,B;

   // _DTraceError("GBDrawRect .... \n");
   if( width == 0 || height == 0 )
   {
      return FALSE;
   }

   alpha = (argb&0xFF000000)>>24;

   R = (argb&0xFF0000)>>16;
   G = (argb&0xFF00)>>8;
   B = argb&0xFF;

   if( 0 < alpha )
   {
      alpha = GFXalphaCur();
   }

   ST_ErrorCode_t ec = ST_NO_ERROR;
   STGXOBJ_Rectangle_t GXrect;
   STGXOBJ_Color_t GXcolor;
   STBLIT_BlitContext_t BLITcxt;
   STEVT_Handle_t EVhandle;
   STEVT_OpenParams_t EVopenPms;
   STEVT_DeviceSubscribeParams_t EVsubPms;
   STEVT_SubscriberID_t EVsubID;  S32 Id=0;//nBLITid

   memset(&LAYER_ViewPortParams, 0, sizeof(STLAYER_ViewPortParams_t));
   memset(&LAYER_ViewPortSource, 0, sizeof(STLAYER_ViewPortSource_t));
   memset(&LAYER_GXOBJ_Bitmap  , 0, sizeof(STGXOBJ_Bitmap_t));
   memset(&LAYER_GXOBJ_Palette, 0, sizeof(STGXOBJ_Palette_t));

   LAYER_ViewPortSource.Data.BitMap_p = &LAYER_GXOBJ_Bitmap;
   LAYER_ViewPortSource.Palette_p     = &LAYER_GXOBJ_Palette;
   LAYER_ViewPortParams.Source_p      = &LAYER_ViewPortSource;

   if( GFX2D_IsDFBLayer[Plane] == FALSE )
   {
      if( GFX_LAYER_Handle[Plane] == 0 )
      {
         _DTraceError("--> This graphic layer %d is undefined on this hardware !\n",Plane);
         return(TRUE);
      }

      ec = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane],&LAYER_ViewPortParams);

      if( ec != ST_NO_ERROR )
      {
         _DTraceError("GBDrawRect --> Unable to get the layer parameters !\n");
         return(TRUE);
      }

      Semaphore_Init_Fifo(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT], 0);

      //iniBLITevent
      EVopenPms.dummy = 0;
      ec = STEVT_Open(EVT_DeviceName[0], &EVopenPms, &EVhandle);

      if( ec != ST_NO_ERROR )
      {
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("--> Unable to create an event handle !\n");
         return(TRUE);
      }

      //register4BLITevents
      memset(&EVsubPms, 0, sizeof(STEVT_DeviceSubscribeParams_t));
      EVsubPms.NotifyCallback   = BLITi_Callback;
      EVsubPms.SubscriberData_p = (void*)(0x00FAA100+BLIT_SEM_EVENT_DRAWRECT);
      ec = STEVT_SubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT,&EVsubPms);

      if( ec != ST_NO_ERROR )
      {
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("GBLITfillRect()unable2subscribe2BLITevents!\n");
         return(TRUE);
      }

      ec = STEVT_GetSubscriberID(EVhandle,&EVsubID);

      if( ec != ST_NO_ERROR )
      {
         STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("--> Unable to get the subscriber id !\n");
         return(TRUE);
      }

      // Setup the blitter context
      memset(&BLITcxt,0,sizeof(STBLIT_BlitContext_t));
      BLITcxt.ColorKeyCopyMode         = STBLIT_COLOR_KEY_MODE_NONE;
      BLITcxt.AluMode                  = STBLIT_ALU_COPY;//STBLIT_ALU_CLEAR;//BLITcxt.AluMode=STBLIT_ALU_ALPHA_BLEND;//2013-04-29mo_th66//
      BLITcxt.EnableMaskWord           = FALSE;
      BLITcxt.MaskWord                 = 0;
      BLITcxt.EnableMaskBitmap         = FALSE;
      BLITcxt.MaskBitmap_p             = NULL;
      BLITcxt.MaskRectangle.PositionX  = 0;
      BLITcxt.MaskRectangle.PositionY  = 0;
      BLITcxt.MaskRectangle.Width      = 0;
      BLITcxt.MaskRectangle.Height     = 0;
      BLITcxt.WorkBuffer_p             = NULL;
      BLITcxt.EnableColorCorrection    = FALSE;
      BLITcxt.ColorCorrectionTable_p   = NULL;
      BLITcxt.Trigger.EnableTrigger    = FALSE;
      BLITcxt.Trigger.ScanLine         = 0;
      BLITcxt.Trigger.Type             = STBLIT_TRIGGER_TYPE_TOP_VSYNC;
      BLITcxt.Trigger.VTGId            = 0;
      BLITcxt.GlobalAlpha              = alpha;   //0;//2013-04-29mo_th66
      BLITcxt.EnableClipRectangle      = FALSE;
      BLITcxt.ClipRectangle.PositionX  = 0;
      BLITcxt.ClipRectangle.PositionY  = 0;
      BLITcxt.ClipRectangle.Width      = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
      BLITcxt.ClipRectangle.Height     = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
      BLITcxt.WriteInsideClipRectangle = FALSE;//BLITcxt.WriteInsideClipRectangle = TRUE;
      BLITcxt.EnableFlickerFilter      = FALSE;
      #if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
      BLITcxt.EnableResizeFilter       = FALSE;
      #endif
      BLITcxt.JobHandle                = STBLIT_NO_JOB_HANDLE;
      BLITcxt.UserTag_p                = NULL;
      BLITcxt.NotifyBlitCompletion     = TRUE;
      BLITcxt.EventSubscriberID        = EVsubID;

      // Clear color
      switch(LAYER_ViewPortParams.Source_p->Data.BitMap_p->ColorType)
      {
      case STGXOBJ_COLOR_TYPE_ARGB8888:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB8888 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB8888;
         GXcolor.Value.ARGB8888.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB8888.R     = R;
         GXcolor.Value.ARGB8888.G     = G;
         GXcolor.Value.ARGB8888.B     = B;
         break;

      case STGXOBJ_COLOR_TYPE_ARGB1555:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB1555 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB1555;
         GXcolor.Value.ARGB1555.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB1555.R     = R;
         GXcolor.Value.ARGB1555.G     = G;
         GXcolor.Value.ARGB1555.B     = B;
         break;

      case STGXOBJ_COLOR_TYPE_ARGB4444:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB4444 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB4444;
         GXcolor.Value.ARGB4444.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB4444.R     = R;
         GXcolor.Value.ARGB4444.G     = G;
         GXcolor.Value.ARGB4444.B     = B;
         break;

      case STGXOBJ_COLOR_TYPE_ARGB8888_255:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB8888_255 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB8888_255;
         GXcolor.Value.ARGB8888.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB8888.R     = R;
         GXcolor.Value.ARGB8888.G     = G;
         GXcolor.Value.ARGB8888.B     = B;
         break;

      default:
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         return(TRUE);
         break;
      }

      if( width > LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width )
      {
         width = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
      }

      if( height > LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height )
      {
         height = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
      }
      // _DTraceError("GBDrawRect Layer(%d,%d), (width,height) = (%d,%d) \n",
      // LAYER_ViewPortParams[Plane].Source_p->Data.BitMap_p->Width,LAYER_ViewPortParams[Plane].Source_p->Data.BitMap_p->Height,width,height);

      ec = STBLIT_DrawHLine(BLIT_Handle[Id],LAYER_ViewPortParams.Source_p->Data.BitMap_p,posX,posY,width,&GXcolor,&BLITcxt);

      if( ec )
      {
         STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("GBDrawRect Unable draw rect using theBLITter!\n");
         return(TRUE);
      }

      //_DTraceError("Semaphore_Wait(BLIT_Sem_EventOccured[Id])");
      Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);/* Wait end of operation */

      ec = STBLIT_DrawVLine(BLIT_Handle[Id],LAYER_ViewPortParams.Source_p->Data.BitMap_p,posX,posY,height,&GXcolor,&BLITcxt);

      if( ec )
      {
         STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("GBDrawRect Unable draw rect using theBLITter!\n");
         return(TRUE);
      }
      // _DTraceError("Semaphore_Wait(BLIT_Sem_EventOccured[Id])");
      Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);/* Wait end of operation */

      ec = STBLIT_DrawHLine(BLIT_Handle[Id],LAYER_ViewPortParams.Source_p->Data.BitMap_p,posX,posY+height,width,&GXcolor,&BLITcxt);

      if( ec )
      {
         STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("GBDrawRect Unable draw rect using theBLITter!\n");
         return(TRUE);
      }

      // _DTraceError("Semaphore_Wait(BLIT_Sem_EventOccured[Id])");
      Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);/* Wait end of operation */

      ec = STBLIT_DrawVLine(BLIT_Handle[Id],LAYER_ViewPortParams.Source_p->Data.BitMap_p,posX+width,posY,height,&GXcolor,&BLITcxt);

      if( ec )
      {
         STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("GBDrawRect Unable draw rect using theBLITter!\n");
         return(TRUE);
      }
      // _DTraceError("Semaphore_Wait(BLIT_Sem_EventOccured[Id])");
      Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);/* Wait end of operation */

      ec = STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);

      if( ec )
      {
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("-> Unable to unsubscribe from blitter events !\n");
         return(TRUE);
      }

      ec = STEVT_Close(EVhandle);

      if( ec )
      {
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
         _DTraceError("-> Unable to close blitter event handle !\n");
         return(TRUE);
      }

      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_DRAWRECT]);
   }

   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GBLITfillRect(int posX,int posY,int width,int height,U32 argb,int Plane){
   STGXOBJ_Bitmap_t         LAYER_GXOBJ_Bitmap;
   STGXOBJ_Palette_t        LAYER_GXOBJ_Palette;
   STLAYER_ViewPortParams_t LAYER_ViewPortParams;
   STLAYER_ViewPortSource_t LAYER_ViewPortSource;

   if( width == 0 || height == 0 )
   {
      return FALSE;
   }

   U8 alpha, R, G, B;

   alpha = (argb&0xFF000000)>>24;
   R = (argb&0xFF0000)>>16;
   G = (argb&0xFF00)>>8;
   B = argb&0xFF;

   if( 0 < alpha )
   {
      alpha = GFXalphaCur();
   }

   ST_ErrorCode_t ec = ST_NO_ERROR;
   STGXOBJ_Rectangle_t GXrect;
   STGXOBJ_Color_t GXcolor;
   STBLIT_BlitContext_t BLITcxt;
   STEVT_Handle_t EVhandle;
   STEVT_OpenParams_t EVopenPms;
   STEVT_DeviceSubscribeParams_t EVsubPms;
   STEVT_SubscriberID_t EVsubID;
   S32 Id = 0;//nBLITid

   if( GFX_LAYER_Handle[Plane] == 0 )
   {
      _DTraceError("GBLITfillRect()GFXlayer%d is undefined on this hardware!\n",Plane);return(TRUE);
   }
   //if(Plane!=0&&Plane!=1){_DTraceError("Plane = %d is in valid. Assign Plane = 0.",Plane);Plane = 0;}
   // GetLayerInformation(Plane);
   // Get the layer informations

   memset(&LAYER_ViewPortParams,0,sizeof(STLAYER_ViewPortParams_t));
   memset(&LAYER_ViewPortSource,0,sizeof(STLAYER_ViewPortSource_t));
   memset(&LAYER_GXOBJ_Bitmap  ,0,sizeof(STGXOBJ_Bitmap_t));
   memset(&LAYER_GXOBJ_Palette, 0,sizeof(STGXOBJ_Palette_t));

   LAYER_ViewPortSource.Data.BitMap_p = &LAYER_GXOBJ_Bitmap;
   LAYER_ViewPortSource.Palette_p     = &LAYER_GXOBJ_Palette;
   LAYER_ViewPortParams.Source_p      = &LAYER_ViewPortSource;

   if( GFX2D_IsDFBLayer[Plane] == FALSE )
   {
      if (GFX_LAYER_Handle[Plane]==0)
      {
         _DTraceError("--> This graphic layer %d is undefined on this hardware !\n",Plane);
         return(TRUE);
      }

      ec = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane],&LAYER_ViewPortParams);

      if( ec != ST_NO_ERROR )
      {
         _DTraceError("GBLITfillRect --> Unable to get the source layer parameters !\n");
         return(TRUE);
      }

      Semaphore_Init_Fifo(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT],0);
      //iniBLITevent
      EVopenPms.dummy = 0;

      ec = STEVT_Open(EVT_DeviceName[0],&EVopenPms,&EVhandle);

      if( ec != ST_NO_ERROR )
      {
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
         _DTraceError("--> Unable to create an event handle !\n");
         return(TRUE);
      }

      //register4BLITevents
      memset(&EVsubPms,0,sizeof(STEVT_DeviceSubscribeParams_t));
      EVsubPms.NotifyCallback   = BLITi_Callback;
      EVsubPms.SubscriberData_p = (void*)(0x00FAA100+BLIT_SEM_EVENT_FILLRECT);

      ec = STEVT_SubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT,&EVsubPms);

      if( ec )
      {
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
         _DTraceError("GBLITfillRect()unable2subscribe2BLITevents!\n");
         return(TRUE);
      }

      ec = STEVT_GetSubscriberID(EVhandle,&EVsubID);

      if( ec )
      {
         STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
         _DTraceError("--> Unable to get the subscriber id !\n");
         return(TRUE);
      }
      // Setup the blitter context
      memset(&BLITcxt,0,sizeof(STBLIT_BlitContext_t));
      BLITcxt.ColorKeyCopyMode = STBLIT_COLOR_KEY_MODE_NONE;
      BLITcxt.AluMode          = STBLIT_ALU_COPY;//STBLIT_ALU_CLEAR;//BLITcxt.AluMode=STBLIT_ALU_ALPHA_BLEND;//2013-04-29mo_th66//
      BLITcxt.EnableMaskWord   = FALSE;
      BLITcxt.MaskWord         = 0;
      BLITcxt.EnableMaskBitmap = FALSE;
      BLITcxt.MaskBitmap_p     = NULL;
      BLITcxt.MaskRectangle.PositionX = 0;
      BLITcxt.MaskRectangle.PositionY = 0;
      BLITcxt.MaskRectangle.Width     = 0;
      BLITcxt.MaskRectangle.Height    = 0;
      BLITcxt.WorkBuffer_p            = NULL;
      BLITcxt.EnableColorCorrection   = FALSE;
      BLITcxt.ColorCorrectionTable_p  = NULL;
      BLITcxt.Trigger.EnableTrigger   = FALSE;
      BLITcxt.Trigger.ScanLine        = 0;
      BLITcxt.Trigger.Type            = STBLIT_TRIGGER_TYPE_TOP_VSYNC;
      BLITcxt.Trigger.VTGId           = 0;
      BLITcxt.GlobalAlpha             = alpha;   //0;//2013-04-29mo_th66
      BLITcxt.EnableClipRectangle     = FALSE;
      BLITcxt.ClipRectangle.PositionX = 0;
      BLITcxt.ClipRectangle.PositionY = 0;
      BLITcxt.ClipRectangle.Width     = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width; //BLITcxt.ClipRectangle.Width=LAYER_ViewPortSource[Plane].Data.BitMap_p->Width;
      BLITcxt.ClipRectangle.Height    = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;//BLITcxt.ClipRectangle.Height=LAYER_ViewPortSource[Plane].Data.BitMap_p->Height;
      BLITcxt.WriteInsideClipRectangle = FALSE;//BLITcxt.WriteInsideClipRectangle = TRUE;
      BLITcxt.EnableFlickerFilter      = FALSE;
      #if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
      BLITcxt.EnableResizeFilter       = FALSE;
      #endif
      BLITcxt.JobHandle                = STBLIT_NO_JOB_HANDLE;
      BLITcxt.UserTag_p                = NULL;
      BLITcxt.NotifyBlitCompletion     = TRUE;
      BLITcxt.EventSubscriberID        = EVsubID;

      // Clear color
      switch(LAYER_ViewPortParams.Source_p->Data.BitMap_p->ColorType)
      {
      case STGXOBJ_COLOR_TYPE_ARGB8888:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB8888 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB8888;
         GXcolor.Value.ARGB8888.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB8888.R     = R;
         GXcolor.Value.ARGB8888.G     = G;
         GXcolor.Value.ARGB8888.B     = B;
         break;

      case STGXOBJ_COLOR_TYPE_ARGB1555:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB1555 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB1555;
         GXcolor.Value.ARGB1555.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB1555.R     = R;
         GXcolor.Value.ARGB1555.G     = G;
         GXcolor.Value.ARGB1555.B     = B;
         break;

      case STGXOBJ_COLOR_TYPE_ARGB4444:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB4444 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB4444;
         GXcolor.Value.ARGB4444.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB4444.R     = R;
         GXcolor.Value.ARGB4444.G     = G;
         GXcolor.Value.ARGB4444.B     = B;
         break;

      case STGXOBJ_COLOR_TYPE_ARGB8888_255:
         //_DTraceError("STGXOBJ_COLOR_TYPE_ARGB8888_255 \n");
         GXcolor.Type                 = STGXOBJ_COLOR_TYPE_ARGB8888_255;
         GXcolor.Value.ARGB8888.Alpha = alpha;//0;//2013-04-29mo_th66
         GXcolor.Value.ARGB8888.R     = R;
         GXcolor.Value.ARGB8888.G     = G;
         GXcolor.Value.ARGB8888.B     = B;
         break;

      default:
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
         return(TRUE);
         break;
      }

      GXrect.PositionX = posX;
      GXrect.PositionY = posY;

      if( width < 0 || height < 0 )
      {
         GXrect.Width = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Width;
         GXrect.Height = LAYER_ViewPortParams.Source_p->Data.BitMap_p->Height;
      }
      else
      {
         GXrect.Width = width;
         GXrect.Height = height;
      }

      ec = STBLIT_FillRectangle(BLIT_Handle[Id],LAYER_ViewPortParams.Source_p->Data.BitMap_p,&GXrect,&GXcolor,&BLITcxt);

      if( ec )
      {
         STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
         _DTraceError("GBLITfillRect()unable2fillRect using theBLITter!\n");
         return(TRUE);
      }
      // _DTraceError("Semaphore_Wait(BLIT_Sem_EventOccured[Id])");
      Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);/* Wait end of operation */
      ec = STEVT_UnsubscribeDeviceEvent(EVhandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);

      if( ec )
      {
         STEVT_Close(EVhandle);
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
         _DTraceError("-> Unable to unsubscribe from blitter events !\n");
         return(TRUE);
      }

      ec = STEVT_Close(EVhandle);

      if( ec )
      {
         Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
         _DTraceError("-> Unable to close blitter event handle !\n");
         return(TRUE);
      }

      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_FILLRECT]);
   }

   return(FALSE);
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t GBLITCopyInit()
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   STEVT_OpenParams_t EVT_OpenParams;
   STEVT_DeviceSubscribeParams_t EVT_SubcribeParams;

   memset(&Bitmap_Context,0,sizeof(APIBitmap_Context_t));

   if(Bitmap_Context.Bitmap_Used == TRUE)
   {
      _DTraceError("Blit Bitmap already initialized");
      return(ST_ERROR_INVALID_HANDLE);
   }
   Bitmap_Context.Bitmap_Used = TRUE;
   /* Initialise event */
   /* ================ */
   memset(&EVT_OpenParams,0,sizeof(STEVT_OpenParams_t));
   ErrCode=STEVT_Open(EVT_DeviceName[0],&EVT_OpenParams,&Bitmap_Context.EVT_Handle);
   if (ErrCode!=ST_NO_ERROR)
   {
      _DTraceError("Unable to open an event instance !!!");
      return(ST_ERROR_NO_MEMORY);
   }
   // Create semaphore
   Semaphore_Init_Fifo(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY],1);
   /* Register for the blitter event */
   /* ============================== */
   memset(&EVT_SubcribeParams,0,sizeof(STEVT_DeviceSubscribeParams_t));
   EVT_SubcribeParams.NotifyCallback   = BLITi_Callback;
   EVT_SubcribeParams.SubscriberData_p = (void *)(0x00FAA100+BLIT_SEM_EVENT_COPY);
   ErrCode=STEVT_SubscribeDeviceEvent(Bitmap_Context.EVT_Handle,BLIT_DeviceName[0],STBLIT_BLIT_COMPLETED_EVT,&EVT_SubcribeParams);
   if (ErrCode!=ST_NO_ERROR)
   {
      _DTraceError("**ERROR** !!! Unable to subscribe to the blitter complete event !!!");
      STEVT_Close(Bitmap_Context.EVT_Handle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      return(ST_ERROR_BAD_PARAMETER);
   }
   ErrCode=STEVT_GetSubscriberID(Bitmap_Context.EVT_Handle,&Bitmap_Context.BLIT_EVT_SubscriberID);
   if (ErrCode!=ST_NO_ERROR)
   {
      _DTraceError("**ERROR** !!! Unable to get the subscriber id !!!");
      STEVT_UnsubscribeDeviceEvent(Bitmap_Context.EVT_Handle,BLIT_DeviceName[0],STBLIT_BLIT_COMPLETED_EVT);
      STEVT_Close(Bitmap_Context.EVT_Handle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      return(ST_ERROR_BAD_PARAMETER);
   }

   return(ErrCode);
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t GBLITCopyTerm()
{
   if(Bitmap_Context.Bitmap_Used == TRUE)
   {
      Bitmap_Context.Bitmap_Used = FALSE;
   }
   STEVT_UnsubscribeDeviceEvent(Bitmap_Context.EVT_Handle,BLIT_DeviceName[0],STBLIT_BLIT_COMPLETED_EVT);
   STEVT_Close(Bitmap_Context.EVT_Handle);
   Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
   return ST_NO_ERROR;
}
//////////////////////////////////////////////////////////////////////////////////////
#if(1)
BOOL GBLITcopy(U32 nBLITid, U32 nGfxSrcId, U32 nGfxDstId)
{
//   _DTraceError("GApiBLITcopy(%d,%d,%d).START---------------\n",nBLITid,nGfxSrcId,nGfxDstId);
   ST_ErrorCode_t ec = ST_NO_ERROR;
   STBLIT_Source_t Src;
   STBLIT_Destination_t Dst;
   STBLIT_BlitContext_t BLITcxt;
   S32 Id = nBLITid, Plane_Src = nGfxSrcId, Plane_Dst = nGfxDstId;
   STGXOBJ_Bitmap_t GBmpSrc,GBmpDst;
   STGXOBJ_Palette_t GPalSrc,GPalDst;
   STLAYER_ViewPortParams_t LVPmsSrc,LVPmsDst;
   STLAYER_ViewPortSource_t LVSrcSrc,LVSrcDst;
   //STEVT_Handle_t BliEvHandle;
   //STEVT_OpenParams_t EvOpenPms;
   //STEVT_DeviceSubscribeParams_t EvSubPms;
   //STEVT_SubscriberID_t EvSubID;

   // Get the layer informations
   memset(&LVPmsSrc,0,sizeof(STLAYER_ViewPortParams_t));
   memset(&LVSrcSrc,0,sizeof(STLAYER_ViewPortSource_t));
   memset(&GBmpSrc ,0,sizeof(STGXOBJ_Bitmap_t));
   memset(&GPalSrc ,0,sizeof(STGXOBJ_Palette_t));

   LVSrcSrc.Data.BitMap_p = &GBmpSrc;
   LVSrcSrc.Palette_p     = &GPalSrc;
   LVPmsSrc.Source_p      = &LVSrcSrc;

   ec = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane_Src],&LVPmsSrc);

   if( ec != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to get the source layer parameters !");
      return(TRUE);
   }

   // Destination
   memset(&LVPmsDst,0,sizeof(STLAYER_ViewPortParams_t));
   memset(&LVSrcDst,0,sizeof(STLAYER_ViewPortSource_t));
   memset(&GBmpDst ,0,sizeof(STGXOBJ_Bitmap_t));
   memset(&GPalDst ,0,sizeof(STGXOBJ_Palette_t));

   LVSrcDst.Data.BitMap_p   = &GBmpDst;
   LVSrcDst.Palette_p      = &GPalDst;
   LVPmsDst.Source_p      = &LVSrcDst;

   ec = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane_Dst],&LVPmsDst);
   if( ec != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to get the destination layer parameters !");
      return(TRUE);
   }

   // Reset layer width-height follow vtg mode params
   STVTG_ModeParams_t VTG_ModeParams;
   STVTG_TimingMode_t VTG_TimingMode;

   ec = STVTG_GetMode(VTG_Handle[Plane_Dst], &VTG_TimingMode, &VTG_ModeParams);
   if( ec != ST_NO_ERROR )
   {
      _DTraceError("STVTG_GetMode--> Unable to get the current mode !\n");
      return(TRUE);
   }

   LVSrcDst.Data.BitMap_p->Width = VTG_ModeParams.ActiveAreaWidth;
   LVSrcDst.Data.BitMap_p->Height = VTG_ModeParams.ActiveAreaHeight;

   // Setup the blitter context
   memset(&BLITcxt,0,sizeof(STBLIT_BlitContext_t));
   BLITcxt.ColorKeyCopyMode         = STBLIT_COLOR_KEY_MODE_NONE;
   BLITcxt.AluMode                  = STBLIT_ALU_COPY;
   BLITcxt.EnableMaskWord           = FALSE;
   BLITcxt.MaskWord                 = 0;
   BLITcxt.EnableMaskBitmap         = FALSE;
   BLITcxt.MaskBitmap_p             = NULL;
   BLITcxt.MaskRectangle.PositionX  = 0;
   BLITcxt.MaskRectangle.PositionY  = 0;
   BLITcxt.MaskRectangle.Width      = 0;
   BLITcxt.MaskRectangle.Height     = 0;
   BLITcxt.WorkBuffer_p             = NULL;
   BLITcxt.EnableColorCorrection    = FALSE;
   BLITcxt.ColorCorrectionTable_p   = NULL;
   BLITcxt.Trigger.EnableTrigger    = FALSE;
   BLITcxt.Trigger.ScanLine         = 0;
   BLITcxt.Trigger.Type             = STBLIT_TRIGGER_TYPE_TOP_VSYNC;
   BLITcxt.Trigger.VTGId            = 0;
   BLITcxt.GlobalAlpha              = 0;
   BLITcxt.EnableClipRectangle      = FALSE;
   BLITcxt.ClipRectangle.PositionX  = 0;
   BLITcxt.ClipRectangle.PositionY  = 0;
   BLITcxt.ClipRectangle.Width      = LVSrcSrc.Data.BitMap_p->Width;
   BLITcxt.ClipRectangle.Height     = LVSrcSrc.Data.BitMap_p->Height;
   BLITcxt.WriteInsideClipRectangle = TRUE;
   BLITcxt.EnableFlickerFilter      = FALSE;
#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
   BLITcxt.EnableResizeFilter       = FALSE;
#endif
   BLITcxt.JobHandle                = STBLIT_NO_JOB_HANDLE;
   BLITcxt.UserTag_p                = NULL;
   BLITcxt.NotifyBlitCompletion     = TRUE;
   BLITcxt.EventSubscriberID        = Bitmap_Context.BLIT_EVT_SubscriberID;
   // Set ForegroundSource & Destination
   Src.Type                         = STBLIT_SOURCE_TYPE_BITMAP;
   Src.Data.Bitmap_p                = LVSrcSrc.Data.BitMap_p;
   Src.Rectangle.PositionX          = Src.Rectangle.PositionY = 0;
   Src.Rectangle.Width              = LVSrcSrc.Data.BitMap_p->Width;
   Src.Rectangle.Height             = LVSrcSrc.Data.BitMap_p->Height;
   Src.Palette_p                    = LVSrcSrc.Palette_p;
   Dst.Bitmap_p                     = LVSrcDst.Data.BitMap_p;
   Dst.Rectangle.PositionX          = Dst.Rectangle.PositionY = 0;
   Dst.Rectangle.Width              = LVSrcDst.Data.BitMap_p->Width;
   Dst.Rectangle.Height             = LVSrcDst.Data.BitMap_p->Height;
   Dst.Palette_p                    = LVSrcDst.Palette_p;

   Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);

   // Do theBLITnow
   ec = STBLIT_Blit(BLIT_Handle[Id], &Src, NULL, &Dst, &BLITcxt);

   if( ec != ST_NO_ERROR )
   {
      Semaphore_Signal(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      _DTraceError("-> Unable to perform a blit copy !\n");
      return(TRUE);
   }
//   _DTraceError("Semaphore_Wait BLIT_SEM_EVENT_COPY Start");

//   _DTraceError("Semaphore_Wait BLIT_SEM_EVENT_COPY End");
//   _DTraceError("GApiBLITcopy(%d,%d,%d).End---------------\n",nBLITid,nGfxSrcId,nGfxDstId);
   return ST_NO_ERROR;
}
#else

BOOL GBLITcopy(U32 nBLITid, U32 nGfxSrcId, U32 nGfxDstId)
{
//   _DTraceError("GApiBLITcopy(%d,%d,%d).START---------------\n",nBLITid,nGfxSrcId,nGfxDstId);
   ST_ErrorCode_t ec = ST_NO_ERROR;
   STBLIT_Source_t Src;
   STBLIT_Destination_t Dst;
   STBLIT_BlitContext_t BLITcxt;
   S32 Id = nBLITid, Plane_Src = nGfxSrcId, Plane_Dst = nGfxDstId;
   STGXOBJ_Bitmap_t GBmpSrc,GBmpDst;
   STGXOBJ_Palette_t GPalSrc,GPalDst;
   STLAYER_ViewPortParams_t LVPmsSrc,LVPmsDst;
   STLAYER_ViewPortSource_t LVSrcSrc,LVSrcDst;
   STEVT_Handle_t BliEvHandle;
   STEVT_OpenParams_t EvOpenPms;
   STEVT_DeviceSubscribeParams_t EvSubPms;
   STEVT_SubscriberID_t EvSubID;

   // Get the layer informations
   memset(&LVPmsSrc,0,sizeof(STLAYER_ViewPortParams_t));
   memset(&LVSrcSrc,0,sizeof(STLAYER_ViewPortSource_t));
   memset(&GBmpSrc ,0,sizeof(STGXOBJ_Bitmap_t));
   memset(&GPalSrc ,0,sizeof(STGXOBJ_Palette_t));

   LVSrcSrc.Data.BitMap_p = &GBmpSrc;
   LVSrcSrc.Palette_p     = &GPalSrc;
   LVPmsSrc.Source_p      = &LVSrcSrc;

   ec = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane_Src],&LVPmsSrc);

   if( ec != ST_NO_ERROR )
   {
      print("GBLITcopy --> Unable to get the source layer parameters !\n");
      return(TRUE);
   }

   // Destination
   memset(&LVPmsDst,0,sizeof(STLAYER_ViewPortParams_t));
   memset(&LVSrcDst,0,sizeof(STLAYER_ViewPortSource_t));
   memset(&GBmpDst ,0,sizeof(STGXOBJ_Bitmap_t));
   memset(&GPalDst ,0,sizeof(STGXOBJ_Palette_t));

   LVSrcDst.Data.BitMap_p   = &GBmpDst;
   LVSrcDst.Palette_p      = &GPalDst;
   LVPmsDst.Source_p      = &LVSrcDst;

   ec = STLAYER_GetViewPortParams(GFX_LAYER_ViewPortHandle[Plane_Dst],&LVPmsDst);
   if( ec != ST_NO_ERROR )
   {
      print("--> Unable to get the destination layer parameters !\n");
      return(TRUE);
   }

   // Create semaphore
   Semaphore_Init_Fifo(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY],0);//GFX_BLIT_Sem_EventOccured[Id] _DTraceError("2013-04-06sa_th66 GApiBLITcopy(0,1,0).Semaphore_Init_Fifo DONE\n");

   // Initialise STBLIT Event */ EvOpenPms.dummy = 0;
   ec = STEVT_Open(EVT_DeviceName[0], &EvOpenPms, &BliEvHandle);

   if( ec != ST_NO_ERROR )
   {
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      print("--> Unable to create an event handle !\n");
      return(TRUE);
   }

   // Register for the BLIT Events
   memset(&EvSubPms,0,sizeof(STEVT_DeviceSubscribeParams_t));
   EvSubPms.SubscriberData_p = (void*)(0x00FAA100 + BLIT_SEM_EVENT_COPY);
   EvSubPms.NotifyCallback   = BLITi_Callback;//GFXi_BLIT_Callback;

   ec = STEVT_SubscribeDeviceEvent(BliEvHandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT,&EvSubPms);

   if( ec != ST_NO_ERROR )
   {
      STEVT_Close(BliEvHandle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      print("--> Unable to subscribe to blitter events !\n");
      return(TRUE);
   }

   ec = STEVT_GetSubscriberID(BliEvHandle,&EvSubID);

   if( ec != ST_NO_ERROR )
   {
      STEVT_UnsubscribeDeviceEvent(BliEvHandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
      STEVT_Close(BliEvHandle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      print("--> Unable to get the subscriber id !\n");
      return(TRUE);
   }

   // Setup the blitter context
   memset(&BLITcxt,0,sizeof(STBLIT_BlitContext_t));
   BLITcxt.ColorKeyCopyMode         = STBLIT_COLOR_KEY_MODE_NONE;
   BLITcxt.AluMode                  = STBLIT_ALU_COPY;
   BLITcxt.EnableMaskWord           = FALSE;
   BLITcxt.MaskWord                 = 0;
   BLITcxt.EnableMaskBitmap         = FALSE;
   BLITcxt.MaskBitmap_p             = NULL;
   BLITcxt.MaskRectangle.PositionX  = 0;
   BLITcxt.MaskRectangle.PositionY  = 0;
   BLITcxt.MaskRectangle.Width      = 0;
   BLITcxt.MaskRectangle.Height     = 0;
   BLITcxt.WorkBuffer_p             = NULL;
   BLITcxt.EnableColorCorrection    = FALSE;
   BLITcxt.ColorCorrectionTable_p   = NULL;
   BLITcxt.Trigger.EnableTrigger    = FALSE;
   BLITcxt.Trigger.ScanLine         = 0;
   BLITcxt.Trigger.Type             = STBLIT_TRIGGER_TYPE_TOP_VSYNC;
   BLITcxt.Trigger.VTGId            = 0;
   BLITcxt.GlobalAlpha              = 0;
   BLITcxt.EnableClipRectangle      = FALSE;
   BLITcxt.ClipRectangle.PositionX  = 0;
   BLITcxt.ClipRectangle.PositionY  = 0;
   BLITcxt.ClipRectangle.Width      = LVSrcSrc.Data.BitMap_p->Width;
   BLITcxt.ClipRectangle.Height     = LVSrcSrc.Data.BitMap_p->Height;
   BLITcxt.WriteInsideClipRectangle = TRUE;
   BLITcxt.EnableFlickerFilter      = FALSE;
#if defined(STBLIT_OBSOLETE_USE_RESIZE_IN_BLIT_CONTEXT)
   BLITcxt.EnableResizeFilter       = FALSE;
#endif
   BLITcxt.JobHandle                = STBLIT_NO_JOB_HANDLE;
   BLITcxt.UserTag_p                = NULL;
   BLITcxt.NotifyBlitCompletion     = TRUE;
   BLITcxt.EventSubscriberID        = EvSubID;
   // Set ForegroundSource & Destination
   Src.Type                         = STBLIT_SOURCE_TYPE_BITMAP;
   Src.Data.Bitmap_p                = LVSrcSrc.Data.BitMap_p;
   Src.Rectangle.PositionX          = Src.Rectangle.PositionY = 0;
   Src.Rectangle.Width              = LVSrcSrc.Data.BitMap_p->Width;
   Src.Rectangle.Height             = LVSrcSrc.Data.BitMap_p->Height;
   Src.Palette_p                    = LVSrcSrc.Palette_p;
   Dst.Bitmap_p                     = LVSrcDst.Data.BitMap_p;
   Dst.Rectangle.PositionX          = Dst.Rectangle.PositionY = 0;
   Dst.Rectangle.Width              = LVSrcDst.Data.BitMap_p->Width;
   Dst.Rectangle.Height             = LVSrcDst.Data.BitMap_p->Height;
   Dst.Palette_p                    = LVSrcDst.Palette_p;

   // Do theBLITnow
   ec = STBLIT_Blit(BLIT_Handle[Id], &Src, NULL, &Dst, &BLITcxt);

   if( ec != ST_NO_ERROR )
   {
      STEVT_UnsubscribeDeviceEvent(BliEvHandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);
      STEVT_Close(BliEvHandle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      print("-> Unable to perform a blit copy !\n");
      return(TRUE);
   }

   //_DTraceError("Semaphore_Wait(BLIT_Sem_EventOccured[Id])");
   Semaphore_Wait(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);/* Wait end of operation */ /* Close the event */

   ec = STEVT_UnsubscribeDeviceEvent(BliEvHandle,BLIT_DeviceName[Id],STBLIT_BLIT_COMPLETED_EVT);

   if( ec != ST_NO_ERROR )
   {
      STEVT_Close(BliEvHandle);
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      print("-> Unable to unsubscribe from blitter events !\n");
      return(TRUE);
   }

   ec = STEVT_Close(BliEvHandle);

   if( ec != ST_NO_ERROR )
   {
      Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
      print("-> Unable to close blitter event handle !\n");
      return(TRUE);
   }

   Semaphore_Delete(BLIT_Sem_EventOccured[BLIT_SEM_EVENT_COPY]);
//   _DTraceError("GApiBLITcopy(%d,%d,%d).END---------------\n",nBLITid,nGfxSrcId,nGfxDstId);

   return ST_NO_ERROR;
}
#endif
//////////////////////////////////////////////////////////////////////////////////////
// Name:        BLITi_Callback
// Description: Blitter event callback
// Note: Use this function when create a task of filling rectangle (use in ClearRect)
//////////////////////////////////////////////////////////////////////////////////////
static void BLITi_Callback(STEVT_CallReason_t Reason,const ST_DeviceName_t RegistrantName,STEVT_EventConstant_t Event,const void *EventData,const void *SubscriberData_p)
{
   U32 Id;
   U32 SubscriberData = (U32)SubscriberData_p;

   // Ignore all blitter callback not for this blit module
   if( (SubscriberData&0xFFFFFF00) != 0x00FAA100 )
   {
      _DTraceError("This callback is not for me !!!");
      return;
   }

   // Get blitter identifier from the subscriber data */
   Id = SubscriberData&0xFF;
//   _DTraceError("Id = %d",Id);
   if(Id >= BLIT_SEM_EVENT_MAX)
   {
      return;
   }
   // Trace events
   switch(Event)
   {
   case STBLIT_BLIT_COMPLETED_EVT :
      Semaphore_Signal(BLIT_Sem_EventOccured[Id]);
      //_DTraceError("%s(%d) - STBLIT_BLIT_COMPLETED_EVT received !\n",__FUNCTION__,Id);
      break;

   case STBLIT_JOB_COMPLETED_EVT  :
      Semaphore_Signal(BLIT_Sem_EventOccured[Id]);
      //_DTraceError("%s(%d) - STBLIT_JOB_COMPLETED_EVT received !\n",__FUNCTION__,Id);
      break;

   default :
      _DTraceError("%s(%d) - Undefined event received !\n",__FUNCTION__,Id);
      break;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
ST_ErrorCode_t FreeGAMMemory(GAMLOAD_Object_t* object)
{
   if( object == NULL )
   {
      return ST_NO_ERROR;
   }

   if( object->Palette == NULL )
   {
      return ST_NO_ERROR;
   }

   if( object->Bitmap == NULL )
   {
      return ST_NO_ERROR;
   }

//   _DTraceError("Freeing memory\n");

   if( object->Palette->Data_Cp != NULL )
   {
      SYS_Memory_Deallocate(SYS_MEMORY_CACHED /*SYS_MEMORY_AVMEM*/,(U32)object->Palette->Data_Cp);
   }

   if( object->Bitmap->Data2_Cp != NULL )
   {
      SYS_Memory_Deallocate(SYS_MEMORY_CACHED /*SYS_MEMORY_AVMEM*/,(U32)object->Bitmap->Data2_Cp);
   }

   if( object->Bitmap->Data1_Cp != NULL )
   {
      SYS_Memory_Deallocate(SYS_MEMORY_CACHED /*SYS_MEMORY_AVMEM*/,(U32)object->Bitmap->Data1_Cp);
   }

   delete object;
   object = NULL;

   return(ST_NO_ERROR);
}
//////////////////////////////////////////////////////////////////////////////////////
