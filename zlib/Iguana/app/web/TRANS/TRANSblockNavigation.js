/**
 * Copyright (c) 2010-2011 iNTERFACEWARE Inc.  All rights reserved.
 * Code for navigating blocks in the translators
 */

 function TRANScontrolBlock2(Scope) {
   // console.log("Scope =", Scope);
   return /*html*/`
   <div class='NIScontrolBlock annav'>
      <span class='nv ncback'>
         <img class='TRANSfirst' src='/TRANS/images/ena_l_stop_arrow.svg'/>
         <img class='TRANSback'  src='/TRANS/images/ena_l_arrow.svg'/>
      </span>
      <span class='TRANSinner'>
         <span class='position'>${Scope.p}</span> of ${Scope.l}</span>
         <span class='nv ncforw'>
            <img class='TRANSnext' src='/TRANS/images/ena_r_arrow.svg'/>
            <img class='TRANSlast' src='/TRANS/images/ena_r_stop_arrow.svg'/>
      </span>
   </div>`;
}

function TRANSsetButtonVisibility(Elements, Hidden) {
   Images = Elements.find("img");
   Images.each(function () {
      const FileName = $(this).attr('src');
      const StartIndex = (FileName.indexOf('/') >= 0) ? FileName.lastIndexOf('/') : 0;
      const DesiredPrefix = Hidden ? 'dis' : 'ena';
      if (FileName.substr(DesiredPrefix.length, 3) != DesiredPrefix) {
         const NewFileName = FileName.substr(0, StartIndex + 1) + DesiredPrefix + FileName.slice(StartIndex + DesiredPrefix.length + 1, FileName.length);
         $(this).attr("src", NewFileName);
      }
   });
}

function TRANSsetAllButtonVisibility(Nav, Scope) {
   TRANSsetButtonVisibility(Nav.find('span.ncback'), Scope.p == 1);
   TRANSsetButtonVisibility(Nav.find('span.ncforw'), Scope.p == Scope.l);
}

function TRANSattachNavClick(Nav, Scope, FunctionScope) {
   Nav.find(".TRANSinner").click(function () { 
      TRANSinsertGotoDiv($(this), Scope, FunctionScope);
   });
   Nav.find(".TRANSfirst").click(function () { 
      TRANSchangeCurrentIteration(FunctionScope, Scope, 1); 
      TRANSsetCode();   
   });
   Nav.find(".TRANSback") .click(function () { 
      TRANSchangeCurrentIteration(FunctionScope, Scope, Math.max(1, Scope.p - 1)); 
      TRANSsetCode();   
   });
   Nav.find(".TRANSnext") .click(function () { 
      TRANSchangeCurrentIteration(FunctionScope, Scope, Math.min(Scope.l, Scope.p + 1)); 
      TRANSsetCode();   
   });
   Nav.find(".TRANSlast") .click(function () { 
      TRANSchangeCurrentIteration(FunctionScope, Scope, Scope.l); 
      TRANSsetCode();   
   });
}

function TRANSchangeCurrentIteration(FunctionScope, Scope, NewIteration, TargetCallee) {
   if (NewIteration == Scope.p && !TargetCallee) return;
   Scope.c = [];
   Scope.p = NewIteration;
   TRANSonScopeIterationChange(TargetCallee,Scope,FunctionScope);
}

function TRANSchangeFunctionIteration(FunctionStartLine, FunctionCallNumber, TargetCallee) {
   let Block = TRANSfindBlockAtLine(FunctionStartLine);
   COL_TRC("Block =", Block);
   if (Block && Block.scope) {
      // TODO - do we need to give the iteration?
      const NewIteration = Math.min(Block.scope.l, FunctionCallNumber);
      COL_TRC("NewIteration =", NewIteration);
      TRANSchangeCurrentIteration(Block.scope, Block.scope, NewIteration, TargetCallee);
   }
}
