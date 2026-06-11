#ifndef __COL_FILTER_H__
#define __COL_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLfilter
//
// Description:
//
// Don't spend much time reading this - not that useful in practice.
//
// COLfilter subclasses are designed to be a chain of sinks to write to.
//
// In order to  achieve this, the subclass must implement COLsink's 
// write() method and, much like the COLsource, it must pass the
// data on to the next COLsink in the chain using a next().write()
// call.
//
// Eliot removed the original idea which also included the idea of a COLsource.
//
// Author: Ryan Dewsbury, Rob Moyse
// Date:   November 12, 2004
//---------------------------------------------------------------------------

#include <COL/COLsink.h>

class COLfilter : public COLsink{ 
public:
   // Constructor takes a pointer to the COLsink object that will be the destination for this filter's data.  
   COLfilter( COLsink* pNext ) : m_pNext(pNext) {};
   virtual ~COLfilter(){}
   
   //virtual bool readAll( COLsource& Source ){ return next().readAll( Source ); }

   // Indicates that any state information retained between write() calls is reset. This allows reuse of a filter with a new stream
   // without reconstructing a new filter.
   virtual void resetFilter() = 0;

   // The next sink we are writing to.
   void setNext( COLsink* pNext ) { m_pNext = pNext; }
   bool hasNext() const { return m_pNext != NULL; }
   COLsink& next() const { return *m_pNext; };

   virtual void onEndStream(){ next().onEndStream(); };
   virtual void flush(){ next().flush(); };
private:
   COLsink* m_pNext;
   COLfilter(const COLfilter& Filter);
   COLfilter& operator=(const COLfilter& Filter);
};

#endif  // end of defensive include
