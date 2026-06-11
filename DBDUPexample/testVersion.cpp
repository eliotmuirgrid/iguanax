// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testVersion
//
// Description: Implementation.
//
// Author: Vismay Shah
// Date:   Fri 17 Jan 2025 12:47pm
// ---------------------------------------------------------------------------
#include "testVersion.h"

#include <DBDUP/DBDUPreleaseInfo.h>
#include <DBDVER/DBDVERversion.h>

#include <UNIT/UNITapp.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

/*
As part of the upgrade process, we call the release server to see the latest version available. This gives:
VersionResponse = 
{
   "version": "10.1.107"
}
CrashReportResponse = 
{
   Success
}
*/

static void testVersionSame(){
   COL_FUNCTION(testVersionSame);
   COL_TRC("Testing the case where release version and local version are " << DBD_STRING_VERSION);
   COLstring ReleaseVersion = DBD_STRING_VERSION;
   bool UpgradeAvailable = false;
   bool IsNewer = false;
   UpgradeAvailable = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   COL_VAR(IsNewer);
   UNIT_ASSERT(!IsNewer);
   UNIT_ASSERT(!UpgradeAvailable);
}

static void testVersionUltraNew(){
   COL_FUNCTION(testVersionUltraNew);
   COLstring ReleaseVersion = COLintToString(DBD_VERSION_MAJOR + 1) + "." + COLintToString(DBD_VERSION_MINOR + 2) + "." + COLintToString(DBD_BUILD_NUMBER + 3);
   COL_TRC("Testing the case where release version is " << ReleaseVersion << " and the current version is " << DBD_STRING_VERSION);
   bool UpgradeAvailable = false;
   bool IsNewer = false;
   UpgradeAvailable = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   UNIT_ASSERT(!IsNewer);
   UNIT_ASSERT(UpgradeAvailable);
}

static void testVersionNewerMajor(){
   COL_FUNCTION(testVersionNewerMajor);
   COLstring ReleaseVersion = COLintToString(DBD_VERSION_MAJOR + 1) + "." + COLintToString(DBD_VERSION_MINOR) + "." + COLintToString(DBD_BUILD_NUMBER);
   COL_TRC("Testing the case where release version is " << ReleaseVersion << " and the current version is " << DBD_STRING_VERSION);
   bool UpgradeAvailable = false;
   bool IsNewer = false;
   UpgradeAvailable = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   UNIT_ASSERT(!IsNewer);
   UNIT_ASSERT(UpgradeAvailable);
}

static void testVersionNewerMinor(){
   COL_FUNCTION(testVersionNewerMinor);
   COLstring ReleaseVersion = COLintToString(DBD_VERSION_MAJOR) + "." + COLintToString(DBD_VERSION_MINOR + 1) + "." + COLintToString(DBD_BUILD_NUMBER);
   COL_TRC("Testing the case where release version is " << ReleaseVersion << " and the current version is " << DBD_STRING_VERSION);
   bool UpgradeAvailable = false;
   bool IsNewer = false;
   UpgradeAvailable = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   UNIT_ASSERT(!IsNewer);
   UNIT_ASSERT(UpgradeAvailable);
}

static void testVersionNewerPatch(){
   COL_FUNCTION(testVersionNewerPatch);
   COLstring ReleaseVersion = COLintToString(DBD_VERSION_MAJOR) + "." + COLintToString(DBD_VERSION_MINOR) + "." + COLintToString(DBD_BUILD_NUMBER + 1);
   COL_TRC("Testing the case where release version is " << ReleaseVersion << " and the current version is " << DBD_STRING_VERSION);
   bool UpgradeAvailable = false;
   bool IsNewer = false;
   UpgradeAvailable = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   UNIT_ASSERT(!IsNewer);
   UNIT_ASSERT(UpgradeAvailable);
}

static void testVersionOlder(){
   COL_FUNCTION(testVersionOlder);
   COLstring ReleaseVersion = "10.1.100";
   COL_TRC("Testing the case where release version is " << ReleaseVersion << " and the current version is " << DBD_STRING_VERSION);
   bool UpgradeAvailable = false;
   bool IsNewer = false;
   UpgradeAvailable = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   COL_VAR(IsNewer);
   UNIT_ASSERT(IsNewer);
   UNIT_ASSERT(!UpgradeAvailable);
}

static void testVersionOlderByOne(){
   COL_FUNCTION(testVersionOlderByOne);
   COLstring ReleaseVersion = COLintToString(DBD_VERSION_MAJOR) + "." + COLintToString(DBD_VERSION_MINOR) + "." + COLintToString(DBD_BUILD_NUMBER - 1);
   COL_TRC("Testing the case where release version is " << ReleaseVersion << " and the current version is " << DBD_STRING_VERSION);
   bool UpgradeAvailable = false;
   bool IsNewer = false;
   UpgradeAvailable = DBDUPcompareVersion(ReleaseVersion, &IsNewer);
   COL_VAR(IsNewer);
   UNIT_ASSERT(IsNewer);
   UNIT_ASSERT(!UpgradeAvailable);
}

void testVersion(UNITapp* pApp) {
   pApp->add("version/same",         &testVersionSame);
   pApp->add("version/newer/ultra",  &testVersionUltraNew);
   pApp->add("version/newer/major",  &testVersionNewerMajor);
   pApp->add("version/newer/minor",  &testVersionNewerMinor);
   pApp->add("version/newer/patch",  &testVersionNewerPatch);
   pApp->add("version/older",        &testVersionOlder);
   pApp->add("version/older/byone",  &testVersionOlderByOne);
}
