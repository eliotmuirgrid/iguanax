//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------
#include <EDI/EDIgrammar.h>
#include <EDI/EDIvmdParser.h> 
#include <FIL/FILutils.h>
#include <GUID/GUIDid.h>

#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void EDIaddMatchRule(const COLstring& MessageName, const COLstring& IdentityValue, EDIcollection* pOutput) {
   COL_FUNCTION(EDIaddMatchRule);
   COL_VAR2(MessageName, IdentityValue);
   EDImatch Match;
   Match.Match = IdentityValue;
   Match.MessageName = MessageName;
   pOutput->MatchRules.push_back(Match);
}

static bool EDIneedsWildcard(const COLstring& Identity) {
   COL_FUNCTION(EDIneedsWildcard);
   if(Identity.size() < 1) return false;
   return Identity[Identity.size() - 1] != '*' && Identity != "ACK";
}

// extend this based on needs
static void EDIcheckIdentityRegex(const COLstring& Identity, COLarray<COLstring>* pIdentityList, COLstring* pErrorMsg) {
   COL_FUNCTION(EDIcheckIdentityRegex);
   COL_VAR(Identity);
   int FoundPos;
   if (Identity.find(&FoundPos, "||") || Identity.find(&FoundPos, "&&")) { 
      *pErrorMsg = "One or more matching rules contained || or &&. These are not supported and may have caused the conversion to produce incorrect matching rules. Please review and fix the matching rules as needed.";
   }
   COLsplit(pIdentityList, Identity, "|");
}

static void EDImapIdentity(const COLstring& MessageName, const COLvar& MessageIdentity, EDIcollection* pOutput, COLstring* pErrorMsg){
   COL_FUNCTION(EDImapIdentity);
   int IdentityLength = MessageIdentity.size();
   // COL_VAR2(MessageIdentity.json(1), IdentityLength);
   COLarray<COLstring> IdentityList;
   // Need to add a '*' to match Chameleon identity behaviour ex: ADT --> ADT*, ADT^A04 --> ADT^A04*
   if (IdentityLength == 1 && !MessageIdentity[0]["value"][0]["text"].asString().is_null()){ 
      COLstring IdentityValue = MessageIdentity[0]["value"][0]["text"];
      EDIcheckIdentityRegex(IdentityValue, &IdentityList, pErrorMsg);
   } else if (IdentityLength >= 2 && !MessageIdentity[0]["value"][0]["text"].asString().is_null() && !MessageIdentity[1]["value"][0]["text"].asString().is_null()){
      COLstring IdentityValue = MessageIdentity[0]["value"][0]["text"].asString() + COL_T("^") + MessageIdentity[1]["value"][0]["text"].asString();
      EDIcheckIdentityRegex(IdentityValue, &IdentityList, pErrorMsg);
   } else {
      COL_TRC("No matching rule found or format not recognized.");
   }
   for(auto it : IdentityList) {
      if(EDIneedsWildcard(it)) { it.append("*"); }
      EDIaddMatchRule(MessageName, it, pOutput);
   }
}

static COLvar EDIgetMsgDescriptions(const COLvar& Messages) {
   COL_FUNCTION(EDIgetMsgDescriptions);
   COLvar Descriptions;
   if(!Messages.isArray()) { return Descriptions; }
   for(int i = 0; i < Messages.size(); i++) { Descriptions[Messages[i]["@"]["name"].asString()] = Messages[i]["description"][0]["text"]; }
   return Descriptions;
}

bool EDIcompare(const EDIuniqueNode& UniqueNode, EDInode* pNode) {
   COL_FUNCTION(EDIcompare);
   COLarray<EDIchild>& Children = UniqueNode._pNode->Children;
   if(!UniqueNode._pNode->Description.equals(pNode->Description) || UniqueNode._pNode->Type != pNode->Type || Children.size() != pNode->Children.size()) { return false;}
   for(int i = 0; i < Children.size(); i++) {
      EDIchild const& Child = Children[i];
      if(Child.Repeats != pNode->Children[i].Repeats || Child.pNode->Name != pNode->Children[i].pNode->Name ||
         Child.Required != pNode->Children[i].Required || Child.Description != pNode->Children[i].Description) {
         return false;
      }
   }
   return true;
}

int EDIcompareNodes(EDInode* pNode, const COLarray<EDIuniqueNode>& UniqueNodes) {
   COL_FUNCTION(EDIcompareNodes);
   for(int i = 0; i < UniqueNodes.size(); i++) { if(EDIcompare(UniqueNodes[i], pNode)) { return i; } }
   return -1;
}

COLstring EDIresolveDuplicateNameAddIndex(EDIcollection* pOutput, const COLstring& OriginalName){
   COL_FUNCTION(EDIresolveDuplicateNameAddIndex);
   int UniqueIndex = 1;
   COLstring NewName;
   do {
      NewName = OriginalName + "_" + COLintToString(UniqueIndex);
      UniqueIndex++;
   } while (pOutput->Nodes.count(NewName));
   return NewName;
}

COLstring EDIresolveDuplicateNames(const COLstring& Parent, const COLstring& Name, EDInode* pNode,
                                   EDIcollection* pOutput, COLmap<COLstring, COLarray<EDIuniqueNode>>* pUniques, COLarray<COLstring>* pParents) {
   COL_FUNCTION(EDIresolveDuplicateNames);
   COLarray<EDIuniqueNode>& UniqueNodes = (*pUniques)[Name];
   COL_VAR(UniqueNodes.size());
   if(pOutput->Nodes.count(Name) == 0) {
      COL_TRC("First instance of: " << Name << " -- adding to unique node map");
      COL_TRC("Has " << pNode->Children.size() << " children");
      UniqueNodes.push_back(EDIuniqueNode(Name, pNode));
      return Name;
   }
   COL_TRC("Found duplicate name: " << Name << " -- comparing with similarly named nodes");
   int DupeIndex = EDIcompareNodes(pNode, UniqueNodes);
   if(DupeIndex != -1) {
      COL_TRC("Found exact match: " << UniqueNodes[DupeIndex]._Name);
      return UniqueNodes[DupeIndex]._Name;
   }
   COLstring NewName = Name;
   int Pos = pParents->size() -1;
   do {
      if (Pos < 0) {
         COLstring NewNameWithIndex = EDIresolveDuplicateNameAddIndex(pOutput, NewName);
         COL_TRC("Node " << NewName << " already exists. Out of options, now trying " << NewNameWithIndex);
         NewName = NewNameWithIndex;
         break;
      }
      COL_TRC("Node " << NewName << " already exists. Now trying " << NewName + "_" + (*pParents)[Pos]);
      NewName.append("_" + (*pParents)[Pos]);
      Pos--;
   } while (pOutput->Nodes.count(NewName) > 0);
   UniqueNodes.push_back(EDIuniqueNode(NewName, pNode));
   COL_TRC("Found no match, setting name to: " << NewName);
   return NewName;
}

static void EDIresolveMsgSegment(const COLstring& Name, EDInode* pParentNode, bool Required, bool Repeats, EDIcollection* pOutput) {
   COL_FUNCTION(EDIresolveMsgSegment);
   COL_VAR3(Name, Repeats, Repeats);
   if(pOutput->Nodes.count(Name) == 0) {
      COL_TRC("Segment -- " << Name << " -- not found in collection -- creating it");
      auto pSegmentNode = new EDInode(Name, "", EDI_SEGMENT);
      pOutput->Nodes.add(Name, pSegmentNode);
      EDIchild Child(pSegmentNode, "", Required, Repeats);
      pParentNode->Children.push_back(Child);
   } else {
      EDIchild Child(pOutput->Nodes[Name], pOutput->Nodes[Name]->Description, Required, Repeats);
      pParentNode->Children.push_back(Child);
   }
}

static void EDIresolveMessageChildren(const COLstring& ParentName, const COLvar& Children, EDInode* pParentNode,
                                      EDIcollection* pOutput, COLmap<COLstring, COLarray<EDIuniqueNode>>* pUniques, COLarray<COLstring>* pParents) {
   COL_FUNCTION(EDIresolveMessageChildren);
   if(!Children.isArray()) { return; }
   for(int i = 0; i < Children.size(); i++) {
      COLstring Name = Children[i]["name"][0]["text"];
      if(Name.is_null()) {
         COL_TRC("Message child node -- " << Name << " not found in collection");
         continue;
      }
      bool Required = !Children[i]["is_optional"][0]["text"].asBool();
      bool Repeats  = Children[i]["is_repeating"][0]["text"].str() == "True";
      EDItype Type  = Children[i]["type"][0]["text"].str() == "group" ? EDI_SEGMENT_GROUP : EDI_SEGMENT;

      if(Type == EDI_SEGMENT_GROUP) {
         COL_TRC("Found nested segment group -- " << Name);
         auto pNestedGroup = new EDInode(Name, "", Type);
         EDIchild Child(pNestedGroup, "", Required, Repeats);
         Child.pNode->Name = EDIresolveDuplicateNames(ParentName, Name, pNestedGroup, pOutput, pUniques, pParents);
         pParents->push_back(Name);
         pOutput->Nodes.add(Child.pNode->Name, pNestedGroup);
         pParentNode->Children.push_back(Child);
         EDIresolveMessageChildren(Child.pNode->Name, Children[i]["message_grammar"], pNestedGroup, pOutput, pUniques, pParents);
         pParents->pop_back();
      }
      else { EDIresolveMsgSegment(Name, pParentNode, Required, Repeats, pOutput); }
   }
}

// Returns a new unique name if Name already exists in pOutput, or the same name is returned
static COLstring EDIresolveMessageNameCollision(const COLstring& Name, const EDIcollection* pOutput, COLmap<COLstring, COLstring>* pMessageNameChanges) {
   COL_FUNCTION(EDIresolveMessageNameCollision);
   if (0 == pOutput->Nodes.count(Name)) { return Name; }
   int Count = 1;
   COLstring NewName;
   do {
      NewName = Name + "_msg_" + COLintToString(Count++);
   } while (pOutput->Nodes.count(NewName) > 0);
   COL_WRN("Message name collision detected. Rename message from " << Name << " to " << NewName);
   (*pMessageNameChanges)[Name] = NewName;
   return NewName;
}

static void EDIloadMessageNodes(const COLvar& Messages, const COLvar& Descriptions, EDIcollection* pOutput, COLmap<COLstring, COLstring>* pMessageNameChanges) {
   COL_FUNCTION(EDIloadMessageNodes);
   if(!Messages.isArray()) { return; }
   COLvar DescriptionMap = EDIgetMsgDescriptions(Descriptions);
   COLmap<COLstring, COLarray<EDIuniqueNode>> Duplicates;
   for(int i = 0; i < Messages.size(); i++) {
      COLstring Name = Messages[i]["@"]["name"];
      if(!Name.is_null()) {
         const COLstring& Description = Descriptions[Name];
         Name = EDIresolveMessageNameCollision(Name, pOutput, pMessageNameChanges);
         auto pNode = new EDInode(Name, Description, EDI_MESSAGE);
         const COLvar& Children = Messages[i]["message_grammar"][0]["message_grammar"];
         COLarray<COLstring> Parents;
         Parents.push_back(Name);
         EDIresolveMessageChildren(Name, Children, pNode, pOutput, &Duplicates, &Parents); // msg3
         pOutput->Nodes.add(Name, pNode);
      }
   }
}

static void EDIvmdMessageIdentity(const COLvar& Messages, const COLmap<COLstring, COLstring>& MessageNameChanges, const COLarray<COLstring>& MatchOrder, EDIcollection* pOutput, COLstring* pErrorMessage) {
   COL_FUNCTION(EDIvmdMessageIdentity);
   if(!Messages.isArray()) { COL_ERR("Weird messages was not an array."); return; }
   if (MatchOrder.size() > 0){
      for(int MatchIndex = 0; MatchIndex < MatchOrder.size(); MatchIndex++){
         for(int i = 0; i < Messages.size(); i++) {
            COLstring Name = Messages[i]["@"]["name"];
            COL_TRC("Pre " << Name);
            if (Name.is_null()) { continue; }
            if (MessageNameChanges.count(Name)) {
               // Message was renamed in EDIloadMessageNodes()
               Name = MessageNameChanges[Name];
            }
            COL_TRC("Post " << Name);
            if (Name == MatchOrder[MatchIndex]){
               if(Messages[i].exists("identifier")) { EDImapIdentity(Name, Messages[i]["identifier"], pOutput, pErrorMessage); }
               else                                 { EDIaddMatchRule(Name, "*", pOutput); }
            }
         }
      }
   } else { // Maybe needed for x12
      COL_TRC("Match order was zero size.");
      for(int i = 0; i < Messages.size(); i++) {
         COLstring Name = Messages[i]["@"]["name"];
         if(!Name.is_null()) {
            if(Messages[i].exists("identifier")) { EDImapIdentity(Name, Messages[i]["identifier"], pOutput, pErrorMessage); }
            else                                 { EDIaddMatchRule(Name, "*", pOutput); }
         }
      }
   }
}

static void EDIparseMatchOrder(const COLvar& Match, const COLmap<COLstring, COLstring>& MessageNameChanges, COLarray<COLstring>& Order){
   COL_FUNCTION(EDIparseMatchOrder);
   if(!Match.isArray()) { return; }
   for (int i = 0; i < Match.size(); i++){
      if (Match[i].exists("@") && Match[i]["@"].isMap() && Match[i]["@"].exists("msg")){
         COL_TRC("Adding " << Match[i]["@"]["msg"].asString());
         COLstring Name = Match[i]["@"]["msg"];
         if (MessageNameChanges.count(Name)) {
            // Message was renamed in EDIloadMessageNodes()
            Name = MessageNameChanges[Name];
         }
         Order.push_back(Name);
      }
   }
}

static COLstring EDImapPrimitiveType(const COLstring& PrimitiveType){
   COL_FUNCTION(EDImapPrimitiveType);
   COL_TRC("Trying to map " + PrimitiveType);
   if      ("String"   == PrimitiveType)    { return "ST"; }
   else if ("Integer"  == PrimitiveType)    { return "NM"; }
   else if ("Double"   == PrimitiveType)    { return "NM"; }
   else if ("Date"     == PrimitiveType)    { return "DT"; }
   else if ("Time"     == PrimitiveType)    { return "TM"; }
   else if ("DateTime" == PrimitiveType)    { return "DTM"; }
   return "";
}

static bool EDIisPrimitiveType(const COLstring& Type){
   COL_FUNCTION(EDIisPrimitiveType);
   if (Type == "String")   return true;
   if (Type == "Integer")  return true;
   if (Type == "Double")   return true;
   if (Type == "Date")     return true;
   if (Type == "Time")     return true;
   if (Type == "DateTime") return true;
   return false;
}

static bool EDIisCompositePrimitive(const COLstring& Name, const COLvar& ChildVar, int ChildVarSize){
   COL_FUNCTION(EDIisCompositePrimitive);
   if (ChildVarSize != 1)                                     { return false; }
   if (!EDIisPrimitiveType(ChildVar["data_type"][0]["text"])) { return false; }
   COL_TRC(Name << " is a primitive composite");
   return true;
}

static void EDIvmdAddChild(const COLstring& ParentName, const COLvar& ChildVar, int ChildSize, EDIcollection* pOutput) {
   COL_FUNCTION(EDIvmdAddChild);
   COLstring ChildName = ChildVar["composite_ref"][0]["text"];
   if(ChildName.is_null() || pOutput->Nodes.count(ChildName) == 0) {
      if (EDIisCompositePrimitive(ParentName, ChildVar, ChildSize)){
         COL_TRC("Child node -- " << ChildName << " not found in collection -- skipping");
         return;
      } else {
         ChildName = EDImapPrimitiveType(ChildVar["data_type"][0]["text"]);
         if (ChildName.is_null()) { 
            COL_TRC("Child node of " << ParentName << " not found in collection -- skipping");
            return;
         }
         // If valid primitive and not in grammar, create a leaf for it
         auto pNode = new EDInode(ChildName, ChildVar["name"][0]["text"], EDI_COMPOSITE);
         pOutput->Nodes.add(ChildName, pNode);
      }
   }
   COLstring Desc     = ChildVar["name"][0]["text"];
   bool      Required = ChildVar["is_required"][0]["text"].asBool();
   int       Max      = ChildVar["max_repeats"][0]["text"].asInt();
   bool      Repeats  = (Max == -1 || Max > 0);
   EDIchild  Child(pOutput->Nodes[ChildName], Desc, Required, Repeats);
   pOutput->Nodes[ParentName]->Children.push_back(Child);
   COL_VAR4(ParentName, ChildName, Desc, Required);
}

// Segments cannot set fields to primitive types.
static void EDIvmdAddChildSegment(const COLstring& ParentName, const COLvar& ChildVar, EDIcollection* pOutput) {
   COL_FUNCTION(EDIvmdAddChildSegment);
   COLstring ChildName = ChildVar["composite_ref"][0]["text"];
   if(ChildName.is_null() || pOutput->Nodes.count(ChildName) == 0) {
      COL_TRC("Child node -- " << ChildName << " not found in collection -- skipping");
      return;
   }
   COLstring Desc     = ChildVar["name"][0]["text"];
   bool      Required = ChildVar["is_required"][0]["text"].asBool();
   int       Max      = ChildVar["max_repeats"][0]["text"].asInt();
   bool      Repeats  = (Max == -1 || Max > 0);
   EDIchild  Child(pOutput->Nodes[ChildName], Desc, Required, Repeats);
   pOutput->Nodes[ParentName]->Children.push_back(Child);
   COL_VAR4(ParentName, ChildName, Desc, Required);
}

static void EDIresolveCompositeChildren(const COLvar& Composites, EDIcollection* pOutput) {
   COL_FUNCTION(EDIresolveCompositeChildren);
   if(!Composites.isArray()) { return; }
   for(int i = 0; i < Composites.size(); i++) {
      COLstring Name     = Composites[i]["@"]["name"];
      COLvar    Children = Composites[i]["field"];
      if(Name.is_null() || !Children.isArray()) { continue; }
      for(int j = 0; j < Children.size(); j++) { EDIvmdAddChild(Name, Children[j], Children.size(), pOutput); }
   }
}

static void EDIresolveSegmentChildren(const COLvar& Segments, EDIcollection* pOutput) {
   COL_FUNCTION(EDIresolveSegmentChildren);
   if(!Segments.isArray()) { return; }
   for(int i = 0; i < Segments.size(); i++) {
      COLstring Name     = Segments[i]["name"][0]["text"];
      COLvar    Children = Segments[i]["field"];
      if(Name.is_null() || !Children.isArray()) { continue; }
      for(int j = 0; j < Children.size(); j++) { EDIvmdAddChildSegment(Name, Children[j], pOutput); }
   }
}

static void EDIloadCompositeNodes(const COLvar& Composites, EDIcollection* pOutput) {
   COL_FUNCTION(EDIloadCompositeNodes);
   if(!Composites.isArray()) { return; }
   for(int i = 0; i < Composites.size(); i++) {
      COLstring Name = Composites[i]["@"]["name"];
      COLstring Desc = Composites[i]["description"][0]["text"];
      if(Name.is_null()) { continue; }
      COL_TRC("Creating composite -- " << Name);
      auto pNode = new EDInode(Name, Desc, EDI_COMPOSITE);
      pOutput->Nodes.add(Name, pNode);
   }
}

static void EDIloadSegmentNodes(const COLvar& Segments, EDIcollection* pOutput) {
   COL_FUNCTION(EDIloadSegmentNodes);
   if(!Segments.isArray()) { return; }
   for(int i = 0; i < Segments.size(); i++) {
      COLstring Name     = Segments[i]["name"][0]["text"];
      COLstring Desc     = Segments[i]["description"][0]["text"];
      if(Name.is_null()) { continue; }
      COL_TRC("Creating segment -- " << Name);
      auto pNode = new EDInode(Name,Desc, EDI_SEGMENT);
      pOutput->Nodes.add(Name, pNode);
   }
}

// https://interfaceware.atlassian.net/wiki/spaces/DEVELOPMEN/pages/2355527718/Conversion+code+from+XML+based+vmds
bool EDItranslateVmd(const COLstring& VmdContent, EDIcollection* pOutput, COLstring* pErrorMsg) {
   COL_FUNCTION(EDItranslateVmd);
   COLvar ParsedXml;
   if(!ParsedXml.parseXml(VmdContent)) { return false; }
   int CurrentConfig = ParsedXml["engine"]["current_config"][0]["text"];
   COL_TRC("CURRENT CONFIG: " << CurrentConfig);
   const COLvar& Config = ParsedXml["engine"]["config"][CurrentConfig];
   if(!Config.isMap()) { return false; }

   EDIloadSegmentNodes(Config["segment"], pOutput);
   EDIloadCompositeNodes(Config["composite"], pOutput);

   EDIresolveSegmentChildren(Config["segment"], pOutput);
   EDIresolveCompositeChildren(Config["composite"], pOutput);

   COLmap<COLstring, COLstring> MessageNameChanges;
   EDIloadMessageNodes(Config["message"], ParsedXml["engine"]["global"][0]["message"], pOutput, &MessageNameChanges);

   COLarray<COLstring> MatchOrder; // The order in which the <match> tags appear is the order in which messages should be matched
   if(Config.exists("match")) { EDIparseMatchOrder(Config["match"], MessageNameChanges, MatchOrder); }
   EDIvmdMessageIdentity(Config["message"], MessageNameChanges, MatchOrder, pOutput, pErrorMsg);
   return true;
}
