//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GUtility.cpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#include "GUtility.hpp"
#include "time.h"
#include "GSystem.hpp"
#include "GKey.hpp"

static TimeZone_t arrTimeZone[] = { {0, 0, TIME_ZONE_ADD, 0, 0, 0, "Africa / Casablanca"},
                                    {7, 0, TIME_ZONE_ADD, 0, 0, 0, "Asia / Hanoi, Jakarta"},
                                    {8, 0, TIME_ZONE_ADD, 0, 0, 0, "Asia / Hong Kong"} };

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////
GError_t ChangeDayTimeZone(DayTime_dtv_t& dt1, TimeZone_dtv_t* stTzone, DayTime_dtv_t* dt2)
{
   U16 odd = 0, total = 0;
   U16 tz_hour, tz_minute;
   U16 num_day;

   if( stTzone == NULL )
   {
      _DTraceDebug("GUtility->ChangeDayTimeZone: TimeZone NULL!!!");
      return GERROR_BAD_PARAMETER;
   }

   if( stTzone->status == TIME_ZONE_ADD )
   {
      tz_hour = stTzone->hour;
      tz_minute = stTzone->minute;
   }
   else //stTzone.status == MINUS)
   {
      tz_hour = -stTzone->hour;
      tz_minute = -stTzone->minute;
   }

   //Second
   dt2->hms.second = dt1.hms.second;

   //Minute
   total = dt1.hms.minute + tz_minute;
   if( total >= 60 )
   {
      dt2->hms.minute = (U8) (total % 60);
      odd = 1;
   }
   else if( total >= 0 )
   {
      dt2->hms.minute = (U8) total;
      odd = 0;
   }
   else
   { //total <= 0
      dt2->hms.minute = (U16) total + 60;
      odd = -1;
   }

   //Hours
   total = dt1.hms.hour + tz_hour + odd;

   if( total >= 24 )
   {
      dt2->hms.hour = (U8) (total % 24);
      odd = 1;
   }
   else if( total >= 0 )
   {
      dt2->hms.hour = (U8) (total);
      odd = 0;
   }
   else
   { //total <= 0
      dt2->hms.hour = (U8) (total + 24);
      odd   = -1;
   }

   //DayWeek
   total = dt1.ymdw.week_day + odd;
   if( total > 7 )
   {
      dt2->ymdw.week_day = 1;
   }
   else if( total > 0 )
   {
      dt2->ymdw.week_day = (U8) total;
   }
   else
   { // total <= 0
      dt2->ymdw.week_day = (U8) (total + 7);
   }

   //Day Month
   num_day = GetNumDayOfMonth(dt1.ymdw.year, dt1.ymdw.month);

   //Day
   total = dt1.ymdw.day + odd;
   if( total > num_day )
   {
      dt2->ymdw.day = 1;
      odd = 1;
   }
   else if( total > 0 )
   {
      dt2->ymdw.day = (U16)total;
      odd   = 0;
   }
   else
   { //total <= 0
      //set day later
      odd = -1;
   }

   //Month
   total = dt1.ymdw.month + odd;
   if( total > 12 )
   {
      dt2->ymdw.year = dt1.ymdw.year + 1;
      dt2->ymdw.month = 1;
   }
   else if( total > 0 )
   {
      dt2->ymdw.year = dt1.ymdw.year;
      dt2->ymdw.month = (U16)total;
   }
   else // total == 0
   {
      dt2->ymdw.year = dt1.ymdw.year - 1;
      dt2->ymdw.month = 12;
   }

   if( odd == -1 )
   {
      dt2->ymdw.day = GetNumDayOfMonth(dt2->ymdw.year, dt2->ymdw.month);
   }

   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
U16 GetNumDayOfMonth(U16 uiYear, U16 uiMonth)
{
   U16 uiNumDay = 0;
   //Day Month
   switch( uiMonth )
   {
   case 1:
   case 3:
   case 5:
   case 7:
   case 8:
   case 10:
   case 12:
      uiNumDay = 31;
      break;

   case 4:
   case 6:
   case 9:
   case 11:
      uiNumDay = 30;
      break;

   case 2:
      if( ((uiYear % 100 != 0) && (uiYear % 4 == 0)) || (uiYear %400 == 0) )
      {
         uiNumDay = 29;
      }
      else
      {
         uiNumDay = 28;
      }
      break;
   }
   return uiNumDay;
}

//////////////////////////////////////////////////////////////////////////////////////
BOOL checkValidTime(YearMontDayWeek_dtv_t dayTime)
{
   if( dayTime.day == 0 || dayTime.month == 0 || dayTime.year == 0 )
   {
      return FALSE;
   }

   if( dayTime.month > 12 )
   {
      return FALSE;
   }

   if( dayTime.day > GetNumDayOfMonth(dayTime.year, dayTime.month) )
   {
      return FALSE;
   }

   return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTION :GetDateFromYearMonth(U16 Year, U8 Month, U8 Day)
// Input: U16 Year
//         U8  Month
//         U8  Day
//
// Output: [0..6] -> [Sun .. Sat]
//////////////////////////////////////////////////////////////////////////////////////
U8 GetDateFromYearMonth(U16 Year, U8 Month, U8 Day)
{
   U8 L, WD;
   U16 MJD;

   if( Month == 1 || Month == 2 )
   {
      L = 1;
   }
   else
   {
      L = 0;
   }

   MJD = 14956 + Day + (U16)( (Year - L)*365.25 ) + (U16)( (Month + 1 + L*12) * 30.6001);

   WD =  ((MJD + 2) % 7); //Mon = 1, Sun = 0.

   //static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
   //Year -= Month < 3;
   //return (Year + Year/4 - Year/100 + Year/400 + t[Month-1] + Day) % 7;

    return(WD);
}

//////////////////////////////////////////////////////////////////////////////////////
U16 GetDaysFromYearMonth(U16 Year, U8 Month, U8 Day)
{
   U16 Days = 0;
   for( int i = 1; i < Month; i++ )
   {
      Days += GetNumDayOfMonth(Year, Month);
   }
   Days += Day;

   return Days;
}

//////////////////////////////////////////////////////////////////////////////////////
U32 GetNumSecondOfMonth(U16 uiYear, U16 uiMonth)
{
   U32 uiRet = 0;
   U8  uiNumDay = 0;

   switch( uiMonth )
   {
   case 4:
   case 6:
   case 9:
   case 11:
      uiNumDay = 30;
      break;

   case 1:
   case 3:
   case 5:
   case 7:
   case 8:
   case 10:
   case 12:
      uiNumDay = 31;
      break;

   case 2:
      if( (uiYear % 100 != 0 && uiYear % 4 == 0) ||  (uiYear % 400 == 0) )
      {
         uiNumDay = 29;
      }
      else
      {
         uiNumDay = 28;
      }
      break;

   default:
      uiNumDay = 0;;
   }

   uiRet = uiNumDay * 24 * 60 * 60;
   return uiRet ;
}

//////////////////////////////////////////////////////////////////////////////////////
/*
GError_t TOT2DTVTime(STTAPI_TOT_t* TOT_time, DayTime_dtv_t* DVT_time)
{
   // Get ymd and hms for DTV_time
   if( TOT_time == NULL || DVT_time == NULL )
   {
      _D_.Error("GUtility->ConvertTOT2DTVTime: TOT_time or DVT_time NULL!!!\n");
      return GERROR_BAD_PARAMETER;
   }

   DVT_time->ymdw.year = TOT_time->TOTData_t.Year;
   DVT_time->ymdw.month = TOT_time->TOTData_t.Month;
   DVT_time->ymdw.day = TOT_time->TOTData_t.Day;
   DVT_time->ymdw.week_day = TOT_time->TOTData_t.Date;
   DVT_time->hms.hour = TOT_time->TOTData_t.Hour;
   DVT_time->hms.minute = TOT_time->TOTData_t.Minute;
   DVT_time->hms.second = TOT_time->TOTData_t.Second;

   // Change DayTime to timezone
   DayTime_dtv_t current = *DVT_time;
   TimeZone_dtv_t stZone;

   stZone.hour = TOT_time->stLocalTimeOffsetDes.local_time_offset >> 8;
   stZone.minute = TOT_time->stLocalTimeOffsetDes.local_time_offset & 0xFF;
   stZone.status = TOT_time->stLocalTimeOffsetDes.local_time_offset_polariry;

   ChangeDayTimeZone(current, &stZone, DVT_time);

   return GERROR_NONE;
}
*/

//////////////////////////////////////////////////////////////////////////////////////
U32 GetNumSecondOfYear(U16 uiYear)
{
   U32 uiRet = 0;
   U16 uiNumDay = 0;

   if( (uiYear % 100 != 0 && uiYear % 4 == 0) ||  (uiYear % 400 == 0) )
   {
      uiNumDay = 366;
   }
   else
   {
      uiNumDay = 365;
   }

   uiRet += uiNumDay * 24 * 60 * 60;
   return uiRet;
}

//////////////////////////////////////////////////////////////////////////////////////
// FUNCTION :GetDayFromSecond(U32 uiSecond, DayTime_dtv *timeBase, DayTime_dtv *pDt)
// Input: uiSecond: second UnixTimestamp
// Output:      DayTime_dtv *pDt
//////////////////////////////////////////////////////////////////////////////////////
GError_t Second2DayTime(U32 uiSecond, DayTime_dtv_t* pDt)
{
   U16 i = 0;
   U32 uiTemp = 0;
   U32 uiTempSec = 0;
   U32 MJD   = 0;
   U16 Y, M, D, L;

   if( pDt == NULL )
   {
      _D_.Error("GetDayFromSecond: pDt  == NULL \n");
      return GERROR_BAD_PARAMETER;
   }
   //Set TimeZone
   //uiSecond += iTimeZone * 60 * 60;
   //year
   uiTempSec = 0;
   uiTemp = 0;
   i = 1970;

   while( uiTemp <= uiSecond )
   {
      uiTempSec = uiTemp;
      uiTemp += GetNumSecondOfYear(i);
      i++;
   }

   pDt->ymdw.year = i - 1;
   uiSecond -= uiTempSec;

   //Month
   uiTemp = 0;
   uiTempSec = 0;
   i = 1;

   while( uiTemp <= uiSecond )
   {
      uiTempSec = uiTemp;
      uiTemp += GetNumSecondOfMonth(pDt->ymdw.year, i);
      i++;
   }

   pDt->ymdw.month = i - 1;
   uiSecond -= uiTempSec;

   //day
   uiTemp = 0;
   uiTempSec = 0;
   i = 1;

   while( uiTemp <= uiSecond )
   {
      uiTempSec = uiTemp;
      uiTemp += 24*60*60;
      i++;
   }

   pDt->ymdw.day = i - 1;
   uiSecond -= uiTempSec;

   //Hour
   uiTemp = 0;
   uiTempSec = 0;
   i = 0;

   while( uiTemp <= uiSecond )
   {
      uiTempSec = uiTemp;
      uiTemp += 60*60;
      i++;
   }
   pDt->hms.hour = i - 1;
   uiSecond -= uiTempSec;

   //Minute
   // add number second of time base
   uiTemp = 0;
   uiTempSec = 0;
   i = 0;

   while( uiTemp <= uiSecond )
   {
      uiTempSec = uiTemp;
      uiTemp +=  60;
      i ++;
   }

   pDt->hms.minute = i - 1;
   uiSecond -= uiTempSec;

   //Second
   pDt->hms.second = uiSecond;

   pDt->ymdw.week_day = GetDateFromYearMonth(pDt->ymdw.year, pDt->ymdw.month, pDt->ymdw.day);

   return GERROR_NONE;
}

//////////////////////////////////////////////////////////////////////////////////////
GError_t GetNextDTVTime(DayTime_dtv_t* startTime, HourMinuteSecond_dtv_t* Duration, DayTime_dtv_t* nextTime)
{
   U32 second;
   GError_t ErrorCode = GERROR_NONE;

   if( startTime == NULL || Duration == NULL || nextTime == NULL )
   {
      _DTraceDebug("GUtility->GetNextDTVTime: StartTime or Duration NULL!!!");
      return GERROR_BAD_PARAMETER;
   }
   second = DayTime2Second(startTime) + Duration->hour*3600 + Duration->minute*60 + Duration->second;
   ErrorCode = Second2DayTime(second, nextTime);

   return ErrorCode;
}

//////////////////////////////////////////////////////////////////////////////////////
// Get second UnixTimestamp from DayTime
//////////////////////////////////////////////////////////////////////////////////////
time_t DayTime2Second(DayTime_dtv_t* DayTime)
{
   struct tm t1;
   time_t secondt1;

   if( DayTime == NULL )
   {
      return 0;
   }

   memset(&t1, 0, sizeof(tm));

   t1.tm_year = (int)(DayTime->ymdw.year - 1900);
   t1.tm_mon  = (int)(DayTime->ymdw.month - 1);
   t1.tm_mday = (int)DayTime->ymdw.day;
   t1.tm_wday = (int)DayTime->ymdw.week_day;
   t1.tm_yday = (int)GetDaysFromYearMonth(t1.tm_year, t1.tm_mon, t1.tm_mday);
   t1.tm_hour = (int)DayTime->hms.hour;
   t1.tm_min  = (int)DayTime->hms.minute;

   secondt1 = mktime(&t1);

   return secondt1;
}

//////////////////////////////////////////////////////////////////////////////////////
// @Function: CheckTime
// @Params:
//          Now: Current System time (seconds UnixTimeStamp)
//          StartTime: Time Start with struct (year,month,day,hour,min,second)
//          Duration: Duration plus from startTime (hour, min, second)
//    @Descriptor: Function check current System Time in duration time from starttime.
//    @Return: Percent current Time in range Time, < 0: mean it not in range time;
//////////////////////////////////////////////////////////////////////////////////////
int CheckTime(time_t Now, DayTime_dtv_t* startTime, HourMinuteSecond_dtv_t* Duration)
{
   time_t secondStart, secondNext;
   time_t durationInSecond = 0;
   DayTime_dtv_t nextTime;
   int percent = -1;

   //Calculate second(UnixTimeStamp) of startTime
   secondStart = DayTime2Second(startTime);

   if( Duration != NULL )
   {
      durationInSecond = Duration->hour*3600 + Duration->minute*60 + Duration->second;
   }

#if 0
   // Get Next DTVTime of channel

   if( GetNextDTVTime(startTime, Duration, &nextTime) != GERROR_NONE )
   {
      _DTraceDebug("GUtility::CheckTime===>Can't get NextDTV time");
      return -1;
   }

   //Calculate second(UnixTimeStamp) of NextTime
   secondNext  = DayTime2Second(&nextTime);

   //_D_.Error("CheckTime Now = %ld, secondStart = %ld, secondNext = %ld \n",Now, secondStart, secondNext);
   if( Now >= secondStart && Now < secondNext )
   {
      percent = 100*(Now-secondStart)/(secondNext - secondStart);
      return percent;
   }

#else

   //_D_.Error("CheckTime Now = %ld, secondStart = %ld, secondNext = %ld \n",Now, secondStart, secondStart + durationInSecond);
   if( Now >= secondStart && Now < (secondStart + durationInSecond) )
   {
      percent = 100*(Now - secondStart)/(durationInSecond);
   }

#endif

   return percent;
}

//////////////////////////////////////////////////////////////////////////////////////
// function      :   compareTime
// params      :   DayTime_dtv_t *time1, DayTime_dtv_t *time2
// result      :    = -1 if time1 < time2; 1 if time1 > time2; 0 if time1 == time2
//////////////////////////////////////////////////////////////////////////////////////
int compareTime(DayTime_dtv_t* time1, DayTime_dtv_t* time2)
{
   if( time1->ymdw.year > time2->ymdw.year )
   {
      return 1;
   }

   if( time1->ymdw.month > time2->ymdw.month )
   {
      return 1;
   }

   if( time1->ymdw.day> time2->ymdw.day )
   {
      return 1;
   }

   if( time1->hms.hour > time2->hms.hour )
   {
      return 1;
   }

   if( time1->hms.minute > time2->hms.minute )
   {
      return 1;
   }
   //if(time1->hms.second > time2->hms.second) return 1; //trungkt comment

   if( time1->ymdw.year < time2->ymdw.year )
   {
      return -1;
   }

   if( time1->ymdw.month < time2->ymdw.month )
   {
      return -1;
   }

   if( time1->ymdw.day < time2->ymdw.day )
   {
      return -1;
   }

   if( time1->hms.hour < time2->hms.hour )
   {
      return -1;
   }

   if( time1->hms.minute < time2->hms.minute )
   {
      return -1;
   }

   //if(time1->hms.second < time2->hms.second) return -1;

   return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
int compareTime2(DayTime_dtv_t time1, DayTime_dtv_t time2)
{
   if( time1.ymdw.year > time2.ymdw.year )
   {
      return 1;
   }

   if( time1.ymdw.year < time2.ymdw.year )
   {
      return -1;
   }

   if( time1.ymdw.month > time2.ymdw.month )
   {
      return 1;
   }

   if( time1.ymdw.month < time2.ymdw.month )
   {
      return -1;
   }

   if( time1.ymdw.day> time2.ymdw.day )
   {
      return 1;
   }

   if( time1.ymdw.day < time2.ymdw.day )
   {
      return -1;
   }

   if( time1.hms.hour > time2.hms.hour )
   {
      return 1;
   }

   if( time1.hms.hour < time2.hms.hour )
   {
      return -1;
   }

   if( time1.hms.minute > time2.hms.minute )
   {
      return 1;
   }

   if( time1.hms.minute < time2.hms.minute )
   {
      return -1;
   }
   //if(time1.hms.second > time2.hms.second) return 1;
   //if(time1.hms.second < time2.hms.second) return -1;

   return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
long getDurationInSec(DayTime_dtv_t* startTime, DayTime_dtv_t* endTime)
{
   //27-02-2014 trungkt comment  + (endTime->hms.second - startTime->hms.second);
   long seconds = (endTime->hms.hour - startTime->hms.hour) * 3600 + (endTime->hms.minute - startTime->hms.minute) * 60;
   return seconds;
}

//////////////////////////////////////////////////////////////////////////////////////
//27-02-2014 trungkt comment  + (endTime->hms.second - startTime->hms.second);
time_t hms2Sec(HourMinuteSecond_dtv_t val)
{
   time_t seconds = val.hour * 3600 + val.minute * 60;//27-02-2014 trungkt comment + val.second;
   return seconds;
}

//////////////////////////////////////////////////////////////////////////////////////
DayTime_dtv_t getNextDayTime(DayTime_dtv_t curTime)
{
   long curTimeToSec = DayTime2Second(&curTime) + 24*3600;
   DayTime_dtv_t result;
   Second2DayTime(curTimeToSec, &result);

   return result;
}

//////////////////////////////////////////////////////////////////////////////////////
U8 getNextWeekDay(U8 curWeekDay)
{
   U8 nextWeekDay = curWeekDay + 1;
   if( nextWeekDay > 7 )
   {
      nextWeekDay = 1;
   }
   return nextWeekDay;
}

//////////////////////////////////////////////////////////////////////////////////////
void initTimeZone()
{
   _D_.Error("initTimeZone() \n");

   if( vTimeZone.size() > 0 )
   {
      vTimeZone.clear();
   }

   for( int i = 0; i < sizeof(arrTimeZone)/sizeof(TimeZone_t); i++ )
   {
      _D_.Error("push_back %d \n",i);
      vTimeZone.push_back(arrTimeZone[i]);
   }

   _D_.Error("size = %d \n", vTimeZone.size());
}

//////////////////////////////////////////////////////////////////////////////////////
std::vector<TimeZone_t> getVectorTimeZone()
{
   return vTimeZone;
}

//////////////////////////////////////////////////////////////////////////////////////
TimeZone_t  getTimeZone(U8 index)
{
   TimeZone_t _timeZone;
   memset(&_timeZone,0,sizeof(TimeZone_t));

   if( vTimeZone.size() > index )
   {
      return vTimeZone[index];
   }

   return _timeZone;
}

//////////////////////////////////////////////////////////////////////////////////////
DayTime_dtv_t* cs_gmtime_r(const time_t* timep, DayTime_dtv_t* r)
{
   const int daysPerMonth[13] =
   {    0,
      (31),
      (31+28),
      (31+28+31),
      (31+28+31+30),
      (31+28+31+30+31),
      (31+28+31+30+31+30),
      (31+28+31+30+31+30+31),
      (31+28+31+30+31+30+31+31),
      (31+28+31+30+31+30+31+31+30),
      (31+28+31+30+31+30+31+31+30+31),
      (31+28+31+30+31+30+31+31+30+31+30),
      (31+28+31+30+31+30+31+31+30+31+30+31),
   };

   time_t i;
   time_t work      = *timep%86400;
   r->hms.second    = work%60; work/=60;
   r->hms.minute   = work%60;
   r->hms.hour      = work/60;
   work         = *timep/86400;
   r->ymdw.week_day= (4+work)%7 + 1;

   for( i = 1970; ; ++i )
   {
      time_t k = is_leap(i)?366:365;
      if( work >= k )
      {
         work-=k;
      }
      else
      {
         break;
      }
   }

   r->ymdw.year     = i-1900;
   r->ymdw.year_day = work;

   r->ymdw.day       = 1;

   if( is_leap(i) && (work>58) )
   {
      if( work == 59 )
      {
         r->ymdw.day=2; /* 29.2. */
      }
      work-=1;
   }

   for( i = 11; i && (daysPerMonth[i]>work); --i ) ;

   r->ymdw.month = i;
   r->ymdw.day += work-daysPerMonth[i];

   return r;
}

//////////////////////////////////////////////////////////////////////////////////////
// Checks if year is a leap year. If so, 1 is returned, else 0.
static int is_leap(unsigned y)
{
   return (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0);
}

//////////////////////////////////////////////////////////////////////////////////////
void convertBMPSize(int maxWidth, int maxHeight, int& width, int& height, int& posX, int& posY)
{
   if( (width > maxWidth) || (height > maxHeight) )
   {
      int ratioW = (int)(maxWidth * 100 / width);
      int ratioH = (int)(maxHeight * 100 / height);

      int ratio = (ratioW > ratioH) ? ratioH : ratioW;

      width = width * ratio / 100;
      height = height * ratio / 100;
   }

   posX += (maxWidth - width) / 2;
   posY += (maxHeight - height) / 2;
}

//////////////////////////////////////////////////////////////////////////////////////
int convertRemote(U32 key)
{
   switch( key )
   {
      case KEY_RC_0:
         return '0';
         break;

      case KEY_RC_1:
         return '1';
         break;

      case KEY_RC_2:
         return '2';
         break;

      case KEY_RC_3:
         return '3';
         break;

      case KEY_RC_4:
         return '4';
         break;

      case KEY_RC_5:
         return '5';
         break;

      case KEY_RC_6:
         return '6';
         break;

      case KEY_RC_7:
         return '7';
         break;

      case KEY_RC_8:
         return '8';
         break;

      case KEY_RC_9:
         return '9';
         break;

      default:
         return key;
         break;
   }
}

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
