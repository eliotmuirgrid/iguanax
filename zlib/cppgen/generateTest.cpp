// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: generate
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Sat 17 Sep 2022 15:13:46 EDT
// ---------------------------------------------------------------------------
#include "generateTest.h"
#include "generate.h"

#include <stdio.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring CPPtestDefine(const COLstring& Name){
   COL_FUNCTION(CPPtestDefine);
   COLstring Result;
   Result = "__TEST" + CPPconvertCamel(Name.substr(4)) +"_H__";
   COL_VAR(Result);
   return Result;
}

void CPPgenerateTestHeaderBody(const COLstring& Name, const COLstring& Author, COLostream& Out){
   COL_FUNCTION(CPPgenerateTestHeaderBody);
   COLstring Define = CPPtestDefine(Name);
   Out << "#ifndef " << Define << newline
       << "#define " << Define << newline
       << "// ---------------------------------------------------------------------------" << newline
       << "// Copyright (C) 1997-" << CPPtimeStamp("%Y") << " iNTERFACEWARE Inc.  All Rights Reserved" << newline
       << "//" << newline
       << "// Module: " << Name << newline
       << "//" << newline
       << "// Description:" << newline
       << "//" << newline
       << "// Tests for TODO" << newline
       << "//" << newline
       << "// Author: " << Author << " " << newline
       << "// Date:   " << CPPtimeStamp("%A %d %B %Y - %I:%M%p") << newline
       << "// ---------------------------------------------------------------------------" << newline
        << newline
       << "class UNITapp;" << newline
       << newline 
       << "void " << Name << "(UNITapp* pApp);" << newline
       << newline
       << "#endif // end of defensive include" << newline;
}

void CPPgenerateTestCppBody(const COLstring& Name, const COLstring& Author, COLostream& Out){
   COL_FUNCTION(CPPgenerateTestCppBody);
   COLstring Prefix = CPPprefix(Name);
   Out << "// ---------------------------------------------------------------------------" << newline
       << "// Copyright (C) 1997-" << CPPtimeStamp("%Y") << " iNTERFACEWARE Inc.  All Rights Reserved" << newline
       << "//" << newline
       << "// Module: " << Name << newline
       << "//" << newline
       << "// Description:" << newline
       << "//" << newline
       << "// Implementation of tests" << newline
       << "//" << newline
       << "// Author: " << Author << " " << newline
       << "// Date:   " << CPPtimeStamp("%A %d %B %Y - %I:%M%p") << newline
       << "// ---------------------------------------------------------------------------" << newline
       << "#include \"" << Name << ".h\"" << newline
       << newline
       << "#include <UNIT/UNITapp.h>" << newline
       << newline
       << "#include <COL/COLlog.h>" << newline
       << "COL_LOG_MODULE;" << newline
       << newline
       << "static void testSimple(){" << newline
       << "   COL_FUNCTION(testSimple);" << newline
       << "}" << newline
       << newline
       << "void " << Name << "(UNITapp* pApp) {" << newline
       << "   pApp->add(\"test/simple\", &testSimple);" << newline
       << "}" << newline;
}

bool CPPhasTestPrefix(const COLstring& Name){
   COL_FUNCTION(CPPhasTestPrefix);
   return (Name.substr(0,4) == "test");
}

bool CPPgenerateTestFiles(const COLstring& Name, const COLstring& Author){
   COL_FUNCTION(CPPgenerateTestFiles);
   COL_VAR(Name);
   if (!CPPhasTestPrefix(Name)){
      COLcout << "File name must be at least 5 characters and start with the prefix test" << newline;
      return false;
   }
   COLsinkString Sink;
   COLostream Out(Sink);
   CPPgenerateTestHeaderBody(Name, Author, Out);
   FILwriteFile(Name + ".h", Sink.string());
   Sink.string().clear();
   CPPgenerateTestCppBody(Name, Author, Out);
   FILwriteFile(Name + ".cpp", Sink.string());
   COLcout << "Remember to add generated files to git:" << newline << "git add " << Name << ".h " << Name << ".cpp" << newline;
   return true;
}
