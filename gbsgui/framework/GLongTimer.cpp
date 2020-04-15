//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GLongTimer.cpp                                                             //
//////////////////////////////////////////////////////////////////////////////////////

#include "GLongTimer.hpp"
#include "GSystem.hpp"
#include "GDebug.hpp"
#include "GDelegate.hpp"
#include "GEvents.hpp"
#include "APIPower.hpp"

bool GLongTimer::MainTaskRunning = false;
int GLongTimer::timerCount = 0;
int GLongTimer::_delayCount = 0;
U32 GLongTimer::now = 0;
Task_t GLongTimer::MainTaskId = NULL; //Task_t GLongTimer::MainTaskId;

GLongTimer** GLongTimer::timerList = new GLongTimer*[MAX_GTIMER_NUMBER];

//namespace GBS {
//namespace STM {
//namespace Framework {
//////////////////////////////////////////////////////////////////////////////////////

GLongTimer::GLongTimer(IExecutable * callBack, int interval, bool oneShot) : GObject()
{
   _callback = NULL;

   if( !GLongTimer::MainTaskRunning )
   {
      if( GLongTimer::MainTaskId != (Task_t)NULL )
      {
         Task_Wait(GLongTimer::MainTaskId);
         task_kill(GLongTimer::MainTaskId, 0, 0);
         Task_Delete(GLongTimer::MainTaskId);
         GLongTimer::MainTaskId = (Task_t)NULL;
      }

      U32 ParamId = 0;

      GLongTimer::MainTaskId = Task_Create(GLongTimer::MainTask, (void*)ParamId, 8192, MIN_USER_PRIORITY, "GLongTimer Task", 0, GLongTimer::MainTaskId);

      if( GLongTimer::MainTaskId == (Task_t)NULL )
      {
         _DTraceError("Cannot create GLongTimer Main Task\n");
      }
      else
      {
         GLongTimer::MainTaskRunning = true;
         _DTraceError("GLongTimer Main Task created\n");
      }
   }

   GLongTimer::now = GSystem::now();
   _lastExecutionTime = GSystem::now();
   _interval = interval;
   _period = 0;
   _counter = 0;
   _isOneShot = oneShot;
   _callback = callBack;
   _isRunning = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::setPeriod(int period)
{
   if( period >= 0 )
   {
      _period = period;
   }
   else
   {
      _period = 0;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::MainTask(void* Parameter)
{
   int j = 0;
   while( GLongTimer::MainTaskRunning )
   {
      //_DTraceError("GLongTimer Main Task cycle\n");
      Task_Delay(ST_GetClocksPerSecond()/20);
      GLongTimer::now = GSystem::now();
      int i;
      _delayCount++;

      if( _delayCount > 40 )
      {
         _delayCount = 0;
         //_DTraceError("GLongTimer::MainTask *** still running\n");
      }

      for( i = 0; i < timerCount; i++ )
      {
         timerList[i]->fire();
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GLongTimer* GLongTimer::createTimer(IExecutable* callBack, int interval, bool oneShot)
{
   if( GLongTimer::timerCount >= MAX_GTIMER_NUMBER )
   {
      _DTraceError("Exceeded maximum number of GLongTimer instance. Please delete unused ones.");
      return NULL;
   }
   else
   {
      GLongTimer* timer = new GLongTimer(callBack, interval, oneShot);
      GLongTimer::timerList[GLongTimer::timerCount++] = timer;
      return timer;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::deleteTimer(GLongTimer* timer)
{
   int i = GLongTimer::timerCount-1;

   while( i >= 0 )
   {
      if( GLongTimer::timerList[i] == timer )
      {
         break;
      }
      i--;
   }

   if( i >= 0 )
   {
      GLongTimer::timerCount--;
      for( ; i < GLongTimer::timerCount; i++ )
      {
         GLongTimer::timerList[i] = GLongTimer::timerList[i+1];
      }
   }

   if( GLongTimer::timerCount <= 0 )
   {
      Task_Wait(MainTaskId);
      task_kill(MainTaskId, 0, 0);
      Task_Delete(MainTaskId);
      MainTaskId = NULL;
      MainTaskRunning = FALSE;
   }

   if( timer != NULL )
   {
      delete timer;
      timer = NULL;   //co can set Timer = NULL ??-- 2012-10-08 hiendv
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::pauseAllTimers()
{
   for( int i = 0; i < timerCount; i++ )
   {
      timerList[i]->stop();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::resumeAllTimers()
{
   for( int i = 0; i < timerCount; i++ )
   {
      timerList[i]->start();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::start()
{
   _counter = 0;
   _lastExecutionTime = GSystem::now();
   _isRunning = true;
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::stop()
{
   _counter=0;
   _isRunning = false;
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::restart()
{
   stop();
   start();
}

//////////////////////////////////////////////////////////////////////////////////////
void GLongTimer::fire()
{
   if( _isRunning && (GLongTimer::now - _lastExecutionTime >= _interval) )
   {
      _lastExecutionTime = GLongTimer::now;

      if( (_callback != NULL) && (CPower::getInstance().getState() != GPS_STANDBY_SYS) )
      {
         _callback->execute(GEVENT_TIMER_TRIGGER, this, NULL);

         if( _period > 0 && _counter >= _period-1 )
         {
            stop();
         }

         if( _isOneShot )
         {
            stop();
         }

         _counter++;
      }
   }
}

//////////////////////////////////////////////////////////////////////////////////////
GLongTimer::~GLongTimer()
{
   if( _isRunning )
   {
      stop();
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
