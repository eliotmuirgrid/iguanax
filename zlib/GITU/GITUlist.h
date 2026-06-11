#ifndef __GITU_LIST_H__
#define __GITU_LIST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUlist
//
// Description:
//
// Abstract object that represents a list of repos.
//
// Author: Eliot Muir
// Date:   Friday 31 March 2023 - 10:09AM
// ---------------------------------------------------------------------------

#include <COL/COLarray.h>

enum GITUrepoType{
   GIT_LIBRARY,
   GIT_COMPONENT
};

class GITUrepoInfo{
public:
   COLstring    Name;
   COLstring    Description;
   COLstring    GitUrl;
   GITUrepoType RepoType;
   COLstring    SourceGitUrl;  // normalized to SSH
   COLstring    Branch;
};

COLostream& operator<<(COLostream& Stream, const GITUrepoInfo& Repo);

GITUrepoInfo GITUlistComponent(const COLstring& Name, const COLstring& Description, const COLstring& GitUrl,
                               const COLstring& SourceGitUrl, const COLstring& Branch);
GITUrepoInfo GITUlistLibrary  (const COLstring& Name, const COLstring& Description, const COLstring& GitUrl,
                             const COLstring& SourceGitUrl, const COLstring& Branch);
GITUrepoInfo GITUlistModule   (const COLstring& Dir, const COLstring& GitUrl, const COLstring& SourceGitUrl,
                            const COLstring& Branch);
class GITUlist{
public:
   // The key is the HTTPS format of the GIT url
   COLarray<GITUrepoInfo> List;
};

COLostream& operator<<(COLostream& Stream, const GITUlist& RepoList);

COLstring GITUrepoTypeString(GITUrepoType Type);

#endif // end of defensive include