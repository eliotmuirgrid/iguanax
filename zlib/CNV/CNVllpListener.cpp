// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CNVllpListener
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 15 December 2024 - 06:10PM
// ---------------------------------------------------------------------------

#include <CNV/CNVllpListener.h>
#include <CNV/CNVchannel.h>

#include <CFG/CFGconfig.h>

#include <COL/COLvar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void CNVllpListener(const COLvar& ChannelData, COLostream& Out){
   COL_FUNCTION(CNVllpListener);
   Out << CNVchannelName(ChannelData) << "_LLPIN" << newline;
   COL_VAR(ChannelData.json(1));
 
   CFGconfig Config;
   Config.CoreConfig.Name = CNVchannelName(ChannelData) + " In";
   Config.CoreConfig.Description = CNVchannelDescription(ChannelData);
   Config.CoreConfig.Fields["Port"] = ChannelData["from_llp_listener"][0]["@"]["port"];
   Config.CoreConfig.Tags.push_back("#hl7");
   Config.CoreConfig.Tags.push_back("#server");
   Config.CoreConfig.Tags.push_back("#imported");
   if (CNVstartAutomatically(ChannelData)){
      Config.CoreConfig.Tags.push_back("#autostart");
   }

   // TODO how do I get the run conmmit
   //      how will I clone the repo 

   COL_VAR(Config);
}