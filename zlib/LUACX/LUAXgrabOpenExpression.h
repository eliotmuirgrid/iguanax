#ifndef __LUAX_GRAB_OPEN_EXPRESSION_H__
#define __LUAX_GRAB_OPEN_EXPRESSION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXgrabOpenExpression
//
// Description:
//
// Hacked code from Lua 5.1.4 which 
//
// Author: Eliot Muir and Kevin Senn
// Date:   Thursday, June 10th, 2010 @ 11:20:31 AM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Returns true if the expression is "open" (incomplete), false otherwise.
// An examples of a "closed"/complete expression is "In['MSH']".  Note that
// while "In.MSH" may be complete, we can't guarantee that, since we aren't actually
// evaluating "In" to see if "MSH" is complete.
//
// Note that an expression for which auto-completion does not make sense is
// not considered to be "open", e.g. "foo('a" (see #21438).
//
// If the expression is incomplete, FullExpression, TrimmedExpression, SearchString,
// IndexChar, and LastPunctuation will be populated.
//
// IndexChar will be set to the character being used to index ('[', '.', ':', '(', or '{').
// IndexChar will be '\0' if there is an open expression, but no indexing in the expression.
//
// LastPunctuation will be set to the last significant punctuation before the
// GlobalCursorPosition.  For example, '[', ']', '(', ')', '.', ':', ',', or '='.
// So it will often be the same as IndexChar.
//
// For example, if the LuaCode at LineNumber is "In.PID['Nam']", and the cursor position is
// 8 (between the 'm' and the '\''), then the three string outputs will be populated as
// follows:
//    FullExpression = "In.PID['Nam']"
//    TrimmedExpression = "In.PID"
//    SearchString = "Nam"
//    IndexChar = '['

#include <COL/COLstring.h>

class LUAXtoken;

enum LUAXexpressionParserState{
   LUAX_INITIAL,
   LUAX_NAME,
   LUAX_IN_BRACKETS,
   LUAX_IN_PARENTHESES,
   LUAX_INDEXED
};

class LUAXparseState{
public:
   bool SearchStringIsStringLiteral = false;
   size_t ExpressionStartPosition = npos;
   size_t ExpressionClosedPosition = npos;
   size_t CountOfOpenBracketsOrParenthesis = 0;
   LUAXexpressionParserState ExpressionState = LUAX_INITIAL;
   bool DoneExpressionSearch = false;
   LUAXtoken* pCurrentToken = NULL;
};

class LUAXautocompletionContext{
public:
   LUAXautocompletionContext() : m_Cursor(0), m_BeginLine(0), IndexChar(0),LastPunctuation(0){}

   COLstring m_Code;
   int m_Cursor;
   int m_BeginLine;

   COLstring FullExpression;
   COLstring TrimmedExpression;
   COLstring SearchString; // The (possibly) incomplete portion of the expression which was chopped off.
   char      IndexChar; // The character being used to index ('[', '.', or ':').
   char      LastPunctuation; // The closest punctuation character before the target position.

   size_t globalCursorPosition() { return m_BeginLine + m_Cursor;} 
};

COLostream& operator<<(COLostream& Stream, const LUAXautocompletionContext& Object);

bool LUAXgrabOpenExpression(LUAXautocompletionContext* pOutput);


#endif // end of defensive include
