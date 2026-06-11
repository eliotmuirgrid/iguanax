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

#include <EDI/EDIgrammar.h>  // switching to using full file names since it makes it easier to move code around

#include <COL/COLlog.h>
COL_LOG_MODULE;

// TODO - could consider using a structure which keeps the length
const char* EDItypeAsString(EDItype Type){
   switch(Type){
   case EDI_MESSAGE       : return "message";
   case EDI_SEGMENT_GROUP : return "segment_group";
   case EDI_SEGMENT       : return "segment"; 
   case EDI_COMPOSITE     : return "composite";
   default                : return "unknown";
   }
}

EDItype EDIstringToType(const COLstring& Type) {
   if(Type.equals("message"))       { return EDI_MESSAGE; }
   if(Type.equals("segment_group")) { return EDI_SEGMENT_GROUP; }
   if(Type.equals("segment"))       { return EDI_SEGMENT; }
   if(Type.equals("composite"))     { return EDI_COMPOSITE; }
   return EDI_SEGMENT; // what do?
}

EDInode::EDInode(const COLstring& Name, const COLstring& _Description, EDItype _Type)
   : Name(Name),Description(_Description), Type(_Type) {
   COL_METHOD(EDInode::EDInode);
}

EDInode::EDInode() : Type(EDI_COMPOSITE) {
   COL_METHOD(EDInode::EDInode);

}

EDIchild::EDIchild(EDInode* pNode, const COLstring& _Description, bool IsRequired, bool IsRepeating)
   : pNode(pNode), Description(_Description), Required(IsRequired), Repeats(IsRepeating) {
   COL_METHOD(EDIchild::EDIchild);
}

EDIcollection::EDIcollection(){
   COL_METHOD(EDIcollection::EDIcollection);
}

EDIcollection::~EDIcollection(){
   COL_METHOD(EDIcollection::~EDIcollection);
   COL_TRC("Cleaning up " << Nodes.size() << " nodes.");
   for (auto i=Nodes.begin(); i != Nodes.end(); i++){
      delete i->second;
   }
}

void EDIcollection::clear(){
   COL_METHOD(EDIcollection::clear);
   Nodes.clear();
   MatchRules.clear();
}

bool EDIchild::isRepeating() const{
   COL_METHOD(EDIchild::isRepeating);
   return Repeats;
}

COLostream& operator<<(COLostream& Stream, const EDInode& Node){
   Stream << Node.Type << " - " << Node.Description << " (" << Node.Children.size() << " children)";
   return Stream;
}