//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDserialize.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <GRID/GRIDserialize.h>
COL_LOG_MODULE;

COLstring GRIDreadAccessToString(const GRIDreadAccessLevel Level) {
   switch(Level) {
      case GRIDreadAccessLevel::None: return "none";
      case GRIDreadAccessLevel::Full: return "full";
      default: return "none";
   }
}

GRIDreadAccessLevel GRIDreadAccessFromString(const COLstring& Str) {
   if(COLlowerCase(Str) == "full") return GRIDreadAccessLevel::Full;
   return GRIDreadAccessLevel::None;
}

GRIDreadAccessLevel GRIDreadAccessFromBool(const bool Value) {
   return Value ? GRIDreadAccessLevel::Full : GRIDreadAccessLevel::None;
}

static bool GRIDloadIguanaFromFile(const COLstring& IguanaId, GRIDconfig* pConfig) {
   COL_FUNCTION(GRIDloadIguanaFromFile);
   const COLstring JsonPath = DIRiguanaJson(IguanaId);
   COL_VAR2(IguanaId, JsonPath);
   if(!FILfileExists(JsonPath)) {
      COL_TRC("Iguana config not found: " << JsonPath);
      return false;
   }
   COLvar Data;
   if(!FILserializeLoadConfig(&Data, JsonPath)) {
      COL_TRC("Failed to load iguana config: " << JsonPath);
      return false;
   }
   if(Data.isNull() || !Data.isMap()) {
      COL_TRC("Invalid iguana config format: " << JsonPath);
      return false;
   }
   const COLstring LoadedId = Data["id"].asString();
   if(LoadedId != IguanaId) {
      COL_TRC("Iguana ID mismatch in file. Expected: " << IguanaId << ", Found: " << LoadedId);
      return false;
   }

   GRIDpeerInfo PeerInfo;
   PeerInfo.Host                = Data["host"].asString();
   PeerInfo.Port                = Data["port"].asInt();
   PeerInfo.Fingerprint         = Data["fingerprint"].asString();
   PeerInfo.ReadAccess          = GRIDreadAccessFromString(Data["access_level"].asString());
   pConfig->m_PeerMap[IguanaId] = PeerInfo;
   COL_TRC("Loaded peer: " << IguanaId);
   return true;
}

bool GRIDloadConfig(GRIDconfig* pConfig) {
   COL_FUNCTION(GRIDloadConfig);
   const COLstring IguanasDir = DIRconfigFile("iguanas/");
   COL_VAR(IguanasDir);
   if(!FILfileExists(IguanasDir)) {
      COL_TRC(IguanasDir << " does not exist.");
      return true;
   }
   FILdirEnumerator Enum(IguanasDir + "*", true);
   COLstring        DirName;
   while(Enum.getFile(DirName)) {
      if(!Enum.isDirectory()) { continue; }
      COL_VAR(DirName);
      if(DirName != pConfig->m_NodeInfo.m_IguanaId) {
         if(!GRIDloadIguanaFromFile(DirName, pConfig)) { COL_TRC("Error loading config for: " << DirName); }
      }
   }
   COL_TRC("GRID enabled=" << pConfig->m_NodeInfo.m_Enabled << " peers=" << pConfig->m_PeerMap.size());
   return true;
}

bool GRIDupdateConfigRemove(const COLstring& PeerId, COLstring& Error) {
   COL_FUNCTION(GRIDupdateConfigRemove);
   try {
      const COLstring IguanaDir = DIRiguanaDir(PeerId);
      if(FILfileExists(IguanaDir)) {
         FILremoveFullDirNew(IguanaDir);
         COL_TRC("Deleted iguana directory: " << IguanaDir);
      }
      return true;
   } catch(const COLerror& E) {
      Error = "Failed to delete iguana config: " + E.description();
      return false;
   }
}

bool GRIDupdateConfigAdd(const COLstring& PeerId, const GRIDpeerInfo& Info, COLstring& Error) {
   COL_FUNCTION(GRIDupdateConfigAdd);
   if(Info.Fingerprint.is_null()) {
      Error = "Fingerprint must be set.";
      return false;
   }
   COLvar IguanaData;
   IguanaData["schema_version"] = 1;
   IguanaData["id"]             = PeerId;
   IguanaData["host"]           = Info.Host;
   IguanaData["port"]           = Info.Port;
   IguanaData["fingerprint"]    = Info.Fingerprint;
   IguanaData["access_level"]   = GRIDreadAccessToString(Info.ReadAccess);
   COL_VAR(IguanaData.json(1));
   try {
      const COLstring JsonPath = DIRiguanaJson(PeerId);
      FILcreateDirectoryTreeToFile(JsonPath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      FILserializeSaveConfig(IguanaData, JsonPath);
      COL_TRC("Saved iguana config: " << JsonPath);
      return true;
   } catch(const COLerror& E) {
      Error = "Failed to save iguana config: " + E.description();
      return false;
   }
}