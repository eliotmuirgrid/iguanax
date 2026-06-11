// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7node
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 07 October 2022 - 11:07AM
// ---------------------------------------------------------------------------

#include <LHL7/LHL7node.h>

#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIsave.h>

#include <NOX/NOXconstant.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

LHL7node::LHL7node(EDIdata* ipNode) : pNode(ipNode){
   COL_METHOD(LHL7node::LHL7node);
}

LHL7node::~LHL7node(){
   COL_METHOD(LHL7node::~LHL7node);
}

NODgrammarType LHL7node::grammarType() const{
   if (pNode->isRepeat()){
      return NOD_VECTOR;
   }
   if (pNode->isLeaf() || pNode->Type == EDIN_SUBSUBFIELD){
      return NOD_SIMPLE;
   } 
   return NOD_COMPLEX;
}

 int LHL7node::protocolType() const {
    return 101;   // TODO get rid of MAJIK number.
 }

 int LHL7node::nodeType() const {
    return pNode->Type;
 }
 
static const COLstring Hl7("hl7");
 

 const COLstring& LHL7node::protocolName() const { return Hl7; }

 static const COLstring Message("message");
 static const COLstring SegmentGroup("segment_group");
 static const COLstring Segment("segment");
 static const COLstring Field("field");
 static const COLstring SubField("subfield");
 static const COLstring SubSubField("subsubfield");
 
 static const COLstring Unknown("unknown");

 const COLstring& LHL7node::nodeTypeName() const{
   switch (pNode->Type){
      case EDIN_MESSAGE      : return Message;
      case EDIN_SEGMENT_GROUP: return SegmentGroup;
      case EDIN_SEGMENT      : return Segment;
      case EDIN_FIELD        : return Field;
      case EDIN_SUBFIELD     : return SubField;
      case EDIN_SUBSUBFIELD  : return SubSubField;
      default                : return Unknown;
   }
 }
 
 const COLstring& LHL7node::name() const{
    return pNode->name();
 }

 bool LHL7node::isReadOnly() const{
   return false;
 }

 bool LHL7node::isNamed() const{
    switch (pNode->Type){
      case EDIN_MESSAGE             : return true;
      case EDIN_SEGMENT_GROUP_ARRAY : return true;
      case EDIN_SEGMENT_GROUP       : return true;
      case EDIN_SEGMENT_ARRAY       : return true;
      case EDIN_SEGMENT             : return true;
      case EDIN_FIELD               : return true;
      case EDIN_SUBFIELD            : return true;
      case EDIN_SUBSUBFIELD         : return true;
      default : return false;
   }
 }

bool LHL7node::isHomogeneous() const{
   switch (pNode->Type){
      case EDIN_SEGMENT_GROUP_ARRAY : return true;
      case EDIN_SEGMENT_ARRAY       : return true;
      case EDIN_FIELD_ARRAY         : return true;
      default                       : return false;
   }
}

bool LHL7node::favorIndexByName() const{
   switch (pNode->Type){
      case EDIN_MESSAGE       : return true;
      case EDIN_SEGMENT_ARRAY : return true;
      case EDIN_SEGMENT_GROUP : return true;
      default : return false;
   }
}

   //virtual bool gracefulNoSuchMember() const { return false; }
bool LHL7node::isNull() const{
   for (int i = 0; i < pNode->Children.size(); i++){ // if leaf, then Children.size() == 0 so we skip this loop
      if (!pNode->Children[i]->isNull()) { return false; }
   }
   return pNode->Data.is_null();
}
   //virtual void setIsNull(bool IsNull) {}
  
void LHL7node::setValue(const COLstring& Value){
   pNode->setValue(Value);
}
const COLstring& LHL7node::value() const{
   return pNode->Data;
}

LHL7node* LHL7nodeGetChild(COLarray< COLref<LHL7node> >* pChildren, int i, EDIdata* pParent){
   COL_FUNCTION(LHL7nodeGetChild);
   COL_VAR2(i, pChildren->size());
   if (pChildren->size() <= i){
      pChildren->resize(i+1);
      COL_VAR(pChildren->size());
   } 
   COLref<LHL7node>* ppChild = &(*pChildren)[i];
   if (ppChild->get() == NULL){
      COL_TRC("Create child");
      *ppChild = new LHL7node(&((*pParent)[i]));   
   }
   COL_VAR(ppChild->get());
   return ppChild->get();
}

// createDummyItem should only be used for repeat nodes. Calling it for other node types will trigger a crash!
const COLref<NODplace> LHL7node::createDummyItem() const{
   COL_METHOD(LHL7node::createDummyItem);
   if ( pNode->isRepeat() ) {
      COL_TRC("Create a new, temporary child");
      COLref<EDIdata> pCurrentPos = new EDIdata(pNode->Collection, pNode->pParentType, pNode->ChildIndex, pNode->Type);
      COLref<LHL7node> Temp       = new LHL7node(pCurrentPos.get());
      NODplace* pTempChild        = LHL7nodeGetChild(&Temp->m_Children, pNode->ChildIndex, Temp->pNode.get());
      COLref<NODplace> Out        = pTempChild;
      return Out;
   }
   COL_TRC("If we hit this code, Iguana will crash");
   COLref<NODplace> Node = new LHL7node(NULL);  //kaboom
   return Node;
}

// In Iguana 6, the parser always initialized all children for a given node. In IguanaX, we only initialize the nodes 
// that we have data for. To preserve the ability to see all the possible children, we need to return the grammar size
// most of the time. The exception is when we are dealing with repeat nodes. In such a case, we need to return the 
// actual size (number of repeats we have in the message) as the grammar size will always be at least 1, even when no 
// repeat is present. In order for repeat nodes to have the correct annotation and intellisense behaviour, we need to 
// use createDummyItem to insert a fake node as needed.
int LHL7node::size() const{
   if ( pNode->isRepeat() ) { return pNode->size(); }
   return pNode->grammarSize();
}

static bool LHL7nodeIsBadIndexGrammar(int i){
   COL_FUNCTION(LHL7nodeIsBadIndexGrammar);
   return (i < 0); // Only need to check for negative indices. If the index exceeds the vector size, the vector gets resized. Other boundary errors are bubbled up from EDI code.
}

void LHL7node::clear(){
   COL_METHOD(LHL7node::clear);
   if (isSimple()){
      COL_TRC("Simple node, just set to empty str");
      setValue(NOD_EMPTY_SIMPLE_VALUE);
   } else if (isVector()){
      COL_TRC("Vector node, remove all items and resize");
      int CountOfItem = m_Children.size();
      m_Children.resize(0);
      pNode->Children.clear();
   } else {// isComplex()
      COL_TRC("Complex node, iterate and clear all children");
      int CountOfItem = m_Children.size();
      for (int i = 0; i < CountOfItem; i++){
         COLref<LHL7node>& pChild = this->m_Children[i];
         if (pChild && !pChild->isNull()){ 
            pChild->clear();
         }
      }
      m_Children.resize(0);
      pNode->Children.clear();
   }
}

void LHL7node::remove(int i){
   COL_METHOD(LHL7node::remove);
   // Unlike in Iguana 6, we don't have a read only hl7 structure so we can just nullify or remove the input.
   if (isHomogeneous()){ // for repeating nodes, have to remove the child from the parent vector to free the memory.
      COL_VAR2(i, size());
      if (i < 0 || i > size() - 1) { // In C++, we care about 0 < index < size - 1, but for error messaging purposes we convert to lua's range of 1 < index < size
         throw COLerror(COL_T("Index ") + COLintToString(i+1) + COL_T(" is out of bounds for node ") + this->name() + COL_T(" with size ") + COLintToString(size())); 
      }
      if (i < m_Children.size()){ m_Children.remove(i); } // The underlying pNode will always have the children availaible, however m_Children is only populated if the node's children were ever directly accessed in translator/annotations so we have to do an additional check here
      pNode->Children.remove(i);
   } else {
      nullify(i);  // the nullify function will check for a valid index
   }
}

NODplace& LHL7node::add(const char* Arg1, const char* Arg2){
   return *this;
}

NODplace& LHL7node::insert(int i, const char* Arg1, const char* Arg2){
   return *this;
}


NODplace& LHL7node::child(int i){
  if (LHL7nodeIsBadIndexGrammar(i)) { throw COLerror(COL_T("Index ") + COLintToString(i + 1) + COL_T(" is out of bounds.")); }
   NODplace* pChild = LHL7nodeGetChild(&m_Children, i, pNode.get());
   return *pChild;
}

const NODplace& LHL7node::child(int i) const{
  if (LHL7nodeIsBadIndexGrammar(i)) { throw COLerror(COL_T("Index ") + COLintToString(i + 1) + COL_T(" is out of bounds.")); }
   NODplace* pChild = LHL7nodeGetChild((COLarray< COLref<LHL7node> >*)&m_Children, i, pNode.get());
   return *pChild;
}

int LHL7node::childIndex(const COLstring& Name) const{
   return pNode->childIndex(Name);
}


NODplace& LHL7node::child(const COLstring& Name){
   int i = childIndex(Name);
   if (-1 == i){
      throw COLerror("No such member", 0);
   }
   NODplace* pChild = LHL7nodeGetChild(&m_Children, i, pNode.get());
   return *pChild;
}

const NODplace& LHL7node::child(const COLstring& Name) const{
   int i = childIndex(Name);
   if (-1 == i){
      throw COLerror("No such member", 0);
   }
   EDIdata* pPlainNode = pNode.get();
   NODplace* pChild = LHL7nodeGetChild((COLarray< COLref<LHL7node> >*)&m_Children, i, pNode.get());
   return *pChild;
}

// Remove a child by name.
void LHL7node::remove(const COLstring& Name){
   COL_METHOD(LHL7node::remove);
   COL_VAR(Name);
   int i = childIndex(Name);
   if (i == -1){
      throw COLerror(Name + " not a member");
   }
   remove(i);
}

// Nullify a child (by integer index or name).
void LHL7node::nullify(int i){
   COL_METHOD(LHL7node::nullify);
   if (isSimple()){
      throw COLerror("The node " + this->name() + " is a leaf node without children.");
   } else if (LHL7nodeIsBadIndexGrammar(i)){
      throw COLerror(COL_T("Index ") + COLintToString(i+1) + COL_T(" is out of bounds for node ") + this->name() + COL_T(" with size ") + COLintToString(size()));
   }
   LHL7node* pChild = LHL7nodeGetChild(&(this->m_Children), i, this->pNode.get());
   if (pChild && !pChild->isNull()){
      pChild->clear();
   }
}

void LHL7node::nullify(const COLstring& Name){
   COL_METHOD(LHL7node::nullify);
   int i = childIndex(Name);
   if (i == -1){
      throw COLerror(Name + " not a member");
   }
   nullify(i);
}

void LHL7node::grammar(COLvar* pGrammar) const{
   COL_METHOD(LHL7node::grammar);
   // Chunder struck - TODO - make this more efficient.
   COLstring StringGrammar, Error;
   EDIsave(pNode->Collection, &StringGrammar, &Error);
   pGrammar->parse(StringGrammar);
}

static bool LHL7nodeIsUnescapedMshField(const LHL7node* pHl7Node){
   COL_FUNCTION(LHL7nodeIsUnescapedMshField);
   // MSH.1 and MSH.2 remain unescaped
   if (pHl7Node->pNode->ChildIndex == 0 && pHl7Node->name() == COL_T("Field Separator"))     return true;
   if (pHl7Node->pNode->ChildIndex == 1 && pHl7Node->name() == COL_T("Encoding Characters")) return true;
   return false;
}

bool LHL7node::shouldEscapeValue() const { 
   COL_METHOD(LHL7node::shouldEscapeValue);
   COL_VAR3(this->name(), this->pNode->pParentType->Name, this->pNode->ChildIndex);
   if ((COL_T("MSH") == this->pNode->pParentType->Name) && LHL7nodeIsUnescapedMshField(this)){
      COL_TRC("This is MSH.1 or MSH.2, don't escape it");
      return false;
   }
   return true; 
} 
