/*
 * APIBitmap.hpp
 *
 *  Created on: Apr 10, 2012
 *      Author: seta
 * Description: wrap all functions related to Bitmap, draw processing
 */

#ifndef APIBITMAP_HPP_
#define APIBITMAP_HPP_

#include "stapp_main.h"
#include "APIDrawParams.hpp"

enum {
   BLIT_SEM_EVENT_CLEARRECT = 0,
   BLIT_SEM_EVENT_DRAWRECT,
   BLIT_SEM_EVENT_FILLRECT,
   BLIT_SEM_EVENT_COPY,
   BLIT_SEM_EVENT_MAX
};

static Semaphore_t BLIT_Sem_EventOccured[BLIT_SEM_EVENT_MAX];

/* Definition of internal subtitle structure */
/* ----------------------------------------- */
typedef struct APIBitmap_Context_s
{
 BOOL                     Bitmap_Used;
 BOOL                     Bitmap_Started;
 STEVT_Handle_t           EVT_Handle;
 Semaphore_t              BLIT_SEM_Event;
 STEVT_SubscriberID_t     BLIT_EVT_SubscriberID;
 STLAYER_ViewPortParams_t GFX_LAYER_ViewPortParams;
 STLAYER_ViewPortSource_t GFX_LAYER_ViewPortSource;
 STGXOBJ_Bitmap_t         GFX_LAYER_GXOBJ_Bitmap;
 STGXOBJ_Palette_t        GFX_LAYER_GXOBJ_Palette;
} APIBitmap_Context_t;

extern "C" {
   void APIBitmapInit();
   void APIBitmapTerm();
   BOOL GFX_SetLayerAlpha(int planeId, int alpha);
   BOOL GFX_SetAntiFlicker(int planeId, BOOL isEnable);

   BOOL DrawGam(const GAMLOAD_Object_t *source, APIDrawParams obj_APIDrawParams);
   BOOL EnableGrafix(int planeId);
   BOOL DisableGrafix(int planeId);
   BOOL ClearRect(int planeId, int posX, int posY, int width, int height);
   BOOL ClearPlane(int planeId);

   ST_ErrorCode_t FreeGAMMemory(GAMLOAD_Object_t *object);
   ST_ErrorCode_t GBLITCopyInit();
   ST_ErrorCode_t GBLITCopyTerm();
   BOOL GBLITcopy(U32 nBLITid=0,U32 nGfxSrcId=1,U32 nGfxDstId=0);
   BOOL GBDrawRect(int posX,int posY,int width,int height,U32 argb=0, int plan =1);
   BOOL GBLITfillRect(int posX,int posY,int width,int height,U32 argb=0,int Plane=1);//2013-05-07tu_th66

   //Convert from normal alpha (0-100%, 100% for pure transparency) to STAPI+SDK alpha (0-128, 0 for pure transparency)
   int toSTMAlpha(int aPercent);
   U8 GFXalphaChg(int c=1);
   U8 GFXalphaCur();
   U8 GFXalphaSet(U8 a=120);
}

#endif /* APIBITMAP_HPP_ */
