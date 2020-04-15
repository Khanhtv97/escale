//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GSystem.hpp                                                                //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GSYSTEM_HPP_
#define GSYSTEM_HPP_

#include "GUtility.hpp"
#include "GTimer.hpp"
#include "GDelegate.hpp"
#include "GObject.hpp"
#include "APIPower.hpp"

//Sun, 01 Jan 2012 00:00:00 GMT
#define SYSTEM_TIME_BASE_INIT 1325376000

#define _S_ GSystem::getInstance()
#define FILTER_TIMER_CALLBACK

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
class GSystem : public virtual GObject
{
private:
   GSystem();
   ~GSystem();

   static time_t  m_baseTime;
   TimeZone_dtv_t m_timeZone;
   static U32 m_diffSecondBaseTime;
   static BOOL m_bDVBTimeTask;
   BOOL m_isNeedReboot;
   int m_timeZoneIndex;

   // reset pending reboot command
   static void WatchDogTask(void* Parameter);

public:
   //BOOL m_isBroadcastTime;

   void init();
   void initWatchDog();

   // current time in ms
   static U32 now();

   // current time in DTV_time
   GError_t getCurrentTime(DayTime_dtv_t& currentTime);
   GError_t getCurrentTimeByTimeZone(DayTime_dtv_t& currentTime);

   TimeZone_dtv_t getTimeZone();

   time_t getCurrentTimeSecond();
   time_t getBaseTime();
   void   setBaseTime(time_t baseTime);
   void   setTimeZoneIndex(int timeIndex);
   void   setDiffSecondBaseTime(U32 _diffSecond);

   // cpu load since last call of this function
   // don vi: 1/100000
   static U32 cpuload();
   static U32 meminfo(int FullInfo = 1);
   static U32 taskinfo();
   static GSystem& getInstance();

   // reboot after 'secondToDelay' seconds, value varies from 0x00 to 0xfe
   static void reboot(U16 secondToDelay = 0);

   void setReboot();
   BOOL isNeedReboot();

   void WDT_ST40(void);
};

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif // GSYSTEM_HPP_
