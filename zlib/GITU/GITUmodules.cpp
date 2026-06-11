// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUmodules
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 13 February 2023 - 03:11PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUcollection.h>
#include <GITU/GITUmodules.h>
#include <GIT/GITpath.h>
#include <INI/INIdata.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

// TODO - search for .gitmodules and try eliminate redundancy
COLstring GITUsubmoduleFile(const COLstring& GitUrl, const COLstring& Branch){
   COL_FUNCTION(GITUsubmoduleFile);
   COLstring FileName = GITgitTemplateDir(GitUrl, Branch) + ".gitmodules";
   COL_VAR2(FileName, GitUrl);
   return FileName;
}

COLstring GITUsubmoduleFileInRepo(const COLstring& RepoDir){
   COL_FUNCTION(GITUsubmoduleFileInRepo);
   COLstring File = RepoDir;
   FILaddPathSeparator(File);
   File += ".gitmodules";
   return File;
}

// We are parsing this format.
/*
[submodule "CHK"]
        path = CHK
        url = git@bitbucket.org:interfaceware/chk.git
[submodule "LLPC"]
        path = LLPC
        url = git@bitbucket.org:interfaceware/llpc.git
*/
// Loads the GitUrl and Name - but not the commit IDs
void GITUmoduleLoad(const COLstring& FileName, COLmap<COLstring, GITUmodule>* pList) {
   COL_FUNCTION(GITUmoduleLoad);
   COLstring Content;
   try {
      FILreadFile(FileName, &Content);
   } catch(const COLerror& E) { COL_TRC(E.description()); }
   COL_VAR2(FileName, Content);
   INIdata Config;
   INIparse(Content, &Config);
   for(auto i = Config.PrivateData.cbegin(); i != Config.PrivateData.cend(); i++) {
      if(i->first.substr(0, 9) == "submodule") {
         COLstring Url = "";
         COLstring Branch = "";
         if(i->second.count("url")) { Url = i->second["url"]; }
         if(i->second.count("branch")) { Branch = i->second["branch"]; }
         if(i->second.count("path")) {
            pList->add(i->second["path"], GITUmodule(Url, Branch));
            COL_TRC("Added " << i->second["path"] << " and " << Url << " and " << Branch);
         }
      }
   }
}

static COLstring GITUmoduleName(COLarray<COLstring> Fields){
   COL_FUNCTION(GITUmoduleName);
   COLstring ModuleName = "";
   for(int i = 3; i < Fields.size(); i++){
      ModuleName += Fields[i];
      if(i != Fields.size() - 1) ModuleName += " ";
   }
   return ModuleName;
}

void GITUmoduleGetCommits(const COLstring& Dir, const COLstring& RelPath, const COLstring& CommitId, COLmap<COLstring, GITUmodule>* pList){
   COL_FUNCTION(GITUmoduleGetCommits);
   COL_VAR(Dir);
   COLstring Out, Err;
   COLstring Args = CommitId.is_null() ? "HEAD" : "-r " + CommitId;
   int ExitCode = PROexecuteString(Dir, "git ls-tree " + Args, &Out, &Err);
   COL_VAR(Out);
   COL_DUMP("Data:", Out.c_str(), Out.size());
   Out.replace("\r\n", "\n"); // for windoze
   COLarray<COLstring> Lines;
   COLsplit(&Lines, Out, "\n");
   for (auto i=Lines.begin(); i != Lines.end(); ++i){
      COL_DUMP("Line", i->c_str(), i->size());
      COLarray<COLstring> Fields;
      i->replace('\t', ' ');
      COLsplit(&Fields, *i, " ");
      COL_VAR(Fields.size());
      if (Fields.size() >= 4){
         COLstring Module = GITUmoduleName(Fields);
         COLstring CommitId = Fields[2];
         // .gitmodules stores git paths with forward slashes, so normalize the
         // lookup key to that format before comparing against ModuleList.
         COLstring FullPath = FILpathSepToPosix(RelPath + Module);
         COL_VAR2(FullPath, CommitId);
         if (pList->count(FullPath) > 0){
            auto j = pList->find(FullPath);
            j.value().CommitId = CommitId;
            COL_TRC("Module " << FullPath << " gets commit ID = " << CommitId);
         }
         else {
            COL_TRC("Module " << FullPath << " not found");
         }
      }
   }
}

COLostream& operator<<(COLostream& Stream, const COLmap<COLstring, GITUmodule>& ModuleList){
   Stream << "Submodules:" << newline;
   for (auto i=ModuleList.begin(); i != ModuleList.end(); ++i){
      Stream << i->first << " (repo=" << i->second.GitUrl << ", commit= " << i->second.CommitId << ")" << newline;
   }
   return Stream;
}
