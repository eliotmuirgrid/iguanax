#ifndef __CFG_CONFIG_H__
#define __CFG_CONFIG_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfig
//
// Label: Translator node/component configuration and status model
//
// Author: Shuo Hou
// Date:   Tuesday, Aug 31th, 2021
//---------------------------------------------------------------------------
#include <COL/COLvar.h>
#include <COL/COLarray.h>

#include <ctime>

class COLstring;
class SCKloop;

enum class CFG_LIGHT {
   OFF         = 0,
   ON          = 1,
   IN_PROGRESS = 2,
   ERR         = 3,
};

const char* CFGoutputLight(CFG_LIGHT Light);

// This maps to the <instance>/components/configurations/<component guid>.json
struct CFGcore {
   CFGcore() : Template(false) {}
   CFGcore(const COLstring& _guid, const COLstring& _name, const COLstring& _description, const COLvar& _tags,
           bool _template);

   COLstring                    Name;
   COLstring                    Description;
   COLstring                    Guid;
   COLstring                    Commit;
   COLstring                    Repo;
   COLstring                    Branch;
   COLstring                    RunComp;
   COLarray<COLstring>          Sources;
   bool                         Template;
   COLarray<COLstring>          Tags;
   COLmap<COLstring, COLstring> Fields;
};

struct CFGstatus {
   CFG_LIGHT Light      = CFG_LIGHT::OFF;
   int       ErrorCount = 0;
   time_t    StartTime  = 0;
   COLstring Border;
   COLstring HtmlBlock;
   COLstring HtmlTooltipBlock;
   time_t    LastActivity;
};

struct CFGconfig {
   CFGconfig() {}
   // Core configuration information
   CFGcore   CoreConfig;
   CFGstatus Status;

   // Server information
   COLstring m_HostServerGuid;

   bool m_Local = true;  // This is a local component rather than being a component on a remote Iguana.

   // Initialization information
   bool m_Initialized = false;
};

void CFGconfigAddSource(const COLstring& Source, CFGconfig* pConfig);
bool CFGconfigRemoveSource(const COLstring& Source, CFGconfig* pConfig);
void CFGcoreToVar(const CFGcore& CoreConfig, COLvar* pOut);

COLostream& operator<<(COLostream& Stream, const CFGconfig& Config);
COLostream& operator<<(COLostream& Stream, const CFGcore& Core);
COLostream& operator<<(COLostream& Stream, const CFGstatus& Status);

#endif  // end of defensive include
