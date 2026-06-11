#ifndef __SCK_TIMER_H__
#define __SCK_TIMER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKtimer
//
// Description:
//
// Convenience object around timers in SCKloop interface - the destructor deregisters
// the timer object which can be helpful compared to using the SCKloop interface for timers directly.
//
// This object is closer to the original EVNtimer interface.
//
// Author : Eliot Muir
// Date:    Fri  4 Sep 2020 10:15:49 EDT
//---------------------------------------------------------------------------

class COLclosure0;
template<typename> class COLauto;

class SCKloop;

class SCKtimer{
public:
   SCKtimer(SCKloop* pLoop);
   ~SCKtimer();
   
   void start(COLauto<COLclosure0> pCallback, int IntervalInMilliseconds=0, bool Repeat=false);
   void stop(); // Stops the timer immediately. Calling stop on a stopped timer has no effect on it. 
   bool isSet() const; // Indicates if the timer is timing. If not true, an event should not be expected.
private:
   int m_TimerId;
   SCKloop* m_pLoop;
   SCKtimer( const SCKtimer& Orig );
   SCKtimer& operator=( const SCKtimer& Orig );
};

#endif // end of defensive include
