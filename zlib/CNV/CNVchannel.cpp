// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CNVchannel
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 15 December 2024 - 05:43PM
// ---------------------------------------------------------------------------

#include <CNV/CNVchannel.h>
#include <CNV/CNVllpListener.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring CNVchannelName(const COLvar& Channel){
   //COL_FUNCTION(CNVchannelName);
   return Channel["@"]["name"];
}

COLstring CNVchannelDescription(const COLvar& Channel){
   //COL_FUNCTION(CNVchannelName);
   return Channel["@"]["description"];
}

bool CNVstartAutomatically(const COLvar& Channel){
   return Channel["@"]["start_automatically"].asString() == "true";
}

void CNVchannelProcess(const COLstring& FileName, COLostream& Out){
   COL_FUNCTION(CNVchannelProcess);
   COL_VAR(FileName);
   COLstring Content;
   FILreadFile(FileName, &Content);
   COLvar Xml;
   Xml.parseXml(Content);
   COL_VAR(Content);
   COL_VAR(Xml.json(1));
   COLvar& Channel = Xml["channel"];
   //COLcout << "Channel " << CNVchannelName(Channel) << newline;
   if (Channel.exists("from_llp_listener")){ CNVllpListener(Channel, Out); }
}

void CNVchannelIterate(COLostream& Out){
   COL_FUNCTION(CNVchannelIterate);
   COLstring ChanDir = "./IguanaMainRepo/Channels/";
   FILdirEnumerator i(ChanDir + FIL_ANYFILE, false);
   COLstring FileName;
   while (i.getFile(FileName)){
      COLstring FullFileName = ChanDir + FileName + "/ChannelConfiguration.xml";
      CNVchannelProcess(FullFileName, Out);
   }
}