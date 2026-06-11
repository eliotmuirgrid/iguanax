// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUlist
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 31 March 2023 - 10:09AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUurl.h>
COL_LOG_MODULE;

COLstring GITUrepoTypeString(GITUrepoType Type){
   switch (Type){
      case GIT_COMPONENT: return "component";
      case GIT_LIBRARY  : return "library";
      default           : return "component"; // need default case for some compilers
   }
}

GITUrepoInfo GITUlistComponent(const COLstring& Name, const COLstring& Description, const COLstring& GitUrl,
                               const COLstring& SourceGitUrl, const COLstring& Branch) {
   COL_FUNCTION(GITUlistComponent);
   GITUrepoInfo Repo;
   Repo.Name         = Name;
   Repo.Description  = Description;
   Repo.GitUrl       = GitUrl;
   Repo.RepoType     = GIT_COMPONENT;
   Repo.SourceGitUrl = SourceGitUrl;
   Repo.Branch       = Branch;
   return Repo;
}

GITUrepoInfo GITUlistLibrary(const COLstring& Name, const COLstring& Description, const COLstring& GitUrl,
                             const COLstring& SourceGitUrl, const COLstring& Branch) {
   COL_FUNCTION(GITUlistLibrary);
   GITUrepoInfo Repo;
   Repo.Name         = Name;
   Repo.Description  = Description;
   Repo.GitUrl       = GitUrl;
   Repo.RepoType     = GIT_LIBRARY;
   Repo.SourceGitUrl = SourceGitUrl;
   Repo.Branch       = Branch;
   return Repo;
}

GITUrepoInfo GITUlistModule(const COLstring& Dir, const COLstring& GitUrl, const COLstring& SourceGitUrl,
                            const COLstring& Branch) {
   COL_FUNCTION(GITUlistModule);
   GITUrepoInfo Repo;
   Repo.Name         = Dir;
   Repo.GitUrl       = GitUrl;
   Repo.RepoType     = GIT_LIBRARY;
   Repo.SourceGitUrl = SourceGitUrl;
   Repo.Branch       = Branch;
   return Repo;
}

COLostream& operator<<(COLostream& Stream, const GITUrepoInfo& Repo){
   COLstring Description = Repo.Description.substr(0,20);
   Description.replace("\n", "\\n");
   Stream << " type = " << GITUrepoTypeString(Repo.RepoType)
          << " name=" << Repo.Name
          << " desc = " << Description
          << " git  = " << Repo.GitUrl
          << " source=" << Repo.SourceGitUrl;
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const GITUlist& RepoList){
   Stream << "GITUlist size = " << RepoList.List.size() << indent << newline;
   for(int i = 0; i < RepoList.List.size(); i++) {
      Stream << RepoList.List[i] << newline;
   }
   Stream << unindent;
   return Stream;
}
