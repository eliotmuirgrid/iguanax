function GITTdrawerHtml() {
   return /*html*/`
   <div class="GITTdrawer">
      <button class="GITTdrawerHeader" type="button">
         <span class="GITTdrawerTitle">ADVANCED</span>
         <span class="GITTdrawerChevron">▸</span>
      </button>

      <div class="GITTdrawerBody">
         <button class="GITTaction GITTactionLibraryUsage" type="button">
            <span class="GITTactionIcon">
               <a class="TRANStoolbarButton TRGTlibraryUsage"></a>
            </span>
            <span class="GITTactionLabel">Library usage</span>
         </button>

         <button class="GITTaction GITTactionTerminal" type="button">
            <span class="GITTactionIcon">
               <a class="TRANStoolbarButton TRGTgitTerminalButton"></a>
            </span>
            <span class="GITTactionLabel">Open Git terminal</span>
         </button>

         <button class="GITTaction GITTactionIguanaXLuaAssistant" type="button">
            <a
               class="TRANStoolbarButton TRANSaiButton"
               target="_blank"
               href="https://chatgpt.com/g/g-6840451476cc8191bb7fe56ce9806315-iguanax-lua"
               title="Open IguanaX Lua Assistant">
            </a>
            <a target="_blank" href="https://chatgpt.com/g/g-6840451476cc8191bb7fe56ce9806315-iguanax-lua" class="GITTactionLabel GITTchatGptLink">Open IguanaX Lua Assistant</a>
         </button>
      </div>
   </div>
   `;
}
