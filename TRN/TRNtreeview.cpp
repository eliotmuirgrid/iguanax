//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtreeview
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, May 26th, 2010 @ 11:42:27 AM
//---------------------------------------------------------------------------
#include "TRNtreeview.h"
#include "TRNtraverser.h"
#include "TRNtitleFromHtmlString.h"

#include <NOD/NODaddress.h>

#include <SFI/SFIhtmlCrop.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
#include <COL/COLauto.h>
COL_LOG_MODULE;

// An "expansion tree" for a tree.
// For each child node which should be expanded, there is an entry in
// m_Expanded.  This is entry is, in turn, the child node's expansion tree.
class TRNexpansionTree{
public:
   // Will return NULL if the child with the specified Key is not in this expansion tree.
   const TRNexpansionTree* getChildExpansionTree(const COLstring& Key) const;
   TRNexpansionTree& addChild(const COLstring& Key);
   void clear() { m_Expanded.clear(); }

private:
   COLhashmap<COLstring, COLauto<TRNexpansionTree> > m_Expanded;
};

const TRNexpansionTree* TRNexpansionTree::getChildExpansionTree(const COLstring& Key) const{
   COLhashmapPlace ChildPlace = m_Expanded.find(Key);
   if (ChildPlace){ return m_Expanded[ChildPlace].get(); } 
   else           { return NULL; }
}

TRNexpansionTree& TRNexpansionTree::addChild(const COLstring& Key){
   COLhashmapPlace ChildPlace = m_Expanded.add(Key, new TRNexpansionTree());
   return m_Expanded[ChildPlace];
}

#define TRN_MAX_NODE_LENGTH 64000
static inline void TRNhtmlCrop(const COLstring& Original, COLstring& Out){
   SFIhtmlCrop(Original.c_str(), Original.size(),
      "<span class=\"line_cropped_warning\">&hellip; This line is too long to display.</span>",
      TRN_MAX_NODE_LENGTH, Out);
}

static void TRNopenVarNode(TRNtraverser& Traverser, COLvar* pTree, bool IncludeTitle){
   COL_FUNCTION(TRNopenVarNode);
   COLstring CollapsedView, ExpandedView, UncroppedCollapsedView, UncroppedExpandedView; 
   Traverser.getTreeviewText(UncroppedExpandedView, UncroppedCollapsedView);
   // While getTreeviewText() can technically fail (not return any strings),
   // in reality it won't, because this only happens with raw Lua data,
   // which by this point has been copied into a NOB structure.
   TRNhtmlCrop(UncroppedCollapsedView, CollapsedView);
   TRNhtmlCrop(UncroppedExpandedView,  ExpandedView);
   int ProtocolType, NodeType;
   bool IsRepeatNode;
   COLstring Name;
   Traverser.getTypeInfo(ProtocolType, NodeType);
   Traverser.getUniqueKey(Name);
   (*pTree)["cv"] = CollapsedView;      // cv == collapsed view
   (*pTree)["ev"] = ExpandedView;       // ev == expanded view
   (*pTree)["pt"] = ProtocolType;       // pt == protocol type
   (*pTree)["nt"] = NodeType;           // nt == node type
   (*pTree)["e"] = Traverser.isNull();  // e == is empty
   (*pTree)["n"] = Name;                // n == name
   (*pTree)["l"] = Traverser.isLeaf();  // l == is leaf
   if (IncludeTitle){
      COLstring Title;
      TRNtitleFromHtmlString(UncroppedExpandedView, Title);
      if (Title.is_null()){ (*pTree)["title"] = " ";  } 
      else {                (*pTree)["title"] = Title;}
   }
}

static void TRNaddVarNodeChildren(TRNtraverser& Traverser, const TRNexpansionTree* pExpansionTree, COLvar* pTree){
   if (!Traverser.isLeaf()){
      COLvar Children;
      Children.setArrayType();
      while (Traverser.stepNext()){
         COLvar OneNode;
         TRNopenVarNode(Traverser, &OneNode, false);
         if (pExpansionTree){
            COLstring ChildKey;
            Traverser.getUniqueKey(ChildKey);
            const TRNexpansionTree* pChildExpansionTree = pExpansionTree->getChildExpansionTree(ChildKey);
            if (!Traverser.isLeaf() && pChildExpansionTree){
               COLauto<TRNtraverser> pChildTraverser = Traverser.traverser();
               TRNaddVarNodeChildren(*pChildTraverser, pChildExpansionTree, &OneNode);
            }
         }
         Children.push_back(OneNode);
      }
      (*pTree)["c"].swap(&Children); // c == children
   }
}

static void TRNshowList(const COLlist<COLstring>& Address){
   COL_TRC("List:");
   COLlist<COLstring>::const_iterator i = Address.cbegin(), End = Address.cend();
   for (; i != End; i++){
      COL_TRC(*i);
   }
}

static void TRNparseStartingAddressJson(const COLstring& StartingAddressJson, COLlist<COLstring>& StartingAddress){
   COL_FUNCTION(TRNparseStartingAddressJson);
   COL_VAR(StartingAddressJson);
   StartingAddress.clear();
   if (StartingAddressJson.is_null()){ return; }
   COLvar varStartingAddress;
   if( !varStartingAddress.parse(StartingAddressJson) ){ COL_ERROR_STREAM_PLAIN("Bad starting address JSON: " << StartingAddressJson, COLerror::PreCondition); }
   else{
      if(!varStartingAddress.isMap()) { return; }
      COLvar::COLvarMap::const_iterator i = varStartingAddress.map().cbegin();
      do{
         StartingAddress.add(i->first);
         varStartingAddress = i->second;
         i = varStartingAddress.map().cbegin();
      } 
      while(varStartingAddress.size() > 0);
   }
   COL_VAR(StartingAddress.size());
#ifdef COL_ENABLE_TRACING
   TRNshowList(StartingAddress);
#endif
}

static void TRNpopulateExpansionTree(const COLvar& varExpansionTree, TRNexpansionTree& ExpansionTree){
   COL_FUNCTION(TRNpopulateExpansionTree);
   if(!varExpansionTree.isMap()) { return; }
   COLvar vExpansionTree = varExpansionTree;
   COLvar::COLvarMap::const_iterator i = vExpansionTree.map().cbegin(), E = vExpansionTree.map().cend();
   for(;i!=E;++i){
      COL_VAR(i->first);
      TRNexpansionTree& ChildTree = ExpansionTree.addChild(i->first);
      TRNpopulateExpansionTree(i->second, ChildTree);
   }
}

void TRNparseExpansionTreeJson(const COLstring& ExpansionTreeJson, TRNexpansionTree& ExpansionTree){
   COL_FUNCTION(TRNparseExpansionTreeJson);
   ExpansionTree.clear();
   if (ExpansionTreeJson.is_null()){ return; }
   COLvar varExpansionTree;
   if( !varExpansionTree.parse(ExpansionTreeJson) ){ COL_ERROR_STREAM_PLAIN("Bad starting address JSON: " << ExpansionTreeJson, COLerror::PreCondition); }
   else { TRNpopulateExpansionTree(varExpansionTree, ExpansionTree); }
}

static void TRNbuildVarTree(TRNtraverser& Traverser, const COLstring& StartingAddressJson, const COLstring& ExpansionTreeJson,
                             const COLstring& FileName, const COLstring& FunctionName, const int Call, const int Line, 
                             COLvar* pTree) {
   COL_FUNCTION(TRNbuildVarTree);
   COL_VAR(StartingAddressJson);
   COLlist<COLstring> StartingAddress;
   TRNparseStartingAddressJson(StartingAddressJson, StartingAddress);
   COL_VAR(StartingAddressJson);
   if (!StartingAddress.size()){ 
      COL_TRC("We've been given the tree with no need to drill down.");
      StartingAddress.add(TRN_ROOT_KEY); 
   }
   TRNexpansionTree ExpansionTree;
   COL_VAR(ExpansionTreeJson);
   TRNparseExpansionTreeJson(ExpansionTreeJson, ExpansionTree);
   const TRNexpansionTree* pExpansionTree = &ExpansionTree;
   COLlist< COLauto<TRNtraverser> > pTraversers;
   TRNtraverser* pTraverser = &Traverser;
   COLstring CurrentKey;
   do{
      COL_TRC("In do while");
      pTraverser->getUniqueKey(CurrentKey);
      COL_VAR(CurrentKey);
      if (StartingAddress[StartingAddress.first()] == CurrentKey){
         if (CurrentKey != TRN_ROOT_KEY){
            COLlistPlace NewTraverserPlace = pTraversers.add(pTraverser->traverser());
            pTraverser = pTraversers[NewTraverserPlace].get();
         }
         if (pExpansionTree) pExpansionTree = pExpansionTree->getChildExpansionTree(CurrentKey);
         StartingAddress.remove(StartingAddress.first());
      }
   } while (StartingAddress.size() && pTraverser->stepNext());
   COL_TRC("After do while");
   if (StartingAddress.size()){
      COL_TRC("Starting address list not empty! Some branches don't exist??");
      const char* pFirst = StartingAddress[StartingAddress.first()].c_str();
      COL_ERROR_STREAM_PLAIN("Specified tree branch does not exist.", COLerror::PreCondition);
   }
   COLvar Root;
   COL_TRC("Getting var node");
   TRNopenVarNode(*pTraverser, &Root, true);
   COL_TRC("Getting Var node children");
   TRNaddVarNodeChildren(*pTraverser, pExpansionTree, &Root);
   COL_TRC("Populating pTree and returning...");
   (*pTree)["Root"].swap(&Root);
   (*pTree)["file_name"] = FileName;
   (*pTree)["funct_name"] = FunctionName;
   (*pTree)["call"] = Call;
   (*pTree)["line"] = Line;
}

void TRNbuildVarTree(const NODaddress& TreeRoot, const COLstring& StartingAddressJson, const COLstring& ExpansionTreeJson,
                      const COLstring& FileName, const COLstring& FunctionName, const int Call, const int Line,
                      COLvar* pTree){
   COLauto<TRNtraverser> pTraverser = TRNcreateTraverser(TreeRoot);
   TRNbuildVarTree(*pTraverser, StartingAddressJson, ExpansionTreeJson, FileName, FunctionName, Call, Line, pTree);
}
