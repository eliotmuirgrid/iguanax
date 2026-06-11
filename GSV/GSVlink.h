#ifndef __GSVLINK_H__
#define __GSVLINK_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GSVlink.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   05/07/24 12:19 PM
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <GITU/GITUurl.h>
class COLvar;

struct GSVlink {
	COLstring m_Web;
	COLstring m_Web_Branch;
	COLstring m_Httpx; // this is http or https, depending on the item
	COLstring m_Ssh;
};

COLstring GSVconstructBranchLink(const GITUurl& Url, const COLstring& WebResult, const COLstring& Branch);
// if repo is from unrecognized server, default to bitbucket formats
bool GSVconstructLink(const COLstring& Repo, GSVlink* pLink, const COLstring& Branch="");
// this makes the link to the server itself, where all the repos are hosted
bool GSVconstructServerLink(const COLstring& ServerUrl, COLstring* pServerLink);

COLvar GSVlinkToVar(const GSVlink& Link);

#endif // end of defensive include
