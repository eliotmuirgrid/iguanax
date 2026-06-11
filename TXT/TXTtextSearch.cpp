//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTtextSearch
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Wednesday, April 15th, 2009 @ 09:19:27 AM
//
//---------------------------------------------------------------------------
#include "TXTprecomp.h"
#pragma hdrstop

#include "TXTtextSearch.h"

#include "TXTsearchUtils.h"  // for TXTcaseInsensitiveSearch()

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <ctype.h>


inline const char* readRegex(const char* pString, REXmatcher& Output)
{
   COL_FUNCTION(readRegex);

   COLstring Regex;

   for(;;)
   {
      size_t SpanSize = strcspn(pString, "/\\");
      if( SpanSize )
      {
         Regex.append(pString, SpanSize);
         pString += SpanSize;
      }

      switch(*pString)
      {
      case '\\':
         if( *++pString )
         {
            Regex.append(pString-1, 2);
            ++pString;
         }
         break;
      case '/':
         ++pString;
         /* no break */
      default: // NUL character
         COL_TRC("Found regex: " << Regex);
         Output.init(Regex, REXmatcher::CaseInsensitive);
         return pString;
      }
   }
}

inline const char* readPhrase(const char* pString, COLstring& Output)
{
   COL_FUNCTION(readPhrase);

   for(;;)
   {
      size_t SpanSize = strcspn(pString, "\"\\");
      if( SpanSize )
      {
         Output.append(pString,SpanSize);
         pString += SpanSize;
      }

      switch(*pString)
      {
      case '\\':
         if( *++pString )
         {
            Output += *pString++;
         }
         break;
      case '"':
         ++pString;
         /* no break */
      default: // NUL character
         COL_TRC("Found phrase: \"" << Output << '"');
         return pString;
      }
   }
}

inline const char* readWord(const char* pString, COLstring& Output)
{
   COL_FUNCTION(readWord);

   size_t SpanSize = strcspn(pString, " \t\"()");
   if( SpanSize )
   {
      Output.append(pString,SpanSize);
      pString += SpanSize;
   }

   COL_TRC("Found word: " << Output);
   return pString;
}


// Allow default copy constructor and assignment operator.
struct TXTtextSearchItem
{
   enum ItemType_t
   {
      DONE,
      LITERAL,
      REGEX,
      GROUP_START,
      GROUP_END,
      DISJUNCTION,
      NEGATION,
   };
   
   ItemType_t ItemType;
   COLstring  Literal;
   REXmatcher Regex;
};


TXTtextSearch::TXTtextSearch()
{
   pSearchItems = new COLarray<TXTtextSearchItem>;

   try
   {
      pSearchItems->push_back().ItemType = TXTtextSearchItem::DONE;
   }
   catch(...)
   {
      delete pSearchItems;
      throw;
   }
}

TXTtextSearch::~TXTtextSearch()
{
   delete pSearchItems;
}


TXTtextSearch::TXTtextSearch(const TXTtextSearch& Other)
{
   pSearchItems = new COLarray<TXTtextSearchItem>;

   try
   {
      *pSearchItems = *Other.pSearchItems;
   }
   catch(...)
   {
      delete pSearchItems;
      throw;
   }
}

TXTtextSearch& TXTtextSearch::operator = (const TXTtextSearch& Other)
{
   if( &Other != this )
   {
      *pSearchItems = *Other.pSearchItems;
   }
   
   return *this;
}


void TXTtextSearch::init(const COLstring& Query)
{
   COL_METHOD(TXTtextSearch::init);

   pSearchItems->clear();

   int OpenGroups = 0;

   for(const char* pString = Query.c_str(); *pString;)
   {
      if( COLisspace(*pString) )
      {
         ++pString;
         continue;
      }

      TXTtextSearchItem& Item = pSearchItems->push_back();

      COL_VAR3(*pString, OpenGroups, pSearchItems->size());
      switch(*pString++)
      {
      case '-':
         Item.ItemType = TXTtextSearchItem::NEGATION;
         COL_TRC("NEGATION (-)");
         break;

      case '(':
         Item.ItemType = TXTtextSearchItem::GROUP_START;
         ++OpenGroups;
         COL_TRC("GROUP_START " << OpenGroups);
         break;

      case ')':
         if( !OpenGroups )
         {
            COL_WRN("Superfluous close-parenthesis.");
            pSearchItems->pop_back();
            break;
         }

         Item.ItemType = TXTtextSearchItem::GROUP_END;
         COL_TRC("GROUP_END " << OpenGroups);
         --OpenGroups;
         break;

      case '/':
         Item.ItemType = TXTtextSearchItem::REGEX;
         pString = readRegex(pString, Item.Regex);
         COL_TRC("REGEX");
         break;
         
      case '"':
         Item.ItemType = TXTtextSearchItem::LITERAL;
         pString = readPhrase(pString, Item.Literal);
         COL_TRC("LITERAL (phrase)");
         break;

      default:
         Item.ItemType = TXTtextSearchItem::LITERAL;
         pString = readWord(pString-1, Item.Literal);

         if( Item.Literal == "OR" )
         {
            Item.ItemType = TXTtextSearchItem::DISJUNCTION;
            COL_TRC("DISJUNCTION");
         }
         else if( Item.Literal == "NOT" )
         {
            Item.ItemType = TXTtextSearchItem::NEGATION;
            COL_TRC("NEGATION (NOT)");
         }
         else
         {
            COL_TRC("LITERAL (word)");
         }
      }
   }

   while(OpenGroups--)
   {
      pSearchItems->push_back().ItemType = TXTtextSearchItem::GROUP_END;
      COL_TRC("GROUP_END (missing)");
   }

   pSearchItems->push_back().ItemType = TXTtextSearchItem::DONE;
   COL_TRC("DONE");
}


static void skipToNext(const TXTtextSearchItem*& pSearchItem)
{
   COL_FUNCTION(skipToNext);

   int OpenGroups = 0;
   for(;; ++pSearchItem)
   {
      switch(pSearchItem->ItemType)
      {
      case TXTtextSearchItem::DONE:
         COL_TRC("DONE");
         return;
      case TXTtextSearchItem::GROUP_START:
         ++OpenGroups;
         COL_VAR(OpenGroups);
         break;
      case TXTtextSearchItem::GROUP_END:
         if( !OpenGroups )
         {
            COL_TRC("GROUP_END");
            ++pSearchItem;
            return;
         }
         --OpenGroups;
         COL_VAR(OpenGroups);
      default:
         break;
        // do nothing.
      }
   }
}

static bool skipToAlternate(const TXTtextSearchItem*& pSearchItem)
{
   COL_FUNCTION(skipToAlternate);

   int OpenGroups = 0;
   for(;; ++pSearchItem)
   {
      switch(pSearchItem->ItemType)
      {
      case TXTtextSearchItem::DONE:
         COL_TRC("DONE (no alternate)");
         return false;
      case TXTtextSearchItem::GROUP_START:
         ++OpenGroups;
         COL_VAR(OpenGroups);
         break;
      case TXTtextSearchItem::GROUP_END:
         if( !OpenGroups )
         {
            COL_TRC("GROUP_END (no alternate)");
            ++pSearchItem;
            return false;
         }
         --OpenGroups;
         COL_VAR(OpenGroups);
         break;
      case TXTtextSearchItem::DISJUNCTION:
         if( !OpenGroups )
         {
            COL_TRC("Found alternate.");
            ++pSearchItem;
            return true;
         }
      default:
         break;
         // do nothing
      }
   }
}

static bool matches(const char* pString, const TXTtextSearchItem*& pSearchItem)
{
   COL_FUNCTION(matches);

   bool Success = true;  // For tests.  Normally we want to succeed.

   for(;;)
   {
      switch(pSearchItem->ItemType)
      {
      case TXTtextSearchItem::GROUP_END:
         COL_TRC("GROUP_END (success)");
         ++pSearchItem;
         return true;

      case TXTtextSearchItem::DONE:
         COL_TRC("DONE (success)");
         return true;

      case TXTtextSearchItem::GROUP_START:
         COL_TRC("GROUP_START");
         if( matches(pString, ++pSearchItem) != Success && !skipToAlternate(pSearchItem) )
         {
            COL_TRC("Group failed, no alternate.");
            return false;
         }
         break;

      case TXTtextSearchItem::LITERAL:
         COL_TRC("LITERAL");
         if( !!TXTcaseInsensitiveSearch(pString, pSearchItem->Literal.c_str()) == Success )
         {
            ++pSearchItem;
         }
         else if( !skipToAlternate(pSearchItem) )
         {
            COL_TRC("Literal failed, no alternate.");
            return false;
         }
         break;

      case TXTtextSearchItem::REGEX:
         COL_TRC("REGEX");
         if( pSearchItem->Regex.isExpressionValid() && pSearchItem->Regex.doesMatch(pString) == Success )
         {
            ++pSearchItem;
         }
         else if( !skipToAlternate(pSearchItem) )
         {
            COL_TRC("Regex failed, no alternate.");
            return false;
         }
         break;

      case TXTtextSearchItem::NEGATION:
         COL_TRC("NEGATION");
         Success = !Success;
         ++pSearchItem;
         continue;

      case TXTtextSearchItem::DISJUNCTION:
         COL_TRC("DISJUNCTION");
         skipToNext(pSearchItem);
         return true;
      }

      Success = true;
   }

   return true;  // Should be unreachable.
}

bool TXTtextSearch::matches(const char* pString) const
{
   COL_METHOD(matches);

   COLPRECONDITION( pString != NULL );
   COLASSERT( pSearchItems && pSearchItems->back().ItemType == TXTtextSearchItem::DONE );

   const TXTtextSearchItem* pFirstSearchItem = pSearchItems->begin();
   return ::matches(pString, pFirstSearchItem);
}


REXmatcher TXTtextSearch::matchedAreaRegex() const
{
   COL_METHOD(matchedAreaRegex);

   // Matches any regex meta-character.  Yep, it's hard to read.
   //
   static const REXmatcher MetaCharacterRegex("([][(){}\\\\^.$|?*+-])");
   COLPRECONDITION(MetaCharacterRegex.isExpressionValid());

   COLstring Pattern = "(";

   for(TXTtextSearchItem* pItem=pSearchItems->begin(), *pEnd=pSearchItems->end(); pItem != pEnd; ++pItem)
   {
      switch(pItem->ItemType)
      {
      case TXTtextSearchItem::REGEX:
         if( pItem->Regex.isExpressionValid() )
         {
            COL_TRC("REGEX " << pItem->Regex.pattern());
            Pattern += pItem->Regex.pattern();
            Pattern += '|';
         }
         else
         {
            COL_WRN("REGEX " << pItem->Regex.pattern() << " (invalid)");
         }
         break;

      case TXTtextSearchItem::LITERAL:{
         COL_TRC("LITERAL " << pItem->Literal);
         COLstring String = pItem->Literal;
         MetaCharacterRegex.regexReplace(String, "\\$1");  // Escape all meta-characters.
         Pattern += String;
         Pattern += '|';
         break;
      }
     
      default:
         break;
      }
   }

   if( Pattern.size() <= 1 )
   {
      Pattern = "^(?!.).";  // Matches nothing and fast.
   }
   else
   {
      Pattern[Pattern.size()-1] = ')';  // Replace the last pipe.
   }

   COL_TRC("Final: " << Pattern);

   REXmatcher MatchedAreaRegex(Pattern, REXmatcher::CaseInsensitive);
   COLPRECONDITION( MatchedAreaRegex.isExpressionValid() );

   return MatchedAreaRegex;
}


void TXTtextSearch::getInvalidRegexes(COLarray<const REXmatcher*>& BadRegexes) const
{
   BadRegexes.clear();

   for(TXTtextSearchItem* pItem=pSearchItems->begin(), *pEnd=pSearchItems->end(); pItem != pEnd; ++pItem)
   {
      if( pItem->ItemType == TXTtextSearchItem::REGEX && !pItem->Regex.isExpressionValid() )
      {
         BadRegexes.push_back(&pItem->Regex);
      }
   }
}

bool TXTtextSearch::matchesAll() const
{
   return pSearchItems->size() == 0 ||
          (pSearchItems->size() == 1 && (*pSearchItems)[0].ItemType == TXTtextSearchItem::DONE);
}

bool TXTtextSearch::toSqliteMatch(COLstring& Output, COLuint32 MinimumTermSize) const
{
   COL_METHOD(TXTtextSearch::toSqliteMatch);
   COLostream OutputStream(Output);
   
   bool Valid = true;
   TXTtextSearchItem* pItem=pSearchItems->begin(), *pEnd=pSearchItems->end();
   for(; Valid && pItem != pEnd; ++pItem)
   {
      switch(pItem->ItemType)
      {
      case TXTtextSearchItem::LITERAL:
         if (pItem->Literal.size() < MinimumTermSize)
         {
            //too small, we don't support the search
            Valid = false;
         }
         else
         {
            OutputStream << '"' << pItem->Literal << " \" ";
         }
         break;
      case TXTtextSearchItem::GROUP_START:
         OutputStream << " ( ";
         break;
      case TXTtextSearchItem::GROUP_END:
         OutputStream << " ) ";
         break;
      case TXTtextSearchItem::DISJUNCTION:
         OutputStream << "OR ";
         break;
      case TXTtextSearchItem::NEGATION:
         OutputStream << "NOT ";
         break;
      case TXTtextSearchItem::REGEX: //can't work with regex's
         Valid = false;
      case TXTtextSearchItem::DONE:
      default:
         break;
      }
   }

   if (!Valid)
   {
      //invalid
      Output.clear();
      COL_VAR(Output);
      return false;
   }
   COL_VAR(Output);
   return true;
}
