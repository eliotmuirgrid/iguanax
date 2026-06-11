// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCdir
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 19 August 2025 - 12:08PM
// ---------------------------------------------------------------------------

#include <DOC/DOCdir.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOChex.h>

#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DOCrepositoryDir(const COLstring& ComponentId){
   COL_FUNCTION(DOCrepositoryDir);
   return DIRcomponentDevDir(ComponentId);
}

COLstring DOCbase(const DOCdocument& Doc){
   COL_FUNCTION(DOCbase);
   COLstring Out = DOCpaddHex(DOCtoHex(Doc.Created));
   COL_VAR(Out);
   Out = Out.substr(0,2) + COL_T("/") 
       + Out.substr(2,2) + COL_T("/") 
       + Out.substr(4,2) + COL_T("/")
       + Out.substr(6,2) + COL_T("/")
       + Out.substr(8,2) + COL_T("/")
       + Out.substr(10,2) + COL_T("/")
       + Out.substr(12,2) + COL_T("/")
       + Doc.Id;
   return Out;
}

COLstring DOCextension(const COLstring& DocSource){
   COL_FUNCTION(DOCextension);
   COL_VAR(DocSource);
   if (DocSource == "eml") { return "eml"; }
   return DocSource;   // default for png, pdf etc.
}

COLstring DOCfileBase(const COLstring& Component, const DOCdocument& Doc){
   COL_FUNCTION(DOCfileBase);
   COLstring FileName = DOCrepositoryDir(Component);
   FILaddPathSeparator(FileName);
   FileName+= DOCbase(Doc);
   return FileName;
}

COLstring DOCfileMeta          (const COLstring& Component, const DOCdocument& Doc){ return DOCfileBase(Component, Doc) + ".info.json";                  }
COLstring DOCfileMd            (const COLstring& Component, const DOCdocument& Doc){ return DOCfileBase(Component, Doc) + ".md";                         }
COLstring DOCfileOriginal      (const COLstring& Component, const DOCdocument& Doc){ return DOCfileBase(Component, Doc) + "." + DOCextension(Doc.Source);}
COLstring DOCfileOriginalLegacy(const COLstring& Component, const DOCdocument& Doc){ return DOCfileBase(Component, Doc) + ".orig";                       }

COLstring DOCfileGuid(const DOCdocument& Doc){
   COLstring Out(Doc.Id);
   Out = Out.substr(0,3) + COL_T("/")
       + Out.substr(3,3) + COL_T("/")
       + Out.substr(6,3) + COL_T("/")
       + Out.substr(9,3) + ".info";
   return Out; 
}