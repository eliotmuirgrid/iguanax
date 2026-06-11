// Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
// Treeview for a NOD tree.
// Requires /treeview/treeview_2_0.js and jquery.
// Author: Kevin Senn
// The presence of ExpansionTree indicates that this node should be expanded. The tree then indicates which (if any) children should be expanded.
function NTRcreateTreeNode(DataNode, Id) {
   let NodeClass = 'nt_' + DataNode.pt + '_' + DataNode.nt;
   let LiClass = '';
   if (DataNode.e) { NodeClass += ' e'; LiClass = 'e'; }
   if (DataNode.r) { NodeClass += ' r'; }
   const IsExpanded = !DataNode.l && DataNode.c;
   const NewNode = TRVcreateNode(DataNode, LiClass, NodeClass, !DataNode.l, IsExpanded, IsExpanded ? DataNode.ev : DataNode.cv, NTRonNodeToggled, Id);
   if (IsExpanded) {
      const ChildContainer = TRVgetChildContainer(NewNode);
      for (let i = 0; i < DataNode.c.length; i++) {
         ChildContainer.appendChild(NTRcreateTreeNode(DataNode.c[i], Id));
      }
   }
   return NewNode;
}

function NTRelementOffsetTop(Element) {
   const parentBorderTop = parseInt(window.getComputedStyle(Element.parentElement).borderTopWidth) || 0;
   const parentPaddingTop = parseInt(window.getComputedStyle(Element.parentElement).paddingTop) || 0;
   const adjustedOffsetTop = Element.offsetTop + parentBorderTop + parentPaddingTop;
   COL_TRC("Element=", Element, "adjustedOffsetTop=", adjustedOffsetTop, "parentBorderTop=", parentBorderTop, "parentPaddingTop=", parentPaddingTop);
   return adjustedOffsetTop;
}

function NTRouterHeight(element) {
   // Get the height including padding and border
   let height = element.offsetHeight;
   const style = window.getComputedStyle(element);
   const marginTop = parseInt(style.marginTop);
   const marginBottom = parseInt(style.marginBottom);
   height += marginTop + marginBottom;
   return height;
}

function NTRscrollToChildren(NodeTextSpan, NodeChildrenContainer) {
   COL_TRC("NodeChildrenContainer=", NodeChildrenContainer);
   // This code assumes that the direct parent of ul.ifwareTreeview is scrollable, and positioned either absolutely or relatively.
   const ScrollingContainer = NodeTextSpan.closest('ul.ifwareTreeview').parentElement;
   const ScrollTop = ScrollingContainer.scrollTop;
   const ScrollHeight = ScrollingContainer.clientHeight;

   const ChildrenTop = NTRelementOffsetTop(NodeChildrenContainer);
   const ChildrenHeight = NTRouterHeight(NodeChildrenContainer);
   const ChildrenBottom = ChildrenTop + ChildrenHeight + 20; // 20 is to make up for the potential horizontal scrollbar on the bottom
   
   COL_TRC("ChildrenTop=", ChildrenTop);
   if (ChildrenBottom - ScrollTop > ScrollHeight) {
      const NodeTop = NTRelementOffsetTop(NodeTextSpan);
      let NewTop = NodeTop;
      if (ChildrenBottom - NodeTop < ScrollHeight) { NewTop = ChildrenBottom - ScrollHeight + 5; } // 5 is just to make it look a little nicer
      ScrollingContainer.scrollTo({
         top: NewTop,
         behavior: 'smooth'
      });
   }
}

// Get the path of the specified node (represented by its li element). The path will be represented as a JSON object, in the following format: {'root':{'parent':{'me':{}}}
function NTRgetNodeAddress(NodeLi) {
   let DataNode = NodeLi.data('DataNode');
   let NodeAddress = new Object();
   NodeAddress[DataNode.n] = new Object();
   while (!NodeLi.closest('ul').hasClass('ifwareTreeview')) {
      NodeLi = NodeLi.parent().closest('li');
      DataNode = NodeLi.data('DataNode');
      const NewAddress = new Object();
      NewAddress[DataNode.n] = NodeAddress;
      NodeAddress = NewAddress;
   }
   return NodeAddress;
}

function NTRdisplayScriptError(NodeChildrenContainer, Response){
   NodeChildrenContainer.classList.add('error');
   NodeChildrenContainer.textContent = "Script encountered an error on line " + Response.LineNumber + " of file " + Response.FileName + ". Error: \n" + Response.Description;
}

function NTRrequestChildren(NodeChildrenContainer, TreeRoot, NodeTextSpan, Id) {
   COL_TRC("Id=", Id);
   if(TRANSviewingTranslator()) {
      NTRtranslatorRequestChildren(NodeChildrenContainer, TreeRoot, NodeTextSpan, Id);
   } else {
      NTRlogsRequestChildren(NodeChildrenContainer, TreeRoot, NodeTextSpan);
   }
}

function NTRonNodeToggled(DataNode, NodeTextSpan, NodeChildrenContainer, Expanded, Id) {
   COL_TRC("Id=", Id);
   const TreeRoot = TRVgetTreeRoot(NodeChildrenContainer);
   const CookieName = TreeRoot.data('CookieName');
   if (Expanded) {
      NodeTextSpan.innerHTML = DataNode.ev;
      NTRrequestChildren(NodeChildrenContainer, TreeRoot, NodeTextSpan, Id);
   } else {
      NodeTextSpan.innerHTML = DataNode.cv;
      NTRshowOrHideEmptyNodes(TreeRoot);
   }
   let ExpansionTree = NTRgetExpansionTree(TreeRoot.get(0)); //NTRgetExpansionTreeFromLocalStorage(ItemId)
   if (CookieName) {
      ExpansionTree = ExpansionTree.replace(/^{('|")r('|"):{}}$/, "");
      if ("" != ExpansionTree) { STORElocalCreate(CookieName, ExpansionTree); } 
      else                     { STORElocalErase(CookieName); }
   }
}

function NTRpopulateExpansionTree(ChildContainer, ExpansionTree) {
   ChildContainer = $(ChildContainer); // ensure it's a jQuery object
   const Children = TRVgetChildren(ChildContainer);
   Children.each(function () {
      const DataNode = $(this).data('DataNode');
      if (DataNode.n != '' && !ExpansionTree[DataNode.n]) { ExpansionTree[DataNode.n] = {}; }
      const GrandChildren = TRVgetChildContainer(this);
      NTRpopulateExpansionTree(GrandChildren, ExpansionTree[DataNode.n]);
   });
}

function NTRgetDropdown(Treeview) {
   return Treeview.closest('.content').siblings('.emptyNodeDropdown');
}

function NTRshowOrHideEmptyNodes(Treeview, ForceRecreateDropdown) {
   const EmptyNodes = Treeview.find('li.e');
   if (Treeview.data('ShowEmptyNodes')) { EmptyNodes.show(); } 
   else                                 { EmptyNodes.hide(); }
   const Dropdown = NTRgetDropdown(Treeview); // Get existing dropdown, if it exists
   if (EmptyNodes.size() > 0) { Dropdown.show(); } 
   else                       { Dropdown.hide(); }
}

function NTRgetExpansionTree(Treeview) {
   const ExpansionTree = {};
   NTRpopulateExpansionTree(Treeview, ExpansionTree);
   return JSON.stringify(ExpansionTree);
}

function NTRinitTreeview(TreeRoot, Container, Item, UseCookie){
   Container.innerHTML = '';
   const Id = Item.data('Id');
   const ShowEmptyNodes = Item.data('ViewMode') == 'show_empty_nodes';
   const CookieName = Id + '_expanded_tree_nodes';
   const jContainer = $(Container);
   if (UseCookie) { jContainer.data('CookieName', CookieName); }
   jContainer.data('Id', Id).data('ShowEmptyNodes', ShowEmptyNodes);
   Container.appendChild(NTRcreateTreeNode(TreeRoot, Id));
   NTRgetDropdown(jContainer).remove(); // Remove previous dropdown, if there is one.
   // The "dropdown" is just a div floating in the top-right corner of the treeview area, with a single checkbox/label.  It appears iff there are any empty nodes present in the tree.
   const jDropdown = $(`<div class="emptyNodeDropdown">
                         <label title="This tree contains some empty nodes. You can select whether or not you want these nodes to be visible.">
                            <input type="checkbox" name="showEmptyNodes" />
                            Show Empty Nodes
                         </label>
                      </div>`);
   jContainer.closest('.content').after(jDropdown);
   const Checkbox = jDropdown.find('input');
   if (ShowEmptyNodes){ Checkbox.attr('checked', 'checked'); }
   Checkbox.change(function(Event){
      const ShowEmptyNodes = $(this).is(':checked');
      Item.data('ViewMode', ShowEmptyNodes ? 'show_empty_nodes' : 'hide_empty_nodes');
      Item.trigger(ITM_VIEW_MODE_CHANGE_EVENT, [Item]);
      jContainer.data('ShowEmptyNodes', ShowEmptyNodes);
      console.log("ShowEmptyNodes:", ShowEmptyNodes);
      NTRshowOrHideEmptyNodes(jContainer);
      TRANSsetCode();
   });
   NTRshowOrHideEmptyNodes(jContainer);
}

function NTRgetExpansionTreeFromLocalStorage(ItemId){
   let TreeData = STORElocalRead(ItemId + "_expanded_tree_nodes");
   if (TreeData == null){
      return {};
   } else {
      return JSON.parse(TreeData);
   }
}