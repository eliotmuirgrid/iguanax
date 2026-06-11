#ifndef __GSVMAP_H__
#define __GSVMAP_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GSVmap.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/07/24 11:38 AM
//  ---------------------------------------------------------------------------
#include <GITU/GITUurl.h>
class COLvar;

struct GSVitem {
	explicit GSVitem(bool Secure=true, const GITUserver& Type=GIT_bitbucket_cloud, int HttpPort=0, int SshPort=0) : m_Secure(Secure), m_Type(Type), m_HttpPort(HttpPort), m_SshPort(SshPort) {}
	bool      m_Secure = true;
	GITUserver m_Type   = GIT_bitbucket_cloud;
	// If these are 0, the default ports for http(s) and ssh are okay to use.
	int 		 m_HttpPort = 0;
	int 		 m_SshPort  = 0;
};

void GSVmapInit();
void GSVmapAdd(const COLstring& Host, const GITUserver& Server, bool Secure, int HttpPort, int SshPort);

bool GSVmapExists(const COLstring& Host);
bool GSVmapSet(const COLvar& Data);
bool GSVmapSave(COLstring* pErr);

GSVitem GSVmapItem(const COLstring& Host);

COLvar GSVmaptoVar();
COLvar GSVitemToVar(const GSVitem& Item);

bool GSVisDefaultHost(const COLstring& Host);
// TODO - move these?
COLstring GSVserviceName(const GITUserver& Host);
GITUserver GSVserviceEnumVal(const COLstring& Host);
COLstring GSVdefaultHost(const GITUserver& Host);

bool GSVserviceIsCloud(const GITUserver& Host);
#endif  // end of defensive include
