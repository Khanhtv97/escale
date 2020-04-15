//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GTimer.hpp                                                                 //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GTIMER_HPP_
#define GTIMER_HPP_

#include "stapp_main.h"
#include "GObject.hpp"

#define MAX_GTIMER_NUMBER 256

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class GTimer : public virtual GObject
{
private:
   static void MainTask(void* Parameter);

   static Task_t m_mainTaskId;
   static bool m_isMainTaskRunning;
   static U32 m_Now; // current time in ms
   static int m_timerCount;
   static GTimer** m_ppTimerList;

   IExecutable* m_pFnCallback;

   int m_interval;
   int m_period;
   int m_counter;

   bool m_isOneShot;
   bool m_isRunning;

   U32 m_lastExecutionTime;

   GTimer(IExecutable* callBack, int interval, bool oneShot);
   ~GTimer();

   void fire();

public:
   static GTimer* createTimer(IExecutable* callBack, int interval, bool oneShot = false);
   static void deleteTimer(GTimer*& pTimer);

   static void pauseAllTimers();
   static void resumeAllTimers();

   void start();
   void stop();
   void restart();
   void setPeriod(int period);
   bool checkRunning() {return m_isRunning;};
   int getInterval() { return m_interval; };
   int setInterval(int inVal) { m_interval = inVal; };
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GTIMER_HPP_ */
