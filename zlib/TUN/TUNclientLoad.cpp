// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNclientLoad
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 26 June 2023 - 02:11PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SFI/SFImd5.h>
#include <TUN/TUNclientLoad.h>
#include <TUN/TUNhash.h>
#include <TUN/TUNkeyConvert.h>
#include <TUN/TUNkeyLoad.h>
#include <TUN/TUNstateClient.h>
COL_LOG_MODULE;

void TUNclientLoad(const COLstring& FileName, TUNstateClient* pState){
   COL_FUNCTION(TUNclientLoad);
   COLstring Dir = FILpathDir(FileName, true);
   COLstring Content;
   FILreadFile(FileName, &Content);
   COL_VAR(Content);
   COLvar C;
   if (!C.parse(Content)){
      COL_VAR(C);
      throw COLerror("Unable to parse " + FileName, 0);
   } 
   TUNkeyLoad(FILpathResolve(C["private_key"], Dir), &pState->m_MyKey);
   pState->m_Name = C["name"];
   if(!C["clients"].isMap()) { return; }
   auto j = C["clients"].map();
   for (auto i=j.begin(); i != j.end(); i++){
      TUNpeer& Peer = pState->m_Peers[i->first];
      Peer.m_Label = i->first;
      TUNkeyPublicLoad(FILpathResolve(i->second, Dir), &Peer.m_PublicKey);
      pState->m_KeyToPeerMap[TUNhashKey(Peer.m_PublicKey)] = i->first;
   }
}

bool TUNclientPeerLoad(TUNstateClient* pState, COLstring* pError) {
   COL_FUNCTION(TUNclientPeerLoad);
   pState->m_Name              = "iguana";
   const COLstring  ClusterDir = DIRclusterDir("/");
   FILdirEnumerator Enum(ClusterDir + FIL_ANYFILE);
   COLstring        File;
   while(Enum.getFile(File)) {
      if(Enum.isDirectory() || FILpathExtLast(File, true) != ".pem") { continue; }
      const COLstring FullPath = FILpathAppend(ClusterDir, File);
      const COLstring Name     = FILpathNameWithoutLastExt(File);
      TUNpeer&        Peer     = pState->m_Peers[Name];
      Peer.m_Label             = Name;
      COL_VAR2(Name, FullPath);
      try {
         COLstring KeyData;
         FILreadFile(FullPath, &KeyData);
         TUNkeyPublicFromPemString(KeyData, &Peer.m_PublicKey);
         Peer.m_Md5Key = SFImd5(KeyData);
         pState->m_KeyToPeerMap[TUNhashKey(Peer.m_PublicKey)] = Name;
      } catch(const COLerror& E) {
         *pError = E.description();
         COL_TRC(*pError);
      }
   }
   return true;
}