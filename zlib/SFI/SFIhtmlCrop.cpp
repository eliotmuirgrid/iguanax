//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhtmlCrop
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, August 30th, 2011 @ 03:06:37 PM
//
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIhtmlCrop.h"

#include <COL/COLsinkString.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static const char* SFI_HTML_TAG_CHARACTER_SET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

void SFIhtmlCrop(const char* pHtmlSource, const COLuint32 SourceSize, const COLstring& TextToAppend, COLuint32 MaxLength, COLsink& Sink)
{
   const char* pSource = pHtmlSource;
   COLuint32 RenderLength = 0, SourcePosition = 0;

   while (SourcePosition < SourceSize)
   {
      size_t IndexOfTagOrEscapeSequence = strcspn(pSource, "&<");

      SourcePosition += IndexOfTagOrEscapeSequence;
      RenderLength += IndexOfTagOrEscapeSequence;

      if (RenderLength >= MaxLength)
      {
         SourcePosition -= (RenderLength - MaxLength);
         pSource = pHtmlSource + SourcePosition;
         break;
      }
      else if (pSource[IndexOfTagOrEscapeSequence] == '<')
      {
         size_t SizeOfTag = strcspn(pHtmlSource + SourcePosition, ">") + 1;
         SourcePosition += SizeOfTag;
      }
      else if (pSource[IndexOfTagOrEscapeSequence] == '&')
      {
         size_t SizeOfEscapeSequence = strcspn(pHtmlSource + SourcePosition, ";") + 1;
         SourcePosition += SizeOfEscapeSequence;
         RenderLength++;
      }

      pSource = pHtmlSource + SourcePosition;
   }

   Sink.write(pHtmlSource, COL_MIN(SourcePosition, SourceSize)); // SourcePosition should never be greater than SourceSize, but might as well be extra safe.
   
   if (SourcePosition < SourceSize)
   {
      COLarray<COLstring> TagStack;
      bool TextCropped = false; // may only be closing tags left, in which case we aren't actually cropping anything.

      while (SourcePosition < SourceSize)
      {
         size_t IndexOfTag = strcspn(pSource, "<");

         SourcePosition += IndexOfTag;

         TextCropped = TextCropped || (IndexOfTag > 0);

         if (pSource[IndexOfTag] == '<')
         {
            size_t SizeOfTag = strcspn(pHtmlSource + SourcePosition + 1, ">") + 2;

            if (pSource[IndexOfTag+1] == '/')
            {
               // Closing tag
               // We add all closing tags that were not opened after the cropping to the result string.
               //
               size_t SizeOfTagName = strspn(pHtmlSource + SourcePosition + 2, SFI_HTML_TAG_CHARACTER_SET);
               COLstring ClosingTagName(pHtmlSource + SourcePosition + 2, SizeOfTagName);

               COLstring TopStackTag;
               while (TagStack.size() && TopStackTag != ClosingTagName)
               {
                  TopStackTag = TagStack[TagStack.size()-1];
                  TagStack.pop_back();
               }

               if (TopStackTag != ClosingTagName)
               {
                  Sink.write(pHtmlSource + SourcePosition, SizeOfTag);
               }
            }
            else
            {
               // Opening tag
               size_t SizeOfTagName = strspn(pHtmlSource + SourcePosition + 1, SFI_HTML_TAG_CHARACTER_SET);
               TagStack.push_back().append(pHtmlSource + SourcePosition + 1,SizeOfTagName);
            }

            SourcePosition += SizeOfTag;
         }

         pSource = pHtmlSource + SourcePosition;
      }

      if (TextCropped)
      {
         Sink.write(TextToAppend.c_str(), TextToAppend.size());
      }
   }
}

void SFIhtmlCrop(const char* pHtmlSource, const COLuint32 SourceSize, const COLstring& TextToAppend, COLuint32 MaxLength, COLstring& Output){
   COLsinkString Sink(Output);
   SFIhtmlCrop(pHtmlSource, SourceSize, TextToAppend, MaxLength, Sink);
}

