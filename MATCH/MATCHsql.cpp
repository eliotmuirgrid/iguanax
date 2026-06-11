//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: MATCHsql.cpp
//
//  Description: SQL-like search implementation using Abstract Syntax Tree (AST) parsing.
//
//  This module implements a search algorithm that supports SQL-like boolean operators:
//  - Sequence matching: Space-separated terms match as ordered sequences
//
//  Operator Precedence (highest to lowest):
//  1. NOT (unary, right-associative)
//  2. AND (binary, left-associative) 
//  3. OR (binary, left-associative)
//
//  Examples:
//  - "#dev #test" → matches the sequence "#dev #test" in order
//  - "#dev #test AND #supp" → matches sequence "#dev #test" AND "#supp"
//  - "error OR warning debug" → error OR sequence "warning debug"
//  - "#dev AND NOT #test #prod" → #dev AND NOT sequence "#test #prod"
//
//  Algorithm Flow:
//  1. Tokenization: Parse input into tokens (TERM, AND, OR, NOT, EOF)
//  2. Validation: Check for malformed sequences (leading/trailing operators, etc.)
//  3. AST Construction: Build expression tree respecting operator precedence
//  4. Evaluation: Traverse AST and perform regex matching for each term
//  5. Position Merging: Combine match positions from all successful terms
//
//  Features:
//  - Case-insensitive matching using REXmatcher
//  - Wildcard support: '*' matches zero or more non-space characters
//  - Exact match: Enforces word boundaries when enabled
//  - Exclusion zones: Respects pre-defined exclusion regions
//  - Match limiting: Configurable maximum number of matches per term
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-12, 11:44a.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLmap.h>
#include <MATCH/MATCHsql.h>
#include <MATCH/MATCHutils.h>
#include <REX/REXmatcher.h>
COL_LOG_MODULE;

enum MATCHtokenType { TOKEN_TERM, TOKEN_AND, TOKEN_OR, TOKEN_NOT, TOKEN_EOF };

struct MATCHtoken {
   MATCHtokenType m_Type;
   COLstring      m_Value;
};

struct MATCHexpNode {
   enum NodeType { TERM, AND_OP, OR_OP, NOT_OP };
   NodeType               m_Type;
   COLstring              m_Term;
   COLarray<MATCHexpNode> m_Children;  // operators

   MATCHexpNode() : m_Type(TERM) {}
   explicit MATCHexpNode(const NodeType t) : m_Type(t) {}
   MATCHexpNode(const NodeType t, const COLstring& termValue) : m_Type(t), m_Term(termValue) {}
};

static bool MATCHevalExpr(const MATCHexpNode& Node, const COLstring& String, bool ExactMatch,
                          COLmap<size_t, size_t>& BestMatchByStart, const MATCHpositions& Exclusions, int& MaxMatches);

static MATCHtokenType MATCHgetType(const COLstring& Value) {
   if(Value == "AND") { return TOKEN_AND; }
   if(Value == "OR")  { return TOKEN_OR; }
   if(Value == "NOT") { return TOKEN_NOT; }
   return TOKEN_TERM;
}

static COLarray<MATCHtoken> MATCHtokenizeSQL(const COLstring& Pattern) {
   COL_FUNCTION(MATCHtokenizeSQL);
   COL_VAR(Pattern);
   COLarray<MATCHtoken> Tokens;
   COLstring            Buffer;
   for(size_t i = 0; i <= Pattern.size(); ++i) {
      if(i == Pattern.size() || COLisspace(static_cast<unsigned char>(Pattern[i]))) {
         if(!Buffer.is_null() && !Buffer.isWhitespace()) { Tokens.push_back({MATCHgetType(Buffer), Buffer}); }
         Buffer.clear();
      } else {
         Buffer += Pattern[i];
      }
   }
   Tokens.push_back({TOKEN_EOF, ""});
   COL_VAR(Tokens.size());
   return Tokens;
}

static void MATCHmergeMatches(const COLmap<size_t, size_t>& SourceMatches, COLmap<size_t, size_t>& TargetMatches) {
   COL_FUNCTION(MATCHmergeMatches);
   for(const auto& kv : SourceMatches) {
      auto& ref = TargetMatches[kv.first];
      if(ref < kv.second) {
         COL_TRC("Updating match at position " << kv.first << ": " << ref << " -> " << kv.second);
         ref = kv.second;
      }
   }
}

static COLpair<MATCHexpNode, size_t> MATCHparseTermSequence(const COLarray<MATCHtoken>& Tokens, size_t Pos) {
   COL_FUNCTION(MATCHparseTermSequence);
   COLstring    Combined;
   const size_t StartPos = Pos;
   while(Pos < Tokens.size() && Tokens[Pos].m_Type == TOKEN_TERM) {
      if(Combined.size() != 0) Combined += " ";
      Combined += Tokens[Pos].m_Value;
      ++Pos;
   }
   COL_VAR2(Combined, Pos - StartPos);
   return {MATCHexpNode(MATCHexpNode::TERM, Combined), Pos};
}

static COLpair<MATCHexpNode, size_t> MATCHparsePrimary(const COLarray<MATCHtoken>& Tokens, size_t Pos) {
   COL_FUNCTION(MATCHparsePrimary);
   COL_VAR2(Tokens.size(), Pos);
   if(Pos >= Tokens.size()) {
      COL_TRC("Position beyond token array, returning empty term");
      return {MATCHexpNode(MATCHexpNode::TERM, ""), Pos};
   }
   if(Tokens[Pos].m_Type == TOKEN_TERM) {
      COL_TRC("Found TERM token, parsing sequence");
      return MATCHparseTermSequence(Tokens, Pos);
   }
   COL_TRC("Unexpected token type: " << Tokens[Pos].m_Type << ", skipping");
   return {MATCHexpNode(MATCHexpNode::TERM, ""), Pos + 1};
}

static COLpair<MATCHexpNode, size_t> MATCHparseNotExpression(const COLarray<MATCHtoken>& Tokens, size_t Pos) {
   COL_FUNCTION(MATCHparseNotExpression);
   COL_VAR2(Tokens.size(), Pos);
   if(Pos < Tokens.size() && Tokens[Pos].m_Type == TOKEN_NOT) {
      size_t count = 0;
      while(Pos < Tokens.size() && Tokens[Pos].m_Type == TOKEN_NOT) {
         ++count;
         ++Pos;
         COL_TRC("Found NOT token, count: " << count);
      }
      auto         Result = MATCHparsePrimary(Tokens, Pos);
      MATCHexpNode Node   = Result.first;
      for(size_t i = 0; i < count; ++i) {
         MATCHexpNode NotNode(MATCHexpNode::NOT_OP);
         NotNode.m_Children.push_back(Node);
         Node = NotNode;
         COL_TRC("Wrapped node with NOT operation, level: " << (i + 1));
      }
      COL_VAR2(count, Result.second);
      return {Node, Result.second};
   }
   COL_TRC("No NOT token found, parsing primary");
   return MATCHparsePrimary(Tokens, Pos);
}

static COLpair<MATCHexpNode, size_t> MATCHparseBinaryOp(
    const COLarray<MATCHtoken>& Tokens, size_t Pos, const MATCHtokenType OpType, const MATCHexpNode::NodeType NodeType,
    COLpair<MATCHexpNode, size_t> (*NextLevel)(const COLarray<MATCHtoken>&, size_t)) {
   COL_FUNCTION(MATCHparseBinaryOp);
   COL_VAR3(Tokens.size(), Pos, OpType);
   const auto   Result = NextLevel(Tokens, Pos);
   MATCHexpNode Left   = Result.first;
   Pos                 = Result.second;
   while(Pos < Tokens.size() && Tokens[Pos].m_Type == OpType) {
      COL_TRC("Found " << (OpType == TOKEN_AND ? "AND" : "OR") << " token at position " << Pos);
      ++Pos;  // consume operator token
      auto         RightResult = NextLevel(Tokens, Pos);
      MATCHexpNode OpNode(NodeType);
      OpNode.m_Children.push_back(Left);
      OpNode.m_Children.push_back(RightResult.first);
      Left = OpNode;
      Pos  = RightResult.second;
      COL_TRC("Created " << (OpType == TOKEN_AND ? "AND" : "OR") << " operation, new position: " << Pos);
   }
   return {Left, Pos};
}

static COLpair<MATCHexpNode, size_t> MATCHparseAndExpression(const COLarray<MATCHtoken>& Tokens, const size_t Pos) {
   return MATCHparseBinaryOp(Tokens, Pos, TOKEN_AND, MATCHexpNode::AND_OP, MATCHparseNotExpression);
}

static COLpair<MATCHexpNode, size_t> MATCHparseOrExpression(const COLarray<MATCHtoken>& Tokens, const size_t Pos) {
   return MATCHparseBinaryOp(Tokens, Pos, TOKEN_OR, MATCHexpNode::OR_OP, MATCHparseAndExpression);
}

static MATCHexpNode MATCHbuildExpressionTree(const COLarray<MATCHtoken>& Tokens) {
   COL_FUNCTION(MATCHbuildExpressionTree);
   for(size_t i = 0; i < Tokens.size(); ++i) {
      if(Tokens[i].m_Type != TOKEN_EOF) { return MATCHparseOrExpression(Tokens, i).first; }
   }
   return MATCHexpNode(MATCHexpNode::TERM, "");
}

static bool MATCHisTokenSequenceMalformed(const COLarray<MATCHtoken>& Tokens) {
   COL_FUNCTION(MATCHisTokenSequenceMalformed);
   if(Tokens.size() <= 1) return true;
   bool HasTerms = false;
   for(size_t i = 0; i < Tokens.size(); ++i) {
      const auto Current = Tokens[i].m_Type;
      if(Current == TOKEN_TERM) { HasTerms = true; }
      if(i == 0 && (Current == TOKEN_AND || Current == TOKEN_OR)) {
         COL_TRC("Found leading operators --> malformed");
         return true;
      }
      if(i == Tokens.size() - 2 && (Current == TOKEN_AND || Current == TOKEN_OR || Current == TOKEN_NOT)) {
         COL_TRC("Found trailing operators --> malformed");
         return true;
      }
      if(i < Tokens.size() - 1) {
         const auto next = Tokens[i + 1].m_Type;
         if((Current == TOKEN_AND || Current == TOKEN_OR) && (next == TOKEN_AND || next == TOKEN_OR)) {
            COL_TRC("Found consecutive binary operators --> malformed");
            return true;
         }
         if(Current == TOKEN_TERM && next == TOKEN_NOT && i + 2 < Tokens.size() && Tokens[i + 2].m_Type == TOKEN_TERM) {
            COL_TRC("Found mixed sequences: TERM NOT TERM --> malformed");
            return true;
         }
      }
   }
   COL_VAR(HasTerms);
   return !HasTerms;
}

static bool MATCHevaluateTerm(const COLstring& Term, const COLstring& String, const bool ExactMatch,
                              COLmap<size_t, size_t>& BestMatchByStart, const MATCHpositions& Exclusions,
                              int& MaxMatches) {
   COL_FUNCTION(MATCHevaluateTerm);
   COL_VAR4(Term, String.size(), ExactMatch, MaxMatches);
   if(Term.is_null() || Term.isWhitespace()) {
      COL_TRC("Term is null or whitespace, returning false");
      return false;
   }

   COLstring Pattern = MATCHescapeRegex(Term);
   if(ExactMatch && Term.find('*') == npos) {
      // Require a non-word char (or start/end) on both sides.
      // Word chars = [A-Za-z0-9_]; punctuation/whitespace count as boundaries.
      Pattern = "(?<![[:alnum:]_])" + Pattern + "(?![[:alnum:]_])";
   }
   COL_VAR(Pattern);

   const REXmatcher Matcher(Pattern, REXmatcher::CaseInsensitive);
   size_t           SearchPos  = 0;
   bool             FoundMatch = false;
   int              MatchCount = 0;

   while(SearchPos <= String.size() && MaxMatches > 0) {
      size_t start, end;
      if(Matcher.findMatch(String.c_str(), SearchPos, start, end)) {
         COL_TRC("Found potential match at (" << start << "," << end << ")");
         if(MATCHoverlapsExclusions(start, end, Exclusions)) {
            COL_TRC("Match overlaps exclusions, skipping");
            SearchPos = end + 1;
            continue;
         }
         COLmap<size_t, size_t> NewMatch;
         NewMatch[start] = end;
         MATCHmergeMatches(NewMatch, BestMatchByStart);
         FoundMatch = true;
         MaxMatches--;
         MatchCount++;
         SearchPos = (end > SearchPos) ? end : SearchPos + 1;
         COL_TRC("Added match, remaining MaxMatches: " << MaxMatches);
      } else {
         COL_TRC("No more matches found");
         break;
      }
   }
   COL_VAR2(FoundMatch, MatchCount);
   return FoundMatch;
}

static bool MATCHevaluateBinaryOp(const MATCHexpNode& Node, const COLstring& String, const bool ExactMatch,
                                  COLmap<size_t, size_t>& BestMatchByStart, const MATCHpositions& Exclusions,
                                  int& MaxMatches, const bool IsAndOperation) {
   COL_FUNCTION(MATCHevaluateBinaryOp);
   COL_VAR3(IsAndOperation, MaxMatches, Node.m_Children.size());
   if(Node.m_Children.size() != 2) return false;

   COLmap<size_t, size_t> LeftMatches, RightMatches;
   int                    LeftMax = MaxMatches, RightMax = MaxMatches;

   const bool LeftResult = MATCHevalExpr(Node.m_Children[0], String, ExactMatch, LeftMatches, Exclusions, LeftMax);
   COL_VAR3(LeftResult, LeftMatches.size(), LeftMax);

   if(IsAndOperation && !LeftResult) {
      COL_TRC("AND operation: left failed, short-circuiting");
      MaxMatches = LeftMax;
      return false;
   }

   const bool RightResult = MATCHevalExpr(Node.m_Children[1], String, ExactMatch, RightMatches, Exclusions, RightMax);
   COL_VAR3(RightResult, RightMatches.size(), RightMax);

   const bool Success = IsAndOperation ? RightResult : LeftResult || RightResult;
   COL_TRC((IsAndOperation ? "AND" : "OR") << " operation result: " << Success);

   if(Success) {
      const size_t InitialSize = BestMatchByStart.size();
      if(IsAndOperation || LeftResult)  { MATCHmergeMatches(LeftMatches, BestMatchByStart); }
      if(IsAndOperation || RightResult) { MATCHmergeMatches(RightMatches, BestMatchByStart); }
      COL_TRC("Merged matches: " << InitialSize << " -> " << BestMatchByStart.size());
   }
   MaxMatches = LeftMax < RightMax ? LeftMax : RightMax;
   return Success;
}

static bool MATCHevaluateNotOp(const MATCHexpNode& Node, const COLstring& String, const bool ExactMatch,
                               const int& MaxMatches, const MATCHpositions& Exclusions) {
   COL_FUNCTION(MATCHevaluateNotOp);
   COL_TRC("Evaluating NOT operation");
   if(Node.m_Children.size() != 1) {
      COL_TRC("NOT operation node doesn't have exactly 1 child");
      return false;
   }
   COLmap<size_t, size_t> TempMatches;
   int                    TempMax = MaxMatches;
   COL_TRC("Evaluating NOT child expression");
   const bool ChildResult = MATCHevalExpr(Node.m_Children[0], String, ExactMatch, TempMatches, Exclusions, TempMax);
   const bool NotResult   = !ChildResult;
   COL_TRC("NOT result: " << ChildResult << " -> " << NotResult);
   return NotResult;
}

static bool MATCHevalExpr(const MATCHexpNode& Node, const COLstring& String, const bool ExactMatch,
                          COLmap<size_t, size_t>& BestMatchByStart, const MATCHpositions& Exclusions, int& MaxMatches) {
   COL_FUNCTION(MATCHevalExpr);
   COL_VAR2(Node.m_Type, MaxMatches);

   switch(Node.m_Type) {
      case MATCHexpNode::TERM:
         COL_TRC("Evaluating TERM: '" << Node.m_Term << "'");
         return MATCHevaluateTerm(Node.m_Term, String, ExactMatch, BestMatchByStart, Exclusions, MaxMatches);

      case MATCHexpNode::AND_OP:
         COL_TRC("Evaluating AND operation");
         return MATCHevaluateBinaryOp(Node, String, ExactMatch, BestMatchByStart, Exclusions, MaxMatches, true);

      case MATCHexpNode::OR_OP:
         COL_TRC("Evaluating OR operation");
         return MATCHevaluateBinaryOp(Node, String, ExactMatch, BestMatchByStart, Exclusions, MaxMatches, false);

      case MATCHexpNode::NOT_OP:
         COL_TRC("Evaluating NOT operation");
         return MATCHevaluateNotOp(Node, String, ExactMatch, MaxMatches, Exclusions);
   }
   COL_TRC("Unknown node type: " << Node.m_Type << ", returning false");
   return false;
}

static bool MATCHprepareTokens(const COLstring& Filter, COLarray<MATCHtoken>& Tokens) {
   COL_FUNCTION(MATCHprepareTokens);
   Tokens = MATCHtokenizeSQL(Filter);
   if(MATCHisTokenSequenceMalformed(Tokens)) {
      COL_TRC("Filter rejected due to malformed token sequence");
      return false;
   }
   return true;
}

static bool MATCHevaluateExpression(const MATCHexpNode& RootExpr, const COLstring& String, const bool ExactMatch,
                                    const MATCHpositions& Exclusions, COLmap<size_t, size_t>& BestMatchByStart,
                                    const int MaxMatches) {
   COL_FUNCTION(MATCHevaluateExpression);
   COL_VAR2(ExactMatch, MaxMatches);
   int        MatchesRemaining = MaxMatches;
   const bool EvalResult = MATCHevalExpr(RootExpr, String, ExactMatch, BestMatchByStart, Exclusions, MatchesRemaining);
   COL_VAR3(EvalResult, BestMatchByStart.size(), MatchesRemaining);
   return EvalResult;
}

bool MATCHsql(const COLstring& Filter, const COLstring& String, const bool ExactMatch, MATCHpositions* pPositions,
              const int MaxMatches) {
   COL_FUNCTION(MATCHsql);
   COL_VAR4(Filter, String.size(), ExactMatch, MaxMatches);
   pPositions->clear();
   COLarray<MATCHtoken> Tokens;
   if(!MATCHprepareTokens(Filter, Tokens)) { return false; }
   const MATCHexpNode RootExpr = MATCHbuildExpressionTree(Tokens);
   MATCHpositions     Exclusions;
   MATCHfindExclusions(String, &Exclusions);
   COLmap<size_t, size_t> BestMatchByStart;
   if(!MATCHevaluateExpression(RootExpr, String, ExactMatch, Exclusions, BestMatchByStart, MaxMatches)) {
      COL_TRC("SQL matching failed");
      return false;
   }
   for(const auto& kv : BestMatchByStart) { pPositions->push_back({kv.first, kv.second}); }
   COL_TRC("SQL matching succeeded with " << pPositions->size() << " positions");
   return true;
}