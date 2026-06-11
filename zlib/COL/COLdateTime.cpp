//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLdateTime implementation
//
// Description:
//
// COLdateTime implementation
//
// Author: Todd Martin
// Date:   Fri 11/06/1998
//
//---------------------------------------------------------------------------

//TODO change all simple types!
//the naming conventions are all wrong...need to unit
//this baby first though to make sure we don't break anything!

#include <math.h>
#include "COLhashmap.h"

#ifdef _WIN32
#  define CHM_FILETIME
#endif

#include "COLmutex.h"
#include "COLlocker.h"
#include "COLlog.h"
COL_LOG_MODULE;

#include <stdio.h>

#ifdef _WIN32
#   include <windows.h>
#   include <process.h>
#else
#   include <sys/select.h>
#   include <sys/time.h>
#   include <unistd.h>
#   include <sched.h>
#endif

#include "COLdateTime.h"

#ifdef _WIN32
static inline void FileTimeToUnixTime(COLuint64* result, const FILETIME* in) {
   const COLuint64 DELTA_EPOCH_IN_USEC = 11644473600000000 /*LL*/;
   COLuint64 v = in->dwHighDateTime;
   v = v << 32;
   v |= in->dwLowDateTime;
   v /= 10;   // convert from 100 nanosecond periods to microseconds
   v -= DELTA_EPOCH_IN_USEC;  // convert to Unix epoch
   *result = v;
}
#endif

// #11252 get a millisecond resultion timestamp consistent with COLdateTime/time(0)/localtime().
// Do not use GetLocalTime().
void COLcurrentTimeStamp(COLtimeStamp* out) {
#ifdef _WIN32
   FILETIME ftime;
# ifndef _WIN32_WCE
   GetSystemTimeAsFileTime(&ftime);
# else
   SYSTEMTIME st;
   GetSystemTime(&st);
   SystemTimeToFileTime(&st, &ftime);
# endif
   COLuint64 now = 0;
   FileTimeToUnixTime(&now, &ftime);
   out->tv_sec  = now / 1000000;
   out->tv_usec = now % 1000000;
#else
   // UNIX
   struct timeval tv;
   ::gettimeofday(&tv, 0);
   out->tv_sec  = tv.tv_sec;
   out->tv_usec = tv.tv_usec;
#endif
}

void COLtimeStampToComponents (
   const COLtimeStamp& TimeStamp,
   COLuint32& Year,
   COLuint32& Month,
   COLuint32& Day,
   COLuint32& Hour,
   COLuint32& Min,
   COLuint32& Sec,
   COLuint32& Ms
)
{
   time_t TimeT = TimeStamp.asTimeT();
   struct tm tm;
#ifdef _WIN32
   tm = *::localtime(&TimeT); // threadsafe on Windows: uses thread local storage
#else
   localtime_r(&TimeT, &tm);
#endif
   Year  = tm.tm_year + 1900;
   Month = tm.tm_mon + 1;
   Day   = tm.tm_mday;
   Hour  = tm.tm_hour;
   Min   = tm.tm_min;
   Sec   = tm.tm_sec;
   Ms    = TimeStamp.milliseconds();
}

const COLstring COLdateTime::DAY_OF_WEEK_AS_STRING[]
= {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
   "Saturday"};

const COLstring COLdateTime::DAY_OF_WEEK_AS_STRING_SHORT[]
= {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const COLstring COLdateTime::MONTH_OF_YEAR_AS_STRING[]
= {"January", "February", "March", "April", "May", "June", "July", "August",
   "September", "October", "November", "December"};

const COLstring COLdateTime::MONTH_OF_YEAR_AS_STRING_SHORT[]
= {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov",
   "Dec"};

// This doesn't necessarily belong in here.  Could be in a more general class or
// even library called NUMBER_AS_STRING.  Maybe one for formatting english, like
// ENG.  And if you wanted to kill an ant, you can lob a grenade.
const COLstring COLdateTime::DAY_OF_MONTH_AS_STRING[]
= {"1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th", "9th", "10th",
   "11th", "12th", "13th", "14th", "15th", "16th", "17th", "18th", "19th",
   "20th", "21st", "22nd", "23rd", "24th", "25th", "26th", "27th", "28th",
   "29th", "30th", "31st"};

const COLstring COLdateTime::MERIDIAN_AS_STRING[] = {"AM", "PM"};
const COLstring COLdateTime::MERIDIAN_AS_STRING_FORMAL[] = {"a.m.", "p.m."};

const COLstring COLdateTime::EMPTY_STRING;

COLostream& operator<<(COLostream& Stream, struct tm TimeStruct) {
   Stream << COL_T("Date: ") << TimeStruct.tm_mday << '/' << TimeStruct.tm_mon + 1 << '/' << TimeStruct.tm_year;
   Stream << COL_T(" Time = ") << TimeStruct.tm_hour  << ':' << TimeStruct.tm_min << ':' << TimeStruct.tm_sec << newline;
   if (TimeStruct.tm_isdst > 0) {
      Stream << COL_T("Daylight savings ON");
   } else if (TimeStruct.tm_isdst == 0) {
      Stream << COL_T("Daylight savings OFF");
   } else {
      Stream << COL_T("Daylight savings UNKNOWN");
   }

   Stream << COL_T(" Day of year = ") << TimeStruct.tm_yday;
   Stream << COL_T(" Day of week = ") << TimeStruct.tm_wday;
   return Stream;
}

static COLhashmap<COLstring,COLint32> ShortMonthToMonthIntMap;

void COLdateTime::initShortMonthLookup() {
   //we have to rebuild the map if it doesn't exist. this part is a bit
   //sketchy, because it depends on the locale.
   if (ShortMonthToMonthIntMap.size() == 0) {
      char Buffer[100];
      for (COLuint32 MonthIndex = 1; MonthIndex <= 12; MonthIndex++) {
         //we use strftime to extract the short month representation
         tm TempTime;
         ::memset(&TempTime,0,sizeof(TempTime));
         TempTime.tm_mon = MonthIndex;
         TempTime.tm_mon--; //0 based
         ::memset(Buffer,0,sizeof(Buffer));
         ::strftime(Buffer,sizeof(Buffer),"%b",&TempTime);
         COLPRECONDITION( strlen(Buffer) == 3); //3 char representation expected
         ShortMonthToMonthIntMap.add(Buffer,MonthIndex);
      }
   }
}


COLint32 COLdateTime::monthFromShortName(const COLstring& ShortMonthName) {
   initShortMonthLookup();
   COLhashmapPlace Place = ShortMonthToMonthIntMap.find(ShortMonthName);
   if (Place) {
      return ShortMonthToMonthIntMap[Place];
   } else {
      return -1;
   }
}

const COLstring& COLdateTime::shortNameFromMonth(COLint32 Month) {
   static const COLstring NullString;
   if (Month < 1 || Month > 12) {
      return NullString;
   }
   initShortMonthLookup();
   for (COLhashmapPlace Place = ShortMonthToMonthIntMap.first(); Place != NULL; Place = ShortMonthToMonthIntMap.next(Place)) {
      if (ShortMonthToMonthIntMap[Place] == Month) {
         return ShortMonthToMonthIntMap.key(Place);
      }
   }
   //this should not be reached really.
   return NullString;
}


class COLdateTimePrivate {
public:
   COLdateTimePrivate() : OffsetInMinutes(0), OffsetIsDefined(false), dt(0), status(COLdateTime::null), SecondFraction(0) {}
   ~COLdateTimePrivate() {}

   //TODO change dt to something meaningful, IE -> Date
   DATE                        dt;
   COLdateTime::DateTimeStatus status;
   double                      SecondFraction;

   bool                  OffsetIsDefined;
   short                       OffsetInMinutes;
};

/////////////////////////////////////////////////////////////////////////////
// COLdateTime class HELPER definitions

#define AFX_OLE_DATETIME_HALFSECOND (1.0 / (2.0 * (60.0 * 60.0 * 24.0)))

// Verifies will fail if the needed buffer size is too large
#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999

// Half a second, expressed in days
#define HALF_SECOND  (1.0/172800.0)

// One-based array of days in year at month start
static int _afxMonthDays[13] =
   {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

/////////////////////////////////////////////////////////////////////////////
// CDateTime class HELPERS - implementation
//TODO change wYear to iYear, etc
//TODO move input parameters to new lines
bool  _COLOleDateFromTm(int wYear, int wMonth, int wDay,
   int wHour, int wMinute, int wSecond, DATE& dtDest)
{
   // Validate year and month (ignore day of week and milliseconds)
   if (wYear > 9999 || wMonth < 1 || wMonth > 12)
      return false;

   //  Check for leap year and set the number of days in the month
   bool bLeapYear = ((wYear & 3) == 0) &&
      ((wYear % 100) != 0 || (wYear % 400) == 0);

   int nDaysInMonth =
      _afxMonthDays[wMonth] - _afxMonthDays[wMonth-1] +
      ((bLeapYear && wDay == 29 && wMonth == 2) ? 1 : 0);

   // Finish validating the date
   if (wDay < 1 || wDay > nDaysInMonth ||
      wHour > 23 || wMinute > 59 ||
      wSecond > 59)
   {
      return false;
   }

   // Cache the date in days and time in fractional days
   long nDate;
   double dblTime;

   //It is a valid date; make Jan 1, 1AD be 1
   nDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
      _afxMonthDays[wMonth-1] + wDay;

   //  If leap year and it's before March, subtract 1:
   if (wMonth <= 2 && bLeapYear)
      --nDate;

   //  Offset so that 12/30/1899 is 0
   nDate -= 693959L;

   dblTime = (((long)wHour * 3600L) +  // hrs in seconds
      ((long)wMinute * 60L) +  // mins in seconds
      ((long)wSecond)) / 86400.;

   dtDest = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

   return true;
}
//TODO change Tm to Time. Worst saving of two letters ever.
// although Tm == struct tm
// I don't think this routine takes account of daylight savings
bool _COLTmFromOleDate(DATE dtSrc, struct tm& tmDest) {
   // The legal range does not actually span year 0 to 9999.
   if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
      return false;

   long nDays;             // Number of days since Dec. 30, 1899
   long nDaysAbsolute;     // Number of days since 1/1/0
   long nSecsInDay;        // Time in seconds since midnight
   long nMinutesInDay;     // Minutes in day

   long n400Years;         // Number of 400 year increments since 1/1/0
   long n400Century;       // Century within 400 year block (0,1,2 or 3)
   long n4Years;           // Number of 4 year increments since 1/1/0
   long n4Day;             // Day within 4 year block
                            //  (0 is 1/1/yr1, 1460 is 12/31/yr4)
   long n4Yr;              // Year within 4 year block (0,1,2 or 3)
   bool bLeap4 = true;     // true if 4 year block includes leap year

   double dblDate = dtSrc; // tempory serial date

   // If a valid date, then this conversion should not overflow
   nDays = (long)dblDate;

   // Round to the second
   dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);

   nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

   dblDate = fabs(dblDate);
   nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);

   // Calculate the day of week (sun=1, mon=2...)
   //   -1 because 1/1/0 is Sat.  +1 because we want 1-based
   tmDest.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;

   // Leap years every 4 yrs except centuries not multiples of 400.
   n400Years = (long)(nDaysAbsolute / 146097L);

   // set nDaysAbsolute to day within 400-year block
   nDaysAbsolute %= 146097L;

   // -1 because first century has extra day
   n400Century = (long)((nDaysAbsolute - 1) / 36524L);

   // Non-leap century
   if (n400Century != 0) {
      // set nDaysAbsolute to day within century
      nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

      // +1 because 1st 4 year increment has 1460 days
      n4Years = (long)((nDaysAbsolute + 1) / 1461L);

      if (n4Years != 0)
         n4Day = (long)((nDaysAbsolute + 1) % 1461L);
      else {
         bLeap4 = false;
         n4Day = (long)nDaysAbsolute;
      }
   } else {
      // Leap century - not special case!
      n4Years = (long)(nDaysAbsolute / 1461L);
      n4Day = (long)(nDaysAbsolute % 1461L);
   }
   if (bLeap4) {
      // -1 because first year has 366 days
      n4Yr = (n4Day - 1) / 365;

      if (n4Yr != 0)
         n4Day = (n4Day - 1) % 365;
   } else {
      n4Yr = n4Day / 365;
      n4Day %= 365;
   }

   // n4Day is now 0-based day of year. Save 1-based day of year, year number
   tmDest.tm_yday = (int)n4Day + 1;
   tmDest.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

   // Handle leap year: before, on, and after Feb. 29.
   if (n4Yr == 0 && bLeap4) {
      // Leap Year
      if (n4Day == 59) {
         /* Feb. 29 */
         tmDest.tm_mon = 2;
         tmDest.tm_mday = 29;
         goto DoTime;
      }

      // Pretend it's not a leap year for month/day comp.
      if (n4Day >= 60)
         --n4Day;
   }

   // Make n4DaY a 1-based day of non-leap year and compute
   //  month/day for everything but Feb. 29.
   ++n4Day;

   // Month number always >= n/32, so save some loop time */
   for (tmDest.tm_mon = (n4Day >> 5) + 1;
      n4Day > _afxMonthDays[tmDest.tm_mon]; tmDest.tm_mon++);

   tmDest.tm_mday = (int)(n4Day - _afxMonthDays[tmDest.tm_mon-1]);

DoTime:
   if (nSecsInDay == 0)
      tmDest.tm_hour = tmDest.tm_min = tmDest.tm_sec = 0;
   else {
      tmDest.tm_sec = (int)nSecsInDay % 60L;
      nMinutesInDay = nSecsInDay / 60L;
      tmDest.tm_min = (int)nMinutesInDay % 60;
      tmDest.tm_hour = (int)nMinutesInDay / 60;
   }

   return true;
}

extern void  _COLTmConvertToStandardFormat(struct tm& tmSrc) {
   // Convert afx internal tm to format expected by runtimes (_tcsftime, etc)
   tmSrc.tm_year -= 1900;  // year is based on 1900
   tmSrc.tm_mon -= 1;      // month of year is 0-based
   tmSrc.tm_wday -= 1;     // day of week is 0-based
   tmSrc.tm_yday -= 1;     // day of year is 0-based
   tmSrc.tm_isdst = -1;    // libc will determine timezone
}

extern double  _COLDoubleFromDate(DATE dt) {
   // No problem if positive
   if (dt >= 0)
      return dt;

   // If negative, must convert since negative dates not continuous
   // (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
   double temp = ceil(dt);
   return temp - (dt - temp);
}

extern DATE  _COLDateFromDouble(double dbl) {
   // No problem if positive
   if (dbl >= 0)
      return dbl;

   // If negative, must convert since negative dates not continuous
   // (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
   double temp = floor(dbl); // dbl is now whole part
   return temp + (temp - dbl);
}

/////////////////////////////////////////////////////////////////////////////
// COLdateTime class

COLdateTime COLdateTime::currentTime() {
   const time_t NowTimeT = ::time(NULL);

   // Cache the expensive currentTime COLdateTime(::time(NULL)) calculation.
   // Put these static variables in this method to avoid static
   // initialization order problems.
   static COLmutex s_CachedCurrentTimeMutex;
   COLlocker Lock(s_CachedCurrentTimeMutex);

   static time_t             s_CachedCurrentTimeT;
   static COLdateTime        s_CachedCurrentTime;

   if (s_CachedCurrentTimeT != NowTimeT) {
      // COLdateTime::currentTime does not use a fractional second.
      s_CachedCurrentTimeT = NowTimeT;
      s_CachedCurrentTime = COLdateTime(NowTimeT);
   }
   return s_CachedCurrentTime;
}

COLdateTime COLdateTime::currentDate() {
   COLdateTime Now = currentTime();
   return COLdateTime(Now.year(), Now.month(), Now.day(), 0, 0, 0);
}

COLdateTime::COLdateTime() : pMember(NULL) {
   pMember = new COLdateTimePrivate;
}

COLdateTime::~COLdateTime() {
   delete pMember;
}

COLdateTime::COLdateTime(const COLdateTime& Orig) : pMember(NULL) {
   pMember = new COLdateTimePrivate;
   operator=(Orig);
}

COLdateTime::COLdateTime(DATE dtSrc) : pMember(NULL) {
   pMember = new COLdateTimePrivate;
   operator=(dtSrc);
}

COLdateTime::COLdateTime(time_t timeSrc) : pMember(NULL) {
   pMember = new COLdateTimePrivate;
   operator=(timeSrc);
}

COLdateTime::COLdateTime ( int Year, int Month, int Day, int Hour, int Minite, int Second) {
   pMember = new COLdateTimePrivate();

   _COLOleDateFromTm(Year,Month, Day, Hour, Minite, Second, pMember->dt);
   setStatus(valid);
}

#ifdef CHM_FILETIME
COLdateTime::COLdateTime(const FILETIME& FileTimeStamp) {
   pMember = new COLdateTimePrivate();
   operator=(FileTimeStamp);
}
#endif

bool COLdateTime::isNull() const {
   return status() == COLdateTime::null;
}

void COLdateTime::setNull() {
   operator=(COLdateTime());
   COLPOSTCONDITION(status() == null);
}

int COLdateTime::year() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_year;
   } else {
      COL_ERROR_STREAM("Date time error while extracting year.", COLerror::InvalidDateTime);
      return -1;
   }
}

int COLdateTime::month() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_mon;
   } else {
      COL_ERROR_STREAM("Date time error while extracting month.", COLerror::InvalidDateTime);
      return -1;
   }
}

int COLdateTime::day() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_mday;
   } else {
      COL_ERROR_STREAM("Date time error while extracting day.", COLerror::InvalidDateTime);
      return -1;
   }
}

int COLdateTime::hour() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_hour;
   } else {
      COL_ERROR_STREAM("Date time error while extracting hour.", COLerror::InvalidDateTime);
      return -1;
   }
}

int COLdateTime::minute() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_min;
   } else {
      COL_ERROR_STREAM("Date time error while extracting minute.", COLerror::InvalidDateTime);
      return -1;
   }
}

int COLdateTime::second() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));


   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_sec;
   } else {
      COL_ERROR_STREAM("Date time error while extracting second.", COLerror::InvalidDateTime);
      return -1;
   }
}

COLint16 COLdateTime::weekOfYear() const {
   COLint16 WeekOfYear;
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if(status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      COLdateTime StartOfYear(year(), 1, 1, 0, 0, 0);

      WeekOfYear = (COLuint16)floor(((*this - StartOfYear).totalDays()
                                    + StartOfYear.dayOfWeek() - 1)
                                    / 7);
   } else {
      WeekOfYear = -1;
      COL_ERROR_STREAM("Date time error while extracting day of week.", COLerror::InvalidDateTime);
   }
   return WeekOfYear;
}

int COLdateTime::dayOfWeek() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_wday;
   } else {
      COL_ERROR_STREAM("Date time error while extracting day of week.", COLerror::InvalidDateTime);
      return -1;
   }
}

int COLdateTime::dayOfYear() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));

   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      return tmTemp.tm_yday;
   } else {
      COL_ERROR_STREAM("Date time error while extracting day of year.", COLerror::InvalidDateTime);
      return -1;
   }
}

time_t COLdateTime::asTimeT() const {
   struct tm tmTemp;
   memset(&tmTemp, 0, sizeof(tmTemp));


   if (status() == valid && _COLTmFromOleDate(pMember->dt, tmTemp)) {
      COL_TRC(COL_T("asTimeT ") << tmTemp);
      // Convert tm from internal format to standard format
      _COLTmConvertToStandardFormat( tmTemp );
      return ::mktime(&tmTemp);
   }
   else if (status() == null) {
      return -1;
   } else {
      COL_ERROR_STREAM("Date time error while extracting day of year.", COLerror::InvalidDateTime);
      return -1;
   }
}

// places 'Sun', 'Mon', Tue.. etc into the Week parameter
void COLdateTime::dayOfWeekString(COLstring& Week) const {

   switch (dayOfWeek()) {
      case 1: Week = "Sun"; break;
      case 2: Week = "Mon"; break;
      case 3: Week = "Tue"; break;
      case 4: Week = "Wed"; break;
      case 5: Week = "Thu"; break;
      case 6: Week = "Fri"; break;
      case 7: Week = "Sat"; break;
   }

}

// places 'Jan', 'Feb'.. etc into the Month parameter
void COLdateTime::monthString(COLstring& Month) const {

   switch (month()) {
      case 1: Month = "Jan"; break;
      case 2: Month = "Feb"; break;
      case 3: Month = "Mar"; break;
      case 4: Month = "Apr"; break;
      case 5: Month = "May"; break;
      case 6: Month = "Jun"; break;
      case 7: Month = "Jul"; break;
      case 8: Month = "Aug"; break;
      case 9: Month = "Sep"; break;
      case 10: Month = "Oct"; break;
      case 11: Month = "Nov"; break;
      case 12: Month = "Dec"; break;
   }

}

const COLstring& COLdateTime::dayOfWeekString(bool IsLong) const {
   return dayOfWeekString(dayOfWeek(), IsLong);
}

const COLstring& COLdateTime::dayOfWeekString(COLuint16 DayOfWeekNumber, bool IsLong) {
   DayOfWeekNumber--;

   if (DayOfWeekNumber > 6) {
      return COLdateTime::EMPTY_STRING;
   } else {
      if (IsLong) {
         return DAY_OF_WEEK_AS_STRING[DayOfWeekNumber];
      } else {
         return DAY_OF_WEEK_AS_STRING_SHORT[DayOfWeekNumber];
      }
   }
}

const COLstring& COLdateTime::monthString(bool IsLong) const {
   return monthNumberToString(month(), IsLong);
}

const COLstring& COLdateTime::monthNumberToString(COLuint16 MonthNumber, bool IsLong) {
   MonthNumber--;

   if (MonthNumber > 11) {
      return COLdateTime::EMPTY_STRING;
   } else {
      if (IsLong) {
         return MONTH_OF_YEAR_AS_STRING[MonthNumber];
      } else {
         return MONTH_OF_YEAR_AS_STRING_SHORT[MonthNumber];
      }
   }
}

const COLstring& COLdateTime::dayOfMonthString() const {
   return dayOfMonthString(day());
}

const COLstring& COLdateTime::dayOfMonthString(COLuint16 DayOfMonth) {
   DayOfMonth--;

   if (DayOfMonth > 30) {
      return COLdateTime::EMPTY_STRING;
   } else {
      return DAY_OF_MONTH_AS_STRING[DayOfMonth];
   }
}

const COLstring& COLdateTime::meridianString(bool IsFormal) const {
   return meridianString(hour(), IsFormal);
}

const COLstring& COLdateTime::meridianString(COLuint16 Hour,
                                             bool IsFormal) {
   if (Hour > 23) {
      return COLdateTime::EMPTY_STRING;
   } else {
      if (Hour < 12) {
	      Hour = 0;
      } else {
         Hour = 1;
      }
      if (IsFormal) {
         return MERIDIAN_AS_STRING_FORMAL[Hour];
      } else {
         return MERIDIAN_AS_STRING[Hour];
      }
   }
}

bool COLdateTime::operator==(const COLdateTime& date) const { return (date.pMember->dt == pMember->dt); }

bool COLdateTime::operator!=(const COLdateTime& date) const { return !operator==(date); }

const COLdateTime& COLdateTime::operator=(DATE Orig) {
   pMember->dt = Orig;
   setStatus(valid);

   return *this;
}

const COLdateTime& COLdateTime::operator=(const time_t& timeSrc) {
   // Convert time_t to struct tm
#ifndef _WIN32
   tm TempTime = {0};
   tm *ptm = localtime_r(&timeSrc,&TempTime);
#else
   tm *ptm = localtime(&timeSrc); // ok - windows
#endif

   if (ptm != NULL) {
      pMember->status = _COLOleDateFromTm((int)(ptm->tm_year + 1900),
         (int)(ptm->tm_mon + 1), (int)ptm->tm_mday,
         (int)ptm->tm_hour, (int)ptm->tm_min,
         (int)ptm->tm_sec, pMember->dt) ? valid : invalid;
   } else {
      // Local time must have failed (timsSrc before 1/1/70 12am)
      setStatus(invalid);
      COLPRECONDITION(false);
   }

   return *this;
}

const COLdateTime& COLdateTime::operator=(const COLdateTime& Orig) {
   pMember->status = Orig.pMember->status;
   pMember->dt     = Orig.pMember->dt;
   pMember->SecondFraction = Orig.pMember->SecondFraction;
   pMember->OffsetInMinutes = Orig.pMember->OffsetInMinutes;
   pMember->OffsetIsDefined = Orig.pMember->OffsetIsDefined;
   return *this;
}

#ifdef CHM_FILETIME
const COLdateTime& COLdateTime::operator=(const FILETIME& filetimeSrc) {
   // Assume UTC FILETIME, so convert to LOCALTIME
   FILETIME filetimeLocal;
   if (!FileTimeToLocalFileTime( &filetimeSrc, &filetimeLocal)) {
      pMember->status = invalid;
   } else {
      // Take advantage of SYSTEMTIME -> FILETIME conversion
      SYSTEMTIME systime;
      pMember->status = FileTimeToSystemTime(&filetimeLocal, &systime) ?
         valid : invalid;

      // At this point systime should always be valid, but...
      if (status() == valid) {
         pMember->status = _COLOleDateFromTm(systime.wYear, systime.wMonth,
            systime.wDay, systime.wHour, systime.wMinute,
            systime.wSecond, pMember->dt) ? valid : invalid;
      }
   }

   return *this;
}
#endif

bool COLdateTime::operator<(const COLdateTime& date) const {
   if (date.isNull() && isNull()) { return false; }
   COLPRECONDITION(status() == valid);
   COLPRECONDITION(date.status() == valid);

   // Handle negative dates
   return _COLDoubleFromDate(pMember->dt) <
   _COLDoubleFromDate(date.pMember->dt);
}

bool COLdateTime::operator>(const COLdateTime& date) const {
   if (date.isNull() && isNull()) { return false; }

   COLPRECONDITION(status() == valid);
   COLPRECONDITION(date.status() == valid);

   // Handle negative dates
   return _COLDoubleFromDate(pMember->dt) >
   _COLDoubleFromDate(date.pMember->dt);
}

bool COLdateTime::operator<=(const COLdateTime& date) const {
   if (date.isNull() && isNull()) { return true; }

   COLPRECONDITION(status() == valid);
   COLPRECONDITION(date.status() == valid);

   // Handle negative dates
   return _COLDoubleFromDate(pMember->dt) <=
   _COLDoubleFromDate(date.pMember->dt);
}

bool COLdateTime::operator>=(const COLdateTime& date) const {
   if (date.isNull() && isNull()) { return true; }

   COLPRECONDITION(status() == valid);
   COLPRECONDITION(date.status() == valid);

   // Handle negative dates
   return _COLDoubleFromDate(pMember->dt) >=
   _COLDoubleFromDate(date.pMember->dt);
}

COLdateTime COLdateTime::operator+(const COLdateTimeSpan& dateSpan) const {
   COLdateTime dateResult;    // Initializes pMember->status to valid

   // If either operand NULL, result NULL
   if (status() == null || dateSpan.status() == COLdateTimeSpan::null) {
      dateResult.setStatus(null);
      return dateResult;
   }

   // If either operand invalid, result invalid
   if (status() == invalid || dateSpan.status() == COLdateTimeSpan::invalid) {
      dateResult.setStatus(invalid);
      return dateResult;
   }

   // Compute the actual date difference by adding underlying dates
   dateResult = _COLDateFromDouble(_COLDoubleFromDate(pMember->dt) + dateSpan.m_span);

   // Validate within range
   dateResult.CheckRange();

   return dateResult;
}

const COLdateTime& COLdateTime::operator+=(const COLdateTimeSpan dateSpan) {
   // If either operand NULL, result NULL
   if (status() == null || dateSpan.status() == COLdateTimeSpan::null) {
      setStatus(null);
      return *this;;
   }

   // If either operand invalid, result invalid
   if (status() == invalid || dateSpan.status() == COLdateTimeSpan::invalid) {
      setStatus(invalid);
      return *this;
   }

   // Compute the actual date difference by adding underlying dates
   *this = _COLDateFromDouble(_COLDoubleFromDate(pMember->dt) + dateSpan.m_span);

   // Validate within range
   CheckRange();

   return *this;
}

COLdateTime COLdateTime::operator-(const COLdateTimeSpan& dateSpan) const {
   COLdateTime dateResult;    // Initializes pMember->status to valid

   // If either operand NULL, result NULL
   if (status() == null || dateSpan.status() == COLdateTimeSpan::null) {
      dateResult.setStatus(null);
      return dateResult;
   }

   // If either operand invalid, result invalid
   if (status() == invalid || dateSpan.status() == COLdateTimeSpan::invalid) {
      dateResult.setStatus(invalid);
      return dateResult;
   }

   // Compute the actual date difference by subtracting underlying dates
   dateResult = _COLDateFromDouble(_COLDoubleFromDate(pMember->dt) - dateSpan.m_span);

   // Validate within range
   dateResult.CheckRange();

   return dateResult;
}

const COLdateTime& COLdateTime::operator-=(const COLdateTimeSpan dateSpan) {
   // If either operand NULL, result NULL
   if (status() == null || dateSpan.status() == COLdateTimeSpan::null) {
      setStatus(null);
      return *this;
   }

   // If either operand invalid, result invalid
   if (status() == invalid || dateSpan.status() == COLdateTimeSpan::invalid) {
      setStatus(invalid);
      return *this;
   }

   // Compute the actual date difference by subtracting underlying dates
   *this = _COLDateFromDouble(_COLDoubleFromDate(pMember->dt) - dateSpan.m_span);

   // Validate within range
   CheckRange();

   return *this;
}

COLdateTimeSpan COLdateTime::operator-(const COLdateTime& date) const {
   COLdateTimeSpan spanResult;

   // If either operand NULL, result NULL
   if (status() == null || date.status() == null) {
      spanResult.setStatus(COLdateTimeSpan::null);
      return spanResult;
   }

   // If either operand invalid, result invalid
   if (status() == invalid || date.status() == invalid) {
      spanResult.setStatus(COLdateTimeSpan::invalid);
      return spanResult;
   }

   // Return result (span can't be invalid, so don't check range)
   return _COLDoubleFromDate(pMember->dt) -
   _COLDoubleFromDate(date.pMember->dt);
}

void COLdateTime::setStatus(DateTimeStatus status) {
   pMember->status = status;
}

COLdateTime::DateTimeStatus COLdateTime::status() const {
   return pMember->status;
}

void COLdateTime::setSecondFraction(double NewValue) {
   COLPRECONDITION(NewValue < 1);
   pMember->SecondFraction = NewValue;
}

const double& COLdateTime::secondFraction() const {
   return pMember->SecondFraction;
}

void COLdateTime::setOffsetInMinutes(short NewValue, bool IsDefined) {
   pMember->OffsetInMinutes = NewValue;
   pMember->OffsetIsDefined = IsDefined;
}

short COLdateTime::offsetInMinutes() const {
   if(!pMember->OffsetIsDefined) {
      return 0;
   }
   return pMember->OffsetInMinutes;
}

bool COLdateTime::offsetIsDefined() const { return pMember->OffsetIsDefined; }

COLdateTime::operator DATE() const { return pMember->dt; }

int COLdateTime::setDateTime(int nYear, int nMonth, int nDay,
   int nHour, int nMin, int nSec) {
   return pMember->status = _COLOleDateFromTm((int)nYear, (int)nMonth,
      (int)nDay, (int)nHour, (int)nMin, (int)nSec, pMember->dt) ?
      valid : invalid;
}

int COLdateTime::setDate(int nYear, int nMonth, int nDay) {
   return setDateTime(nYear, nMonth, nDay, 0, 0, 0);
}

int COLdateTime::setTime(int nHour, int nMin, int nSec) {
   if (nHour < 0 || nHour > 23 ||
       nMin < 0 || nMin > 59 ||
       nSec < 0 || nSec > 59) {
      return pMember->status = invalid;
   }

   pMember->dt = (((long)nHour * 3600L) +  // hrs in seconds
      ((long)nMin * 60L) +  // mins in seconds
      ((long)nSec)) / 86400.;
   return pMember->status = valid;
}

int COLdateTime::setUpdatedTime(int nHour, int nMin, int nSec) {
   return setDateTime(year(), month(), day(), nHour, nMin, nSec);
}


COLstring COLformatDateTimeOffset(const COLdateTime& DateTime) {
   if(!DateTime.offsetIsDefined()) {
      return "";
   }
   char Buffer[32];
   if(DateTime.offsetInMinutes()>=0) {
      int Hours = DateTime.offsetInMinutes() / 60;
      int Minutes = DateTime.offsetInMinutes() % 60;
      snprintf(Buffer, 32, "+%02d%02d", Hours, Minutes);
   } else {
      int Hours = (-DateTime.offsetInMinutes()) / 60;
      int Minutes = (-DateTime.offsetInMinutes()) % 60;
      snprintf(Buffer, 32, "-%02d%02d", Hours, Minutes);
   }
   return Buffer;
}

COLstring COLdateTime::format(const char* pFormat) const {
   // Original behaviour.
   struct tm tmTemp = {0};
   if (status() == null) {
      return "";
   } else if (status() == invalid || !_COLTmFromOleDate(pMember->dt, tmTemp)) {
      return "Invalid Date";
   }
   _COLTmConvertToStandardFormat(tmTemp);

   // Produce the output one %-escape at a time.  This way we can skip
   // unknown sequences if strftime() returns 0 as an error (this is not
   // standard behaviour, but observed under Windows).  This code DOES
   // NOT THROW since not all implementations will return 0 on invalid
   // conversion specifications, and we cannot assume which codes will
   // be supported (so we don't validate the input ourselves).
   //
   COLstring Out;
   if(pFormat) while(*pFormat) {
      // Copy everything up to the first % from pFormat to Out.
      size_t n = strcspn(pFormat, "%");
      if(n) {
         Out.append(pFormat, n);
         pFormat += n;
         if(!*pFormat) break;  // We're done.
      }
      // Now process a conversion.  Most sequences are two chars long,
      // so we make that assumption here and fix-up as needed later.
      n = 2;
      switch(pFormat[1]) {
      case '\0':  // Degenerate case: single '%' at end of string.
         n = 1;
         /* no break */
      case '%':
         Out.append(1, '%');
         break;
      case 'Z':
         // BUG (original behaviour): %Z is supposed to be the
         // timezone abbrev., like "EST".
         /* no break */
      case 'z':
         // We have to do this ourselves since we can't pack timezone
         // information into all "struct tm" values on all platforms.
         Out += COLformatDateTimeOffset(*this);
         break;
      case 'E':
      case 'O':
         // POSIX locale extension, three characters long.
         if(pFormat[2] && pFormat[2] != '%') n = 3;
         /* no break */
      default:
         // Extract all n characters of the conversion specification,
         // and let strftime() process it in isolation.
         char fmt[8], buf[128];
         memcpy(fmt, pFormat, n);
         fmt[n] = '\0';
         size_t ok = strftime(buf, sizeof buf, fmt, &tmTemp);
         if(ok) {
            Out.append(buf, ok);
         } else {
            Out.append(fmt, n);
         }
      }
      pFormat += n;
   }
   return Out;
}

void COLdateTime::printOn(COLostream& Stream) const {
   try {
      if (isNull()) {
         Stream << "<null>";
      } else if (status() == invalid) {
         Stream << "(invalid)";
      } else {
         Stream << dayOfWeekString(true) << COL_T(", ") << monthString(true) << ' '
                << dayOfMonthString() << COL_T(", ") << year() << COL_T(" @ ");

         if((hour() % 12) <= 9) {
            if(hour() % 12 == 0) {
               Stream << COL_T("12");
            } else {
            Stream << '0' << hour() % 12;
            }
         } else {
            Stream << hour() % 12;
         }
         Stream  << ':';
         if(minute() <= 9) {
            Stream << '0';
         }
         Stream << minute() << ':';
         if(second() <= 9) {
            Stream << '0';
         }

         Stream << second() << ' ' << meridianString();
      }
   } catch(COLerror& Error) {
      Stream << "Invalid datetime (" << Error << ')';
   }
}

// TODO - using sscanf is a hack we are reduced to because Windoze doesn't
// have a strptime implementation - we either need an alternative or find a
// free - non GNU! implementation.
void COLdateTime::fromString(const COLstring& String) {
   //this is not the converse operation to printOn
   int nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0, nSec=0;
   int ReturnCode = sscanf(String.c_str(), "%02d/%02d/%04d %02d:%02d:%02d",
                          &nDay,
                          &nMonth,
                          &nYear,
                          &nHour,
                          &nMin,
                          &nSec);
   if (6 != ReturnCode) {
      COL_ERROR_STREAM_PLAIN(COL_T("Couldn't parse date '") << String << "'.  Required format is DD/MM/YYYY HH:MM:DD", COLerror::PreCondition);
   }
   setDateTime( nYear, nMonth, nDay, nHour, nMin, nSec );
}

void COLdateTime::fromStringYearFirst(const COLstring& String) {
   int nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0, nSec=0;
   int ReturnCode = sscanf(String.c_str(), "%04d-%02d-%02d %02d:%02d:%02d",
      &nYear,
      &nMonth,
      &nDay,
      &nHour,
      &nMin,
      &nSec);
   if (6 != ReturnCode) {
      COL_ERROR_STREAM_PLAIN(COL_T("Couldn't parse date '") << String << "'.  Required format is YYYY-MM-DD HH:MM:DD", COLerror::PreCondition);
   }
   setDateTime( nYear, nMonth, nDay, nHour, nMin, nSec );
}

COLostream& operator<<(COLostream& Stream, const COLdateTime& DateTime) {
   DateTime.printOn(Stream);
   return Stream;
}

void COLdateTime::CheckRange() {
   if (pMember->dt > MAX_DATE || pMember->dt < MIN_DATE) // about year 100 to about 9999
      setStatus(invalid);
}


/////////////////////////////////////////////////////////////////////////////
// COLdateTimeSpan class helpers

#define MAX_DAYS_IN_SPAN    3615897L

/////////////////////////////////////////////////////////////////////////////
// COLdateTimeSpan class

const COLstring COLdateTimeSpan::TIME_UNITS[]={"day","hour","minute","second"};

COLdateTimeSpan::COLdateTimeSpan()
 : m_span(0), m_status(valid)
{ }

COLdateTimeSpan::COLdateTimeSpan(const COLdateTimeSpan& Orig)
: m_span(Orig.m_span), m_status(Orig.m_status)
{ }

COLdateTimeSpan::COLdateTimeSpan(double Span)
: m_span(Span), m_status(valid)
{ }

COLdateTimeSpan::COLdateTimeSpan (
   long Days,
   int Hours,
   int Minutes,
   int Seconds
) : m_status(valid)
{
   m_span = Days + (Hours + ((Minutes + Seconds/ 60.0) / 60.0) ) / 24.0;
}

long COLdateTimeSpan::days() const {
   return (long)m_span;
}

long COLdateTimeSpan::hours() const {
   COLPRECONDITION(status() == valid);

   double dblTemp;

   // Truncate days and scale up
   dblTemp = modf(m_span, &dblTemp);

   long lReturns = (long)((dblTemp + AFX_OLE_DATETIME_HALFSECOND) * 24);
   if (lReturns >= 24)
      lReturns -= 24;

   return lReturns;
}

long COLdateTimeSpan::minutes() const {
   COLPRECONDITION(status() == valid);

   double dblTemp;

   // Truncate hours and scale up
   dblTemp = modf(m_span * 24, &dblTemp);

   long lReturns = (long) ((dblTemp + AFX_OLE_DATETIME_HALFSECOND) * 60);
   if (lReturns >= 60)
      lReturns -= 60;

   return lReturns;
}

long COLdateTimeSpan::seconds() const {
   COLPRECONDITION(status() == valid);

   double dblTemp;

   // Truncate minutes and scale up
   dblTemp = modf(m_span * 24 * 60, &dblTemp);

   long lReturns = (long) ((dblTemp + AFX_OLE_DATETIME_HALFSECOND) * 60);
   if (lReturns >= 60)
      lReturns -= 60;

   return lReturns;
}

const COLdateTimeSpan& COLdateTimeSpan::operator=(double dblSpanSrc) {
   m_span = dblSpanSrc;
   setStatus(valid);
   return *this;
}

const COLdateTimeSpan& COLdateTimeSpan::operator=(const COLdateTimeSpan& dateSpanSrc) {
   m_span = dateSpanSrc.m_span;
   m_status = dateSpanSrc.m_status;
   return *this;
}

bool COLdateTimeSpan::operator==(const COLdateTimeSpan& Rhs) const {
   if(m_status != Rhs.m_status)   { return false; }
   if(*this < Rhs || *this > Rhs) { return false; }
   return true;
}

bool COLdateTimeSpan::operator!=(const COLdateTimeSpan& Rhs) const {
   return !(*this == Rhs);
}

bool COLdateTimeSpan::operator<(const COLdateTimeSpan& Rhs) const {
   if(m_status != Rhs.m_status || m_status == null) {
      return false;
   }
   return m_span < Rhs.m_span;
}

bool COLdateTimeSpan::operator>(const COLdateTimeSpan& Rhs) const {
   return Rhs < *this;
}

bool COLdateTimeSpan::operator<=(const COLdateTimeSpan& Rhs) const {
   return (*this < Rhs) || (*this == Rhs); // We avoid using ! due to the possibility of m_status differing
}

bool COLdateTimeSpan::operator>=(const COLdateTimeSpan& Rhs) const {
   return (Rhs <= *this);
}

COLdateTimeSpan COLdateTimeSpan::operator+(const COLdateTimeSpan& dateSpan) const {
   COLdateTimeSpan dateSpanTemp;

   // If either operand Null, result Null
   if (status() == null || dateSpan.status() == null) {
      dateSpanTemp.setStatus(null);
      return dateSpanTemp;
   }

   // If either operand Invalid, result Invalid
   if (status() == invalid || dateSpan.status() == invalid) {
      dateSpanTemp.setStatus(invalid);
      return dateSpanTemp;
   }

   // Add spans and validate within legal range
   dateSpanTemp.m_span = m_span + dateSpan.m_span;
   dateSpanTemp.CheckRange();

   return dateSpanTemp;
}

COLdateTimeSpan COLdateTimeSpan::operator-(const COLdateTimeSpan& dateSpan) const {
   COLdateTimeSpan dateSpanTemp;

   // If either operand Null, result Null
   if (status() == null || dateSpan.status() == null) {
      dateSpanTemp.setStatus(null);
      return dateSpanTemp;
   }

   // If either operand Invalid, result Invalid
   if (status() == invalid || dateSpan.status() == invalid) {
      dateSpanTemp.setStatus(invalid);
      return dateSpanTemp;
   }

   // Subtract spans and validate within legal range
   dateSpanTemp.m_span = m_span - dateSpan.m_span;
   dateSpanTemp.CheckRange();

   return dateSpanTemp;
}

const COLdateTimeSpan& COLdateTimeSpan::operator+=(const COLdateTimeSpan dateSpan) { return *this = *this + dateSpan; }

const COLdateTimeSpan& COLdateTimeSpan::operator-=(const COLdateTimeSpan dateSpan) { return *this = *this - dateSpan; }

COLdateTimeSpan COLdateTimeSpan::operator-() const {
   COLdateTimeSpan NewSpan(*this);
   if(NewSpan.status() == valid) {
      NewSpan.m_span = -NewSpan.m_span;
      NewSpan.CheckRange();
   }

   return NewSpan;
}

COLdateTimeSpan COLdateTimeSpan::operator*(double Scale) const {
   COLdateTimeSpan NewSpan(*this);
   if(NewSpan.status() == valid) {
      NewSpan.m_span *= Scale;
      NewSpan.CheckRange();
   }
   return NewSpan;
}

COLdateTimeSpan COLdateTimeSpan::operator/(double Scale) const {
   COLdateTimeSpan NewSpan(*this);
   if(NewSpan.status() == valid) {
      NewSpan.m_span /= Scale;
      NewSpan.CheckRange();
   }
   return NewSpan;
}

void COLdateTimeSpan::setDateTimeSpan(
   long lDays, int nHours, int nMins, int nSecs)
{
   // set date span by breaking into fractional days (all input ranges valid)
   m_span = lDays + ((double)nHours)/24 + ((double)nMins)/(24*60) +
      ((double)nSecs)/(24*60*60);

   setStatus(valid);
}

void COLdateTimeSpan::CheckRange() {
   if(m_span < -MAX_DAYS_IN_SPAN || m_span > MAX_DAYS_IN_SPAN)
      setStatus(invalid);
}

void COLdateTimeSpan::printOn(COLostream& Stream) const {
   long TimeArray[4] = { days(), hours(), minutes(), seconds() };
   COLuint32 TimeUnitCount = 4 - (!TimeArray[3] + !TimeArray[2] + !TimeArray[1] + !TimeArray[0]);

   for (COLuint32 TimeUnitIndex = 0; TimeUnitIndex < 4; TimeUnitIndex++) {
      if (TimeArray[TimeUnitIndex]) {
         Stream << TimeArray[TimeUnitIndex] << ' ' << COLdateTimeSpan::TIME_UNITS[TimeUnitIndex];
         if (TimeArray[TimeUnitIndex] > 1) { Stream << 's'; }
         if (TimeUnitCount > 1) {
            Stream << ' ';
            TimeUnitCount--;
         }
      }
   }
}

COLostream& operator<<(COLostream& Stream, const COLdateTimeSpan& Span) {
   Span.printOn(Stream);
   return Stream;
}

double COLdateTimeSpan::totalDays() const    { return m_span; }

double COLdateTimeSpan::totalHours() const   { return totalDays() * 24; }

double COLdateTimeSpan::totalMinutes() const { return totalHours() * 60; }

double COLdateTimeSpan::totalSeconds() const { return totalMinutes() * 60; }

// COLcompileTimeStamp functions.

// This function is non-static so that we can test COLcompileTimeStamp(void).
// It shouldn't be declared in the header file.
COLdateTime COLtimeFromString(const COLstring& String) {
   // assuming "Mth DD YYYY hh:mm:ss" format
   char MonthBuffer[100];
   ::memset(MonthBuffer,0,sizeof(MonthBuffer));
   COLint32 Day = 0;
   COLint32 Year = 0;
   COLint32 Hour = 0;
   COLint32 Minute = 0;
   COLint32 Second = 0;
   ::sscanf(String.c_str(),"%s %i %i %i:%i:%i",MonthBuffer,&Day,&Year,&Hour,&Minute,&Second);

   COLPOSTCONDITION(strlen(MonthBuffer) == 3)
   COLPOSTCONDITION(Day > 0 && Day <= 31)
   COLPOSTCONDITION(Year != 0)
   COLPOSTCONDITION(Hour >= 0 && Hour < 24)
   COLPOSTCONDITION(Minute >= 0 && Hour < 60)
   COLPOSTCONDITION(Second >= 0 && Second < 60)

   COLint32 Month = COLdateTime::monthFromShortName(MonthBuffer);
   COLPOSTCONDITION(Month != -1)

   COLdateTime Return;
   Return.setDateTime(Year,
      Month,
      Day,
      Hour,
      Minute,
      Second);
   COLPOSTCONDITION(Return.status() == COLdateTime::valid);
   return Return;
}

COLdateTime COLcompileTimeStamp() {
   COL_FUNCTION(COLcompileTimeStamp);

   // convert from __DATE__ and __TIME__
   // These macros are set when the file is compiled.
   COLstring DateAsString = static_cast<const char*>(__DATE__);
   COLstring TimeAsString = static_cast<const char*>(__TIME__);

   //concat
   DateAsString += " ";
   DateAsString += TimeAsString;

   COLdateTime CompileTimeStamp = COLtimeFromString(DateAsString);

   COL_VAR(CompileTimeStamp);
   return CompileTimeStamp;
}

COLdateTime COLcompileTimeStamp(const COLstring& TimeStamp) {
   COL_FUNCTION(COLcompileTimeStamp);
   COL_VAR(TimeStamp);

   COLint32 Year = 0;
   COLint32 Month = 0;
   COLint32 Day = 0;
   COLint32 Hour = 0;
   COLint32 Minute = 0;
   COLint32 Second = 0;

   ::sscanf(TimeStamp.c_str(), "%4d%2d%2d%2d%2d%2d", &Year, &Month, &Day, &Hour, &Minute, &Second);

   COLPOSTCONDITION(Year != 0)
   COLPOSTCONDITION(Month > 0 && Month <= 12)
   COLPOSTCONDITION(Day > 0 && Day <= 31)
   COLPOSTCONDITION(Hour >= 0 && Hour < 24)
   COLPOSTCONDITION(Minute >= 0 && Hour < 60)
   COLPOSTCONDITION(Second >= 0 && Second < 60)

   COLdateTime CompileTimeStamp(Year, Month, Day, Hour, Minute, Second);
   COLPOSTCONDITION(CompileTimeStamp.status() == COLdateTime::valid);

   COL_VAR(CompileTimeStamp);
   return CompileTimeStamp;
}

void COLtimeStamp::addMilliseconds(int Milliseconds){
   COL_METHOD(COLtimeStamp::addMilliseconds);
   COL_VAR2(tv_sec, tv_usec);
   tv_sec  += Milliseconds / 1000;
   tv_usec += (Milliseconds % 1000) * 1000;
   if (tv_usec >= 1000000){
      tv_sec += tv_usec / 1000000;
      tv_usec = tv_usec % 1000000;
   }
   COL_VAR2(tv_sec, tv_usec);
}

bool operator==(const COLtimeStamp& a, const COLtimeStamp& b){
   return ((a.tv_sec == b.tv_sec) && (a.tv_usec == b.tv_usec)) ? true : false;
}

bool operator<=(const COLtimeStamp& a, const COLtimeStamp& b){
   if (a.tv_sec < b.tv_sec){ return true; }
   else if (a.tv_sec == b.tv_sec){
      if (a.tv_usec <= b.tv_usec){ return true; }
   }
   return false;
}

bool operator<(const COLtimeStamp& Lhs, const COLtimeStamp& Rhs){
   if (Lhs.tv_sec < Rhs.tv_sec){ return true; }
   else if (Lhs.tv_sec == Rhs.tv_sec){
      if (Lhs.tv_usec < Rhs.tv_usec){ return true; }
   }
   return false;
}

int COLtimeStampDiffInMillisecond(const COLtimeStamp& Lhs, const COLtimeStamp& Rhs){
   COL_FUNCTION(COLtimeStampDiffInMillisecond);
   int Seconds = Lhs.tv_sec - Rhs.tv_sec;
   COL_VAR(Seconds);
   int Microseconds = Lhs.tv_usec - Rhs.tv_usec;
   COL_VAR(Microseconds);
   if (Microseconds < 0 && Seconds > 0){
      Microseconds += 1000000;
      Seconds -= 1;
   }
   return (Seconds * 1000) + (Microseconds / 1000);

}

COLostream& operator<<(COLostream& Stream, const COLtimeStamp& Time) {
   Stream << "Time: " << Time.tv_sec << " seconds and " << Time.tv_usec << " microseconds";
   return Stream;
}
