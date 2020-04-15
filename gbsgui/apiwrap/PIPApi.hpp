//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2015 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: PIPApi.cpp                                                                 //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef PIP_API_HPP_
#define PIP_API_HPP_

#include "stapp_main.h"
#include "GDebug.hpp"

typedef struct
{
   U32 PipID;
   PIP_Event_t EventType;
} PIP_Event_Data_t;

//namespace GBS {
//namespace STM {
//namespace API {

//////////////////////////////////////////////////////////////////////////////////////
extern "C"
{
   BOOL GPIPOpen(U32 pipID, U32 videoID, U32 gfxID);
   BOOL GPIPClose(U32 pipID);
   BOOL GPIPEnable(U32 pipID);
   BOOL GPIPDisable(U32 pipID);
   BOOL GPIPPause(U32 pipID);
   BOOL GPIPResume(U32 pipID);
   BOOL GPIPSetWindow(U32 pipID, int xPos, int yPos, int width, int height);
   BOOL GPIPSwapViewPorts(U32 pipID);
   BOOL GPIPSubscribeCommandEvent(U32 pipID);
}
//////////////////////////////////////////////////////////////////////////////////////
//} //API
//} //STM
//} //GBS
#endif //PIP_API_HPP_
