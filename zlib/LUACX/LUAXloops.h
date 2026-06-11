#ifndef __LUAX_LOOPS_H__
#define __LUAX_LOOPS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXscope
//
// Description:
//
// LUAXscope represents a scope of annotations.  These can have children nested recursively.
//
// Author: Kevin Senn
// Date:   Wednesday, October 12th, 2011 @ 03:06:25 PM
//---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLarray.h>

// Can represent the scope of a Lua function, or the scope of a loop.
// POD - allow default copy constructor and assignment operator.

#define TRN_UNKNOWN_TARGET_ITERATION -1

class TRNline{
public:
   TRNline() : m_Count(0) {}
   int m_Count;
   COLstring m_Data;
};

class LUAXscope{
public:
   LUAXscope();
   ~LUAXscope();

   // Start/end lines for scope
   int m_Start;
   int m_End;

   enum eType{
      FUNCTION,
      DO_BLOCK,
      LOOP
   };

   eType m_Type;

   // Start/end of of scope (in characters).
   size_t m_StartTokenPosition; // last character of beginning token
   size_t m_EndTokenPosition; // first character of end token

   COLarray<LUAXscope> m_Scopes;
   bool m_Initialized;
   int m_CurrentIteration;
   int m_TargetIteration; // This is the iteration we want to collect data on.
   bool m_IterationOver; // Do we have to start a new iteration for any lines found?
   int m_MostRecentLine;
   COLhashmap<int, TRNline> m_Lines;
};

int LUAXscopeTargetIteration(const LUAXscope& Scope);

COLostream& operator<<(COLostream& Stream, const LUAXscope& Scope);

// Find loops in the first function defined in LuaCode after StartPosition.
// Scope will be set to be the function's scope, with its inner loop scopes
// populated.
void LUAXfindLoops(const COLstring& LuaCode, int StartPosition, int StopPosition, LUAXscope& Scope);

// Populates the scope for all functions in a Lua chunk.  The main chunk
// will be considered a "function", which is consistent with how Lua treats it.
// Scope's various start/end members will not be populated (they will be -1).
void LUAXfindAllScopes(const COLstring& LuaCode, LUAXscope& Scope);

#endif // end of defensive include
