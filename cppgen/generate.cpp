// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: generate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sat 17 Sep 2022 15:13:46 EDT
// ---------------------------------------------------------------------------

#include "generate.h"

#include <FIL/FILutils.h>

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <time.h>

bool CPPisUpper(char Char){
   return Char >= 'A' && Char <='Z';
}

bool CPPhasPrefix(const COLstring& Name){
   COL_FUNCTION(CPPhasPrefix);
   if (Name.size() < 4){
      return false;
   }
   return CPPisUpper(Name[0]) && CPPisUpper(Name[1]) && CPPisUpper(Name[2]);
}

COLstring CPPprefix(const COLstring& Name){
   COL_FUNCTION(CPPprefix);
   int i =0;
   while (CPPisUpper(Name[i]) && i < Name.size()){
      i++;
   }
   return Name.substr(0, i);
}

COLstring CPPconvertCamel(const COLstring& Camel){
   COL_FUNCTION(CPPconvertCamel);
   COL_VAR(Camel);
   COLstring Result;
   int i=0;
   while (i < Camel.size()){
      if (CPPisUpper(Camel[i])){
         Result += "_";
      }
      Result += Camel[i];
      i++;
   }
   COL_VAR(Result);
   Result = Result.toUpperCase();
   COL_VAR(Result);
   return Result;
}

COLstring CPPdefine(const COLstring& Name){
   COL_FUNCTION(CPPdefine);
   COLstring Prefix = CPPprefix(Name);
   COLcout << "Prefix: " << Prefix << newline;
   COLstring Convert = CPPconvertCamel(Name.substr(Prefix.size()));

   COLstring DefInclude = "__" + Prefix + "_" + Convert + "_H__";
   COL_VAR(DefInclude);
   return DefInclude;
}

COLstring CPPtimeStamp(const char* pPattern){
   COL_FUNCTION(CPPtimeStamp);
   time_t Now;
   struct tm* pTimeInfo;
   char Buffer[80];
   time(&Now);
   pTimeInfo = localtime( &Now );
   strftime(Buffer,80,pPattern,pTimeInfo);
   COL_VAR(Buffer);
   return Buffer;
}

void CPPgenerateHeaderBody(const COLstring& Name, const COLstring& Author, COLostream& Out){
   COL_FUNCTION(CPPgenerateHeaderBody);
   COLstring Define = CPPdefine(Name);
   Out << "#ifndef " << Define << newline
       << "#define " << Define << newline
       << "// ---------------------------------------------------------------------------" << newline
       << "// Copyright (C) 1997-" << CPPtimeStamp("%Y") << " iNTERFACEWARE Inc.  All Rights Reserved" << newline
       << "//" << newline
       << "// Module: " << Name << newline
       << "//" << newline
       << "// Description:" << newline
       << "//" << newline
       << "// TODO" << newline
       << "//" << newline
       << "// Author: " << Author << " " << newline
       << "// Date:   " << CPPtimeStamp("%A %d %B %Y - %I:%M%p") << newline
       << "// ---------------------------------------------------------------------------" << newline
       << newline
       << newline
       << newline
       << "#endif // end of defensive include" << newline;
}

void CPPgenerateCppBody(const COLstring& Name, const COLstring& Author, COLostream& Out){
   COL_FUNCTION(CPPgenerateCppBody);
   COLstring Prefix = CPPprefix(Name);
   Out << "// ---------------------------------------------------------------------------" << newline
       << "// Copyright (C) 1997-" << CPPtimeStamp("%Y") << " iNTERFACEWARE Inc.  All Rights Reserved" << newline
       << "//" << newline
       << "// Module: " << Name << newline
       << "//" << newline
       << "// Description:" << newline
       << "//" << newline
       << "// Implementation" << newline
       << "//" << newline
       << "// Author: " << Author << " " << newline
       << "// Date:   " << CPPtimeStamp("%A %d %B %Y - %I:%M%p") << newline
       << "// ---------------------------------------------------------------------------" << newline
       << newline
       << "#include <" << Prefix << "/" << Name << ".h>" << newline
       << newline
       << "#include <COL/COLlog.h>" << newline
       << "COL_LOG_MODULE;" << newline
       << newline;
}

bool CPPgenerateCode(const COLstring& Name, const COLstring& Author){
   COL_FUNCTION(CPPgenerateCode);
   COL_VAR(Name);
   if (!CPPhasPrefix(Name)){
      COLcout << "File name must be at least 4 characters or more and start with capital letters for a prefix" << newline;
      return false;
   }
   COLsinkString Sink;
   COLostream Out(Sink);
   CPPgenerateHeaderBody(Name, Author, Out);
   FILwriteFile(Name + ".h", Sink.string());
   Sink.string().clear();
   CPPgenerateCppBody(Name, Author, Out);
   FILwriteFile(Name + ".cpp", Sink.string());
   COLcout << "Remember to add generated files to git:" << newline << "git add " << Name << ".h " << Name << ".cpp" << newline;
   return true;
}
