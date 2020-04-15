//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2015 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: PIPApi.cpp                                                                 //
//////////////////////////////////////////////////////////////////////////////////////

#include "PIPApi.hpp"
#include "GEventManager.hpp"

//namespace GBS {
//namespace STM {
//namespace API {
//////////////////////////////////////////////////////////////////////////////////////
#if defined(PIP_MAX_NUMBER)&&defined(GFX_LAYER_MAX_NUMBER)&&defined(BLIT_MAX_NUMBER)

static void GPIPEventCallback(PIP_Event_t event, U32 pipID);

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPOpen(U32 pipID, U32 videoID, U32 gfxID)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 ID = pipID;
   S32 VIDEO = videoID;
   S32 LAYER = gfxID;

   // Get PIP Identifier
   if( ID >= PIP_MAX_NUMBER )
   {
      _DTraceError("--> Invalid pip ID, should be [0,1,..] !\n");
      return FALSE;
   }

   // Get VIDEO Identifier
   if( VIDEO >= VID_MAX_NUMBER )
   {
      _DTraceError("--> Invalid video ID, should be [0,1,..] !\n");
      return FALSE;
   }

   if( VID_Codec[VIDEO].VID_Handle == 0 )
   {
      _DTraceError("--> This video %d is not yet initialized, you need to start a video decode first !\n", VIDEO);
      return FALSE;
   }

   // Get LAYER Identifier
   if( LAYER >= GFX_LAYER_MAX_NUMBER )
   {
      _DTraceError("--> Invalid gfx layer ID, should be [0,1,..] !\n");
      return FALSE;
   }

   if( GFX_LAYER_Handle[LAYER] == 0 )
   {
      _DTraceError("--> This gfx layer %d is undefined on this hardware !\n", LAYER);
      return FALSE;
   }

   // Open the PIP
   ErrCode = PIP_Open(ID, VIDEO, LAYER);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to open the pip %d ErrCode=%d !\n", ID, ErrCode);
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPClose(U32 pipID)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 ID = pipID;

   // Get PIP Identifier
   if( ID >= PIP_MAX_NUMBER )
   {
      _DTraceError("--> Invalid pip ID, should be [0,1,..] !\n");
      return FALSE;
   }

   // Enable the PIP
   ErrCode = PIP_Close(ID);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to close the pip %d ErrCode=% !\n", ID, ErrCode);
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPEnable(U32 pipID)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 ID = pipID;

   // Get PIP Identifier
   if( ID >= PIP_MAX_NUMBER )
   {
      _DTraceError("--> Invalid pip ID, should be [0,1,..] !\n");
      return FALSE;
   }

   // Enable the PIP
   ErrCode = PIP_Enable(ID);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to enable the pip %d ErrCode=%d !\n",ID,ErrCode);
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPDisable(U32 pipID)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 ID = pipID;

   // Get PIP Identifier
   if( ID >= PIP_MAX_NUMBER )
   {
      _DTraceError("--> Invalid pip ID, should be [0,1,..] !\n");
      return FALSE;
   }

   // Disable the PIP
   ErrCode = PIP_Disable(ID);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to disable the pip %d ErrCode=%d !\n", ID, ErrCode);
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPPause(U32 pipID)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 ID = pipID;

   // Get PIP Identifier
   if( ID >= PIP_MAX_NUMBER )
   {
      _DTraceError("--> Invalid pip ID, should be [0,1,..] !\n");
      return FALSE;
   }

   // Pause the PIP
   ErrCode = PIP_Pause(ID);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to pause the pip %d ErrCode=%d !\n", ID, ErrCode);
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPResume(U32 pipID)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 ID = pipID;

   // Get PIP Identifier
   if( ID >= PIP_MAX_NUMBER )
   {
      _DTraceError("--> Invalid pip ID, should be [0,1,..] !\n");
      return FALSE;
   }

   // Pause the PIP
   ErrCode = PIP_Resume(ID);

   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to resume the pip %d ErrCode=% !\n", ID, ErrCode);
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPSetWindow(U32 pipID, int xPos, int yPos, int width, int height)
{
   ST_ErrorCode_t ErrCode = ST_NO_ERROR;
   S32 ID = pipID;
   S32 XPOS = xPos;
   S32 YPOS = yPos;
   S32 WIDTH = width;
   S32 HEIGHT = height;

   // Get PIP Identifier
   if( ID >= PIP_MAX_NUMBER )
   {
      _DTraceError("--> Invalid pip ID, should be [0,1,..] !\n");
      return FALSE;
   }

   // Get X position
   if( XPOS < 0 || YPOS < 0 || WIDTH <= 0 || HEIGHT <= 0 )
   {
      _DTraceError("--> Invalid X, Y, WIDTH, HEIGHT, should be a value>=0 !\n");
      return FALSE;
   }

   // Set the window of the PIP
   ErrCode = PIP_SetWindow(ID, XPOS, YPOS, WIDTH, HEIGHT);
   if( ErrCode != ST_NO_ERROR )
   {
      _DTraceError("--> Unable to set window of the pip %d ErrCode=%d !\n",ID,ErrCode);
      return FALSE;
   }

   // Return no errors
   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPSwapViewPorts(U32 pipID)
{
   return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
static void GPIPEventCallback(PIP_Event_t event, U32 pipID)
{
   if( event != PIP_EVT_NONE )
   {
      GEvents gPIPEvent = GEVENT_PIP_COMMAND_DONE;
      PIP_Event_Data_t eventData;
      eventData.PipID = pipID;
      eventData.EventType = event;
      GEventManager::getInstance().fire(gPIPEvent, &eventData);
   }
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPSubscribeCommandEvent(U32 pipID)
{
   ST_ErrorCode_t errCode = PIP_SubscribeCommandEvent(pipID, GPIPEventCallback);
   if( errCode != ST_NO_ERROR )
   {
      _DTraceError("Unable to subscribe PIP command event");
      return FALSE;
   }

   return TRUE;
}

#else

//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPOpen(U32 pipID, U32 videoID, U32 gfxID){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPClose(U32 pipID){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPEnable(U32 pipID){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPDisable(U32 pipID){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPPause(U32 pipID){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPResume(U32 pipID){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPSetWindow(U32 pipID, int xPos, int yPos, int width, int height){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPSwapViewPorts(U32 pipID){return FALSE;}
//////////////////////////////////////////////////////////////////////////////////////
BOOL GPIPSubscribeCommandEvent(U32 pipID){return FALSE;}

#endif
//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
