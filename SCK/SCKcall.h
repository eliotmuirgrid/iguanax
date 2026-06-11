#ifndef __SCK_CALL_H__
#define __SCK_CALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKcall
//
// Description:
//
// Code to implement a call from worker thread to other threads.
//
// Author: Vismay Shah
// Date:   Thursday 30 March 2023 - 12:47PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLmap.h>
#include <COL/COLweb.h>
class COLstring;
class COLvar;
class SCKloop;
class COLevent;

// Call this to setup the singleton used by SCKcall
void SCKdefineSingleton(SCKloop* pLoop, COLhashmap<COLstring, COLwebHandlerFunc>* pFuncMap, COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pDataMap);
void SCKcallInternalResponse(const COLvar& FromApi, COLvar* pOut, COLevent* pEvent);

// This is used by Iguana to get the list of endpoints for iguana.call
void SCKlistApis(COLmap<COLstring, bool>* pOut);

// Make sure pOut is a reference of a COLvar created in the worker thread.
// Agent is typically a component when SCKcall from Lua
// If SCKcall from Lua as a specific user, Agent is the username and ComponentAgent should be false. See LIGiguanaCall for example.
bool SCKcall(const COLstring& CallPath, const COLvar& In, const COLstring& Agent, COLvar* pOut, bool ComponentAgent = true);

// This form for unit tests ONLY
bool SCKcall(const COLstring& CallPath, const COLvar& In, COLvar* pOut, COLhashmap<COLstring, COLwebHandlerFunc>* pWorkerThreadMap,
             COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >* pMainThreadMap, SCKloop* pLoop);

#endif // end of defensive include
