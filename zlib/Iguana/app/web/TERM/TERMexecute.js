function TERMexecuteCommand(CommandChain, callback){
   let ApiData = {};
   ApiData.component = PAGEhashParameter("component");
   ApiData.commands = CommandChain;
   ApiData.development = TRANSdevelopment();
   TERMinputOutputHtml(CommandChain[0], "", true, false);
   APIcall('git/custom_command', ApiData, function(R) {
      if(!TRANSviewingTranslator()) return;
      if (!R.success) return TGITterminal.output(R.error);
      let Output = R.data;
      TERMhandleLibraryCreateError(Output);
      TERMinputOutputHtml(CommandChain[0], Output[0], false, true);
      for (let i=1; i < Output.length; i++) TERMinputOutputHtml(CommandChain[i], Output[i], true, true);
      TRANSfileCurrentReload();
      MODELforcePoll('TRANSmodel');
      TERMsetFocus();
      if(callback) callback();
   });
}

function TERMhandleLibraryCreateError(Output){
   if (!Output.length) return;
   for (let i=0; i < Output.length; i++) {
      if (typeof(Output[i]) === "string") {
         if (Output[i].search("fatal: pathspec") != -1){
            Output[i] = Output[i].concat("If you have encountered this error when creating a new libary, you may ignore it.");
         }
      } 
   }
}

function TERMinputOutputHtml(input, output, includePrompt, includeOutput){
   let Div = document.querySelector(".TGITterm");
   if(!Div) TGITopenBranchTerminal();
   let InputHtml = TERMpromptHtml(input);
   if(includePrompt) TGITterminal.DOM.output.insertAdjacentHTML('beforeEnd', InputHtml);
   if(!includeOutput) return;
   let Lines = output.split('\n');
   for (let i=0; i < Lines.length; i++) TGITterminal.output(Lines[i]);
}

function TERMpromptHtml(prompt){
   let ComponentId = PAGEhashParameter("component");
   return /*html*/`
   <div class="TERMcommand">
      <div class="TERMprompt">${ESChtmlEscape(ComponentId)}></div>
      <div class="TERMpreserveWhitespace">${ESChtmlEscape(prompt)}</div>
   </div>
   `;
}