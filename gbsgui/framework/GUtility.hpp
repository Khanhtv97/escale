//////////////////////////////////////////////////////////////////////////////////////
// Copyright (c) GBS 2012 All Right Reserved, http://gbs-jsc.com.                   //
// This source is subject to the GBS Permissive License. Please see the License.txt //
// file for more information.                                                       //
//                                                                                  //
// file: GUtility.hpp                                                               //
//////////////////////////////////////////////////////////////////////////////////////

#ifndef GUTILITY_HPP_
#define GUTILITY_HPP_

#include "GDebug.hpp"
#include "GObject.hpp"
#include "GEvents.hpp"
#include "GError.hpp"
#include <vector>

//////////////////////////////////////////////////////////////////////////////////////
// Time
//////////////////////////////////////////////////////////////////////////////////////
#define MAX_TIME_ZONE  74
#define TIME_ZONE_ADD   0
#define TIME_ZONE_MINUS 1
#define MAXBUFSIZE   1024

typedef struct
{
   U16 day;       // [1, 31] , the day of the month, in the range 1 to 31
   U16 month;     //*[1, 12] , the number of months since january, in the range 1 to 12.
   U16 year;      // [1900, ..] , the number of years, since 1900.
   U8  week_day;  // [0, 6] = [SUN, SAT] ,  the number of week days since sun day, in the range 0 to 6.
   U16 year_day;  /* Days in year.[0-365]*/
}
YearMontDayWeek_dtv_t;

typedef struct
{
   //U8 day;
   U8 hour;   // [0, 24) the number of hours, in the range 0 to 23.
   U8 minute; // [0, 60) the number of minutes, in the range 0 to 59.
   U8 second; // [0, 60) the number of seconds, in the range 0 to 59.
}
HourMinuteSecond_dtv_t;

typedef struct
{
   YearMontDayWeek_dtv_t ymdw;
   HourMinuteSecond_dtv_t hms;
}
DayTime_dtv_t;

typedef struct
{
   U8 hour;
   U8 minute;
   U8 status;// ADD or MINUS
   U8 CountryCode[3];
}
TimeZone_dtv_t;

typedef struct
{
   TimeZone_dtv_t time;
   char           countryName[30];
}
TimeZone_t;

static std::vector<TimeZone_t> vTimeZone;
typedef long int time_t;

//namespace GBS {
//namespace STM {
//namespace Framework {

//////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
GError_t ChangeDayTimeZone(DayTime_dtv_t& dt1, TimeZone_dtv_t* stTzone, DayTime_dtv_t* dt2);
U16 GetNumDayOfMonth(U16 uiYear, U16 uiMonth);
U8  GetDateFromYearMonth(U16 Year, U8 Month, U8 Day);
U16 GetDaysFromYearMonth(U16 Year, U8 Month, U8 Day);
U32 GetNumSecondOfYear(U16 uiYear);
U32 GetNumSecondOfMonth(U16 uiYear, U16 uiMonth);

GError_t GetNextDTVTime(DayTime_dtv_t* startTime, HourMinuteSecond_dtv_t* Duration, DayTime_dtv_t* nextTime);
//GError_t TOT2DTVTime(STTAPI_TOT_t* TOT_time, DayTime_dtv_t* DVT_time);

GError_t Second2DayTime(U32 uiSecond, DayTime_dtv_t* pDt);
time_t DayTime2Second(DayTime_dtv_t* DayTime);

int CheckTime(time_t Now, DayTime_dtv_t* startTime, HourMinuteSecond_dtv_t* Duration);
BOOL checkValidTime(YearMontDayWeek_dtv_t dayTime);
void initTimeZone();
std::vector<TimeZone_t> getVectorTimeZone();
TimeZone_t getTimeZone(U8 index);


int compareTime(DayTime_dtv_t* time1, DayTime_dtv_t* time2);
int compareTime2(DayTime_dtv_t time1, DayTime_dtv_t time2);
long getDurationInSec(DayTime_dtv_t* startTime, DayTime_dtv_t* endTime);
time_t hms2Sec(HourMinuteSecond_dtv_t val);
DayTime_dtv_t getNextDayTime(DayTime_dtv_t curTime);
U8 getNextWeekDay(U8 curWeekDay);
int convertRemote(U32 key); // convert ma key number

void convertBMPSize(int maxWidth, int maxHeight, int& width, int& height, int& posX, int& posY);

DayTime_dtv_t* cs_gmtime_r(const time_t* timep, DayTime_dtv_t* r);
static int is_leap(unsigned y);
} // end extern "C"

//////////////////////////////////////////////////////////////////////////////////////
//} //Framework
//} //STM
//} //GBS
#endif /* GUTILITY_HPP_ */
