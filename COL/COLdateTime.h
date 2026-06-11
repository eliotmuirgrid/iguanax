#ifndef __COL_DATE_TIME_H__
#define __COL_DATE_TIME_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLdateTime
//
// Description:
//
// ELIOT - COLdateTime is LEGACY - DON"T USE IT FROM NEW CODE.  time_t is better thought out in my opinion
// (truth is I detest Microsoft with a passion for their horrible operating system)
// Eliot - these day's I would recommend just using time_t and the related C library
// functions - epoch time is better than stuff from MFC...
//
// This class is a wrapper around the DATE 8 byte float representation
// of a date.  The class is inspired by the MFC COLdateTime class.
//
// Note that when we subtract one COLdateTime object from another we get
// a COLdateTimeSpan object.  COLdateTime objects can be handy when you
// want to time a process using the following:
//
// COLdateTime StartTime = COLdateTime::currentTime();
//
// // do stuff
//
// COLdateTime EndTime = COLdateTime::currentTime();
//
// COLcout << COL_T("The process started at ") << StartTime 
//         << COL_T("and took ") << EndTime - StartTime;
//
// The above code works because both COLdateTime and COLdateTimeSpan
// have overloaded COLostream << operators (as do most objects in our library).
// 
// Author: Todd Martin
// Date:   Fri 11/06/1998 
//---------------------------------------------------------------------------

class COLstring;

#ifndef DATE
   typedef double DATE;
#endif

#include <time.h>
#include "COLrefCounted.h"

#define COL_HOURS_IN_A_DAY 24

//TODO
// This class needs to be thoroughly unit tested and account
// taken of Daylight savings in a thorough way.

//TODO: get rid of this commented code

//#ifndef _TIME_T_DEFINED 
//typedef long time_t;        /* time value */
//#define _TIME_T_DEFINED     /* avoid multiple def's of time_t */
//#endif

class COLdateTimeSpan;
class COLdateTimePrivate;
class COLstring;

// LEGACY - DON'T USE FOR NEW CODE
class COLdateTime : public COLrefCounted{  // hmmm what is this for?
// Constructors
public:

   const static COLstring DAY_OF_WEEK_AS_STRING[];
   const static COLstring DAY_OF_WEEK_AS_STRING_SHORT[];
   const static COLstring MONTH_OF_YEAR_AS_STRING[];
   const static COLstring MONTH_OF_YEAR_AS_STRING_SHORT[];
   const static COLstring DAY_OF_MONTH_AS_STRING[];
   const static COLstring MERIDIAN_AS_STRING[];
   const static COLstring MERIDIAN_AS_STRING_FORMAL[];

   const static COLstring EMPTY_STRING;

   static COLdateTime currentTime();
   static COLdateTime currentDate();

   COLdateTime();
   virtual ~COLdateTime();

   COLdateTime(const COLdateTime& Orig);
   COLdateTime(DATE dtSrc);
//TODO consider using a #define for time_t
   COLdateTime(time_t timeSrc);
   //COLdateTime(const SYSTEMTIME& systimeSrc);

   // The following is only really used in the FILe library for the FILdirEnumerator class - have a look at that if you have
   // questions about this code.
#ifdef CHM_FILETIME  //  classes which need this constructor have to have #define CHM_FILETIME in them before including COLdateTime
   COLdateTime(const FILETIME& FileTimeStamp);
#endif

   COLdateTime(int nYear, int nMonth, int nDay,
      int nHour, int nMin, int nSec);

// Attributes
public:
   enum DateTimeStatus
   {
      valid = 0,
      invalid = 1,    // Invalid date (out of range, etc.)
      null = 2        // Literally has no value
   };


   void setStatus(DateTimeStatus status);
   DateTimeStatus status() const;

   // determine if this is a null date
   bool isNull() const;
   void setNull();

   // fractional second part
   void setSecondFraction(double NewValue);
   const double& secondFraction() const;
   // zone
   void setOffsetInMinutes(short NewValue, bool IsDefined);
   short offsetInMinutes() const;
   bool offsetIsDefined() const;

   // Warning - this method is quite expensive - it takes a lot of
   // transformation to yield a time_t from the internal format of COLdateTime
   // which is a floating point value with the number of days since 1899
   time_t asTimeT() const;

   //bool GetAsSystemTime(SYSTEMTIME& sysTime) const;
//TODO Change ints to COLints and doubles to COLfloat64s
   int year() const;
   int month() const;       // month of year (1 = Jan)
   int day() const;         // day of month (0-31)
   int hour() const;        // hour in day (0-23)
   int minute() const;      // minute in hour (0-59)
   int second() const;      // second in minute (0-59)

   /**
    * Purpose: Returns the number of weeks that has elapse in this year.
    * Preconditions: This date time has been initialized correctly.
    * Postconditions: Returns the number of weeks that has elapsed starting
    *    from 0 to 53.  The first weeks includes all days of the year starting
    *    from January 1 up to and including the immediate saturday, and every
    *    sunday will increment the week number by one.
    */
   COLint16 weekOfYear() const;
   
   int dayOfWeek() const;   // 1=Sun, 2=Mon, ..., 7=Sat
   int dayOfYear() const;   // days since start of year, Jan 1 = 1
   
   // places 'Sun', 'Mon', Tue.. etc into the Week parameter
   void dayOfWeekString(COLstring& Week) const;

   // places 'Jan', 'Feb'.. etc into the Month parameter   
   void monthString(COLstring& Month) const;

   const COLstring& dayOfWeekString(bool IsLong = true) const;
   const COLstring& monthString(bool IsLong = true) const;
   const COLstring& dayOfMonthString() const;
   const COLstring& meridianString(bool IsFormal = false) const;

   const static COLstring& dayOfWeekString(COLuint16 DayOfWeekNumber,
                                           bool IsLong = true);
   const static COLstring& monthNumberToString(COLuint16 MonthNumber,
                                       bool IsLong = true);
   const static COLstring& dayOfMonthString(COLuint16 DayOfMonth);
   const static COLstring& meridianString(COLuint16 Hour,
                                          bool IsFormal = false);

   // Originally was in COLcompileTimeStamp, but it should really be here.
   // Returns 1 given "Jan", 2 given "Feb", etc.
   static COLint32 monthFromShortName(const COLstring& ShortMonthName);
   
   //empty string returned if not found
   static const COLstring& shortNameFromMonth(COLint32 Month);
private: // helpers for monthFromShortName and shortNameFromMonth
   static void initShortMonthLookup();
   

// Operations
public:
   const COLdateTime& operator=(const COLdateTime& Orig);
   const COLdateTime& operator=(DATE Orig);

   const COLdateTime& operator=(const time_t& timeSrc);
   //const COLdateTime& operator=(const SYSTEMTIME& systimeSrc);

#ifdef CHM_FILETIME
   const COLdateTime& operator=(const FILETIME& filetimeSrc);
#endif

   bool operator==(const COLdateTime& date) const;
   bool operator!=(const COLdateTime& date) const;
   bool operator<(const COLdateTime& date) const;
   bool operator>(const COLdateTime& date) const;
   bool operator<=(const COLdateTime& date) const;
   bool operator>=(const COLdateTime& date) const;

   // DateTime math
   COLdateTime operator+(const COLdateTimeSpan& dateSpan) const;
   COLdateTime operator-(const COLdateTimeSpan& dateSpan) const;
   const COLdateTime& operator+=(const COLdateTimeSpan dateSpan);
   const COLdateTime& operator-=(const COLdateTimeSpan dateSpan);

   // DateTimeSpan math
   COLdateTimeSpan operator-(const COLdateTime& date) const;

   operator DATE() const;

   int setDateTime(int nYear, int nMonth, int nDay,
                   int nHour, int nMin,   int nSec);
   int setDate(int nYear, int nMonth, int nDay);
   int setTime(int nHour, int nMin, int nSec); //Resets the current objects date to beginning of time. 
   int setUpdatedTime(int nHour, int nMin, int nSec); //Keeps the current objects date as is.

//TODO get rid of old comments   
//   bool ParseDateTime(COLstring& String, DWORD dwFlags = 0,
//                            LCID lcid = LANG_USER_DEFAULT);

   // formatting
  // COLstring Format(size_t dwFlags = 0, LCID lcid = LANG_USER_DEFAULT) const;
   COLstring format(const char* lpszFormat) const;
   //COLstring Format(unsigned int nFormatID) const;
   
   void printOn(COLostream& Stream) const;

   void fromString(const COLstring& String);// day/month/year  hour:min:sec
   void fromStringYearFirst(const COLstring& String);// YYYY-MM-DD  HH:MM:SS

// Implementation
protected:
   COLdateTimePrivate* pMember;
   void CheckRange();
   friend class COLdateTimeSpan;
};

COLostream& operator<<(COLostream& Stream, const COLdateTime& DateTime);

class COLdateTimeSpan
{
// Constructors
public:
   COLdateTimeSpan();

   COLdateTimeSpan(double dblSpanSrc);
   COLdateTimeSpan(const COLdateTimeSpan& Orig);
   COLdateTimeSpan(long lDays, int nHours, int nMins, int nSecs);

// Attributes
public:
   enum DateTimeSpanStatus
   {
      valid = 0,
      invalid = 1,    // Invalid span (out of range, etc.)
      null = 2        // Literally has no value
   };

   const static COLstring TIME_UNITS[];
   
   double m_span;
   DateTimeSpanStatus m_status;

   void setStatus(DateTimeSpanStatus status) { m_status = status; }
   DateTimeSpanStatus status() const { return m_status; }

   double totalDays() const;    // span in days (about -3.65e6 to 3.65e6)
   double totalHours() const;   // span in hours (about -8.77e7 to 8.77e6)
   double totalMinutes() const; // span in minutes (about -5.26e9 to 5.26e9)
   double totalSeconds() const; // span in seconds (about -3.16e11 to 3.16e11)

   long days() const;       // component days in span
   long hours() const;      // component hours in span (-23 to 23)
   long minutes() const;    // component minutes in span (-59 to 59)
   long seconds() const;    // component seconds in span (-59 to 59)

// Operations
public:
   const COLdateTimeSpan& operator=(double Orig);
   const COLdateTimeSpan& operator=(const COLdateTimeSpan& Orig);

   bool operator==(const COLdateTimeSpan& dateSpan) const;
   bool operator!=(const COLdateTimeSpan& dateSpan) const;
   bool operator<(const COLdateTimeSpan& dateSpan) const;
   bool operator>(const COLdateTimeSpan& dateSpan) const;
   bool operator<=(const COLdateTimeSpan& dateSpan) const;
   bool operator>=(const COLdateTimeSpan& dateSpan) const;

   // DateTimeSpan math
   COLdateTimeSpan operator+(const COLdateTimeSpan& dateSpan) const;
   COLdateTimeSpan operator-(const COLdateTimeSpan& dateSpan) const;
   const COLdateTimeSpan& operator+=(const COLdateTimeSpan dateSpan);
   const COLdateTimeSpan& operator-=(const COLdateTimeSpan dateSpan);
   COLdateTimeSpan operator-() const;
   COLdateTimeSpan operator*(double) const;
   COLdateTimeSpan operator/(double) const;


   void printOn(COLostream& Stream) const;

   void setDateTimeSpan(long lDays, int nHours, int nMins, int nSecs);

// Implementation
public:
   void CheckRange();
   friend class COLdateTime;
};

COLostream& operator<<(COLostream& Stream, const COLdateTimeSpan& Span);

COLstring COLformatDateTimeOffset(const COLdateTime& DateTime);

// a timestamp structure - just plain old data - no ctor, no dtor.
struct COLtimeStamp
{
   COLuint64 tv_sec;  // time_t compatible
   COLuint64 tv_usec; // microseconds - NOT milliseconds!
   time_t asTimeT() const { return (time_t)tv_sec; }
   int milliseconds() const { return (int)(tv_usec / 1000); }
   void addMilliseconds(int Milliseconds);
};

bool operator==(const COLtimeStamp& a, const COLtimeStamp& b);
bool operator<=(const COLtimeStamp& a, const COLtimeStamp& b);
bool operator<(const COLtimeStamp& Lhs, const COLtimeStamp& Rhs);
bool operator==(const COLtimeStamp& Lhs, const COLtimeStamp& Rhs);
COLostream& operator<<(COLostream& Stream, const COLtimeStamp& Time);
int COLtimeStampDiffInMillisecond(const COLtimeStamp& Lhs, const COLtimeStamp& Rhs);
// Populate the COLtimeStamp with the current time with microsecond resolution
//
void COLcurrentTimeStamp(COLtimeStamp*);

// Convert from COLtimeStamp to year, month, day, hour, minute, second, millisecond
//
void COLtimeStampToComponents
(
   const COLtimeStamp& TimeStamp,
   COLuint32& Year,
   COLuint32& Month,
   COLuint32& Day,
   COLuint32& Hour,
   COLuint32& Min,
   COLuint32& Sec,
   COLuint32& Ms
);

// These functions used to be in their own class (COLcompileTimeStamp) but were
// moved here in #22340. They're used primarily for maintenance expiry validation.
COLdateTime COLcompileTimeStamp();
// This function uses the TimeStamp value to get the compile time.
// The string must be in the form: "YYYYMMDDhhmmss"
COLdateTime COLcompileTimeStamp(const COLstring& TimeStamp);

#endif // end of defensive include
