//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GTimer.cpp                                                                 //
//////////////////////////////////////////////////////////////////////////////////////

#include "GTimer.hpp"
#include "GSystem.hpp"
#include "GDebug.hpp"
#include "GDelegate.hpp"
#include "GEvents.hpp"
#include "APIPower.hpp"

bool GTimer::m_isMainTaskRunning = false;
int GTimer::m_timerCount = 0;
Task_t GTimer::m_mainTaskId = (Task_t)NULL;
GTimer** GTimer::m_ppTimerList = new GTimer*[MAX_GTIMER_NUMBER];

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
GTimer::GTimer(IExecutable* callBack, int interval, bool oneShot) : GObject()
{
   if( !GTimer::m_isMainTaskRunning )
   {
      if( GTimer::m_mainTaskId != (Task_t)NULL )
      {
         Task_Wait(GTimer::m_mainTaskId);
         task_kill(GTimer::m_mainTaskId, 0, 0);
         Task_Delete(GTimer::m_mainTaskId);
         GTimer::m_mainTaskId = (Task_t)NULL;
      }

      GTimer::m_mainTaskId = Task_Create(GTimer::MainTask, NULL, 8192, MIN_USER_PRIORITY, "GTimer Task", 0, GTimer::m_mainTaskId);

      if( GTimer::m_mainTaskId == (Task_t)NULL )
      {
         _DTraceError("Cannot create GTimer Main Task\n");
      }
      else
      {
         GTimer::m_isMainTaskRunning = true;
         _DTraceError("GTimer Main Task created\n");
      }
   }

   m_lastExecutionTime = GSystem::now();
   m_interval = interval;
   m_period = 0;
   m_counter = 0;
   m_isOneShot = oneShot;
   m_pFnCallback = callBack;
   m_isRunning = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::setPeriod(int period)
{
   if( period >= 0 )
   {
      m_period = period;
   }
   else
   {
      m_period = 0;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::MainTask(void* Parameter)
{
   while( GTimer::m_isMainTaskRunning )
   {
      Task_Delay(ST_GetClocksPerSecond()/100);
      for( int i = 0; i < m_timerCount; i++ )
      {
         m_ppTimerList[i]->fire();
         Task_Delay(ST_GetClocksPerSecond()/100);
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GTimer* GTimer::createTimer(IExecutable* callBack, int interval, bool oneShot)
{
   if( GTimer::m_timerCount >= MAX_GTIMER_NUMBER )
   {
      _DTraceError("Exceeded maximum number of GTimer instance. Please delete unused ones.");
      return NULL;
   }
   else
   {
      GTimer* timer = new GTimer(callBack, interval, oneShot);
      GTimer::m_ppTimerList[GTimer::m_timerCount] = timer;
      GTimer::m_timerCount++;
      return timer;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::deleteTimer(GTimer*& pTimer)
{
   int i = GTimer::m_timerCount-1;
   bool found = false;

   while( i >= 0 )
   {
      if( GTimer::m_ppTimerList[i] == pTimer )
      {
         found = true;
         break;
      }
      i--;
   }

   if( i >= 0 )
   {
      GTimer::m_timerCount--;
      for( ; i < GTimer::m_timerCount; i++ )
      {
         GTimer::m_ppTimerList[i] = GTimer::m_ppTimerList[i+1];
      }
   }

   if( found && pTimer != NULL )
   {
      delete pTimer;
      pTimer = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::pauseAllTimers()
{
   for( int i = 0; i < m_timerCount; i++ )
   {
      m_ppTimerList[i]->stop();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::resumeAllTimers()
{
   for( int i = 0; i < m_timerCount; i++ )
   {
      m_ppTimerList[i]->start();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::start()
{
   m_counter = 0;
   m_lastExecutionTime = GSystem::now();
   m_isRunning = true;
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::stop()
{
   m_counter = 0;
   m_isRunning = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::restart()
{
   stop();
   start();
}

//////////////////////////////////////////////////////////////////////////////////////
void GTimer::fire()
{
   U32 now = GSystem::now();

   if( m_isRunning && ( now - m_lastExecutionTime >= m_interval) )
   {
      m_lastExecutionTime = now;

      if( (m_pFnCallback != NULL) && (CPower::getInstance().getState() != GPS_STANDBY_SYS) )
      {
         m_pFnCallback->execute(GEVENT_TIMER_TRIGGER, this, NULL);
         //_DTraceError("\n\n ON TIMER 0x%08X", m_pFnCallback);
         //Task_Delay(100);

         if( m_isOneShot || ((m_period > 0) && (m_counter >= m_period-1)) )
         {
            stop();
         }

         m_counter++;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GTimer::~GTimer()
{
   if( m_isRunning )
   {
      stop();
   }

   if( m_pFnCallback != NULL )
   {
      delete m_pFnCallback;
      m_pFnCallback = NULL;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
