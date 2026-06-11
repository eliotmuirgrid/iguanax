// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CACHEfile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 04:26PM
// ---------------------------------------------------------------------------
#include <CACHE/CACHEfile.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GZIP/GZIPutils.h>
#include <PACK/PACKattach.h>
COL_LOG_MODULE;

static COLvar* s_pCACHEvar = 0;

extern "C" unsigned char PACKdata[];
extern "C" unsigned int PACKdataSize;

static void CACHEprint() {
   COL_FUNCTION(CACHEprint);
   COL_TRC("If you want a list of files in the cache change to debug level tracing.");
   for(const auto& it : s_pCACHEvar->map()) {
      COL_DBG(it->first);
   }
}

void CACHEsetCache(COLvar* pCache) {
   COL_FUNCTION(CACHEsetCache);
   if(!pCache->size()) { return; }
   s_pCACHEvar = pCache;
   CACHEprint();
}

bool CACHEload() {
   COL_FUNCTION(CACHEload);
   COLstring Error, Binary;

   COLvar* pFileCache = new COLvar();
   if (PACKdataSize > 0){
      COL_TRC("Loading from C!");
      int BytesConsumed;
      COLvar::ParseState Result = pFileCache->parseBinary(PACKdata, PACKdataSize, &BytesConsumed);
      COLASSERT(Result == COLvar::kSUCCESS);
      COLASSERT(BytesConsumed == PACKdataSize);
      CACHEsetCache(pFileCache);
      return true;
   } else {
      Binary = FILpathAppend(FILexecutableDir(), FILexecutableFileName());
      if (PACKextractFromExe(Binary, pFileCache, &Error)) {  // TODO mixing concerns of reading the data and parsing it makes it harder to alter the flow
         COL_TRC("Loaded cache from executable: " << Binary);
         CACHEsetCache(pFileCache);
         return true;
      } else {
         delete pFileCache;
         COL_TRC("Did not load cache from executable: " << Error);
         return false;
      }
   }
}

static void CACHEcopyCaBundle(const COLstring& OutputLocation){
   COL_FUNCTION(CACHEcopyCaBundle);
   // copy from main2/Iguana/app/cabundle/cacert.pem
   COLstring CaLocation, CaLocationErr;
   FILpathSimplify(FILpathAppend(FILexecutableDir(), "../cabundle/cacert.pem"), &CaLocation, &CaLocationErr);
   FILcorrectPathSeparators(CaLocation);
   FILcopyFile(CaLocation, OutputLocation, true);
}

void CACHEsaveCaBundle(){
   COL_FUNCTION(CACHEsaveCaBundle);
   COLstring OutputCaBundlePath = DIRfileResourceCaFile();
   FILcorrectPathSeparators(OutputCaBundlePath);
   if (!CACHEexists()) {
      FILcreateDirectoryTreeToFile(OutputCaBundlePath, FIL_USER_RWX);
      CACHEcopyCaBundle(OutputCaBundlePath);
      COL_TRC("No cache");
      return;
   }
   COLstring Path = "/cabundle/cacert.pem";
   COLstring Content;
   if (!s_pCACHEvar->exists(Path)) {
      COL_TRC("Didn't find it");
      return;
   } 
   GZIPuncompress((*s_pCACHEvar)[Path].asString(), &Content);
   try {
      FILwriteFile(OutputCaBundlePath, Content);
   } catch (const COLerror& Error) {
      COL_ERR(Error.description());
   }
}

static bool CACHEcheckCustomHelpFile(const COLstring& Path, COLstring* pContent) {
   COL_FUNCTION(CACHEcheckCustomHelpFile);
   COL_TRC("Have custom help file -- checking disk");
   if(!FILfileExists(Path)) {
      COL_TRC(Path << " not found on disk");
      return false;
   }
   try {
      FILreadFile(Path, pContent);
      return true;
   }
   catch(const COLerror& e) { COL_ERR(e.description()); }
   return false;
}

bool CACHEreadFile(const COLstring& Path, COLstring* pContent) {
   COL_FUNCTION(CACHEreadFile);
   COL_VAR(Path);
   if (!CACHEexists()) {
      if (!FILfileExists(Path)) { COL_TRC(Path + " not found on disk"); return false; }
      FILreadFile(Path, pContent);
      COL_VAR2(Path, pContent->size());
      return true;
   }

   if (!s_pCACHEvar->exists(Path)) {
      if(FILpathExtLast(Path, true) != ".help") { COL_TRC(Path + " not found in cache"); return false; }
      return CACHEcheckCustomHelpFile(Path, pContent);
   }

   if(FILpathExtLast(Path, true) == ".help") { GZIPuncompress((*s_pCACHEvar)[Path].asString(), pContent); }
   else                                      { *pContent = s_pCACHEvar->getString(Path); }
   COL_DBG("Content=\n" << *pContent);
   return true;
}

void CACHEfreeCache() {
   COL_FUNCTION(CACHEfreeCache);
   delete s_pCACHEvar;
}

bool CACHEexists() {
   COL_FUNCTION(CACHEexists);
   bool Result = s_pCACHEvar && s_pCACHEvar->size();
   COL_VAR(Result);
   return Result;
}

COLstring CACHEhash() {
   COL_FUNCTION(CACHEhash);
   if(!CACHEexists()) { return ""; }
   return (*s_pCACHEvar)["hash"].asString();
}

COLstring CACHEhelpHash() {
   COL_FUNCTION(CACHEhelpHash);
   if(!CACHEexists()) { return ""; }
   return (*s_pCACHEvar)["help_hash"].asString();
}

void CACHEgetDeprecated(COLstring* pDeprecated) {
   COL_FUNCTION(CACHEgetDeprecated);
   const COLstring DepFile = CACHEhelpHash() + "deprecated.txt";
   if(!CACHEexists() || s_pCACHEvar->map().count(DepFile) == 0) { return; }
   *pDeprecated = (*s_pCACHEvar)[DepFile].asString();
}

static COLvar CACHEcreateHelpVar(const COLstring& HelpPrefix, const COLstring& FileName, const COLvar& FileData) {
   COL_FUNCTION(CACHEcreateHelpVar);
   COLvar    Result, HelpVar;
   COLstring HelpData;
   GZIPuncompress(FileData, &HelpData);
   if(!HelpVar.parse(HelpData) || HelpVar["Name"].asString().is_null()) {
      COLstring FunctionName = FileName.substr(HelpPrefix.size());
      FunctionName.remove(FunctionName.size() - FILpathExtLast(FunctionName, true).size());
      FunctionName.replace("/", ".");
      Result["name"] = FunctionName;
   } else {
      Result["name"] = HelpVar["Name"].asString();
   }
   Result["location"] = FileName;
   return Result;
}

void CACHEgetHelpList(COLvar* pHelpVar) {
   COL_FUNCTION(CACHEgetHelpList);
   const COLstring HelpPrefix = CACHEhelpHash();
   for(const auto& it : s_pCACHEvar->map()) {
      if(FILpathExtLast(it->first, true) != ".help") { continue; }  // not help file
      pHelpVar->push_back(CACHEcreateHelpVar(HelpPrefix, it.first, it.second));
   }
   COL_VAR(pHelpVar->json(1));
}

bool CACHEsetupDefaultVmd(COLstring* pVmdPath) {
   COL_FUNCTION(CACHEsetupDefaultVmd);
   if(!CACHEexists()) {
      *pVmdPath = DIRvmdDir("Default.vmd");
      return true;
   }
   COL_VAR(s_pCACHEvar->map().count("/vmd/Default.vmd"));
   const COLstring& VmdData     = (*s_pCACHEvar)["/vmd/Default.vmd"].asString();
   const COLstring  VmdResource = FILpathAppend(DIRresourcesDir(), "Default.vmd");
   *pVmdPath                    = VmdResource;
   COL_VAR2(VmdData.size(), *pVmdPath);
   try {
      if(FILfileExists(VmdResource) && FILfileSize(VmdResource) > 0) {
         COL_TRC(VmdResource << " already exists");
         return true;
      }
      FILwriteFile(VmdResource, VmdData);
      return true;
   } catch(const COLerror& E) {
      COL_ERR(E.description());
      return false;
   }
}