//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfig
//
// Label: Translator node object model
//
// Author: Shuo Hou
// Date:   Tuesday, Aug 31th, 2021
//---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigSave.h>
#include <COL/COLlog.h>
#include <HASH/HASHutils.h>
COL_LOG_MODULE;

void CFGconfigAddSource(const COLstring& Source, CFGconfig* pConfig) {
   COL_FUNCTION(CFGconfigAddSource);
   for(int i = 0; i < pConfig->CoreConfig.Sources.size(); ++i) {
      if(pConfig->CoreConfig.Sources[i] == Source) { return; }
   }
   pConfig->CoreConfig.Sources.push_back(Source);
}

bool CFGconfigRemoveSource(const COLstring& Source, CFGconfig* pConfig) {
   COL_FUNCTION(CFGconfigRemoveSource);
   for(int i = 0; i < pConfig->CoreConfig.Sources.size(); ++i) {
      if(pConfig->CoreConfig.Sources[i] == Source) {
         pConfig->CoreConfig.Sources.remove(i);
         return true;
      }
   }
   return false;
}

void CFGcoreToVar(const CFGcore& CoreConfig, COLvar* pOut) {
   COL_FUNCTION(CFGconfigRemoveSource);
   COLvar& Out        = *pOut;
   Out["guid"]        = CoreConfig.Guid;
   Out["commit"]      = CoreConfig.Commit;
   Out["description"] = CoreConfig.Description;
   Out["name"]        = CoreConfig.Name;
   Out["run_branch"]  = CoreConfig.Branch;
   Out["run_repo"]    = CoreConfig.Repo;
   Out["run_comp"]    = CoreConfig.RunComp;
   Out["template"]    = CoreConfig.Template;
   Out["sources"]     = CFGconfigSerializeArray(CoreConfig.Sources);
   Out["tags"]        = CFGconfigSerializeArray(CoreConfig.Tags);
}

const char* CFGoutputLight(CFG_LIGHT Light) {
   switch(Light) {
      case CFG_LIGHT::OFF: return "OFF";
      case CFG_LIGHT::ON: return "ON";
      case CFG_LIGHT::IN_PROGRESS: return "YELLOW";
      case CFG_LIGHT::ERR: return "RED";
      default: return "UNKNOWN";
   }
}

COLostream& operator<<(COLostream& Stream, const CFGconfig& Config) {
   Stream << "\nCOMPONENT" << newline;
   Stream << "CONFIG" << Config.CoreConfig << newline;
   Stream << "STATUS" << Config.Status << newline;
   Stream << "OTHER";
   Stream << "\n\tInitialized  = " << Config.m_Initialized << "\n\tServerGuid   = " << Config.m_HostServerGuid
          << "\n\tIsLocal      = " << Config.m_Local;
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const CFGcore& Core) {
   Stream << "\n\tName         = " << Core.Name << "\n\tDescription  = " << Core.Description
          << "\n\tGuid         = " << Core.Guid << "\n\tTemplate     = " << Core.Template
          << "\n\tCommit       = " << Core.Commit << "\n\tRepo         = " << Core.Repo;
   // TODO sources and field values?
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const CFGstatus& Status) {
   Stream << "\n\tLight        = " << CFGoutputLight(Status.Light) << "\n\tErrors       = " << Status.ErrorCount
          << "\n\tActivity     = " << Status.LastActivity << "\n\tBorder       = " << Status.Border
          << "\n\tTooltip      = " << Status.HtmlTooltipBlock << "\n\tHtml         = " << Status.HtmlBlock;

   return Stream;
}
CFGcore::CFGcore(const COLstring& _guid, const COLstring& _name, const COLstring& _description, const COLvar& _tags,
                 bool _template) {
   Guid        = _guid;
   Name        = _name;
   Description = _description;
   Template    = _template;
   Commit      = "";
   Repo        = "";
   HASHvarToArr(_tags, &Tags);
}
