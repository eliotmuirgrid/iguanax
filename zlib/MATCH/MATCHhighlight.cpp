//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHhighlight.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   14/05/24 2:16 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <MATCH/MATCHhighlight.h>
#include <MATCH/MATCHsearch.h>
#include <MATCH/MATCHutils.h>
#include <REX/REXmatcher.h>
COL_LOG_MODULE;

static const size_t MATCH_MAX_LINES_CONTEXT       = 5;    // Maximum lines of context to show before match
static const size_t MATCH_MAX_CHARS_IN_LINE       = 200;  // Maximum chars before adjusting line start in multi-line
static const size_t MATCH_ONELINER_CONTEXT        = 100;  // Context chars before match in one-liners
static const size_t MATCH_ELLIPSIS_LINE_THRESHOLD = 5;    // Only show "... " if more than this many lines before match


static COLstring MATCHconvertNewlineToBr(const char c) {
   if(c == '\n' || c == '\r') { return "<br>"; }
   return {1, c};
}

static COLstring MATCHhtmlEscapeChar(const char c) {
   switch(c) {
      case '&': return "&amp;";
      case '<': return "&lt;";
      case '>': return "&gt;";
      case '"': return "&quot;";
      case '\'': return "&#39;";
      case '\n': return "<br>";
      case '\r': return "<br>";
      default: return {1, c};
   }
}

static COLstring MATCHhtmlEscape(const COLstring& Input) {
   COLstring Out;
   for(size_t i = 0; i < Input.size(); ++i) { Out += MATCHhtmlEscapeChar(Input[i]); }
   return Out;
}

// this is more of a safeguard in case MATCH search somehow does not get the longest length at a specific position
// ex. [{0,5}, {0,4}] would lead to bad behavior -- see highlight/duplicate_*
static void MATCHinsertionSort(COLarray<size_t>& Vec) {
   COL_FUNCTION(MATCHinsertionSort);
   for(size_t i = 1; i < Vec.size(); ++i) {
      const size_t key = Vec[i];
      size_t       j   = i;
      while(j > 0 && Vec[j - 1] > key) {
         Vec[j] = Vec[j - 1];
         --j;
      }
      Vec[j] = key;
   }
}

// Find highlight ends starting at pos
static COLarray<size_t>* MATCHfindHighlightEnds(COLmap<size_t, COLarray<size_t>>& HighlightStarts, const size_t pos) {
   COL_FUNCTION(MATCHfindHighlightEnds);
   auto it = HighlightStarts.find(pos);
   if(it == HighlightStarts.end()) return nullptr;
   return &it->second;
}

// Highlight substrings inside given text using highlight positions relative to text start
static COLstring MATCHhighlightSubstrWithin(const COLstring& Text, const MATCHpositions& HlPositions) {
   COL_FUNCTION(MATCHhighlightSubstrWithin);
   COLstring                         Result;
   size_t                            Pos = 0;
   COLmap<size_t, COLarray<size_t>> HlStarts;
   for(const auto& p : HlPositions) { HlStarts[p.first].push_back(p.second); }

   while(Pos < Text.size()) {
      const COLarray<size_t>* EndsVec = MATCHfindHighlightEnds(HlStarts, Pos);
      if(EndsVec) {
         COLarray<size_t> ends = *EndsVec;
         MATCHinsertionSort(ends);
         size_t endPos = ends.back();
         if(endPos >= Text.size()) endPos = Text.size() - 1;
         Result += "<span class='LOGhighlight'>";
         for(; Pos <= endPos && Pos < Text.size(); ++Pos) { Result += Text[Pos]; }
         Result += "</span>";
      } else {
         Result += Text[Pos];
         ++Pos;
      }
   }
   return Result;
}

// Find all URLs in the input string, returning vector of (start,end) pairs (inclusive)
static MATCHpositions MATCHfindUrls(const COLstring& String) {
   COL_FUNCTION(MATCHfindUrls);
   const COLstring  URL_PATTERN = R"((https?:\/\/(?:www\.|(?!www))[a-zA-Z0-9\-._~%:/?#[\]@!$&'()*+,;=]+))";
   const REXmatcher UrlMatcher(URL_PATTERN, REXmatcher::CaseInsensitive);
   MATCHpositions UrlPositions;
   size_t                             SearchPos = 0;
   while(SearchPos < String.size()) {
      size_t UrlStart, UrlEnd;
      if(UrlMatcher.findMatch(String.c_str(), SearchPos, UrlStart, UrlEnd)) {
         UrlPositions.push_back({UrlStart, UrlEnd});
         SearchPos = UrlEnd > SearchPos ? UrlEnd : SearchPos + 1;
      } else {
         break;
      }
   }
   return UrlPositions;
}

// Extract highlight positions overlapping a range [start, end]
static MATCHpositions MATCHgetHighlightsInRange(
    const MATCHpositions& Highlights, const size_t Start, const size_t End) {
   COL_FUNCTION(MATCHgetHighlightsInRange);
   MATCHpositions Result;
   for(const auto& h : Highlights) {
      if(!(h.second < Start || h.first > End)) {
         size_t s = (h.first < Start) ? 0 : h.first - Start;
         size_t e = (h.second > End) ? End - Start : h.second - Start;
         Result.push_back({s, e});
      }
   }
   return Result;
}

// Process a URL substring, adding highlights inside the <a> tag
static COLstring MATCHprocessUrlSegment(const COLstring& String, const COLpair<size_t, size_t>& UrlPos,
                                        const MATCHpositions& Highlights) {
   COL_FUNCTION(MATCHprocessUrlSegment);

   const size_t    UrlStart = UrlPos.first;
   const size_t    UrlEnd   = UrlPos.second;
   const COLstring UrlText  = String.substr(UrlStart, UrlEnd - UrlStart + 1);

   // Escape URL for href attribute
   const COLstring HrefValue = MATCHhtmlEscape(UrlText);

   // Get highlights relative to the URL segment
   const auto HighlightsInUrl = MATCHgetHighlightsInRange(Highlights, UrlStart, UrlEnd);

   // Build <a> tag with escaped href, but process link text with highlight logic
   COLstring Output;
   Output += "<a href=\"" + HrefValue + "\" target=\"_blank\">";
   Output += MATCHhighlightSubstrWithin(UrlText, HighlightsInUrl);
   Output += "</a>";
   return Output;
}


// Process highlight spans outside URLs
static COLstring MATCHprocessHighlightSpan(const COLstring& String, size_t& Pos, const bool EscapeHtml,
                                           COLmap<size_t, COLarray<size_t>>& HighlightStarts) {
   COL_FUNCTION(MATCHprocessHighlightSpan);
   const COLarray<size_t>* HighlightEnds = MATCHfindHighlightEnds(HighlightStarts, Pos);
   if(!HighlightEnds) {
      COLstring Ch = EscapeHtml ? MATCHhtmlEscapeChar(String[Pos]) : COLstring(1, String[Pos]);
      ++Pos;
      return Ch;
   }
   COLarray<size_t> SortedEnds = *HighlightEnds;
   MATCHinsertionSort(SortedEnds);
   size_t HighlightEnd = SortedEnds.back();
   if(HighlightEnd >= String.size()) HighlightEnd = String.size() - 1;
   if(HighlightEnd < Pos) {
      COLstring Ch = EscapeHtml ? MATCHhtmlEscapeChar(String[Pos]) : COLstring(1, String[Pos]);
      ++Pos;
      return Ch;
   }
   COLstring Output = "<span class='LOGhighlight'>";
   for(; Pos <= HighlightEnd && Pos < String.size(); ++Pos) { Output += MATCHconvertNewlineToBr(String[Pos]); }
   Output += "</span>";
   return Output;
}

static size_t MATCHcomputeWindowEnd(const COLstring& String, const int Threshold, const int Maxlines) {
   COL_FUNCTION(MATCHcomputeWindowEnd);
   const size_t StringSize = String.size();
   size_t       WindowEnd  = StringSize;
   if((size_t)Threshold < StringSize) WindowEnd = Threshold;
   if(Maxlines >= 0) {
      size_t lines = 0;
      for(size_t i = 0; i < WindowEnd; ++i) {
         if(StringSize > i && String[i] == '\n') {
            ++lines;
            if(lines >= (size_t)Maxlines) {
               WindowEnd = i + 1;
               break;
            }
         }
      }
   }
   return WindowEnd;
}

static size_t MATCHcountLinesBetween(const COLstring& String, const size_t StartPos, const size_t EndPos) {
   COL_FUNCTION(MATCHcountLinesBetween);
   size_t Lines = 0;
   for(size_t i = StartPos; i < EndPos && i < String.size(); ++i) {
      if(String[i] == '\n') { ++Lines; }
   }
   return Lines;
}

static void MATCHadjustWindow(const COLstring& String, size_t& WindowStart, size_t& WindowEnd,
                              const size_t FirstMatchStart, const int Threshold, const int Maxlines) {
   COL_FUNCTION(MATCHadjustWindow);
   const size_t StringSize = String.size();

   if(Threshold == -1) {
      WindowStart = 0;
   } else {
      size_t MatchLineStart = 0;
      for(size_t i = 0; i < FirstMatchStart && i < StringSize; ++i) { if(String[i] == '\n') { MatchLineStart = i + 1; } }

      const size_t MatchPositionInLine = FirstMatchStart - MatchLineStart;

      if(MatchLineStart == 0) {
         WindowStart = MatchPositionInLine > MATCH_ONELINER_CONTEXT ? FirstMatchStart - MATCH_ONELINER_CONTEXT : 0;
      } else {
         if(MatchPositionInLine > MATCH_MAX_CHARS_IN_LINE) { MatchLineStart = FirstMatchStart - MATCH_MAX_CHARS_IN_LINE; }
         size_t LinesBack = 0;
         WindowStart      = MatchLineStart;
         size_t pos       = MatchLineStart;
         while(pos > 0 && LinesBack < MATCH_MAX_LINES_CONTEXT) {
            --pos;
            if(String[pos] == '\n') {
               ++LinesBack;
               if(LinesBack < MATCH_MAX_LINES_CONTEXT) { WindowStart = pos + 1; }
            }
         }
         if(pos == 0 && LinesBack < MATCH_MAX_LINES_CONTEXT) { WindowStart = 0; }
      }
   }

   WindowEnd = WindowStart + (size_t)Threshold;
   if(WindowEnd > StringSize) WindowEnd = StringSize;

   if(Maxlines >= 0) {
      size_t lines = 0;
      for(size_t i = WindowStart; i < WindowEnd; ++i) {
         if(StringSize > i && String[i] == '\n') {
            ++lines;
            if(lines >= (size_t)Maxlines) {
               WindowEnd = i + 1;
               break;
            }
         }
      }
   }
}

static void MATCHgenerateMarkupInWindow(const COLstring& String, const MATCHpositions& Positions,
                                        const size_t WindowStart, const size_t WindowEnd, const bool ConvertUrls,
                                        const bool EscapeHtml, COLstring* pOut) {
   COL_FUNCTION(MATCHgenerateMarkupWindow);
   MATCHpositions SubPositions;
   for(const auto& pos : Positions) {
      // Only use positions within window
      if(pos.first >= WindowStart && pos.second < WindowEnd) {
         SubPositions.push_back({pos.first - WindowStart, pos.second - WindowStart});
      }
      // Handle overlap at window boundaries
      else if(pos.second >= WindowStart && pos.first < WindowEnd) {
         size_t start = pos.first < WindowStart ? 0 : pos.first - WindowStart;
         size_t end   = pos.second >= WindowEnd ? WindowEnd - WindowStart - 1 : pos.second - WindowStart;
         SubPositions.push_back({start, end});
      }
   }
   const COLstring SearchString = String.substr(WindowStart, WindowEnd - WindowStart);
   *pOut                        = MATCHgenerateMarkup(SearchString, SubPositions, ConvertUrls, EscapeHtml, -1, -1);
}

COLstring MATCHgenerateMarkup(const COLstring& String, const MATCHpositions& Positions, const bool ConvertUrls,
                              const bool EscapeHtml, const int Threshold, const int Maxlines) {
   COL_FUNCTION(MATCHgenerateMarkup);
   if(Threshold != -1 || Maxlines != -1) {
      size_t WindowStart = 0;
      size_t WindowEnd   = String.size();

      if(Threshold != -1 || Maxlines != -1) {
         // Use the first match to center the window, or start from beginning if no matches
         if(Positions.size() != 0) {
            const size_t FirstMatchStart = Positions.front().first;
            MATCHadjustWindow(String, WindowStart, WindowEnd, FirstMatchStart, Threshold, Maxlines);
         } else {
            WindowEnd = MATCHcomputeWindowEnd(String, Threshold, Maxlines);
         }
      }

      COLstring Result;
      MATCHgenerateMarkupInWindow(String, Positions, WindowStart, WindowEnd, ConvertUrls, EscapeHtml, &Result);

      if(WindowStart != 0 && Positions.size() > 0) {
         const size_t FirstMatchStart = Positions.front().first;
         const size_t LinesBefore = MATCHcountLinesBetween(String, 0, FirstMatchStart);
         if(LinesBefore > MATCH_ELLIPSIS_LINE_THRESHOLD) { Result.prepend("... "); }
      }
      return Result;
   }

   COLmap<size_t, COLarray<size_t>> HighlightStarts;
   for(const auto& h : Positions) { HighlightStarts[h.first].push_back(h.second); }
   MATCHpositions UrlPositions;
   if(ConvertUrls) { UrlPositions = MATCHfindUrls(String); }

   // Find exclusion ranges to skip escaping/highlighting
   MATCHpositions Exclusions;
   MATCHfindExclusions(String, &Exclusions);

   COLstring Output;
   size_t    Pos = 0, UrlIndex = 0, ExclusionIndex = 0;

   while(Pos < String.size()) {
      // If current pos is at a exclusion start, copy snippet directly
      if(ExclusionIndex < Exclusions.size() && Pos == Exclusions[ExclusionIndex].first) {
         const size_t start = Exclusions[ExclusionIndex].first;
         const size_t end   = Exclusions[ExclusionIndex].second;
         Output += String.substr(start, end - start + 1);
         Pos = end + 1;
         ++ExclusionIndex;
         continue;
      }
      if(ConvertUrls && UrlIndex < UrlPositions.size() && UrlPositions[UrlIndex].first == Pos) {
         Output += MATCHprocessUrlSegment(String, UrlPositions[UrlIndex], Positions);
         Pos = UrlPositions[UrlIndex].second + 1;
         ++UrlIndex;
         continue;
      }
      Output += MATCHprocessHighlightSpan(String, Pos, EscapeHtml, HighlightStarts);
   }
   return Output;
}

int MATCHsearchAndCreateMarkup(const COLstring& Filter, const COLstring& String, const bool Exact, COLstring* pOut,
                               const bool ConvertUrls, const int Threshold, const int Maxlines, const bool EscapeHtml) {
   COL_FUNCTION(MATCHsearchAndCreateMarkup);
   MATCHpositions Positions;
   MATCHsearch(Filter, String, Exact, &Positions);
   size_t WindowStart = 0;
   size_t WindowEnd   = MATCHcomputeWindowEnd(String, Threshold, Maxlines);
   if(Positions.size() == 0) {
      MATCHgenerateMarkupInWindow(String, Positions, WindowStart, WindowEnd, ConvertUrls, EscapeHtml, pOut);
   } else {
      const size_t FirstMatchStart = Positions.front().first;
      MATCHadjustWindow(String, WindowStart, WindowEnd, FirstMatchStart, Threshold, Maxlines);
      MATCHgenerateMarkupInWindow(String, Positions, WindowStart, WindowEnd, ConvertUrls, EscapeHtml, pOut);
   }
   if(WindowStart != 0 && Positions.size() > 0) {
      const size_t FirstMatchStart = Positions.front().first;
      const size_t LinesBefore     = MATCHcountLinesBetween(String, 0, FirstMatchStart);
      if(LinesBefore > MATCH_ELLIPSIS_LINE_THRESHOLD) { pOut->prepend("... "); }
   }
   return Positions.size();
}
