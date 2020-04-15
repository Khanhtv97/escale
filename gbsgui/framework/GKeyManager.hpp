//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: KeyManager.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GKEYMANAGER_HPP_
#define GKEYMANAGER_HPP_

#include "stapp_main.h"
#include "GObject.hpp"
#include "GEvents.hpp"
#include "GKey.hpp"

#define KEY_INPUT_RC       0x01   // Remote controller
#define KEY_INPUT_FP       0x02   // Front panel buttons
#define KEY_INPUT_UART     0x04   // Keyboard on UART terminal
#define KEY_INPUT_GDB      0x08   // Keyboard on gdb console (stdin)
#define KEY_INPUT_PIO      0x10   // Keyboard on PIO 2.4-2.5 PS/2 Keyboard

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

class GKeyManager : public virtual GObject
{
private:
   GKeyManager();
   ~GKeyManager();

   static void KeyListenerTask(void* Parameter);
   static void KeyListenerTask_PS2(void* Parameter);

   U8 m_inputMode;

public:
   static GKeyManager& getInstance();
   void stopKeyManagerTask();
   void startKeyManagerTask();

   void getKeyName(U32 gkey, char* keyname);
   void setInputMode(U8 mode);
   void flushAllKey();

   U8 getInputMode();

   U32 scanKeyPressed();

   BOOL isKeyWakeup(const U32 keyCode);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GKEYMANAGER_HPP_ */
