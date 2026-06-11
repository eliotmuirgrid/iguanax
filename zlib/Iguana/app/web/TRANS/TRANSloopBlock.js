/**
 * Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
 * Code for loop blocks in the translator
 */

// Return the width of the widest loop block.
function TRANScreateLoopBlocks(Container, Scope, LineHeight, FunctionScope, Depth = 1) {
   const CountOfChild = Scope.c.length;
   let MaxWidth = 0;
   for (let i = 0; i < CountOfChild; i++) {
      const ThisWidth = TRANScreateLoopBlock(Container, Scope.c[i], Scope.s, LineHeight, FunctionScope, Depth);
      if (ThisWidth > MaxWidth) MaxWidth = ThisWidth;
   }
   return MaxWidth;
}

// Returns the width of the loop block.
function TRANScreateLoopBlock(Container, ChildScope, ParentStartLine, LineHeight, FunctionScope, Depth) {
   if (ChildScope.l == 0) return;
   const LoopBlock = $('<div class="ANNloopBlock" />').appendTo(Container);
   const Top = (ChildScope.s - ParentStartLine) * LineHeight;
   LoopBlock.css('top', Top + 'px');
   LoopBlock.css('z-index', Depth); // Set the z-index based on Depth
   LoopBlock.html(ChildScope.d);
   const Nav = $(TRANScontrolBlock2(ChildScope)).prependTo(LoopBlock.find('pre:first'));
   TRANSsetAllButtonVisibility(Nav, ChildScope);
   TRANSattachNavClick(Nav, ChildScope, FunctionScope);
   const MaxChildWidth = TRANScreateLoopBlocks(LoopBlock, ChildScope, LineHeight, FunctionScope, Depth + 1); // Increment Depth for nested blocks
   const MinWidth = MaxChildWidth + 30; // 30 is for some padding
   if (LoopBlock.innerWidth() < MinWidth) {
      LoopBlock.width(MinWidth);
   }
   return LoopBlock.outerWidth();
}
