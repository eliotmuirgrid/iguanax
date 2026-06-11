// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: EDIdata
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 05 October 2022 - 07:58PM
// ---------------------------------------------------------------------------

#include <EDI/EDIdata.h>
#include <EDI/EDIformat.h>
#include <EDI/EDIformatHtml.h>
#include <EDI/EDIgrammar.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

// It might work better to have repeating fields, segment groups and segments - this would flatten these types
const char* EDIdataTypeAsString(EDIdataType Type){
   switch(Type){
      case EDIN_MESSAGE            : return "Message";
      case EDIN_SEGMENT_GROUP_ARRAY: return "SegmentGroupArray";
      case EDIN_SEGMENT_GROUP      : return "SegmentGroup";
      case EDIN_SEGMENT_ARRAY      : return "SegmentArray";
      case EDIN_SEGMENT            : return "Segment";
      case EDIN_FIELD_ARRAY        : return "FieldArray";
      case EDIN_FIELD              : return "Field";
      case EDIN_SUBFIELD           : return "Subfield";
      case EDIN_SUBSUBFIELD        : return "SubSubField";
      default                      : return "";
   }
};

bool EDIisRepeat(EDIdataType Type){
   switch(Type){
      case EDIN_SEGMENT_GROUP_ARRAY: 
      case EDIN_SEGMENT_ARRAY      : 
      case EDIN_FIELD_ARRAY        : 
         return true;
      default: return false;
   }
}

bool EDIdata::isRepeat() const{
   COL_METHOD(EDIdata::isRepeat);
   return EDIisRepeat(Type);
}

bool EDIdata::isNull() const{
   COL_METHOD(EDIdata::isNull);
   // A null node is a leaf node with no Data, or one with children if the children are also null
   for (int i = 0; i < Children.size(); i++){ // if leaf, then Children.size() == 0 so we skip this loop
      if (!Children[i]->isNull()) { return false; }
   }
   return Data.is_null();
}

bool EDIdata::isLeaf() const{ // true if children.size = 0, false if it has repeats.
   COL_METHOD(EDIdata::isLeaf);
   if (EDIisRepeat(Type)){
      COL_TRC("It is repeating so this is not a leaf.");
      return false;
   }
   if (ChildIndex == -1){
      COL_TRC("ChildIndex == -1 so this is not a leaf");
      return false;
   }
   if (pParentType->Children[ChildIndex].pNode->Children.size() == 0){
      COL_VAR(*pParentType->Children[ChildIndex].pNode);
      COL_TRC("Child grammar has zero children so we have a leaf.");
      return true;
   } else {
      COL_TRC("Child grammar has children so it is not a leaf");
      return false;
   }
}

bool EDIdata::isRequired() const{
   COL_METHOD(EDIdata::isRequired);
   if (ChildIndex == -1){
      return false;
   }
   return pParentType->Children[ChildIndex].Required;
}

int EDIdata::size() const{
   COL_METHOD(EDIdata::size);
   return Children.size();
}

int EDIdata::grammarSize() const{
   switch(Type){
   case EDIN_MESSAGE: return pParentType->Children.size();
   case EDIN_SEGMENT_GROUP_ARRAY : 
   case EDIN_SEGMENT_ARRAY       : 
   case EDIN_FIELD_ARRAY         : 
      return  COL_MAX(1, Children.size());
   default:
      return pParentType->Children[ChildIndex].pNode->Children.size();
   }
}

const COLstring& EDIdata::value() const{
   COL_METHOD(EDIdata::value);
   return Data;
}

const COLstring& EDIdata::name() const{
   COL_METHOD(EDIdata::name);
   COL_VAR2(EDIdataTypeAsString(Type), ChildIndex);
   if (-1 == ChildIndex){
      return Data;
   }
   switch(Type){
      case EDIN_MESSAGE:             return Data;
      case EDIN_SEGMENT_GROUP_ARRAY: return pParentType->Children[ChildIndex].pNode->Name;
      case EDIN_SEGMENT_GROUP      : return pParentType->Children[ChildIndex].pNode->Name;
      case EDIN_SEGMENT_ARRAY      : return pParentType->Children[ChildIndex].pNode->Name;
      case EDIN_SEGMENT            : return pParentType->Children[ChildIndex].pNode->Name;
      default:                       return pParentType->Children[ChildIndex].Description;
   }
}

EDIdataType EDImapChildType(EDIdataType ParentType, const EDIchild& Child) {
   COL_FUNCTION(EDImapChildType);
   EDItype GrammarType = Child.pNode->Type;
   bool IsRepeating = Child.isRepeating();
   COL_VAR3(Child.pNode->Name, Child.Description, Child.isRepeating());
   COL_VAR3(IsRepeating, EDIdataTypeAsString(ParentType), EDItypeAsString(GrammarType));

   if(IsRepeating) {
      switch(ParentType) {
      case EDIN_MESSAGE:             return (GrammarType == EDI_SEGMENT_GROUP ? EDIN_SEGMENT_GROUP_ARRAY : EDIN_SEGMENT_ARRAY);
      case EDIN_SEGMENT_GROUP_ARRAY: return EDIN_SEGMENT_GROUP;
      case EDIN_SEGMENT_GROUP:       return (GrammarType == EDI_SEGMENT_GROUP ? EDIN_SEGMENT_GROUP_ARRAY : EDIN_SEGMENT_ARRAY);  
      case EDIN_SEGMENT_ARRAY:       return EDIN_SEGMENT;
      case EDIN_SEGMENT:             return EDIN_FIELD_ARRAY;
      case EDIN_FIELD_ARRAY:         return EDIN_FIELD;
      case EDIN_FIELD:               return EDIN_SUBFIELD;  
      case EDIN_SUBFIELD:            return EDIN_SUBSUBFIELD;
      case EDIN_SUBSUBFIELD:
         throw COLerror("Sub sub fields cannot have children", 0);
      default: return EDIN_MESSAGE;  // should not reach it.
      }
   } else {
      switch(ParentType) {
      case EDIN_MESSAGE:             return (GrammarType == EDI_SEGMENT_GROUP ? EDIN_SEGMENT_GROUP : EDIN_SEGMENT);
      case EDIN_SEGMENT_GROUP_ARRAY: return EDIN_SEGMENT_GROUP;
      case EDIN_SEGMENT_GROUP:       return (GrammarType == EDI_SEGMENT_GROUP ? EDIN_SEGMENT_GROUP : EDIN_SEGMENT);
      case EDIN_SEGMENT_ARRAY:       return EDIN_SEGMENT;
      case EDIN_SEGMENT:             return EDIN_FIELD;
      case EDIN_FIELD_ARRAY:         return EDIN_FIELD;
      case EDIN_FIELD:               return EDIN_SUBFIELD;
      case EDIN_SUBFIELD:            return EDIN_SUBSUBFIELD;
      case EDIN_SUBSUBFIELD:
         throw COLerror("Sub sub fields cannot have children", 0);
      default:
         return EDIN_MESSAGE;
      }
   }
}

EDIdataType EDIarrayChild(EDIdataType Type){
   COL_FUNCTION(EDIarrayChild);
   switch(Type){
      case EDIN_SEGMENT_GROUP_ARRAY: return EDIN_SEGMENT_GROUP;
      case EDIN_SEGMENT_ARRAY:       return EDIN_SEGMENT;
      case EDIN_SEGMENT:             return EDIN_FIELD_ARRAY;
      case EDIN_FIELD_ARRAY:         return EDIN_FIELD;
      default:
         throw COLerror("This is not an array type.", 0);
   }
}

void EDIdata::addRepeat(int i){
   COL_METHOD(EDIdata::addRepeat);
   COL_VAR(i);
   switch (Type){
      case EDIN_SUBFIELD: 
      case EDIN_SUBSUBFIELD:
         throw COLerror(COLstring("This is a leaf node.") + EDIdataTypeAsString(Type), 0);
      default:
         break;
   }
   for (int j = Children.size(); j <=i; j++){
       Children.push_back(new EDIdata(Collection, pParentType, ChildIndex, EDIarrayChild(Type)));
   }
}

void EDIdata::setValue(const COLstring& NewData, COLarray<COLstring>* pHere){
   COL_METHOD(EDIdata::setValue);
   COLstring Location;
   if (pHere && pHere->size() > 0){
      COLarray<COLstring>& Here = *pHere;
      Location.append(Here[0]);
      for (int i = 1; i < Here.size(); i++) { Location.append("[" + Here[i] + "]"); }
   }
   if (isRepeat()){
      throw COLerror("Cannot assign a string value to a repeating node. " + Location,0);
   }
   if (Type == EDIN_SUBSUBFIELD && pParentType->Children[ChildIndex].pNode->Children.size() > 0){ // IX-4167 This allows us to "flatten" structures which would otherwise index too deep
      Data = NewData;
      return; 
   }

   if (pParentType->Children[ChildIndex].pNode->Children.size() > 0){
      throw COLerror("Cannot assign a string value to a node with children. " + Location,0);
   }
   Data = NewData;
}

// non repeating 
void EDIdata::addChild(int i){
   COL_METHOD(EDIdata::addChild);
   COL_VAR2(ChildIndex, i);  
   COL_VAR(pParentType->Children.size());
   EDInode* pNewParentType = pParentType->Children[ChildIndex].pNode;
   if (i >= pNewParentType->Children.size()){
      COL_VAR2(i, pParentType->Children.size());
      COL_TRC("Boundary error.");
      throw COLerror("Boundary error", 0);
   }
   for (int j = Children.size(); j <=i; j++){
      Children.push_back(new EDIdata(Collection, pNewParentType, j, EDImapChildType(Type, pNewParentType->Children[j])));
   }
}

void EDIdata::addRootChild(int i){
   COL_METHOD(EDIdata::addRootChild);
   if (i >= pParentType->Children.size()){
      throw COLerror("Boundary error", 0);
   }
   for (int j = Children.size(); j <=i; j++){
       Children.push_back(new EDIdata(Collection, pParentType, j, EDImapChildType(Type, pParentType->Children[j])));
   }    
}

// Add field
// Loop through missing fields
// If a field is repeating field then we add a field array or
// we add a field


EDIdata& EDIdata::operator[](int i){
   COL_METHOD(EDIdata::operator[int]);
   COL_VAR(i);
   if (i < Children.size()){
      return *Children[i];
   }
   if (ChildIndex == -1){
      addRootChild(i);
   } else if (isRepeat()){
      addRepeat(i);
   } else {
      addChild(i);
   }
   return *Children[i];
}

const EDIdata& EDIdata::operator[](int i) const{
   COL_METHOD(EDIdata::operator[]const);
   return *Children[i];
}

int EDIdata::childIndex(const COLstring& Name) const{
   COL_METHOD(EDIdata::childIndex);
   if (isRepeat()){
      return -1;
   }
   const EDInode* pType;
   COL_VAR(ChildIndex);
   if (ChildIndex == -1){
      COL_TRC("We have root node");
      pType = pParentType;  // root
   } else {
      COL_TRC("We have a non root node");
      COL_VAR2(ChildIndex, pParentType->Children[ChildIndex].pNode->Name);
      pType = pParentType->Children[ChildIndex].pNode;
   }
   for (int i=0; i < pType->Children.size(); i++){
      COL_VAR2(pType->Children[i].pNode->Name, pType->Children[i].Description); // check both since description is the human-readable name of the node
      if (Name == pType->Children[i].pNode->Name || Name == pType->Children[i].Description){
         return i;
      }
   }  
   return -1;
}

// TODO - doing linear lookup for now - syntax trumps efficiency.
EDIdata& EDIdata::operator[](const COLstring& Name){
   COL_METHOD(EDIdata::operator[string]);
   int i = childIndex(Name);
   if (i == -1){
      throw COLerror(Name + " not a member", 0);  // we have a repeating node.
   }
   return (this)->operator[](i);
}

EDIdata& EDIdata::operator=(const COLstring& iData){
   COL_METHOD(EDIdata::operator=);
   COL_VAR(Data);
   Data = iData;
   return *this;
}

bool EDIdata::operator==(const EDIdata& Other) const{
   COL_METHOD(EDIdata::operator==);
   bool IsEqual = (Data == Other.Data) 
            && (Type == Other.Type)
            && (ChildIndex == Other.ChildIndex)
            && (Children.size() == Other.Children.size());
   if (!IsEqual) { return false; } // early return before iterating children if nodes already don't match
   for(int i = 0; i < Children.size(); i++){
      IsEqual = (*Children[i] == *Other.Children[i]);
      if (!IsEqual) { return false; }
   }
   return true;
}

void EDIdata::flatwire(COLstring* pOut) const{
   COL_METHOD(EDTdata::flatwire);
   switch (Type){
   case EDIN_MESSAGE:             return EDIformatMessage        (*this, pOut);
   case EDIN_SEGMENT_GROUP_ARRAY: return EDIformatMessage        (*this, pOut);
   case EDIN_SEGMENT_GROUP:       return EDIformatMessage        (*this, pOut);
   case EDIN_SEGMENT_ARRAY:       return EDIformatMessage        (*this, pOut);
   case EDIN_SEGMENT:             return EDIformatSegment        (*this, pOut);
   case EDIN_FIELD_ARRAY:         return EDIformatRepeatingField (*this, pOut);
   case EDIN_FIELD:               return EDIformatField          (*this, pOut);
   case EDIN_SUBFIELD:            return EDIformatSubField       (*this, pOut);
   default : *pOut += Data;
   }
}

void EDIdata::flatwireHtml(COLstring* pOut) const{
   COL_METHOD(EDTdata::flatwireHtml);
   switch (Type){
   case EDIN_MESSAGE:             return EDIformatMessageHtml        (*this, pOut);
   case EDIN_SEGMENT_GROUP_ARRAY: return EDIformatMessageHtml        (*this, pOut);
   case EDIN_SEGMENT_GROUP:       return EDIformatMessageHtml        (*this, pOut);
   case EDIN_SEGMENT_ARRAY:       return EDIformatMessageHtml        (*this, pOut);
   case EDIN_SEGMENT:             return EDIformatSegmentHtml        (*this, pOut);
   case EDIN_FIELD_ARRAY:         return EDIformatRepeatingFieldHtml (*this, pOut);
   case EDIN_FIELD:               return EDIformatFieldHtml          (*this, pOut);
   case EDIN_SUBFIELD:            return EDIformatSubFieldHtml       (*this, pOut);
   default : *pOut += Data;
   }
}

void EDIdata::toVar(COLvar* pOut) const{
   COL_METHOD(toVar);
   COLvar Temp;
   Temp["n"] = name();
   Temp["t"] = COLintToString((int)Type);
   int Max = size();
   if (Max > 0){
      Temp["c"].setArrayType();
      for (int i=0; i < Max; i++){
         Children[i]->toVar(&Temp["c"]);
      }
   } else {
      Temp["v"] = value();
   }
   pOut->push_back(Temp);
}


COLostream& operator<<(COLostream& Stream, const EDIdata& Node){
   Stream << Node.name() << " leaf = " << Node.isLeaf();
   if (Node.isLeaf()){
      Stream << " value=" << Node.value() << newline;
   } else {
      Stream << " childcount = " << Node.size() << indent << newline;
      for (int i=0; i < Node.size(); i++){
         Stream << Node[i];
      }
      Stream << unindent;
   }
   return Stream;
}
