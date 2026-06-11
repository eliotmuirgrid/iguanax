// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTRNbinaryFile
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 12 May 2023 - 11:47 AM
// ---------------------------------------------------------------------------
#include "testTRNbinaryFile.h"

#include <UNIT/UNITapp.h>

#include <TRN/TRNideFileLoad.h>

// #include <COL/COLclosure.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


// As part of the compile process for annotations, we need to insert special functions into the lua code to mark
// the beginning and end of loops. This test exercises that, making sure TRNloopInsertEndCalls returns whether the
// code was modified or not correctly. See bug from IX-1702.
static void testCompileLoop(){
   COL_FUNCTION(testCompileLoop);

   // empty string
   UNIT_ASSERT(!TRNisBinaryContent(""));

   // curly quotes (’), accented vowels, and combining Unicode characters that can span 2-3 bytes each.
   UNIT_ASSERT(!TRNisBinaryContent("Héllö Wörld! C’est l’été. T̶e̶s̶t̶i̶n̶g̶ ̶s̶t̶r̶i̶k̶e̶t̶h̶r̶o̶u̶g̶h̶ and Ŵîđĝĕţş. 100% Cotton."));

   // \t (tabs) and \n (newlines). Note that \x07 (the "bell" character) is a control character
   UNIT_ASSERT(!TRNisBinaryContent(R"(\t\tif (ptr == nullptr) {\n\t\t\tlog_error("Null \x07 bell sequence?");\n\t\t\treturn false;\n\t\t})"));

   // First 32 bytes of a Windows PE
   UNIT_ASSERT( TRNisBinaryContent("\x4D\x5A\x90\x00\x03\x00\x00\x00\x04\x00\x00\x00\xFF\xFF\x00\x00\xB8\x00\x00\x00\x00\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00"));

   // Random Binary Data (No Nulls, but invalid UTF-8)
   COLstring Random;
   for (int i = 0; i < 100; i++) { Random.append(1, 0x80 + (i % 64)); }
   UNIT_ASSERT( TRNisBinaryContent(Random));

   // Mixed Content (50% Text, 50% Binary)
   COLstring Mixed("This is a text header.");
   Mixed.append(Mixed.size(), 0xFF);
   UNIT_ASSERT( TRNisBinaryContent(Mixed));

   // https://interfaceware.atlassian.net/browse/IXFI-204
   COLstring Test1 = R"1q2w(ID,Name,Title,Company,Notes,Quote,Address,Metadata,Amount,Percent,Special
1,"Smith, John","Senior ""Lead"" Engineer","ACME Corp","Handles R&D — especially AI/ML","""Innovation distinguishes between a leader and a follower."" – Steve Jobs","123 Main St., Apt #5, New York, NY","{""role"":""admin"",""active"":true}","$1,234.56","99.5%","Symbols: !@#$%^&*()_+-=[]{}|;':,./<>?"
2,"O'Connor, Mary","Director – Operations","Global-Tech","Works across EU–US teams","""Efficiency is doing things right; effectiveness is doing the right things."" – Drucker","45 King’s Rd., London, UK","{""role"":""manager"",""active"":false}","€987.65","87%","Emoji: 😀🚀✨"
3,"Chen ""Charlie"" Wei","Data Scientist","未来科技 (FutureTech)","Focus: NLP, CV, LLMs","""数据是新的石油"" (Data is the new oil)","88 科技大道, 上海","{""languages"":[""Python"",""R"",""Julia""]}","¥12,345.00","76.3%","Unicode: 漢字 / 한글 / العربية"
4,"García, Ana","VP — Strategy","Innovación S.A.","Latin-America expansion","""El conocimiento es poder"" – Francis Bacon","Calle 42 Nº 123, Bogotá","{""regions"":[""LATAM"",""EMEA""]}","$5,678.90","65%","Accents: áéíóú ñ ü"
5,"Müller, Franz","Senior DevOps Engineer","Cloud–Systems GmbH","Maintains CI/CD pipelines","""Automate everything!""","Hauptstraße 77, Berlin","{""tools"":[""Docker"",""K8s"",""Terraform""]}","€3,210.00","91.25%","German: ß ä ö ü"
6,"Nakamura, Yuki","Product Manager","株式会社テック","Coordinates cross-team releases","""改善 (Kaizen) is continuous improvement""","〒100-0001 東京都千代田区","{""priority"":""high"",""tags"":[""mobile"",""api""]}","¥7,890.00","88%","Japanese: 日本語"
7,"Doe, Jane","Researcher","Bio-Labs","Works on CRISPR, mRNA","""Science, not silence""","789 Science Blvd., Boston, MA","{""lab"":""B-12"",""clearance"":""level-3""}","$2,345.67","73.8%","Greek letters: α β γ δ"
8,"Al-Farsi, Omar","Security Analyst","Cyber-Sec Intl.","Focus on SOC — threat intel","""Trust, but verify""","شارع الملك فهد، الرياض","{""alerts"":42,""risk"":""medium""}","$4,567.89","82%","Arabic: مرحبا"
9,"Petrov, Ivan","Backend Developer","Data–Stream LLC","Maintains legacy APIs","""Code is like humor. When you have to explain it, it’s bad.""","ул. Ленина, 10, Москва","{""stack"":[""Go"",""PostgreSQL""]}","₽12,345.67","79%","Cyrillic: Привет"
10,"Test, Multiline","QA Engineer","Edge Cases Inc.","Testing newline fields","""First line
Second line
Third line""","404 Test Dr., Suite 1","{""tests"":[""unit"",""integration"",""e2e""]}","$0.00","100%","Tabs	and	spaces")1q2w";
   UNIT_ASSERT(!TRNisBinaryContent(Test1));

}

void testTRNbinaryFile(UNITapp* pApp) {
   pApp->add("is/binary/file",             &testCompileLoop);
}
