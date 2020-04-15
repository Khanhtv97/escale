//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GLongTimer.hpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GLONGTIMER_HPP_
#define GLONGTIMER_HPP_

#include "stapp_main.h"
#include "GObject.hpp"

#define MAX_GLONGTIMER_NUMBER 256

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

class GLongTimer : public virtual GObject
{
private:
   //static int _delayCount; // for test only
   static Task_t MainTaskId;
   static bool MainTaskRunning;
   static U32 now; // current time in ms
   static int timerCount;
   static GLongTimer** timerList;
   static int _delayCount;

   IExecutable* _callback;

   int _interval;
   int _period;
   int _counter;

   bool _isRunning;
   bool _isOneShot;

   U32 _lastExecutionTime;

   GLongTimer(IExecutable* callBack, int interval, bool oneShot);
   ~GLongTimer();

   static void MainTask(void* Parameter);
   void fire();

public:
   static GLongTimer* createTimer(IExecutable* callBack, int interval, bool oneShot = false);
   static void deleteTimer(GLongTimer*);

   static void pauseAllTimers();
   static void resumeAllTimers();

   void start();
   void stop();
   void restart();
   void setPeriod(int period);
   bool checkRunning() {return _isRunning;};
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GLONGTIMER_HPP_ */
