//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIThtmlCrop
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, September 8th, 2011 @ 11:46:21 AM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFIThtmlCrop.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <SFI/SFIhtmlCrop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring testHtmlCropHelper(const COLstring& Html, COLuint32 Length)
{
   COLstring Cropped;
   SFIhtmlCrop(Html.c_str(), Html.size(), "...", Length, Cropped);
   return Cropped;
}

static void testHtmlCrop()
{
   // Base cases - crop all or nothing
   TST_ASSERT_EQUALS("...", testHtmlCropHelper("Some HTML without tags", 0));
   TST_ASSERT_EQUALS("Some <span>HTML</span>", testHtmlCropHelper("Some <span>HTML</span>", 256));

   // Simple cases
   TST_ASSERT_EQUALS("Some HTML...", testHtmlCropHelper("Some HTML without tags", 9));
   TST_ASSERT_EQUALS("Text &amp; e...", testHtmlCropHelper("Text &amp; escape sequence", 8));

   // Input has exactly the right number of characters.
   TST_ASSERT_EQUALS("Some <b>HTML</b> with<br> <span style='display:block;'>tags</span> in it.", testHtmlCropHelper("Some <b>HTML</b> with<br> <span style='display:block;'>tags</span> in it.", 26));

   // Input has one less than the right number of characters.
   TST_ASSERT_EQUALS("Some <b>HTML</b> with<br> <span style='display:block;'>tags</span> in it...", testHtmlCropHelper("Some <b>HTML</b> with<br> <span style='display:block;'>tags</span> in it.", 25));

   // Tag is opened before cropping, and closed after cropping.
   TST_ASSERT_EQUALS("Some <span>HT</span>...", testHtmlCropHelper("Some <span>HTML</span>", 7));
   TST_ASSERT_EQUALS("<div style='white-space: nowrap;'>Some <span>HT</span></div>...", testHtmlCropHelper("<div style='white-space: nowrap;'>Some <span>HTML</span> with nested tags</div>", 7));

   // Tags are opened and closed after cropping.
   TST_ASSERT_EQUALS("Some...", testHtmlCropHelper("Some <span>HTML</span>", 4));
   TST_ASSERT_EQUALS("Some...", testHtmlCropHelper("Some <div style='white-space: nowrap;'><span>HTML</span> with tags</div>", 4));
   TST_ASSERT_EQUALS("Some...", testHtmlCropHelper("Some <span><a><b><h1><div>HTML</div></b></a></span>", 4));

   // Consecutive tags and escape sequences.
   TST_ASSERT_EQUALS("Some...", testHtmlCropHelper("Some <span><b>Tags</b></span> &amp;&amp; escape characters", 4));
   TST_ASSERT_EQUALS("Some <span><b>Tags</b></span> &amp;&amp; e...", testHtmlCropHelper("Some <span><b>Tags</b></span> &amp;&amp; escape characters", 14));

   // Random closing tag after cropping.
   TST_ASSERT_EQUALS("Some HT</span>...", testHtmlCropHelper("Some HTML</span>", 7));

   // Unclosed tags.
   TST_ASSERT_EQUALS("Some <br>HT...", testHtmlCropHelper("Some <br>HTML", 7));
   TST_ASSERT_EQUALS("Some <br>HT...", testHtmlCropHelper("Some <br>HTML<br>", 7));
   TST_ASSERT_EQUALS("Some <br>HT</div>...", testHtmlCropHelper("Some <br>HTML<br><br><br><span></span></div>", 7));

   // Tags and escape sequences right around the cropping point.
   TST_ASSERT_EQUALS("Some <span>H</span>...", testHtmlCropHelper("Some <span>HTML</span>", 6));
   TST_ASSERT_EQUALS("Text &amp;...", testHtmlCropHelper("Text &amp; escape sequence", 6));
   TST_ASSERT_EQUALS("Some ...", testHtmlCropHelper("Some <span>HTML</span>", 5));
   TST_ASSERT_EQUALS("Text ...", testHtmlCropHelper("Text &amp; escape sequence", 5));

   // HTML length is longer than max length, but rendered length is shorter.
   TST_ASSERT_EQUALS("Please don't &quot;crop&quot; me", testHtmlCropHelper("Please don't &quot;crop&quot; me", 29));
}

TSTtestable* SFIThtmlCrop()
{
   TST_START_COLLECTION("SFIThtmlCrop")
      TST_ADD_TEST_FUNCTION(testHtmlCrop);
   TST_END_FIXTURE
};
