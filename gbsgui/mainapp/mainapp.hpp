//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: mainapp.h                                                                  //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef MAINAPP_HPP_
#define MAINAPP_HPP_

#include "stddefs.h"

//namespace GBS {
//namespace STM {
//namespace Application {
//////////////////////////////////////////////////////////////////////////////////////
extern "C"
{
   ST_ErrorCode_t STGUI_Init(void);
   ST_ErrorCode_t STGUI_Splash_Init(void);
   ST_ErrorCode_t STGUI_Run(void);
   ST_ErrorCode_t STGUI_Term(void);
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Application
//} //STM
//} //GBS
#endif /* MAINAPP_HPP_ */
