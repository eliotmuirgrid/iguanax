#ifndef _GRIDSTATS_H
#define _GRIDSTATS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDstats.h
//
//  Description: System stats collector for live resource snapshots
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>

class COLvar;
class SCKloop;

class GRIDstatsCollector {
public:
   explicit GRIDstatsCollector(SCKloop* pLoop);
   ~GRIDstatsCollector();

   bool getCurrentStats(COLvar* pOut) const;

private:
   GRIDstatsCollector(const GRIDstatsCollector&)            = delete;
   GRIDstatsCollector& operator=(const GRIDstatsCollector&) = delete;
};

#endif  // end of defensive include
