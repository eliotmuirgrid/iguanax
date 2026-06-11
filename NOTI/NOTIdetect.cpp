// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIdetect
//
//  Description:
//
//  Implementation
//
//  Author: Eliot Muir
//  Date:   Tue 13 Sep 2022 15:10:50 EDT
// ---------------------------------------------------------------------------
#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLutils.h>
#include <HASH/HASHutils.h>
#include <MATCH/MATCHsearch.h>
#include <NOTI/NOTIdetect.h>
#include <NOTI/NOTImodel.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBmessage.h>
COL_LOG_MODULE;

typedef bool (*NOTIdetectFunc)(const CFGconfig&, const COLstring&);

static COLvar NOTIruleToVar(const NOTIrule& Rule) {
   COLvar Out;
   Out["criteria"]  = NOTIruleToString(Rule.Type);
   Out["threshold"] = Rule.Threshold;
   Out["priority"]  = NOTIpriorityToString(Rule.Priority);
   return Out;
}

static bool NOTIcomponentInError(const CFGconfig& Comp, const COLstring& Threshold) {
   return Comp.Status.ErrorCount > 0;
}

static bool NOTIcomponentQueueCount(const CFGconfig& Comp, const COLstring& Threshold) {
   return SDBIgetQueueCount(Comp.CoreConfig.Guid) >= atoi(Threshold.c_str());
}

static bool NOTIcomponentInactive(const CFGconfig& Comp, const COLstring& Threshold) {
   COL_VAR(Comp.Status.StartTime);
   if(Comp.Status.StartTime == 0){
      COL_TRC("Component is not running");
      return false;
   }
   time_t Now = time(NULL);
   int ThresholdInSeconds = atoi(Threshold.c_str()) * 60;
   COL_VAR3(Comp.Status.LastActivity, Now, ThresholdInSeconds);
   if(Comp.Status.LastActivity == 0){
      return Comp.Status.StartTime + ThresholdInSeconds < Now;
   }
   return Comp.Status.LastActivity + ThresholdInSeconds < Now;
}

static bool NOTIcheckInstanceRunning(BLUAinstanceManager* pManager, const COLstring& ComponentId) {
   bool Running = BLUAinstanceRunning(*pManager, ComponentId);
   if (!Running) { COL_TRC("Notification component " << ComponentId << " is OFF"); }
   return Running;
}

static bool NOTIrunComponentIfNeeded(const COLvar& Notifications, BLUAinstanceManager* pManager, const COLstring& ComponentId) {
   if (Notifications.size() > 0) {
      COL_VAR(Notifications.json());
      if (pManager->m_Instances.count(ComponentId)) {
         BLUAcallFunction(pManager->m_Instances[ComponentId], "main", Notifications.json());
      }
      return true;
   }
   COL_TRC("No notification rules triggered");
   return false;
}

static bool NOTIcheckLimit(const COLstring& Guid, COLmap<COLstring, COLarray<time_t>>* pMessageTimes, int Limit) {
   COL_FUNCTION(NOTIcheckLimit);
   if(Limit == 0) {
      COL_TRC("No limit on messages");
      return true;
   }
   const time_t Current = time(0);
   if(!pMessageTimes->count(Guid)) {
      COL_TRC(Guid << " does not have any history of messages -- add notification");
      COLarray<time_t> NewArr;
      NewArr.push_back(Current);
      pMessageTimes->add(Guid, NewArr);
      return true;
   }
   COLarray<time_t>& ComponentMessages = (*pMessageTimes)[Guid];
   if(ComponentMessages.size() < Limit) {
      time_t lastMessageTime = ComponentMessages.back();
      const int intervalInSeconds = 3600 / Limit; // Interval to spread notifications over an hour
      // Check if the time since the last notification exceeds the interval
      if (Current - lastMessageTime >= intervalInSeconds) {
         COL_TRC("Adding notification after interval has passed");
         ComponentMessages.push_back(Current);
         return true;
      } else {
         COL_TRC("Waiting to add notification until interval has passed");
         return false;
      }
   }
   // Check if the oldest message is outside the 1-hour window
   if (Current - ComponentMessages[0] > 3600) {
      COL_TRC("Removing first entry since it is beyond 1hr -- add notification");
      ComponentMessages.remove(0);
      ComponentMessages.push_back(Current);
      return true;
   }
   COL_TRC("Limit of " << Limit << " reached for this component -- no notification");
   return false;
}

static void NOTIaddUnique(const COLvar& Item, COLvar* pNotifications) {
   COL_FUNCTION(NOTIaddUnique);
   if(pNotifications->isNull()) {
      pNotifications->push_back(Item);
      return;
   }
   for(const auto& it : pNotifications->array()) {
      if(it == Item) {
         COL_TRC("Notification already exists -- skipping");
         return;
      }
   }
   pNotifications->push_back(Item);
}

void NOTIpatternRule(SDBmessage* pMessage, NOTIrule* pRule, const CFGconfig& ComponentConfig, COLvar* pNotifications) {
   COL_FUNCTION(NOTIpatternRule);
   const COLstring GlobExpression = HASHarrToString(pRule->Tags);
   const COLstring TagString      = HASHarrToString(ComponentConfig.CoreConfig.Tags);
   if(!MATCHsearch(GlobExpression, TagString, true)) { return; }
   COL_TRC(ComponentConfig.CoreConfig.Guid << " has a tag match");
   if(MATCHsearch(pRule->Threshold, pMessage->Data, true)) {
      COL_TRC("Found matching pattern: " + pRule->Threshold);
      COLvar Item = NOTIruleToVar(*pRule);
      Item["component"] = pMessage->Id.MessageId.ComponentId;
      Item["msg_time"]  = COLuint64ToString(pMessage->Id.MessageId.DateTime);
      Item["msg_index"] = COLintToString(pMessage->Id.MessageId.Index);
      // Item["msg_data"]  = pMessage->Data;
      if(NOTIcheckLimit(pMessage->Id.MessageId.ComponentId, &pRule->MessageTimes, pRule->Limit)) {
         NOTIaddUnique(Item, pNotifications);
      }
   }
}

void NOTIlogContainsPattern(SDBmessage* pMessage, CFGmap* pMap, NOTImodel* pModel, BLUAinstanceManager* pManager){
   COL_FUNCTION(NOTIlogContainsPattern);
   NOTImodel& Model = *pModel;
   const COLstring& NotificationComponent = Model.ComponentId;
   const COLstring& ComponentId =  pMessage->Id.MessageId.ComponentId;
   if (!NOTIcheckInstanceRunning(pManager, NotificationComponent)) { return; } // if notification component is off no need to check rules.
   if (ComponentId == NotificationComponent) { return; } // prevent infinite loop where if we log the message that came in, we will run this function again
   if (!pMap->ConfigMap.count(ComponentId)) { return; }  // component no longer exists
   const CFGconfig& ComponentConfig = pMap->ConfigMap[ComponentId];
   COL_TRC("Checking if component meets rule hashes");
   COLvar Notifications;
   for (auto i = Model.Rules.begin(); i != Model.Rules.end(); i++){
      if (i->Type != NOTIruleType::LOG_CONTAINS) { continue; }
      NOTIpatternRule(pMessage, i, ComponentConfig, &Notifications);
   }
   COL_VAR(Notifications.size());
   NOTIrunComponentIfNeeded(Notifications, pManager, NotificationComponent);
}

static NOTIdetectFunc NOTIgetDetectFunction(NOTIruleType RuleType) {
   COL_FUNCTION(NOTIgetDetectFunction);
   switch (RuleType){
      case NOTIruleType::INACTIVE  : return &NOTIcomponentInactive;
      case NOTIruleType::QUEUE     : return &NOTIcomponentQueueCount;
      case NOTIruleType::HAS_ERRORS: return &NOTIcomponentInError;
      default: return NULL;
   }
}

void NOTIdetectRule(const CFGmap& Map, NOTIrule* pRule, COLvar* pNotifications) {
   COL_FUNCTION(NOTIdetectRule);
   if(NOTIruleType::LOG_CONTAINS == pRule->Type) {
      COL_TRC("Skip log contains rule ");
      return;
   }
   const COLstring GlobExpression = HASHarrToString(pRule->Tags);
   NOTIdetectFunc  pFunc          = NOTIgetDetectFunction(pRule->Type);
   for(auto i = Map.ConfigMap.cbegin(); i != Map.ConfigMap.cend(); i++) {
      const COLstring TagString = HASHarrToString(i->second.CoreConfig.Tags);
      COL_VAR2(TagString, GlobExpression);
      if(!MATCHsearch(GlobExpression, TagString, true)) { continue; }
      if((*pFunc)(i->second, pRule->Threshold)) {
         COLvar Item       = NOTIruleToVar(*pRule);
         Item["component"] = i.value().CoreConfig.Guid;
         if(NOTIcheckLimit(i->second.CoreConfig.Guid, &pRule->MessageTimes, pRule->Limit)) {
            NOTIaddUnique(Item, pNotifications);
         }
      }
   }
   COL_VAR(pNotifications->size());
}

int NOTIdetectAll(CFGmap* pMap, NOTImodel* pModel, BLUAinstanceManager* pManager) {
   COL_FUNCTION(NOTIdetectAll);
   NOTImodel& Model = *pModel;
   const COLstring& ComponentId = Model.ComponentId;
   if (!NOTIcheckInstanceRunning(pManager, ComponentId)) { return 0; } // if notification component is off no need to check rules.
   COL_TRC("Checking if component meets rule hashes");
   COLvar Notifications;
   for (auto & Rule : Model.Rules){
      NOTIdetectRule(*pMap, &Rule, &Notifications);
   }
   NOTIrunComponentIfNeeded(Notifications, pManager, ComponentId);
   return Notifications.size();
}

void NOTIdetectPoll(CFGmap* pMap, NOTImodel* pModel, BLUAinstanceManager* pManager){
   COL_FUNCTION(NOTIdetectPoll);
   NOTIdetectAll(pMap,pModel, pManager);
}
