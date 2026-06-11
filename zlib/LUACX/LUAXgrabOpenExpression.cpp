//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXXgrabOpenExpression
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 10th, 2010 @ 11:26:06 AM
//---------------------------------------------------------------------------
#include "LUAXgrabOpenExpression.h"
#include "LUAXlexCode.h"

#include <COL/COLauto.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "lauxlib.h"
#include "llex.h"

static LUAXtoken* LUAXrfindBracket(LUAXtoken* pStart){
   //TODO - this doesn't actually handle balancing of mixed bracket types
   size_t BracketCount = 0;
   pStart = pStart->pNext;
   while(pStart){
      switch (pStart->Token){
      case '{':
      case '(':
         if (!BracketCount) { return pStart;}
         BracketCount--;
         break;
      case '}':
      case ')':
         BracketCount++;
         break;
      default: break;  //skip the rest
      }
      pStart = pStart->pNext;
   }
   return 0;
}

// TODO - turn into method?
static inline void LUAXsetLastPunctuation(char& LastPunctuation, char NewValue){
   if (!LastPunctuation) LastPunctuation = NewValue;
}

// TODO - turn into method?
static inline void LUAXsetIndexChar(char& IndexChar, char NewValue, char& LastPunctuation){
   IndexChar = NewValue;
   if (NewValue) LUAXsetLastPunctuation(LastPunctuation, NewValue);
}

static bool LUAXhandleCaseComma(LUAXautocompletionContext* pContext, LUAXparseState* pParserState){
   COL_FUNCTION(LUAXhandleCaseComma);
   LUAXsetLastPunctuation(pContext->LastPunctuation, ',');
   pParserState->pCurrentToken = LUAXrfindBracket(pParserState->pCurrentToken);
   if (!pParserState->pCurrentToken) return false;
   LUAXsetIndexChar(pContext->IndexChar, (char)pParserState->pCurrentToken->Token, pContext->LastPunctuation);
   pParserState->ExpressionState = LUAX_INDEXED;
   LUAXtoken* pTemp = pParserState->pCurrentToken;
   pTemp = pTemp->pNext; //closed expr ends just before the bracket
   if (pTemp && pParserState->ExpressionClosedPosition == npos) pParserState->ExpressionClosedPosition = pTemp->TokenEnd;
   return true;
}

static bool LUAXhandleCaseName(LUAXautocompletionContext* pContext, LUAXparseState* pParserState){
   COL_FUNCTION(LUAXhandleCaseName);
   LUAXtoken*& pCurrentToken = pParserState->pCurrentToken;
   if (pCurrentToken->TokenEnd < pContext->globalCursorPosition()){
      COL_TRC("The cursor is after this name.  This could mean the user typed some whitespace after the name, or it could be an incomplete string (i.e. print 'hello)");
      return false;
   }
   pParserState->ExpressionStartPosition = pCurrentToken->TokenEnd - pCurrentToken->TokenSize;
   LUAXsetIndexChar(pContext->IndexChar, '\0', pContext->LastPunctuation);
   pParserState->ExpressionState = LUAX_NAME;
   LUAXtoken* pTemp = pCurrentToken;
   pCurrentToken = pCurrentToken->pNext;
   if (pCurrentToken && (pCurrentToken->Token == '(' || pCurrentToken->Token == '{')){
      LUAXsetIndexChar(pContext->IndexChar, (char)pCurrentToken->Token, pContext->LastPunctuation);
      pParserState->ExpressionState = LUAX_INDEXED; 
   } else if (pCurrentToken && pCurrentToken->Token == ','){
      return LUAXhandleCaseComma(pContext, pParserState);
   } else { pCurrentToken = pTemp; }
   COL_TRC("Returning true");
   return true;
}

static bool LUAXhandleCaseNumber(LUAXautocompletionContext* pContext, LUAXparseState* pParserState){
   COL_FUNCTION(LUAXhandleCaseNumber);
   // #15664 - we do some manipulation here to treat numbers as search strings.
   // (the problem is that Lua will tokenize something like "In.5" into two tokens,
   // "In" and ".5").
   size_t NearestDot = pContext->m_Code.rfind(".", pParserState->pCurrentToken->TokenEnd-1);
   size_t NearestColon = pContext->m_Code.rfind(":", pParserState->pCurrentToken->TokenEnd-1);
   bool ColonFound = (NearestColon != npos && NearestColon > NearestDot);
   if (ColonFound) NearestDot = NearestColon;
   size_t TokenBegin = pParserState->pCurrentToken->pNext ? pParserState->pCurrentToken->pNext->TokenEnd : 0;
   if (NearestDot != npos && NearestDot >= TokenBegin){
      COLstring BeforeDot = pContext->m_Code.substr(TokenBegin, NearestDot-TokenBegin);
      if (!BeforeDot.isWhitespace()) return false;
      size_t NumberStart = NearestDot+1;
      pContext->SearchString = pContext->m_Code.substr(NumberStart, pParserState->pCurrentToken->TokenEnd - NumberStart);
      LUAXsetIndexChar(pContext->IndexChar, ColonFound ? ':' : '.', pContext->LastPunctuation);
   } else {
      pParserState->pCurrentToken = pParserState->pCurrentToken->pNext;
      if (pParserState->pCurrentToken || (pParserState->pCurrentToken->Token != '[' && pParserState->pCurrentToken->Token != '.' && pParserState->pCurrentToken->Token != ':')) return false;
      LUAXsetIndexChar(pContext->IndexChar, (char)pParserState->pCurrentToken->Token, pContext->LastPunctuation);
   }
   pParserState->ExpressionState = LUAX_INDEXED;
   return true;
}

static bool LUAXhandleCaseBracketDotColon(LUAXautocompletionContext* pContext, LUAXparseState* pParserState){
   COL_FUNCTION(LUAXhandleCaseBracketDotColon);
   char TokenChar = (char)pParserState->pCurrentToken->Token;
   if (TokenChar != '[' && pParserState->SearchStringIsStringLiteral){ return false; }// Fix for #21438.
   LUAXsetIndexChar(pContext->IndexChar, TokenChar, pContext->LastPunctuation);
   pParserState->ExpressionState = LUAX_INDEXED;
   return true;
}

static void LUAXupdateExpressionAndPunctuationCloseIndex(LUAXautocompletionContext* pContext,LUAXparseState* pParserState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuationCloseIndex);
   LUAXsetLastPunctuation(pContext->LastPunctuation, ']');
   if (pParserState->ExpressionState == LUAX_NAME) { pParserState->DoneExpressionSearch = true; }
   else if (pParserState->ExpressionState != LUAX_IN_PARENTHESES) {
      pParserState->CountOfOpenBracketsOrParenthesis++;
      if (pParserState->ExpressionState != LUAX_IN_BRACKETS){
         if (pParserState->ExpressionClosedPosition == npos) pParserState->ExpressionClosedPosition = pParserState->pCurrentToken->TokenEnd; // Mark the "closed" position
         pParserState->ExpressionState = LUAX_IN_BRACKETS;
      }
   }
}

static void LUAXupdateExpressionAndPunctuationOpenIndex(LUAXautocompletionContext* pContext, LUAXparseState* pParserState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuationOpenIndex);
   LUAXsetLastPunctuation(pContext->LastPunctuation, '[');
   if (pParserState->ExpressionState == LUAX_INDEXED) { COL_ERROR_STREAM_PLAIN("Unexpected '['", COLerror::ParsingError); }
   else if (pParserState->ExpressionState == LUAX_IN_BRACKETS) {
      if (--(pParserState->CountOfOpenBracketsOrParenthesis) == 0){
         if (!pContext->IndexChar) LUAXsetIndexChar(pContext->IndexChar, '[', pContext->LastPunctuation);
         pParserState->ExpressionState = LUAX_INDEXED;
      }
   }  
   else if (pParserState->ExpressionState == LUAX_NAME) { pParserState->DoneExpressionSearch = true; } // end expression
}

static void LUAXupdateExpressionAndPunctuationCloseBracket(LUAXautocompletionContext* pContext, LUAXparseState* pParseState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuationCloseBracket);
   LUAXsetLastPunctuation(pContext->LastPunctuation, (char)pParseState->pCurrentToken->Token);
   if (pParseState->ExpressionState == LUAX_NAME) { pParseState->DoneExpressionSearch = true; COL_TRC("End expression");}
   else if (pParseState->ExpressionState != LUAX_IN_BRACKETS) {
      pParseState->CountOfOpenBracketsOrParenthesis++;
      COL_TRC("Increment bracket count");
      COL_VAR(pParseState->CountOfOpenBracketsOrParenthesis);
      if (pParseState->ExpressionState != LUAX_IN_PARENTHESES) {
         COL_TRC("Mark the 'closed' position");
         if (pParseState->ExpressionClosedPosition == npos) pParseState->ExpressionClosedPosition = pParseState->pCurrentToken->TokenEnd;
         pParseState->ExpressionState = LUAX_IN_PARENTHESES;
      }
   }
}

static void LUAXupdateExpressionAndPunctuationOpenBracket(LUAXautocompletionContext* pContext, LUAXparseState* pParseState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuationOpenBracket);
   LUAXsetLastPunctuation(pContext->LastPunctuation, (char)pParseState->pCurrentToken->Token);
   if (pParseState->ExpressionState == LUAX_INDEXED) { COL_ERROR_STREAM_PLAIN("Unexpected '('", COLerror::ParsingError); }
   else if (pParseState->ExpressionState == LUAX_IN_PARENTHESES) {
      if (--(pParseState->CountOfOpenBracketsOrParenthesis) == 0) pParseState->ExpressionState = LUAX_INDEXED;
   }
   else if (pParseState->ExpressionState == LUAX_NAME) { pParseState->DoneExpressionSearch = true; } // end expression
}

static void LUAXupdateExpressionAndPunctuationDotColon(LUAXautocompletionContext* pContext, LUAXparseState* pParserState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuationDotColon);
   LUAXsetLastPunctuation(pContext->LastPunctuation, (char)pParserState->pCurrentToken->Token);
   if (pParserState->ExpressionState == LUAX_INDEXED) { COL_ERROR_STREAM_PLAIN("Unexpected '" << ((char)pParserState->pCurrentToken->Token) << "'", COLerror::ParsingError); }
   else if (pParserState->ExpressionState == LUAX_NAME) {
      if (!pContext->IndexChar) LUAXsetIndexChar(pContext->IndexChar, (char)pParserState->pCurrentToken->Token, pContext->LastPunctuation);
      pParserState->ExpressionState = LUAX_INDEXED;
   }
}

static void LUAXupdateExpressionAndPunctuationName(LUAXparseState* pParserState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuationName);
    if (pParserState->ExpressionState == LUAX_INDEXED) {
      // Expressions, for our purposes, always start with a TK_NAME token.
      pParserState->ExpressionStartPosition = pParserState->pCurrentToken->TokenEnd - pParserState->pCurrentToken->TokenSize;
      // Mark the "closed" position
      if (pParserState->ExpressionClosedPosition == npos) pParserState->ExpressionClosedPosition = pParserState->pCurrentToken->TokenEnd;
      pParserState->ExpressionState = LUAX_NAME;
   }
   else if (pParserState->ExpressionState == LUAX_NAME) { pParserState->DoneExpressionSearch = true; } // end expression
}

static void LUAXupdateExpressionAndPunctuationStringNumber(LUAXparseState* pParserState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuationStringNumber);
   if (pParserState->ExpressionState == LUAX_INDEXED)   { COL_ERROR_STREAM_PLAIN("Unexpected string or number token", COLerror::ParsingError); }
   else if (pParserState->ExpressionState == LUAX_NAME) { pParserState->DoneExpressionSearch = true; } // end expression
}

static void LUAXupdateExpressionAndPunctuation(LUAXautocompletionContext* pContext, LUAXparseState* pParserState){
   COL_FUNCTION(LUAXupdateExpressionAndPunctuation);
   while (pParserState->pCurrentToken && !pParserState->DoneExpressionSearch){
      COL_VAR(pParserState->pCurrentToken->Token);  // TODO - can we express the code?
      switch(pParserState->pCurrentToken->Token){
      case ']'       : LUAXupdateExpressionAndPunctuationCloseIndex  (pContext, pParserState); break;
      case '['       : LUAXupdateExpressionAndPunctuationOpenIndex   (pContext, pParserState); break;
      case '}'       :
      case ')'       : LUAXupdateExpressionAndPunctuationCloseBracket(pContext, pParserState); break;
      case '{'       :
      case '('       : LUAXupdateExpressionAndPunctuationOpenBracket (pContext, pParserState); break;
      case '.'       :
      case ':'       : LUAXupdateExpressionAndPunctuationDotColon    (pContext, pParserState); break;
      case TKX_NAME  : LUAXupdateExpressionAndPunctuationName(pParserState); break;
      case TKX_STRING:
      case TKX_NUMBER: LUAXupdateExpressionAndPunctuationStringNumber(pParserState);           break;
      case '='       : LUAXsetLastPunctuation(pContext->LastPunctuation, '='); // no break
      default        : if (pParserState->ExpressionState == LUAX_NAME || pParserState->ExpressionState == LUAX_INDEXED) { pParserState->DoneExpressionSearch = true; } // end expression
      }
      pParserState->pCurrentToken = pParserState->pCurrentToken->pNext;
   }
}

bool LUAXgrabOpenExpression(LUAXautocompletionContext* pContext){
   COL_FUNCTION(LUAXgrabOpenExpression);
   COL_DUMP("Code:", pContext->m_Code.c_str(), pContext->m_Code.size());
   COL_VAR(*pContext);
   LUAXparseState ParseState;   
   luax_State* L = lua_open();
   COLauto<LUAXtoken> pTail = LUAXlexCode(L, pContext->m_Code, "LUAXlexCode", pContext->globalCursorPosition(), true, pContext->SearchString, ParseState.SearchStringIsStringLiteral);
   lua_close(L);
   ParseState.pCurrentToken = pTail.get();
   pContext->IndexChar       = '\0';
   pContext->LastPunctuation = '\0';
   if (!ParseState.pCurrentToken) return false;
   // Doing a separate switch for initial state is easier than trying to work it into the main loop's switch.
   switch(ParseState.pCurrentToken->Token){
      case TKX_NAME:   if(!LUAXhandleCaseName  (pContext, &ParseState)) { return false; } break;
      case TKX_NUMBER: if(!LUAXhandleCaseNumber(pContext, &ParseState)) { return false; } break;
      case ',':        if(!LUAXhandleCaseComma (pContext, &ParseState)) { return false; } break;
      case TKX_STRING:
         if (!ParseState.pCurrentToken->pNext) { return false; } // IX-4369 pNext is null, need to exit
         ParseState.pCurrentToken = ParseState.pCurrentToken->pNext;
         if (ParseState.pCurrentToken->Token != '[') return false; // else, we can continue through to the case below.
      case '[':
      case '.':
      case ':':
      case '(':
      case '{': if(!LUAXhandleCaseBracketDotColon(pContext, &ParseState)) { return false; } break;
      case '=': LUAXsetLastPunctuation(pContext->LastPunctuation, '=');
         // no break
      default: return false;
   }
   ParseState.pCurrentToken = ParseState.pCurrentToken->pNext;
   LUAXupdateExpressionAndPunctuation(pContext, &ParseState);
   if (ParseState.ExpressionStartPosition == npos) { return false; }
   pContext->FullExpression = pContext->m_Code.substr(ParseState.ExpressionStartPosition, pContext->globalCursorPosition() - ParseState.ExpressionStartPosition);
   COL_VAR(pContext->FullExpression);
   if (pContext->FullExpression.find('\n') != npos){
      COL_ERROR_STREAM_PLAIN("For auto-completion to work, expression cannot span multiple lines.", COLerror::ParsingError); // This limitation is due to CodeMirror (on the client-side).
   }
   if (ParseState.ExpressionClosedPosition == npos){
      pContext->TrimmedExpression.clear();
      pContext->SearchString = pContext->FullExpression;
   } else {
      long length = ParseState.ExpressionClosedPosition - ParseState.ExpressionStartPosition;
      if (length <= 0) { //Don't try to get a substring of a negative length. There is no valid trimmed expression here, so just clear it.
         pContext->TrimmedExpression.clear();
      } else {
         pContext->TrimmedExpression = pContext->m_Code.substr(ParseState.ExpressionStartPosition, (size_t)length);
      }
   }
   return true;
}

COLostream& operator<<(COLostream& Stream, const LUAXautocompletionContext& Object){
   Stream << "Context: ";
   return Stream;
}