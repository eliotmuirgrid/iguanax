//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDstats.cpp
//
//  Description: System stats collector for live resource snapshots
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <ctime>
#include <COL/COLlog.h>
#include <COL/COLostream.h>
#include <COL/COLvar.h>
#include <GRID/GRIDstats.h>
#include <INFO/INFOusage.h>
COL_LOG_MODULE;

struct GRIDstatsData {
   time_t    Timestamp;
   COLstring CpuPercent;
   COLstring MemoryVirtualMb;
   COLstring MemoryResidentMb;
   COLstring MemoryPhysicalMb;
   COLstring DiskSizeGb;
   COLstring DiskUsedGb;
   COLstring DiskFreeGb;
};

static COLstring GRIDstatsFormatNumber(const double Value) {
   COL_FUNCTION(GRIDstatsFormatNumber);
   const COLint64 Rounded    = static_cast<COLint64>(Value * 100.0 + 0.5);
   const COLint64 Integer    = Rounded / 100;
   const COLint64 Fractional = Rounded % 100;
   COLstring      Result;
   COLostream     Out(Result);
   Out << Integer << ".";
   if(Fractional < 10) { Out << "0"; }
   Out << Fractional;
   return Result;
}

static GRIDstatsData GRIDstatsCollectAndFormat() {
   COL_FUNCTION(GRIDstatsCollectAndFormat);
   static const double MB = 1000000.0;
   static const double GB = 1000000000.0;

   const INFOmemory Memory     = INFOmemoryStats();
   const int        CpuPercent = static_cast<int>(INFOcpuPercent() + 0.5);
   const INFOdisk   Disk       = INFOdiskStats();
   const time_t     Now        = time(NULL);
   GRIDstatsData    Stats;
   Stats.Timestamp        = Now;
   Stats.CpuPercent       = COLint64ToString(CpuPercent);
   Stats.MemoryVirtualMb  = GRIDstatsFormatNumber(Memory.Virtual / MB);
   Stats.MemoryResidentMb = GRIDstatsFormatNumber(Memory.Resident / MB);
   Stats.MemoryPhysicalMb = GRIDstatsFormatNumber(Memory.Physical / MB);
   Stats.DiskSizeGb       = GRIDstatsFormatNumber(Disk.Size / GB);
   Stats.DiskUsedGb       = GRIDstatsFormatNumber(Disk.Used / GB);
   Stats.DiskFreeGb       = GRIDstatsFormatNumber(Disk.Free / GB);
   return Stats;
}

static COLvar GRIDstatsFormatAsJson(const GRIDstatsData& Stats) {
   COL_FUNCTION(GRIDstatsFormatAsJson);
   COLvar Result;
   Result["timestamp"]       = COLint64ToString(Stats.Timestamp);
   Result["cpu_percent"]     = Stats.CpuPercent;
   Result["mem_virtual_mb"]  = Stats.MemoryVirtualMb;
   Result["mem_resident_mb"] = Stats.MemoryResidentMb;
   Result["mem_physical_mb"] = Stats.MemoryPhysicalMb;
   Result["disk_size_gb"]    = Stats.DiskSizeGb;
   Result["disk_used_gb"]    = Stats.DiskUsedGb;
   Result["disk_free_gb"]    = Stats.DiskFreeGb;
   return Result;
}

GRIDstatsCollector::GRIDstatsCollector(SCKloop* pLoop) {
   COL_METHOD(GRIDstatsCollector::GRIDstatsCollector);
   (void)pLoop;
}

GRIDstatsCollector::~GRIDstatsCollector() {
   COL_METHOD(GRIDstatsCollector::~GRIDstatsCollector);
}

bool GRIDstatsCollector::getCurrentStats(COLvar* pOut) const {
   COL_METHOD(GRIDstatsCollector::getCurrentStats);
   try {
      const GRIDstatsData Stats = GRIDstatsCollectAndFormat();
      *pOut                     = GRIDstatsFormatAsJson(Stats);
      return true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      (*pOut)["error"] = E.description();
      return false;
   }
}
