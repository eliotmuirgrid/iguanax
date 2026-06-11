// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTImodel
//
//  Description: Implementation
//
//  Author: Eliot Muir
//  Date:   August 31st 2022
// ---------------------------------------------------------------------------
#include <NOTI/NOTImodel.h>

#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void NOTIruleTags(const COLvar& RuleVar, COLarray<COLstring>* pTags) {
	COL_FUNCTION(NOTIruleTags);
	COLarray<COLstring> Tags;
	// old vs new format loading
	if (RuleVar.exists("tags") && RuleVar["tags"].isArray()) {
		for (const auto& it : RuleVar["tags"].array()) { Tags.push_back(it); }
	} else {
		COLsplit(&Tags, RuleVar["matches"].asString(), " ");
	}
	for (const auto& it : Tags) { if (!it.isWhitespace()) { pTags->push_back(it); } }
}

COLstring NOTIpriorityToString(const NOTIpriority& Priority) {
   COL_FUNCTION(NOTIpriorityToString);
   switch(Priority) {
      case NOTIpriority::HIGH: return "high";
      case NOTIpriority::MEDIUM: return "medium";
      default: return "low";
   }
}

static NOTIpriority NOTIstringToPriority(const COLstring& Priority) {
   COL_FUNCTION(NOTIstringToPriority);
   if(Priority == "high")   { return NOTIpriority::HIGH; }
   if(Priority == "medium") { return NOTIpriority::MEDIUM; }
   return NOTIpriority::LOW;
}

static bool NOTIvalidateNum(const COLstring& NumString) {
   for (int i = 0; i < NumString.size();i ++) {
      const char c = NumString[i];
      if (c < '0' || c > '9') { return false; }
   }
   char* endPtr;
   long long num = std::strtoll(NumString.c_str(), &endPtr, 10);
   if(num > COL_INT_32_MAX) { return false; }
   return true;
}

static bool NOTIruleFromVar(const COLvar& RuleVar, NOTIrule* pRule, COLstring* pErr){
   COL_FUNCTION(NOTIruleFromVar);
   COL_VAR(RuleVar.json(1));
   if ("error_count"   == RuleVar["type"]) pRule->Type = NOTIruleType::HAS_ERRORS;
   if ("queue_count"   == RuleVar["type"]) pRule->Type = NOTIruleType::QUEUE;
   if ("inactive_time" == RuleVar["type"]) pRule->Type = NOTIruleType::INACTIVE;
   if ("log_contains"  == RuleVar["type"]) pRule->Type = NOTIruleType::LOG_CONTAINS;
   const COLstring ThresholdStr = RuleVar["threshold"].asString();
   if(pRule->Type != NOTIruleType::LOG_CONTAINS && !NOTIvalidateNum(ThresholdStr)) {
		const COLstring Err = "Threshold must be a positive integer less than 2147483647\n";
		if(pErr->find(Err) == npos) { pErr->append(Err); }
		return false;
	}
   pRule->Threshold = ThresholdStr;
   if(RuleVar.exists("limit")) {
      int Limit = atoi(RuleVar["limit"].asString().c_str());
      if(Limit < 0) {
			COLstring Err = "Limit must be a positive integer\n";
			if(pErr->find(Err) == npos) { pErr->append(Err); }
			return false;
		}
      pRule->Limit = Limit;
   }
   pRule->Priority = NOTIstringToPriority(RuleVar["priority"]);
   NOTIruleTags(RuleVar, &pRule->Tags);
	return true;
}

static void NOTImodelFromVar(NOTImodel* pOut, const COLvar& Input, COLstring* pErr) {
	COL_FUNCTION(NOTImodelFromVar);
	COLarray<NOTIrule> NewRules;
	for(int i = 0; i < Input["rules"].size(); i++) {
		const COLvar& RuleVar = Input["rules"][i];
		NOTIrule Rule;
		if(NOTIruleFromVar(RuleVar, &Rule, pErr) && Rule.Tags.size() > 0) { NewRules.push_back(Rule); }
	}
	pOut->ComponentId = Input["component_id"];
	pOut->IsOn        = Input["is_on"];
	pOut->Rules       = NewRules;
}

static void NOTImodelFromVarOldFormat(NOTImodel* pOut, const COLvar& Input){
   COL_FUNCTION(NOTImodelFromVarOldFormat);
   COL_VAR(Input);
   pOut->Rules.clear();
   for (int i = 0; i < Input.size(); i ++){
      const COLvar& RuleVar = Input[i];
      if (pOut->ComponentId.is_null()) pOut->ComponentId = RuleVar["invoked_component"];
      NOTIrule Rule;
      COLarray<COLstring> Tags;
      COLsplit(&Tags, RuleVar["components"], " ");
      Rule.Tags = Tags;
      Rule.Threshold = RuleVar["parameter"];
      if ("error_count"   == RuleVar["condition"]) Rule.Type = NOTIruleType::HAS_ERRORS;
      if ("queue_count"   == RuleVar["condition"]) Rule.Type = NOTIruleType::QUEUE;
      if ("inactive_time" == RuleVar["condition"]) Rule.Type = NOTIruleType::INACTIVE;
      pOut->Rules.push_back(Rule);
   }
}

COLstring NOTIruleToString(const NOTIruleType& Rule){
   COL_FUNCTION(NOTIruleToString);
   switch (Rule) {
      case NOTIruleType::HAS_ERRORS: return "error_count";
      case NOTIruleType::QUEUE:      return "queue_count";
      case NOTIruleType::INACTIVE:   return "inactive_time";
      case NOTIruleType::LOG_CONTAINS: return "log_contains";
      default:                       return "";
   }
}

void NOTImodelFromDiscVar(NOTImodel* pOut, const COLvar& Input){
   COL_FUNCTION(NOTImodelFromDiscVar);
	COLstring Err;
   if (Input.exists("component_id") && Input.exists("rules")){
      NOTImodelFromVar(pOut, Input, &Err);
		COL_TRC(Err);
   } else {
      NOTImodelFromVarOldFormat(pOut, Input);   // backward compatibility
   }
}

static void NOTImodelToVar(COLvar* pOut, const NOTImodel& Input) {
   COL_FUNCTION(NOTImodelToVar);
   COLvar& Out = *pOut;
   Out["component_id"] = Input.ComponentId;
   Out["rules"].setArrayType();
   for (const auto & i : Input.Rules) {
      COLvar Rule;
      Rule["tags"].setArrayType();
      for(const auto& it : i.Tags) { Rule["tags"].push_back(it); }
      Rule["type"]      = NOTIruleToString(i.Type);
      Rule["priority"]  = NOTIpriorityToString(i.Priority);
      Rule["threshold"] = i.Threshold;
      Rule["limit"]     = i.Limit;
      Out["rules"].push_back(Rule);
   }
   Out["is_on"] = Input.IsOn;
   COL_VAR(Out);
}

void NOTImodelToDiscVar(COLvar* pOut, const NOTImodel& Input){
   COL_FUNCTION(NOTImodelToDiscVar);
   NOTImodelToVar(pOut, Input);
}

bool NOTImodelFromWebVar(NOTImodel* pOut,const COLstring& Component, bool IsOn, const COLvar& Rules, COLstring* pErr) {
	COL_FUNCTION(NOTImodelFromWebVar);
   if(!Rules.isArray()) {
      *pErr = "Invalid Data";
      return false;
   }
   if(!Rules.size()) {
      *pErr = "Invalid Data";
      return false;
   }

	COLarray<NOTIrule> NewRules;
	for(const auto& it : Rules.array()) {
		NOTIrule Rule;
		if(!NOTIruleFromVar(it, &Rule, pErr)) { return false; }
		if(Rule.Tags.size() > 0) { NewRules.push_back(Rule); }
	}
	pOut->ComponentId = Component;
	pOut->IsOn        = IsOn;
	pOut->Rules       = NewRules;
	return true;
}

void NOTImodelToWebVar(COLvar* pOut, const NOTImodel& Input){
   COL_FUNCTION(NOTImodelToWebVar);
   NOTImodelToVar(pOut, Input);  // currently web var is the same as disk var
}

void NOTIcreateDefault(NOTImodel* pModel) {
   COL_FUNCTION(NOTIcreateDefault);
   COLarray<COLstring> Tags;
   Tags.push_back("#prod");
   pModel->Rules.clear();
   pModel->Rules.push_back(NOTIrule(Tags, NOTIruleType::HAS_ERRORS, "0"));
   pModel->Rules.push_back(NOTIrule(Tags, NOTIruleType::INACTIVE, "360"));
   pModel->Rules.push_back(NOTIrule(Tags, NOTIruleType::QUEUE, "10"));
   pModel->ComponentId = "NONE";
}

COLostream& operator<<(COLostream& Stream, const NOTImodel& Model) {
	Stream << Model.Rules.size() << " rules:" << newline;
	for(const auto& Rule : Model.Rules) { Stream << Rule << newline; }
	return Stream;
}

COLostream& operator<<(COLostream& Stream, const NOTIrule& Rule){
   COLstring Tags;
   for(const auto& it : Rule.Tags) { Tags += it + " "; }
   Stream << "Tags             = " << Tags;
   Stream << "\nThreshold         = " << Rule.Threshold;
   Stream << "\nRule              = " << NOTIruleToString(Rule.Type) << newline;

   return Stream;
}
