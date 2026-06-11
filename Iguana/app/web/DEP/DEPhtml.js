function DEPnameHtml(Name){
   return /*html*/`<a href="@${Name}" target="_blank">${Name}</a>`;
}

function DEPtooltipHtml(Data) {
   const container = document.createElement('div');
   container.innerHTML = /*html*/`
     <div>
       <div class="DEPheader">
         The function ${DEPnameHtml(Data.Name)} is deprecated. See docs for migration details.
       </div>
       <div class="DEPdescription">
       </div>
     </div>
   `;
   const Options = {
     modules: { toolbar: false },
     readOnly: true,
     theme: 'snow'
   };
   const editor = new Quill(container.querySelector('.DEPdescription'), Options);
   editor.root.innerHTML = Data.Description;
   return container.innerHTML;
 }
 