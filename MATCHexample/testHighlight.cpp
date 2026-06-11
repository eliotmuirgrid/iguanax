//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testHighlight.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   14/05/24 2:18 PM
//  ---------------------------------------------------------------------------
#include "testHighlight.h"

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <LINK/LINKconstants.h>
#include <MATCH/MATCHhighlight.h>
#include <MATCH/MATCHsearch.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testAssert(const COLstring& Filter, const COLstring& Html, const int Matches, const COLstring& Out) {
   COLstring Actual;
   UNIT_ASSERT_EQUALS(Matches, MATCHsearchAndCreateMarkup(Filter, Html, false, &Actual, true, -1, -1));
   UNIT_ASSERT_EQUALS(Out, Actual);
}

static void testHighlightDuplicateStartsPlain() {
   // Two ranges share start 0; longest should be chosen
   const COLstring S = "abcdefg";
   MATCHpositions Positions;

   // Insert longer first, shorter second
   Positions.push_back({0, 5}); // "abcdef"
   Positions.push_back({0, 2}); // "abc"

   const COLstring Out = MATCHgenerateMarkup(S, Positions, false, true);
   UNIT_ASSERT_EQUALS("<span class='LOGhighlight'>abcdef</span>g", Out);
}

static void testHighlightDuplicateStartsPlainReversed() {
   // Same as above but reverse insertion order; result must be identical
   const COLstring S = "abcdefg";
   MATCHpositions Positions;

   // Insert shorter first, longer second
   Positions.push_back({0, 2}); // "abc"
   Positions.push_back({0, 5}); // "abcdef"

   const COLstring Out = MATCHgenerateMarkup(S, Positions, false, true);
   UNIT_ASSERT_EQUALS("<span class='LOGhighlight'>abcdef</span>g", Out);
}

static void testHighlightDuplicateStartsInUrl() {
   // Inside URL, duplicate-start ranges must also merge to farthest end
   const COLstring                    S = "https://example.com";
   MATCHpositions Positions;

   // Entire URL vs. a shorter prefix; both start at 0
   Positions.push_back({0, 18});  // full "https://example.com" (length 19 => last index 18)
   Positions.push_back({0, 3});   // "http"

   const COLstring Out = MATCHgenerateMarkup(S, Positions, true, true);
   UNIT_ASSERT_EQUALS(
       "<a href=\"https://example.com\" target=\"_blank\"><span class='LOGhighlight'>https://example.com</span></a>",
       Out);
}

static void testHighlightExclusions() {
   COL_FUNCTION(testHighlightExclusions);
   const COLstring Snippet = "<span class=\"line_cropped_warning\">&hellip; This line is too long to display.</span>";

   // search term only appears inside the excluded snippet (no matches expected)
   testAssert("line", "AAA " + Snippet + " BBB", 0,
              "AAA <span class=\"line_cropped_warning\">&hellip; This line is too long to display.</span> BBB");

   // mtches before and after the excluded snippet, snippet remains unmodified
   testAssert("line", "line before " + Snippet + " after line", 2,
              "<span class='LOGhighlight'>line</span> before <span class=\"line_cropped_warning\">&hellip; This line "
              "is too long to display.</span> after <span class='LOGhighlight'>line</span>");

   // url conversion still works, highlight appears inside URL, not in the excluded snippet
   testAssert("example", "See https://example.com/path " + Snippet + " ok", 1,
              "See <a href=\"https://example.com/path\" target=\"_blank\">https://<span "
              "class='LOGhighlight'>example</span>.com/path</a> <span class=\"line_cropped_warning\">&hellip; This "
              "line is too long to display.</span> ok");

   // html escaping occurs outside the snippet, '&' inside snippet is not highlighted/escaped
   testAssert("&", "A & B " + Snippet + " &tail", 2,
              "A <span class='LOGhighlight'>&</span> B <span class=\"line_cropped_warning\">&hellip; This line is too "
              "long to display.</span> <span class='LOGhighlight'>&</span>tail");
}

static void testHighlightEdgeCases() {
   COL_FUNCTION(testHighlightEdgeCases);
   // Empty string test
   testAssert("test", "", 0, "");

   // Search term longer than text
   testAssert("hello", "hi", 0, "hi");

   // Special characters
   testAssert("!@#$%^&()_+", "test!@#$%^&()_+ test", 1, "test<span class='LOGhighlight'>!@#$%^&()_+</span> test");

   // Multiple line breaks
   testAssert("line", "line1\n\n\nline2\nline3", 3,
              "<span class='LOGhighlight'>line</span>1<br><br><br><span class='LOGhighlight'>line</span>2<br><span "
              "class='LOGhighlight'>line</span>3");

   // HTML entities
   testAssert("&", "Test &lt;div&gt; &amp; &quot;quoted&quot;", 5,
              "Test <span class='LOGhighlight'>&</span>lt;div<span class='LOGhighlight'>&</span>gt; <span "
              "class='LOGhighlight'>&</span>amp; <span class='LOGhighlight'>&</span>quot;quoted<span "
              "class='LOGhighlight'>&</span>quot;");

   // Complex URL with special characters
   testAssert("param", "https://example.com/path?param=value&other=123#fragment", 1,
              "<a href=\"https://example.com/path?param=value&amp;other=123#fragment\" "
              "target=\"_blank\">https://example.com/path?<span "
              "class='LOGhighlight'>param</span>=value&other=123#fragment</a>");
}

// included from IX-3731
static void testHtml() {
   const COLstring Html =
       "<b>IguanaX <> Migration:</b>&nbsp; <div><ul><li>They have not answered any IguanaX emails. Have only answered "
       "emails over the last 2 years about "
       "pricing.&nbsp;</li></ul><div><b>Bottlenecks:</b></div><div><ul><li><b>#NoContact -&nbsp;</b>the customer does "
       "not answer emails regarding IguanaX. Efforts have been made over the last few years to get them on the phone "
       "and discuss Iguana but they have always gone silent.&nbsp;</li><li>Have not yet discussed pricing, but can "
       "anticipate&nbsp;<b>#Cost</b>&nbsp;being a potential bottleneck - they requested to lower their pricing in 2022 "
       "from $3000 to $2000 due to decreased usage.</li></ul></div></div>";
   COLstring Filter = "&nbsp";
   COLstring Expected =
       "&lt;b&gt;IguanaX &lt;&gt; Migration:&lt;/b&gt;<span class='LOGhighlight'>&nbsp</span>; "
       "&lt;div&gt;&lt;ul&gt;&lt;li&gt;They have not answered any IguanaX emails. Have only answered emails over the "
       "last 2 years about pricing.<span "
       "class='LOGhighlight'>&nbsp</span>;&lt;/li&gt;&lt;/ul&gt;&lt;div&gt;&lt;b&gt;Bottlenecks:&lt;/b&gt;&lt;/"
       "div&gt;&lt;div&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;#NoContact -<span "
       "class='LOGhighlight'>&nbsp</span>;&lt;/b&gt;the customer does not answer emails regarding IguanaX. Efforts "
       "have been made over the last few years to get them on the phone and discuss Iguana but they have always gone "
       "silent.<span class='LOGhighlight'>&nbsp</span>;&lt;/li&gt;&lt;li&gt;Have not yet discussed pricing, but can "
       "anticipate<span class='LOGhighlight'>&nbsp</span>;&lt;b&gt;#Cost&lt;/b&gt;<span "
       "class='LOGhighlight'>&nbsp</span>;being a potential bottleneck - they requested to lower their pricing in 2022 "
       "from $3000 to $2000 due to decreased usage.&lt;/li&gt;&lt;/ul&gt;&lt;/div&gt;&lt;/div&gt;";
   testAssert(Filter, Html, 6, Expected);

   Filter = "-";
   Expected =
       "&lt;b&gt;IguanaX &lt;&gt; Migration:&lt;/b&gt;&amp;nbsp; &lt;div&gt;&lt;ul&gt;&lt;li&gt;They have not answered "
       "any IguanaX emails. Have only answered emails over the last 2 years about "
       "pricing.&amp;nbsp;&lt;/li&gt;&lt;/ul&gt;&lt;div&gt;&lt;b&gt;Bottlenecks:&lt;/b&gt;&lt;/"
       "div&gt;&lt;div&gt;&lt;ul&gt;&lt;li&gt;&lt;b&gt;#NoContact <span "
       "class='LOGhighlight'>-</span>&amp;nbsp;&lt;/b&gt;the customer does not answer emails regarding IguanaX. "
       "Efforts have been made over the last few years to get them on the phone and discuss Iguana but they have "
       "always gone silent.&amp;nbsp;&lt;/li&gt;&lt;li&gt;Have not yet discussed pricing, but can "
       "anticipate&amp;nbsp;&lt;b&gt;#Cost&lt;/b&gt;&amp;nbsp;being a potential bottleneck <span "
       "class='LOGhighlight'>-</span> they requested to lower their pricing in 2022 from $3000 to $2000 due to "
       "decreased usage.&lt;/li&gt;&lt;/ul&gt;&lt;/div&gt;&lt;/div&gt;";
   testAssert(Filter, Html, 2, Expected);
}

static void testHighlight() {
   COL_FUNCTION(testHighlight);
   MATCHpositions Positions;
   const COLstring& Single          = "This should be highlighted";
   MATCHsearch("this", Single, false, &Positions);
   UNIT_ASSERT_EQUALS(1, Positions.size());
   const COLstring& SingleHighlight = MATCHgenerateMarkup(Single, Positions, true, true);
   UNIT_ASSERT_EQUALS("<span class='LOGhighlight'>This</span> should be highlighted", SingleHighlight);

   const COLstring& Double          = "all all";
   MATCHsearch("all", Double, false, &Positions);
   UNIT_ASSERT_EQUALS(2, Positions.size());
   const COLstring& DoubleHighlight = MATCHgenerateMarkup(Double, Positions, true, true);
   UNIT_ASSERT_EQUALS("<span class='LOGhighlight'>all</span> <span class='LOGhighlight'>all</span>", DoubleHighlight);

   const COLstring& Substr          = "highlight only high";
   MATCHsearch("high", Substr, false, &Positions);
   UNIT_ASSERT_EQUALS(2, Positions.size());
   const COLstring& SubstrHighlight = MATCHgenerateMarkup(Substr, Positions, true, true);
   UNIT_ASSERT_EQUALS("<span class='LOGhighlight'>high</span>light only <span class='LOGhighlight'>high</span>", SubstrHighlight);

   const COLstring& Multi          = "highlight everything here";
   MATCHsearch("high* every* here", Multi, false, &Positions);
   UNIT_ASSERT_EQUALS(1, Positions.size());
   const COLstring& MultiHighlight = MATCHgenerateMarkup(Multi, Positions, true, true);
   UNIT_ASSERT_EQUALS("<span class='LOGhighlight'>highlight everything here</span>",MultiHighlight);

   const COLstring& Url = "https://bitbucket.org/interfaceware";
   MATCHsearch("bit", Url, false, &Positions);
   UNIT_ASSERT_EQUALS(1, Positions.size());
   const COLstring& UrlHighlight = MATCHgenerateMarkup(Url, Positions, true, true);
   UNIT_ASSERT_EQUALS(
       "<a href=\"https://bitbucket.org/interfaceware\" target=\"_blank\">https://<span "
       "class='LOGhighlight'>bit</span>bucket.org/interfaceware</a>",
       UrlHighlight);

   MATCHsearch("bit bu", Url, false, &Positions);
   UNIT_ASSERT_EQUALS(0, Positions.size());
   const COLstring& UrlDoubleHighlight = MATCHgenerateMarkup(Url, Positions, true, true);
   UNIT_ASSERT_EQUALS(
       "<a href=\"https://bitbucket.org/interfaceware\" target=\"_blank\">https://bitbucket.org/interfaceware</a>",
       UrlDoubleHighlight);

   const COLstring& Test =
       "#git #endpoint #start Begin refresh of git cache.\n"
       "Dir: /Users/void/Library/IguanaX/git_cache/\n"
       "More info: " +
       LINKgitCache;
   MATCHsearch("git", Test, true, &Positions);
   COL_VAR(Positions.size());
   UNIT_ASSERT_EQUALS(2, Positions.size());
   const COLstring& TestHighlight = MATCHgenerateMarkup(Test, Positions, true, true);
   UNIT_ASSERT_EQUALS(
       "#<span class='LOGhighlight'>git</span> #endpoint #start Begin refresh of <span class='LOGhighlight'>git</span> cache.<br>Dir: "
       "/Users/void/Library/IguanaX/git_cache/<br>More info: <a href=\"" +
           LINKgitCache + "\" target=\"_blank\">https://man.interfaceware.com/code/2647621645</a>",
       TestHighlight);

   MATCHsearch("* git", Test, true, &Positions);
   UNIT_ASSERT_EQUALS(1, Positions.size());
   const COLstring& TestHighlightWildcard = MATCHgenerateMarkup(Test, Positions, true, true);
   UNIT_ASSERT_EQUALS(
       "#git #endpoint #start Begin refresh <span class='LOGhighlight'>of git</span> cache.<br>Dir: "
       "/Users/void/Library/IguanaX/git_cache/<br>More info: <a href=\"" + LINKgitCache +
       "\" target=\"_blank\">https://man.interfaceware.com/code/2647621645</a>",
       TestHighlightWildcard);


   const COLstring& PreWildcard = "#startup Log system initialization completed.";
   MATCHsearch("*up", PreWildcard, true, &Positions);
   UNIT_ASSERT_EQUALS(1, Positions.size());
   const COLstring& TestHighlightWildcard2 = MATCHgenerateMarkup(PreWildcard, Positions, true, true);
   UNIT_ASSERT_EQUALS("<span class='LOGhighlight'>#startup</span> Log system initialization completed.",
                      TestHighlightWildcard2);
}

void testHighlight(UNITapp* pApp) {
   pApp->add("highlight",            &testHighlight);
   pApp->add("highlight/html",       &testHtml);
   pApp->add("highlight/edge_cases", &testHighlightEdgeCases);
   pApp->add("highlight/exclusions", &testHighlightExclusions);
   pApp->add("highlight/duplicate_starts_plain",            &testHighlightDuplicateStartsPlain);
   pApp->add("highlight/duplicate_starts_plain_reversed",   &testHighlightDuplicateStartsPlainReversed);
   pApp->add("highlight/duplicate_starts_in_url",           &testHighlightDuplicateStartsInUrl);
}
