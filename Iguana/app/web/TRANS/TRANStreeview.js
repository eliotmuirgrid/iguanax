/** 
 * Copyright (c) 2010-2014 iNTERFACEWARE Inc.  All rights reserved.
 */

/*
 * Creates and returns a DOM element for a tree node.  The element should be put inside
 * a <ul> tag with class "ifwareTreeview".
 * DataNode is any generic object, that will be passed into the onNodeToggled and onNodeClicked event handlers.
 * LiClass is a class that will be applied to the entire tree node.  For example, if the class had the property
 *         display:none, then the node will not be shown in the tree (and there will be no empty space).
 * NodeClass is a class that will only be applied to the node's icon and text.
 * IsExpandable specifies whether or not the node is expandable/collapsable.
 * IsExpanded specifies whether or not the node should start off expanded.
 * DataNode can also be accessed by calling $(NodeLi).data('DataNode'), on some NodeLi.
 */
function TRVcreateNode(DataNode, LiClass, NodeClass, IsExpandable, IsExpanded, NodeHtml, onNodeToggled, Id) {
   COL_TRC("Id=", Id);
   const NodeLi = document.createElement('li');
   const jNodeLi = $(NodeLi);
   jNodeLi.data('DataNode', DataNode);
   if (IsExpandable && IsExpanded) {
      NodeLi.className = 'collapsable';
   } else if (IsExpandable && !IsExpanded) {
      NodeLi.className = 'expandable';
   } else {
      NodeLi.className = 'leaf';
   }
   if (LiClass) NodeLi.className += ' ' + LiClass;
   NodeLi.innerHTML =
      `<div>\
          <span class="treeIcon"></span>
          <span class="nodeText ` + NodeClass + '">' + NodeHtml + `</span>
       </div>
       <ul` + (IsExpanded ? '' : ' style="display:none;"') + '></ul>';

   jNodeLi.children('div').click(function (EventObject) {
      const NodeSpan = jNodeLi.find('span.nodeText:first').get(0);
      const NodeChildren = jNodeLi.find('ul:first').get(0);
      NodeChildren.innerHTML = '';
      if (jNodeLi.hasClass('expandable')) {
         jNodeLi.removeClass('expandable');
         jNodeLi.addClass('collapsable');
         NodeChildren.style.display = '';
         onNodeToggled(DataNode, NodeSpan, NodeChildren, true, Id);
      } else if (jNodeLi.hasClass('collapsable')) {
         jNodeLi.removeClass('collapsable');
         jNodeLi.addClass('expandable');
         NodeChildren.style.display = 'none';
         onNodeToggled(DataNode, NodeSpan, NodeChildren, false, Id);
      }

      //if (onNodeClicked) {
      //   onNodeClicked(DataNode, NodeSpan, NodeChildren, Id);
      //}   
      EventObject.stopPropagation();
   });
   if (IsExpandable) {
      jNodeLi.find('span.nodeText:first')
         .mouseover(function () { $(this).addClass('hover')   })
         .mouseout (function () { $(this).removeClass('hover')});
   }
   return NodeLi;
}

function TRVgetChildContainer(NodeLi) {
   return $(NodeLi).find('ul:first').get(0);
}

function TRVgetChildren(NodeChildContainer) {
   return $(NodeChildContainer).children('li.collapsable');
}

// Returns jQuery wrapper of the root ul of the tree.
function TRVgetTreeRoot(NodeChildContainer) {
   return $(NodeChildContainer).closest('ul.ifwareTreeview');
}
