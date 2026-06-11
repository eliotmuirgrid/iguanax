let DBMimportWindow;

function DBMrender(FileName){
   if(document.querySelector('.DBMwindow')) return;
   let CardConfig = {
      title     : 'VDB Importer',
      content   : DBMhtml(),
      className : 'DBMwindow',
      width     : '95%',
      Zindex    : 805,
      callback  : function(){ 
         MODELremove('DBMmodel'); 
         MODELforcePoll('TBLmodel'); 
      }
   }
   DBMimportWindow = POPpopoutOpen(CardConfig);
   DBMmodelInit(FileName);
   DBMsetAllListeners();
}

function DBMhtml(){ 
   return /*html*/`
   <div class="DBMmodal">  
      <div class="DBMsection">
         <div class="DBMheader">1. IMPORT FROM:</div>
         <div class="DBMfromFileDiv">
            <div class="DBMfromFileSearchDiv">
               <input class="DBMfromFileSearch FORMinput" placeholder="Search filter">
            </div>
            <div class="DBMfromFileContent"></div>
            <div class="DBMfromFilePathDiv">
               <div class="DBMfromFilePathHeader">From:</div>
               <div class="DBMfromFilePath"></div>
               <div class="DBMfromFile" title="Import from existing VDB file">
                  <div  class="BUTTONcancel BUTTONstandard" title="Import from existing VDB file">...</div>
               </div>
               <div class="DBMfromDatabase" title="Import from database connection">
                  <div class="BUTTONcancel BUTTONstandard" title="Import from database connection">DB</div>
               </div>
            </div>
         </div>
      </div>
      <div class="DBMimportSection">
         <div class="DBMheader">2. IMPORT CONTENT:</div>
         <div class="DBMimportListContent"></div>
      </div>   
      <div class="DBMsection">
         <div class="DBMheader">3. IMPORT TO:</div>
         <div class="DBMtoFileDiv">
            <div class="DBMtoFileSearchDiv"><input class="DBMtoFileSearch FORMinput" placeholder="Search filter"></div>
            <div class="DBMtoFileContent"></div>
            <div class="DBMtoFilePathDiv">
               <div class="DBMtoFilePathHeader">To:</div>
               <div class="DBMtoFilePath"></div>
            </div>
         </div>
      </div>
      <div></div>
      <div></div>
      <div class="DBMimportBanner BUTTONgroup">
         <div class="DBMimportCancel BUTTONdefault BUTTONstandard">CLOSE</div>
         <div class="DBMimportButton BUTTONaction BUTTONstandard">IMPORT</div>
      </div>   
   </div>`;
}
