//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: htmldiff
//
// Description:
//
// An adaption off the htmldiff command line application that diffs two files and
// gives the result in html format.
//
// Author: Ivan Hamer
// Date:   Monday, December 6th, 2010 @ 11:13:29 AM
//
// Adapted to library: Jon Scalise
// Date:   Tue 21 Jul 2015 12:14:47 EDT
//---------------------------------------------------------------------------

#include <SFI/SFIhtmlEscape.h>

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>
#include <FIL/FILutils.h>

#include <ctype.h>
#include <algorithm>

#include <COL/COLhashmap.h>
#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// <summary>
// This Class implements the Difference Algorithm published in
// "An O(ND) Difference Algorithm and its Variations" by Eugene Myers
// Algorithmica Vol. 1 No. 2, 1986, p 251.
//
// There are many C, Java, Lisp implementations public available but they all seem to come
// from the same source (diffutils) that is under the (unfree) GNU public License
// and cannot be reused as a sourcecode for a commercial application.
// There are very old C implementations that use other (worse) algorithms.
// Microsoft also published sourcecode of a diff-tool (windiff) that uses some tree data.
// Also, a direct transfer from a C source to C# is not easy because there is a lot of pointer
// arithmetic in the typical C solutions and i need a managed solution.
// These are the reasons why I implemented the original published algorithm from the scratch and
// make it avaliable without the GNU license limitations.
// I do not need a high performance diff tool because it is used only sometimes.
// I will do some performace tweaking when needed.
//
// The algorithm itself is comparing 2 arrays of numbers so when comparing 2 text documents
// each line is converted into a (hash) number. See DiffText().
//
// Some chages to the original algorithm:
// The original algorithm was described using a recursive approach and comparing zero indexed arrays.
// Extracting sub-arrays and rejoining them is very performance and memory intensive so the same
// (readonly) data arrays are passed arround together with their lower and upper bounds.
// This circumstance makes the LCS and SMS functions more complicate.
// I added some code to the LCS function to get a fast response on sub-arrays that are identical,
// completely deleted or inserted.
//
// The result from a comparisation is stored in 2 arrays that flag for modified (deleted or inserted)
// lines in the 2 data arrays. These bits are then analysed to produce a array of Item objects.
//
// Further possible optimizations:
// (first rule: don't do it; second: don't do it yet)
// The arrays DataA and DataB are passed as parameters, but are never changed after the creation
// so they can be members of the class to avoid the paramter overhead.
// In SMS is a lot of boundary arithmetic in the for-D and for-k loops that can be done by increment
// and decrement of local variables.
// The DownVector and UpVector arrays are alywas created and destroyed each time the SMS gets called.
// It is possible to reuse tehm when transfering them to members of the class.
// See TODO: hints.
//
// diff.cs: A port of the algorythm to C#
// Copyright (c) by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
//
// Changes:
// 2002.09.20 There was a "hang" in some situations.
// Now I undestand a little bit more of the SMS algorithm.
// There have been overlapping boxes; that where analyzed partial differently.
// One return-point is enough.
// A assertion was added in CreateDiffs when in debug-mode, that counts the number of equal (no modified) lines in both arrays.
// They must be identical.
//
// 2003.02.07 Out of bounds error in the Up/Down vector arrays in some situations.
// The two vetors are now accessed using different offsets that are adjusted using the start k-Line.
// A test case is added.
//
// 2006.03.05 Some documentation and a direct Diff entry point.
//
// 2006.03.08 Refactored the API to static methods on the Diff class to make usage simpler.
// 2006.03.10 using the standard Debug class for self-test now.
//            compile with: csc /target:exe /out:diffTest.exe /d:DEBUG /d:TRACE /d:SELFTEST Diff.cs
// 2007.01.06 license agreement changed to a BSD style license.
// 2007.06.03 added the Optimize method.
// 2007.09.23 UpVector and DownVector optimization by Jan Stoklasa ().
// 2008.05.31 Adjusted the testing code that failed because of the Optimize method (not a bug in the diff algorithm).
// 2008.10.08 Fixing a test case and adding a new test case.
// </summary>

class DiffData
{
public:
   DiffData(const COLarray<int> &initData) {
      data = initData;
      Length = initData.size ();
      modified.resize(Length + 2);
      int i = 0 ;
      for (i = 0 ; i < modified.size(); ++i) {
         modified[i] = false;
      }
   }

   DiffData(const COLstring& initData) {
      data.resize(initData.size());
      int i = 0 ;
      for (i = 0; i < initData.size(); ++i) {
         data[i] = (int)initData[i];
      }
      Length = initData.size ();
      modified.resize(Length + 2);
      for (i = 0 ; i < modified.size(); ++i) {
         modified[i] = false;
      }
   }

   int Length; // Number of elements (lines).
   COLarray<int> data; // Buffer of numbers that will be compared.

   // Array of booleans that flag for modified data.
   // This is the result of the diff.
   // This means deletedA in the first Data or inserted in the second Data.
   COLarray<bool> modified;
};

class Diff
{
public:
   struct Item
   {
      int StartA;
      int StartB;
      int deletedA;
      int insertedB;
   };

   struct SMSRD // Shortest Middle Snake Return Data
   {
      int x, y;
   };

   // Find the difference in 2 text documents, comparing by textlines.
   // The algorithm itself is comparing 2 arrays of numbers so when comparing 2 text documents
   // each line is converted into a (hash) number. This hash-value is computed by storing all
   // textlines into a common hashtable so i can find dublicates in there, and generating a
   // new number each time a new textline is inserted.
   static COLarray<Item> diffTextByLines(const COLstring &TextA, const COLstring &TextB, bool trimSpace, bool ignoreSpace, bool ignoreCase) {
      COL_FUNCTION(diffTextByLines);
      COLhashmap<COLstring, int> h;
      DiffData DataA (diffCodes(TextA, h, trimSpace, ignoreSpace, ignoreCase));
      DiffData DataB (diffCodes(TextB, h, trimSpace, ignoreSpace, ignoreCase));
      int MAX = DataA.Length + DataB.Length + 1;

      COLarray<int> DownVector;
      DownVector.resize (2 * MAX + 2);

      COLarray<int> UpVector;
      UpVector.resize (2 * MAX + 2);

      LCS(DataA, 0, DataA.Length, DataB, 0, DataB.Length, DownVector, UpVector);

      optimizeDiff(DataA);
      optimizeDiff(DataB);
      return createDiffs(DataA, DataB);
   }


   // If a sequence of modified lines starts with a line that contains the same content
   // as the line that appends the changes, the difference sequence is modified so that the
   // appended line and not the starting line is marked as modified.
   // This leads to more readable diff sequences when comparing text files.
   static void optimizeDiff(DiffData Data) {
      COL_FUNCTION(optimizeDiff);
      int StartPos, EndPos;
      StartPos = 0;
      while (StartPos < Data.Length) {
         while ((StartPos < Data.Length) && (Data.modified[StartPos] == false))
            StartPos++;
         EndPos = StartPos;
         while ((EndPos < Data.Length) && (Data.modified[EndPos] == true))
            EndPos++;

         if ((EndPos < Data.Length) && (Data.data[StartPos] == Data.data[EndPos])) {
            Data.modified[StartPos] = false;
            Data.modified[EndPos] = true;
         } else {
            StartPos = EndPos;
         }
      }
   }

   static COLarray<Item> diffInt(const COLarray<int> &ArrayA, const COLarray<int> &ArrayB) {
      COL_FUNCTION(diffInt);
      DiffData DataA (ArrayA);
      DiffData DataB (ArrayB);

      int MAX = DataA.Length + DataB.Length + 1;

      COLarray<int> DownVector;
      DownVector.resize (2 * MAX + 2);

      COLarray<int> UpVector;
      UpVector.resize (2 * MAX + 2);

      LCS(DataA, 0, DataA.Length, DataB, 0, DataB.Length, DownVector, UpVector);
      return createDiffs(DataA, DataB);
   }

   static void adjustUtf8(COLarray<Item>* pDiffs, const COLstring &A, const COLstring &B) {
      COL_FUNCTION(adjustUtf8);
      const int Size = pDiffs->size();
      for (int i = 0; i < Size; ++i) {
         Item& Cur = (*pDiffs)[i];

         if (Cur.StartA < A.size()) {
            while (Cur.StartA > 0 && (A[Cur.StartA] & 0xC0) == 0x80) {
               --Cur.StartA;
               ++Cur.deletedA;
            }
         }

         if (Cur.StartB < B.size()) {
            while (Cur.StartB > 0 && (B[Cur.StartB] & 0xC0) == 0x80) {
               --Cur.StartB;
               ++Cur.insertedB;
            }
         }
      }
   }

   static COLarray<Item> diffString(const COLstring &A, const COLstring &B) {
      COL_FUNCTION(diffString);
      DiffData DataA(A);
      DiffData DataB(B);

      int MAX = DataA.Length + DataB.Length + 1;

      COLarray<int> DownVector;
      DownVector.resize (2 * MAX + 2);
      COLarray<int> UpVector;
      UpVector.resize (2 * MAX + 2);
      LCS(DataA, 0, DataA.Length, DataB, 0, DataB.Length, DownVector, UpVector);
      COLarray<Item> Diffs = createDiffs(DataA, DataB);
      adjustUtf8(&Diffs, A, B);
      return Diffs;
   }

   // This function converts all textlines of the text into unique numbers for every unique textline
   // so further work can work only with simple numbers.
   static COLarray<int> diffCodes(const COLstring &aText, COLhashmap <COLstring, int> &h, bool trimSpace, bool ignoreSpace, bool ignoreCase) {
      COL_FUNCTION(diffCodes);
      // get all codes of the text
      COLarray<COLstring> Lines;
      COLarray<int> Codes;
      int lastUsedCode = h.size();
      COLstring aCode;
      COLstring s;

      getLines(aText, Lines);
      Codes.resize(Lines.size());
      for (int i = 0; i < Lines.size(); ++i) {
         s = Lines[i];
         if (trimSpace || ignoreSpace)
            s = COLstripWhiteSpace (s, true, true);

         if (ignoreSpace) {
            // s = Regex.Replace(s, "\\s+", " ");
            COLstring NoSpace;
            bool inSpace = true;
            for (int i = 0; i < s.size(); i++) {
               if (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n') {
                  if (!inSpace) {
                     inSpace = true;
                     NoSpace = NoSpace + " ";
                  }
               } else {
                  inSpace = false;
                  NoSpace = NoSpace + s[i];
               }
            }
            s = NoSpace;
         }

         if (ignoreCase) {
            s.toLowerCase();
         }
         COLhashmapPlace Place = h.find(s);

         if (Place == NULL) {
            lastUsedCode++;
            h[s] = lastUsedCode;
            Codes[i] = lastUsedCode;
         } else {
            Codes[i] = h[s];
         }
      }
      return Codes;
   }

   static void getLines(const COLstring &aText, COLarray<COLstring> &Lines) {
      COL_FUNCTION(getLines);
      Lines.clear();
      const size_t InputSize = aText.size();
      size_t SubstrStart = 0;
      size_t Cursor = 0;

      while (Cursor < InputSize) {
         if (aText[Cursor] == '\n') {
            Lines.push_back(aText.substr(SubstrStart, Cursor - SubstrStart));
            SubstrStart = Cursor + 1;
         }
         ++Cursor;
      }
      if(InputSize) {
         Lines.push_back(aText.substr(SubstrStart, InputSize-SubstrStart));
      }
   }

   // This is the algorithm to find the Shortest Middle Snake (SMS).
   static SMSRD SMS(DiffData DataA, int LowerA, int UpperA, DiffData DataB, int LowerB, int UpperB, COLarray<int> &DownVector, COLarray<int> &UpVector) {

      SMSRD ret;
      int MAX = DataA.Length + DataB.Length + 1;
      int DownK = LowerA - LowerB; // the k-line to start the forward search
      int UpK = UpperA - UpperB; // the k-line to start the reverse search

      int Delta = (UpperA - LowerA) - (UpperB - LowerB);
      bool oddDelta = (Delta & 1) != 0;

      // The vectors in the publication accepts negative indexes. the vectors implemented here are 0-based
      // and are access using a specific offset: UpOffset UpVector and DownOffset for DownVektor
      int DownOffset = MAX - DownK;
      int UpOffset = MAX - UpK;
      int MaxD = ((UpperA - LowerA + UpperB - LowerB) / 2) + 1;

      DownVector[DownOffset + DownK + 1] = LowerA;
      UpVector[UpOffset + UpK - 1] = UpperA;

      for (int D = 0; D <= MaxD; D++) {
         int k;

         // Extend the forward path.
         for (k = DownK - D; k <= DownK + D; k += 2) {
            // find the only or better starting point
            int x, y;
            if (k == DownK - D) {
               x = DownVector[DownOffset + k + 1]; // down
            } else {
               x = DownVector[DownOffset + k - 1] + 1; // a step to the right
               if ((k < DownK + D) && (DownVector[DownOffset + k + 1] >= x)) {
                  x = DownVector[DownOffset + k + 1]; // down
               }
            }
            y = x - k;

            // find the end of the furthest reaching forward D-path in diagonal k.
            while ((x < UpperA) && (y < UpperB) && (DataA.data[x] == DataB.data[y])) {
               x++; y++;
            }
            DownVector[DownOffset + k] = x;

            // overlap ?
            if (oddDelta && (UpK - D < k) && (k < UpK + D)) {
               if (UpVector[UpOffset + k] <= DownVector[DownOffset + k]) {
                  ret.x = DownVector[DownOffset + k];
                  ret.y = DownVector[DownOffset + k] - k;
                  return (ret);
               }
            }
         } // for k

         // Extend the reverse path.
         for (k = UpK - D; k <= UpK + D; k += 2) {
            // find the only or better starting point
            int x, y;
            if (k == UpK + D) {
               x = UpVector[UpOffset + k - 1]; // up
            } else {
               x = UpVector[UpOffset + k + 1] - 1; // left
               if ((k > UpK - D) && (UpVector[UpOffset + k - 1] < x))
                  x = UpVector[UpOffset + k - 1]; // up
            } // if
            y = x - k;

            while ((x > LowerA) && (y > LowerB) && (DataA.data[x - 1] == DataB.data[y - 1])) {
               x--; y--; // diagonal
            }
            UpVector[UpOffset + k] = x;

            // overlap ?
            if (!oddDelta && (DownK - D <= k) && (k <= DownK + D)) {
               if (UpVector[UpOffset + k] <= DownVector[DownOffset + k]) {
                  ret.x = DownVector[DownOffset + k];
                  ret.y = DownVector[DownOffset + k] - k;
                  return (ret);
               }
            }
         } // for k
      } // for D
      throw 0;
   }

   // This is the divide-and-conquer implementation of the longest common-subsequence (LCS)
   // algorithm.
   // The published algorithm passes recursively parts of the A and B sequences.
   // To avoid copying these arrays the lower and upper bounds are passed while the sequences stay constant.
   static void LCS(DiffData &DataA, int LowerA, int UpperA, DiffData &DataB, int LowerB, int UpperB, COLarray<int> &DownVector, COLarray<int> &UpVector) {
      // Fast walkthrough equal lines at the start
      while (LowerA < UpperA && LowerB < UpperB && DataA.data[LowerA] == DataB.data[LowerB]) {
         LowerA++; LowerB++;
      }

      // Fast walkthrough equal lines at the end
      while (LowerA < UpperA && LowerB < UpperB && DataA.data[UpperA - 1] == DataB.data[UpperB - 1]) {
         --UpperA; --UpperB;
      }

      if (LowerA == UpperA) {
         // mark as inserted lines.
         while (LowerB < UpperB)
            DataB.modified[LowerB++] = true;

      } else if (LowerB == UpperB) {
         // mark as deleted lines.
         while (LowerA < UpperA)
            DataA.modified[LowerA++] = true;

      } else {
         // Find the middle snake and length of an optimal path for A and B
         SMSRD smsrd = SMS(DataA, LowerA, UpperA, DataB, LowerB, UpperB, DownVector, UpVector);
         // Debug.Write(2, "MiddleSnakeData", COLstring.Format("{0},{1}", smsrd.x, smsrd.y));

         // The path is from LowerX to (x,y) and (x,y) to UpperX
         LCS(DataA, LowerA, smsrd.x, DataB, LowerB, smsrd.y, DownVector, UpVector);
         LCS(DataA, smsrd.x, UpperA, DataB, smsrd.y, UpperB, DownVector, UpVector);  // 2002.09.20: no need for 2 points
      }
   }


   // Scan the tables of which lines are inserted and deleted,
   // producing an edit script in forward order.
   static COLarray<Item> createDiffs(DiffData DataA, DiffData DataB) {
      Item aItem;
      COLarray<Item> result;

      int StartA, StartB;
      int LineA, LineB;

      LineA = 0;
      LineB = 0;
      while (LineA < DataA.Length || LineB < DataB.Length) {
         if ((LineA < DataA.Length) && (!DataA.modified[LineA]) && (LineB < DataB.Length) && (!DataB.modified[LineB])) {
            // equal lines
            LineA++;
            LineB++;

         } else {
            // maybe deleted and/or inserted lines
            StartA = LineA;
            StartB = LineB;

            while (LineA < DataA.Length && (LineB >= DataB.Length || DataA.modified[LineA])) {
               // while (LineA < DataA.Length && DataA.modified[LineA])
               LineA++;
            }
            while (LineB < DataB.Length && (LineA >= DataA.Length || DataB.modified[LineB])) {
               // while (LineB < DataB.Length && DataB.modified[LineB])
               LineB++;
            }
            if ((StartA < LineA) || (StartB < LineB)) {
               // store a new difference-item
               aItem.StartA = StartA;
               aItem.StartB = StartB;
               aItem.deletedA = LineA - StartA;
               aItem.insertedB = LineB - StartB;
               result.push_back(aItem);
            } // if
         } // if
      } // while

      return (result);
   }
};

void addRow(COLstring* pHtml, int Num, const COLstring& Code, const COLstring& Class) {
   COL_FUNCTION(addRow);
   COLstring& Html = *pHtml;
   Html.append(COL_T("<tr><td class='ln'>"));
   Html.append(Num == 0 ? " " : COLintToString(Num));
   Html.append(COL_T("</td><td class='cd"));
   if (! Class.is_null()) {
      Html.append(COL_T(" "));
      Html.append(Class);
   }
   Html.append(COL_T("'>"));
   Html.append(Code);
   Html.append(COL_T("</td></tr>\n"));
}

void addCondensedRow(COLstring* pHtml, int NumA, int NumB, const COLstring& Code, const COLstring& Class) {
   COL_FUNCTION(addCondensedRow);
   COLstring& Html = *pHtml;
   Html.append(COL_T("<tr><td class='ln'>"));
   Html.append(NumA == 0 ? " " : COLintToString(NumA));
   Html.append(COL_T("</td><td class='ln'>"));
   Html.append(NumB == 0 ? " " : COLintToString(NumB));
   Html.append(COL_T("</td><td class='cd"));
   if (! Class.is_null()) {
      Html.append(COL_T(" "));
      Html.append(Class);
   }
   Html.append(COL_T("'>"));
   Html.append(Code);
   Html.append(COL_T("</td></tr>\n"));
}

void htmlEscape(const COLstring& Src, COLstring& Dest){
   COL_FUNCTION(htmlEscape);
   SFIhtmlEscapeNoNewline(Src.c_str(), Src.size(), Dest);
}

COLstring htmlEscapeCOLstring(const COLstring &UnescapedString) {
   COL_FUNCTION(htmlEscapeCOLstring);
   COLstring EscapedString;
   htmlEscape(UnescapedString, EscapedString);
   return EscapedString;
}


int htmlFindIndexAvoidingEscaped(const COLstring &Str, int NonEscapedIndex) {
   int Result = 0;
   for (int i = 0; i < NonEscapedIndex; i++) {
      if (Str[Result] == '&') {
         while (Str[Result] != ';') {
            Result++;
         }
      }
      Result++;
   }
   return Result;
}

bool isBufferAscii(const char* Buffer, int BufferLen) {
   char CharCode;
   bool CharIsAscii;
   bool BufferIsAscii = true;
   for (int i = 0; i < BufferLen && BufferIsAscii; ++i) {
      CharCode = Buffer[i];
      CharIsAscii = !(iscntrl((unsigned char) CharCode)) || COLisspace ((unsigned char) CharCode);
      BufferIsAscii = BufferIsAscii && CharIsAscii;
   }
   return BufferIsAscii;
}

bool HTOOLreadAsciiFile(const COLstring &Name, COLstring *pString) {
   COL_FUNCTION(HTOOLreadAsciiFile);
   if(!FILfileExists(Name)){
      COL_TRC("File does not exist: " + Name);
      return false;
   }
   FILreadFile(Name,pString);
   return isBufferAscii(pString->get_buffer(), pString->size());
}

// This function accounts for html escaped characters (&amp, &nbsp...) int pText
void htmlInsertHightlightSpan(COLstring *pText, int Start, int Length) {
   COL_FUNCTION(htmlInsertHightlightSpan);
   if (Length == 0) { return; }
   int LeftPoint = htmlFindIndexAvoidingEscaped(*pText, Start);
   int RightPoint = htmlFindIndexAvoidingEscaped(*pText, Start + Length);
   *pText = pText->substr(0, LeftPoint) +
      "<span class='highlight'>" + pText->substr(LeftPoint, RightPoint - LeftPoint) + "</span>" +
      pText->substr(RightPoint);
}

void writeSummary(int LinesAdded, int LinesDeleted, int LinesChanged, int MaxLinesReached, COLvar* pResult) {
   COL_FUNCTION(writeSummary);
   COLvar& Result = *pResult;
   Result["added"]   = LinesAdded;
   Result["deleted"] = LinesDeleted;
   Result["changed"] = LinesChanged;
   Result["maxLinesReached"] = MaxLinesReached;
}


// We want to display CONTEXT_ZONE lines above and below the diff for the condensed view.
static const int CONTEXT_ZONE = 3;

// determine the number of lines can be displayed above the diff
static void insertContextLinesAbove(const COLarray<Diff::Item>&Diffs, int CurrentDiffIndex, const COLarray<COLstring>& LinesA, COLstring* pOut) {
   COL_FUNCTION(insertContextLinesAbove);
   const Diff::Item* pCurr = &Diffs[CurrentDiffIndex];
   const Diff::Item* pPrev = CurrentDiffIndex == 0 ? NULL : &Diffs[CurrentDiffIndex - 1];
   COL_VAR2(pCurr->StartA, pCurr->deletedA);
   int LinesAbove = std::min(pCurr->StartA, CONTEXT_ZONE);
   if (pPrev) {
      COL_VAR2(pPrev->StartA, pPrev->deletedA);
      LinesAbove = pCurr->StartA - (pPrev->StartA + pPrev->deletedA);
      if (LinesAbove > CONTEXT_ZONE * 2) {
         LinesAbove = CONTEXT_ZONE;
      }
   }
   COL_VAR(LinesAbove);
   for (int i = LinesAbove; i > 0; i--) {
      int CurrentLine = pCurr->StartA - i;
      COLstring Escaped = htmlEscapeCOLstring(LinesA[CurrentLine]);
      addCondensedRow(pOut, CurrentLine + 1, 0, Escaped, "");
   }
}

// determine the number of lines can be displayed below the diff
static bool insertContextLinesBelow(const COLarray<Diff::Item>&Diffs, int CurrentDiffIndex, const COLarray<COLstring>& LinesA, COLstring* pOut) {
   COL_FUNCTION(insertContextLinesBelow);
   const Diff::Item* pCurr = &Diffs[CurrentDiffIndex];
   const Diff::Item* pNext = CurrentDiffIndex == Diffs.size() - 1 ? NULL : &Diffs[CurrentDiffIndex + 1];
   COL_VAR2(pCurr->StartA, pCurr->deletedA);
   int LinesBelow = pCurr->StartA + pCurr->deletedA;
   LinesBelow = std::min(LinesA.size() - LinesBelow, CONTEXT_ZONE);
   if (pNext) {
      COL_VAR2(pNext->StartA, pNext->deletedA);
      LinesBelow = pNext->StartA - (pCurr->StartA + pCurr->deletedA);
      if (LinesBelow > CONTEXT_ZONE * 2) {
         LinesBelow = CONTEXT_ZONE;
      } else {
         // if there are less than 10 lines between the end of current diff and the beginning
         // of the next diff, let insertContextLinesAbove() handle it in next iteration.
         COL_TRC("Overlapping context zone. Don't insert divider.");
         return false;  // don't insert divider
      }
   }
   COL_VAR(LinesBelow);
   int Start = pCurr->StartA + pCurr->deletedA;
   for (int i = 0; i < LinesBelow; i++) {
      int CurrentLine = Start + i;
      COLstring Escaped = htmlEscapeCOLstring(LinesA[CurrentLine]);
      addCondensedRow(pOut, CurrentLine + 1, 0, Escaped, "");
   }
   return pNext ? true : false;  // insert divider unless this is the last diff item
}

void buildCondensedHtmlDiffs(const COLarray<Diff::Item> &EditScript, const COLstring &A, const COLstring &B,
                             const COLstring &fileA, const COLstring &fileB, int* pMaxLinesReached, COLvar* pResult) {
   COL_FUNCTION (buildCondensedHtmlDiffs);
   COLstring CondensedHtml = "<table class='TGITdiffTable'><thead></thead><tbody>";
   COLarray<COLstring> LinesA, LinesB;
   Diff::getLines( A, LinesA );
   Diff::getLines( B, LinesB );
   COL_VAR2(LinesA.size(), LinesB.size());
   int EditScriptSize = EditScript.size();
   int MaxInt = 1000;
   int Count = 0;
   for (int i = 0; i < EditScriptSize && Count < MaxInt; i++) {
      const Diff::Item &EditItem = EditScript[i];
      insertContextLinesAbove(EditScript, i, LinesA, &CondensedHtml);
      int j, CurrentLine;
      for (j = 0; j < EditItem.deletedA && Count < MaxInt; j++) {
         CurrentLine = EditItem.StartA + j;
         COLstring Escaped = htmlEscapeCOLstring(LinesA[CurrentLine]);
         addCondensedRow(&CondensedHtml, CurrentLine + 1, 0, Escaped, "TGITdelDiff");
         Count++;
      }
      for (j = 0; j < EditItem.insertedB && Count < MaxInt; j++) {
         CurrentLine = EditItem.StartB + j;
         COLstring Escaped = htmlEscapeCOLstring(LinesB[CurrentLine]);
         addCondensedRow(&CondensedHtml, 0, CurrentLine + 1, Escaped, "TGITinsDiff");
         Count++;
      }
      bool InsertDivider = insertContextLinesBelow(EditScript, i, LinesA, &CondensedHtml);
      if (InsertDivider && i < EditScriptSize - 1) {
         addCondensedRow(&CondensedHtml, 0, 0, "...", "TGITdivDiff");
         Count++;
      }
   }

   CondensedHtml += COL_T("</tbody></table>");
   COL_VAR(Count);
   pResult->swap(&CondensedHtml);
}

void buildFullHtmlDiffs(const COLarray<Diff::Item>& EditScript, const COLstring& A, const COLstring& B, const COLstring& fileA,
                             const COLstring& fileB, int* pLinesAdded, int* pLinesDeleted, int* pLinesChanged, int* pMaxLinesReached, COLvar* pResult) {
   COL_FUNCTION(buildFullHtmlDiffs);
   COLASSERT(pLinesAdded);
   COLASSERT(pLinesDeleted);
   COLASSERT(pLinesChanged);
   COLvar& Result = *pResult;
   COLarray<COLstring> LinesA, LinesB;
   Diff::getLines( A, LinesA );
   Diff::getLines( B, LinesB );
   enum {
      DELETED = 1,
      INSERTED = 2
   };
   COLarray<int> LineFlagsA, LineFlagsB;
   LineFlagsA.resize(LinesA.size());
   LineFlagsB.resize(LinesB.size());
   int k;
   for (k = 0; k < LineFlagsA.size(); k++) { LineFlagsA[k] = 0; }
   for (k = 0; k < LineFlagsB.size(); k++) { LineFlagsB[k] = 0; }
   int MaxLines = LinesA.size();
   int EditScriptSize = EditScript.size();
   for (int i = 0; i < EditScriptSize; i++) {
      const Diff::Item &EditItem = EditScript[i];
      int j;
      for (j = 0; j < EditItem.deletedA; j++) {
         int Line = EditItem.StartA + j;
         LineFlagsA[Line] |= DELETED;
      }
      for (j = 0; j < EditItem.insertedB; j++) {
         int Line = EditItem.StartB + j;
         LineFlagsB[Line] |= INSERTED;
      }
      if (EditItem.insertedB > 0 && EditItem.deletedA > 0) { MaxLines += (EditItem.insertedB > EditItem.deletedA)? EditItem.insertedB : EditItem.deletedA; }
      else                                                 { MaxLines += EditItem.insertedB + EditItem.deletedA; }
   }
   COLstring FromContent = COL_T("<table class='TGITdiffTable TGITdiffFrom'><thead></thead><tbody>");
   COLstring ToContent   = COL_T("<table class='TGITdiffTable TGITdiffTo'><thead></thead><tbody>");
   int Ai = 0;
   int Bi = 0;
   int MaxInt = 1000;
   if(MaxLines > MaxInt){
      MaxLines = MaxInt;
      *pMaxLinesReached = MaxInt;
   }
   for (int l = 0; l < MaxLines; l++) {
      if (Ai >= LinesA.size() && Bi >= LinesB.size()) break;
      bool Deleted = false;
      bool Inserted = false;
      COLstring OutTextA;
      COLstring OutTextB;
      if (Ai < LinesA.size()) {
         Deleted = LineFlagsA[Ai] & DELETED;
         OutTextA = htmlEscapeCOLstring(LinesA[Ai]);
      }
      if (Bi < LinesB.size()) {
         Inserted = !!(LineFlagsB[Bi] & INSERTED);
         OutTextB = htmlEscapeCOLstring(LinesB[Bi]);
      }
      int LineNumA = Ai + 1;
      int LineNumB = Bi + 1;
      if (!Inserted && !Deleted) {
         addRow(&FromContent, LineNumA, OutTextA, "");
         addRow(&ToContent,   LineNumB, OutTextB, "");
         Ai++;
         Bi++;
         continue;
      }
      if (Inserted && Deleted) {
         // Diff the Line itself Here
         COLarray<Diff::Item> Result = Diff::diffString(LinesA[Ai],LinesB[Bi]);
         if (Result.size() < 4) {
            for (int i = Result.size() - 1; i >= 0; i--) {
               const Diff::Item &EditScript = Result[i];
               htmlInsertHightlightSpan(&OutTextA, EditScript.StartA, EditScript.deletedA);
               htmlInsertHightlightSpan(&OutTextB, EditScript.StartB, EditScript.insertedB);
            }
         }
         else {
            htmlInsertHightlightSpan(&OutTextA, 0, LinesA[Ai].size());
            htmlInsertHightlightSpan(&OutTextB, 0, LinesB[Bi].size());
         }
         addRow(&FromContent, LineNumA, OutTextA, "TGITchangeDiff");
         addRow(&ToContent,   LineNumB, OutTextB, "TGITchangeDiff");
         (*pLinesChanged)++;
         Ai++;
         Bi++;
      }
      else if (Inserted) {
         OutTextA = " ";
         addRow(&FromContent, LineNumA, OutTextA, "TGITdumDiff");
         addRow(&ToContent,   LineNumB, OutTextB, "TGITinsDiff");
         (*pLinesAdded)++;
         Bi++;
      }
      else if (Deleted) {
         OutTextB = " ";
         addRow(&FromContent, LineNumA, OutTextA, "TGITdelDiff");
         addRow(&ToContent,   LineNumB, OutTextB, "TGITdumDiff");
         (*pLinesDeleted)++;
         Ai++;
      }
   }
   FromContent.append(COL_T("</tbody></table>"));
   ToContent.append(COL_T("</tbody></table>"));
   Result["from"].swap(&FromContent);
   Result["to"].swap(&ToContent);
}

void writeBinaryDiffSummary(const COLstring& A, const COLstring& B, COLvar* pResult) {
   COL_FUNCTION(writeBinaryDiffSummary);
   COLvar& Result = *pResult;
   Result = "Binary files differ.";
   if (A.size() != B.size()) {
      return;
   }
   bool Equal = true;
   for (int i = 0; i < A.size() && Equal; ++i) {
      Equal = (A[i] == B[i]);
   }
   if (Equal) {
      Result = "Binary files are identical.";
   }
}

void HTOOLhtmldiff(const COLstring& FileA, const COLstring& FileB, COLvar* pDiffResult){
   COL_FUNCTION(HTOOLhtmldiff);
   COLvar& DiffResult = *pDiffResult;

   if (!FILfileExists(FileA)) {
      COL_ERROR_STREAM_PLAIN("File " << FileA << " does not exist.", COLerror::PreCondition);
      return;
   }
   if (!FILfileExists(FileB)) {
      COL_ERROR_STREAM_PLAIN("File " << FileB << " does not exist.", COLerror::PreCondition)
      return;
   }
   if(FILfileSize(FileA) > 1048576 || FILfileSize(FileB) > 1048576) {
      COL_ERROR_STREAM_PLAIN("File too large to compare", COLerror::PreCondition);
      return;
   }
   COLstring A;
   COL_TRC("Reading file " + FileA);
   bool AisAscii = HTOOLreadAsciiFile(FileA, &A);

   COLstring B;
   COL_TRC("Reading file " + FileB);
   bool BisAscii = HTOOLreadAsciiFile(FileB, &B);

   COLarray<Diff::Item> EditScript;
   if (AisAscii && BisAscii) {
      COL_TRC("Computing Diff");
      A.replace("\r\n", "\n");
      B.replace("\r\n", "\n");
      EditScript = Diff::diffTextByLines(A,B,false,true,false);

      if (EditScript.size() > 0) {
         COL_TRC("Building full results");
         DiffResult["status"] = "Files differ.";
         int LinesAdded = 0;
         int LinesDeleted = 0;
         int LinesChanged = 0;
         int MaxLinesReached = 0;
         buildFullHtmlDiffs(EditScript, A, B, FileA, FileB, &LinesAdded, &LinesDeleted, &LinesChanged, &MaxLinesReached, &DiffResult["full"]);
         buildCondensedHtmlDiffs(EditScript, A, B, FileA, FileB, &MaxLinesReached, &DiffResult["condensed"]);
         COL_TRC("Building line over line results");
         writeSummary(LinesAdded, LinesDeleted, LinesChanged, MaxLinesReached, &DiffResult["summary"]);
      } else {
         DiffResult["status"] = "No changes.";
      }
   } else {
      writeBinaryDiffSummary(A, B, &DiffResult["status"]);
   }
}

void HTOOLhtmldiffFileContent(COLstring FileAContent, const COLstring& FileA, COLstring FileBContent, const COLstring& FileB, COLvar* pDiffResult){
   COL_FUNCTION(HTOOLhtmldiffFileContent);
   COL_TRC("Computing Diff");
   COLvar& DiffResult = *pDiffResult;
   FileAContent.replace("\r\n", "\n");
   FileBContent.replace("\r\n", "\n");
   COLarray<Diff::Item> EditScript = Diff::diffTextByLines(FileAContent,FileBContent,false,true,false);
   if (EditScript.size() > 0) {
      COL_TRC("Building full results");
      DiffResult["status"] = "Files differ.";
      int LinesAdded = 0;
      int LinesDeleted = 0;
      int LinesChanged = 0;
      int MaxLinesReached = 0;
      buildFullHtmlDiffs(EditScript, FileAContent, FileBContent, FileA, FileB, &LinesAdded, &LinesDeleted, &LinesChanged, &MaxLinesReached, &DiffResult["full"]);
      buildCondensedHtmlDiffs(EditScript, FileAContent, FileBContent, FileA, FileB, &MaxLinesReached, &DiffResult["condensed"]);
      COL_TRC("Building line over line results");
      writeSummary(LinesAdded, LinesDeleted, LinesChanged, MaxLinesReached, &DiffResult["summary"]);
   } else {
      DiffResult["status"] = "No changes.";
   }
}