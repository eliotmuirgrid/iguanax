#ifndef __NOTI_DETECT_H__
#define __NOTI_DETECT_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIdetect
//
//  Description:
//
//  Functional code to detect if notification rules have been triggered
//
//  Author: Eliot Muir
//  Date:   Tue 13 Sep 2022 15:10:50 EDT
// ---------------------------------------------------------------------------
class COLvar;
class BLUAinstanceManager;
class NOTImodel;
class NOTIrule;
class SDBmessage;

struct CFGconfig;
struct CFGmap;

void NOTIdetectPoll(CFGmap* pMap, NOTImodel* pModel, BLUAinstanceManager* pManager);

// This returns the number of triggers
int NOTIdetectAll(CFGmap* pMap, NOTImodel* pModel, BLUAinstanceManager* pManager);  // go through all the rules

void NOTIpatternRule(SDBmessage* pMessage, NOTIrule* pRule, const CFGconfig& ComponentConfig, COLvar* pNotifications);

// Rule called as a callback from the logging system
void NOTIlogContainsPattern(SDBmessage* pMessage, CFGmap* pMap, NOTImodel* pModel, BLUAinstanceManager* pManager);

// for unit test
void NOTIdetectRule(const CFGmap& Map, NOTIrule* pRule, COLvar* pNotifications);

#endif // end of defensive include
