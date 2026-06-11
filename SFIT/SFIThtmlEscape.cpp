//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIThtmlEscape
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, December 17th, 2012 @ 03:44:40 PM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFIThtmlEscape.h"

#include <SFI/SFIhtmlEscape.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLsinkString.h>
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

inline void SFIThtmlEscapeImpl(COLstring& Target, const COLstring& Source, const char* pNewLineReplacements){
   const char* Replacements[256] = {0};
   const char* pSpanStr = "&<>\"";

   Replacements['\0'] = " ";
   Replacements['&']  = "&amp;";
   Replacements['>']  = "&gt;";
   Replacements['<']  = "&lt;";
   Replacements['"']  = "&quot;";
    if (pNewLineReplacements)
   {
      pSpanStr = "&<>\"\r\n";
      Replacements['\r'] = pNewLineReplacements;
      Replacements['\n'] = pNewLineReplacements;
   }

   const char* pSource    = Source.c_str();
   const char* pSourceEnd = pSource + Source.size();

   COLPRECONDITION( *pSourceEnd == '\0' );

   // This will also find embedded NUL characters.
   //
   for(const char* pCurrent; (pCurrent = pSource + strcspn(pSource, pSpanStr)) != pSourceEnd; pSource = pCurrent + 1)
   {
      const char* pReplacementText = Replacements[(unsigned char)*pCurrent];

      Target.append(pSource, pCurrent - pSource);
      Target.append(pReplacementText, strlen(pReplacementText));

      if( pNewLineReplacements && (*pCurrent == '\r' && *(pCurrent + 1) == '\n') )
      {
         ++pCurrent;
      }
   }

   Target.append(pSource, pSourceEnd - pSource);
}

// See http://fogbugz.ifware.dynip.com/default.asp?W5038
void testSpeed(){
   COLstring Source;
   const int CountOfRep = 500;
   for (int i=0; i < 20000; i++){
      Source.append("The rain in \"spain\" is good & fun be it not <blah>\r\n");
   }
   COLdateTime Start = COLdateTime::currentTime();
   COL_VAR(Source.size());
   COL_TRC("SFIThtmlEscapeImpl");
   COLstring Out;
   for (int j=0; j < CountOfRep; j++){
      Out.zero();
      SFIThtmlEscapeImpl(Out, Source, "<br>");
   }
   COL_TRC("Done");
   COLdateTime End = COLdateTime::currentTime();

   COLdateTime Start2 = COLdateTime::currentTime();
   COL_TRC("SFIhtmlEscapeWithCRAsNewline");
   COLstring Out2;
   for (int j2=0; j2 < CountOfRep; j2++){
      Out2.zero();
      COLsinkString Sink(Out2);
      SFIhtmlEscapeWithCRAsNewline(Source.c_str(), Source.size(), Sink);
   }
   COL_TRC("Done");
   COLdateTime End2 = COLdateTime::currentTime();

   COL_TRC(End - Start << " vs. " << End2 - Start2);

   TST_ASSERT_EQUALS(Out, Out2);
}

void test(){
   COLstring Source;
   for (int i=0; i < 5000; i++){
      Source.append("The rain in \"spain\" is good & fun be it not <blah>\r\n");
   }
   COL_VAR(Source.size());
   COL_TRC("SFIThtmlEscapeImpl");
   COLstring Out;
   SFIThtmlEscapeImpl(Out, Source, "<br>");
   COL_TRC("Done");
   COL_TRC("SFIhtmlEscapeWithCRAsNewline");
   COLstring Out2;
   COLsinkString Sink(Out2);
   SFIhtmlEscapeWithCRAsNewline(Source.c_str(), Source.size(), Sink);
   COL_TRC("Done");
   TST_ASSERT_EQUALS(Out, Out2);
}


TSTtestable* SFIThtmlEscape(){
   TST_START_COLLECTION("SFIThtmlEscape")
      //TST_ADD_TEST_FUNCTION(testSpeed);
      TST_ADD_TEST_FUNCTION(test)
   TST_END_FIXTURE
};

