//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LX12node.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/8/23 2:16 PM
//  ---------------------------------------------------------------------------
#include <LX12//LX12node.h>

#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>

#include <NOX/NOXconstant.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

LX12node::LX12node(EDIdata* ipNode) : pNode(ipNode){
   COL_METHOD(LX12node::LX12node);
}

LX12node::~LX12node(){
   COL_METHOD(LX12node::~LX12node);
}

NODgrammarType LX12node::grammarType() const{
   if (pNode->isRepeat()){
      return NOD_VECTOR;
   }
   if (pNode->isLeaf() || pNode->Type == EDIN_SUBSUBFIELD){
      return NOD_SIMPLE;
   }
   return NOD_COMPLEX;
}

int LX12node::protocolType() const {
   return 101;   // TODO get rid of MAJIK number.
}

int LX12node::nodeType() const {
   return pNode->Type;
}

static const COLstring X12("x12");


const COLstring& LX12node::protocolName() const { return X12; }

static const COLstring Message("message");
static const COLstring SegmentGroup("segment_group");
static const COLstring Segment("segment");
static const COLstring Field("field");
static const COLstring SubField("subfield");
static const COLstring SubSubField("subsubfield");

static const COLstring Unknown("unknown");

const COLstring& LX12node::nodeTypeName() const{
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

const COLstring& LX12node::name() const{
   return pNode->name();
}

bool LX12node::isReadOnly() const{
   return false;
}

bool LX12node::isNamed() const{
   switch (pNode->Type){
      case EDIN_MESSAGE             : return true;
      case EDIN_SEGMENT_GROUP_ARRAY : return true;
      case EDIN_SEGMENT_GROUP       : return true;
      case EDIN_SEGMENT_ARRAY       : return true;
      case EDIN_SEGMENT             : return true;
      default : return false;
   }
}

bool LX12node::isHomogeneous() const{
   return false;
}

bool LX12node::favorIndexByName() const{
   switch (pNode->Type){
      case EDIN_MESSAGE       : return true;
      case EDIN_SEGMENT_ARRAY : return true;
      case EDIN_SEGMENT_GROUP : return true;
      default : return false;
   }
}

//virtual bool gracefulNoSuchMember() const { return false; }
bool LX12node::isNull() const{
   for (int i = 0; i < pNode->Children.size(); i++){ // if leaf, then Children.size() == 0 so we skip this loop
      if (!pNode->Children[i]->isNull()) { return false; }
   }
   return pNode->Data.is_null();
}
//virtual void setIsNull(bool IsNull) {}

void LX12node::setValue(const COLstring& Value){
   pNode->setValue(Value);
}
const COLstring& LX12node::value() const{
   return pNode->Data;
}

const COLref<NODplace> LX12node::createDummyItem() const{
   COLref<NODplace> Node = new LX12node(NULL);  //kaboom
   return Node;
}

int LX12node::size() const{
   return pNode->grammarSize();
}

static bool LX12nodeIsBadIndexGrammar(const LX12node* pNode, int i){
   COL_FUNCTION(LX12nodeIsBadIndexGrammar);
   return (i < 0);
}

void LX12node::clear(){
   COL_METHOD(LX12node::clear);
   if (isSimple()){
      COL_TRC("Simple node, just set to empty str");
      setValue(NOD_EMPTY_SIMPLE_VALUE);
   } else if (isVector()){
      COL_TRC("Vector node, clear children of node");
      pNode->Children.clear();
   } else {// isComplex()
      COL_TRC("Complex node, iterate and clear all children");
      int CountOfItem = pNode->size();
      for (int i = 0; i < CountOfItem; i++){
         COLref<LX12node>& pChild = this->m_Children[i];
         if (pChild && !pChild->isNull()){ 
            pChild->clear();
         }
      }
      m_Children.resize(0);
      pNode->Children.clear();
   }
}

void LX12node::remove(int i){
   // Same thing as LHL7node::remove. We don't have a read only x12 structure so we can just nullify the input.
   // the nullify function will check for a valid index
   nullify(i);
}
NODplace& LX12node::add(const char* Arg1, const char* Arg2){
   return *this;
}

NODplace& LX12node::insert(int i, const char* Arg1, const char* Arg2){
   return *this;
}

LX12node* LX12nodeGetChild(COLarray< COLref<LX12node> >* pChildren, int i, EDIdata* pParent){
   COL_FUNCTION(LHL7nodeGetChild);
   COL_VAR2(i, pChildren->size());
   if (pChildren->size() <= i){
      pChildren->resize(i+1);
      COL_VAR(pChildren->size());
   } 
   COLref<LX12node>* ppChild = &(*pChildren)[i];
   if (ppChild->get() == NULL){
      COL_TRC("Create child");
      *ppChild = new LX12node(&((*pParent)[i]));   
   }
   COL_VAR(ppChild->get());
   return ppChild->get();
}

NODplace& LX12node::child(int i){
   if (LX12nodeIsBadIndexGrammar(this, i)) { throw COLerror(COL_T("Index ") + COLintToString(i + 1) + COL_T(" is out of bounds.")); }
   NODplace* pChild = LX12nodeGetChild(&m_Children, i, pNode.get());
   return *pChild;
}

const NODplace& LX12node::child(int i) const{
   if (LX12nodeIsBadIndexGrammar(this, i)) { throw COLerror(COL_T("Index ") + COLintToString(i + 1) + COL_T(" is out of bounds.")); }
   NODplace* pChild = LX12nodeGetChild((COLarray< COLref<LX12node> >*)&m_Children, i, pNode.get());
   return *pChild;
}

int LX12node::childIndex(const COLstring& Name) const{
   return pNode->childIndex(Name);
}

NODplace& LX12node::child(const COLstring& Name){
   int i = childIndex(Name);
   if (-1 == i){
      throw COLerror("No such member", 0);
   }
   NODplace* pNew = new LX12node(&(*pNode)[i]);
   return *pNew;
}

const NODplace& LX12node::child(const COLstring& Name) const{
   int i = childIndex(Name);
   if (-1 == i){
      throw COLerror("No such member", 0);
   }
   EDIdata* pPlainNode = pNode.get();
   NODplace* pNew = new LX12node(&(*pPlainNode)[i]);
   return *pNew;
}

// Remove a child by name.
void LX12node::remove(const COLstring& Name){
   // TODO
}

// Nullify a child (by integer index or name).
void LX12node::nullify(int i){
   COL_METHOD(Lx12node::nullify);
   if (isSimple()){
      throw COLerror("The node " + this->name() + " is a leaf node without children.");
   } else if (LX12nodeIsBadIndexGrammar(this, i)){
      throw COLerror(COL_T("Index ") + COLintToString(i+1) + COL_T(" is out of bounds for node ") + this->name() + COL_T(" with size ") + COLintToString(size()));
   }
   LX12node* pChild = LX12nodeGetChild(&(this->m_Children), i, this->pNode.get());
   if (pChild && !pChild->isNull()){
      pChild->clear();
   }
}

void LX12node::nullify(const COLstring& Name){
   // TODO
}
