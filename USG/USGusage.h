#ifndef __USG_USAGE_H__
#define __USG_USAGE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USGusage
//
// Description:
//
// Iguana usage
//
// Author: John Q
// Date:   Tue Oct  2 10:16:34 EDT 2024
// ---------------------------------------------------------------------------
#include <time.h>
#include <COL/COLvar.h>

class USGusage {
public:
   int   LoginCount  {0};
   // add more usages here, don't forget to update toVar() and reset()

   void updateUploadTime();
   void setLastError(const COLstring& Error);
   COLvar toVar() const;
private:
   void reset();

   time_t m_LastUpload {0};
   COLstring m_LastError;
};

USGusage& USGusages();

#endif // end of defensive include
