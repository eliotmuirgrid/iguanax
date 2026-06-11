//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfigSave
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 12 Jul 2022 14:34:23 EDT
//---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigSave.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <PASS/PASSword.h>
COL_LOG_MODULE;

COLvar CFGconfigSerializeFields(const COLmap<COLstring, COLstring>& Fields) {
   COL_FUNCTION(CFGconfigSerializeFields);
   COLvar MapVar;
   MapVar.setMapType();
   for(const auto& it : Fields) {
      COLstring Value;
      bool      Success = PASSencrypt(it.second, &Value);
      MapVar[it.first]  = Success ? Value : it.second;
   }
   COL_VAR(MapVar.json(1));
   return MapVar;
}

COLvar CFGconfigSerializeArray(const COLarray<COLstring>& Array) {
   COL_FUNCTION(CFGconfigSerializeArray);
   COLvar Var;
   Var.setArrayType();
   for(const auto& it : Array) { Var.push_back(it); }
   COL_VAR(Var.json(1));
   return Var;
}

// This just for serializing CFGconfig into a COLvar
COLvar CFGconfigSerialize(const CFGconfig& Config) {
   COL_FUNCTION(CFGconfigSerialize);
   COLvar    NodeVar;
   COLstring Commit = Config.CoreConfig.Commit;
   if(Commit == "DEVELOPMENT") {
      Commit = "";
   } else if(Commit.size() > 7) {
      Commit = Commit.substr(0, 7);
   }
   NodeVar["run_commit"]  = Commit;
   NodeVar["name"]        = Config.CoreConfig.Name;
   NodeVar["description"] = Config.CoreConfig.Description;
   NodeVar["guid"]        = Config.CoreConfig.Guid;
   NodeVar["template"]    = Config.CoreConfig.Template;
   NodeVar["run_branch"]  = Config.CoreConfig.Branch;
   NodeVar["run_repo"]    = Config.CoreConfig.Repo;
   NodeVar["run_comp"]    = Config.CoreConfig.RunComp;
   NodeVar["tags"]        = CFGconfigSerializeArray(Config.CoreConfig.Tags);
   NodeVar["fields"]      = CFGconfigSerializeFields(Config.CoreConfig.Fields);
   COLvar& Sources        = NodeVar["sources"];
   Sources.setArrayType();
   for(const auto& Source : Config.CoreConfig.Sources) { Sources.push_back(Source); }
   COL_VAR(NodeVar);
   return NodeVar;
}
