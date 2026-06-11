function CYTOcreateHelpBox() {
   if(document.querySelector('.CYTOhelpBox')) { return; }
   const container   = document.querySelector('.DASHdashboard');
   const helpBox     = document.createElement('div');
   helpBox.className = 'CYTOhelpBox';
   helpBox.innerHTML = /*html*/`
      <div class="CYTOhelpTrigger">?</div>
      <div class="CYTOhelpContent">
         <div class="CYTOhelpItem">
            <span class="CYTOhelpKey">Right-click</span>Context actions
         </div>
         <div class="CYTOhelpItem">
            <span class="CYTOhelpKey">Shift + click node</span>Multi-select
         </div>
         <div class="CYTOhelpItem">
            <span class="CYTOhelpKey">Shift + drag</span>Box select
         </div>
         <div class="CYTOhelpItem">
            <span class="CYTOhelpKey">Click list row</span>Center on node
         </div>
      </div>
   `;
   container.appendChild(helpBox);
}
   