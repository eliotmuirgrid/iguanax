//---------------------------------------------------------------------------
// Copyright (C) 1997-2016 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: html_preview
//
// Description:
//
// Parses a file off disk and gives the result back as formatted HTML.
//
//
// Author: Jon Scalise
// Date:   Thu 14 Jan 2016 17:21:43 EST
//---------------------------------------------------------------------------

#include <SFI/SFIhtmlEscape.h>
#include <FIL/FILutils.h>
#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void htmlEscapeFile(const COLstring& Src, COLstring& Dest){
   COL_FUNCTION(htmlEscape);
   SFIhtmlEscapeNoNewline(Src.c_str(), Src.size(), Dest);
}

static void addRow(COLstring* pHtml, int Num, const COLstring& LineContents) {
   COL_FUNCTION(addRow);
   COLstring& Html = *pHtml;
   Html.append(COL_T("<tr><td class='ln'>"));
   Html.append(Num == 0 ? " " : COLintToString(Num));
   Html.append(COL_T("</td><td class='cd'>"));
   Html.append(LineContents);
   Html.append(COL_T("</td></tr>\n"));
}

static void getLines(const COLstring& FileContent, COLarray<COLstring>& Lines) {
   COL_FUNCTION(getLines);
   Lines.clear();
   const size_t InputSize = FileContent.size();
   size_t SubstrStart = 0;
   size_t Cursor = 0;

   while (Cursor < InputSize) {
      if (FileContent[Cursor] == '\n') {
         Lines.push_back(FileContent.substr(SubstrStart, Cursor - SubstrStart));
         SubstrStart = Cursor + 1;
      }
      ++Cursor;
   }
   if(InputSize) {
      Lines.push_back(FileContent.substr(SubstrStart, InputSize-SubstrStart));
   }
}

static void renderHTMLFromFileContents(const COLstring& FileContents, COLstring* pHtml){
   COL_FUNCTION(renderHTMLFromFileContents);
   COLstring& Html = *pHtml;
   COLarray<COLstring> FileLines;
   getLines(FileContents, FileLines);
   Html = COL_T("<table class='TGITpreviewTable'><thead></thead><tbody>");
   for(int i = 0; i < FileLines.size(); i++){
      addRow(pHtml, i + 1, FileLines[i]);
   }
   Html.append(COL_T("</tbody></table>"));
}

void htmlpreview(const COLstring& FullFilePath, COLstring* pResult){
   COL_FUNCTION(htmlpreview);
   COLstring& HtmlString = *pResult;
   COLstring FileContents;
   FILreadFile(FullFilePath, &FileContents);
   COL_VAR(FileContents);
   COLstring HTMLescapedFileContents;
   htmlEscapeFile(FileContents, HTMLescapedFileContents);
   renderHTMLFromFileContents(HTMLescapedFileContents, &HtmlString);
   COL_VAR(HtmlString);
}
