let VMDimportWindow;

function IMPrender(FileName){
   if(document.querySelector('.IMPwindow')) return;
   let CardConfig = {
      title     : 'VMD Importer',
      content   : IMPhtml(),
      className : 'IMPwindow',
      width     : '95%',
      Zindex    : 805,
      callback  : function(){ 
         MODELremove('IMPmodel'); 
         MODELforcePoll('EDImodel');
      }
   }
   VMDimportWindow = POPpopoutOpen(CardConfig);
   IMPmodelInit(FileName);
   IMPsetAllListeners();
}

function IMPhtml(){ 
   return /*html*/`
   <div class="IMPmodal">  
      <div class="IMPsection">
      <div class="IMPheader">1. IMPORT FROM:</div>
         <div class="IMPfromFileDiv">
            <div class="IMPfromFileSearchDiv">
               <input class="IMPfromFileSearch FORMinput" placeholder="Search filter">
            </div>
            <div class="IMPfromFileContent"></div>
            <div class="IMPfromFilePathDiv">
               <div class="IMPfromFilePathHeader">From:</div>
               <div class="IMPfromFilePath"></div>
               <div class="IMPfromFile" title="Import from existing VMD file">
                  <div  class="BUTTONcancel BUTTONstandard" title="Import from existing VMD file">...</div>
               </div>
               <div class="IMPfromLibrary" title="Import from HL7 library">
                  <div class="BUTTONcancel BUTTONstandard IMPhl7Select" title="Import from HL7 library">HL7</div>
                  <div class="BUTTONcancel BUTTONstandard IMPx12Select" title="Import from X12 library">X12</div>
               </div>
            </div>
         </div>
      </div>
      <div class="IMPimportSection">
         <div class="IMPheader">2. IMPORT CONTENT:</div>
         <div class="IMPimportListContent"></div>
      </div>   
      <div class="IMPsection">
         <div class="IMPheader">3. IMPORT TO:</div>
         <div class="IMPtoFileDiv">
            <div class="IMPtoFileSearchDiv"><input class="IMPtoFileSearch FORMinput" placeholder="Search filter"></div>
            <div class="IMPtoFileContent"></div>
            <div class="IMPtoFilePathDiv">
               <div class="IMPtoFilePathHeader">To:</div>
               <div class="IMPtoFilePath"></div>
               <div class="IMPtoFile" title="Export to a different file">
                  <div class="BUTTONcancel BUTTONstandard" title="Export to a different file">...</div>
               </div>
            </div>
         </div>
      </div>
      <div></div>
      <div></div>
      <div class="IMPimportBanner BUTTONgroup">
         <div class="IMPimportCancel BUTTONdefault BUTTONstandard">CLOSE</div>
         <div class="IMPimportButton BUTTONaction BUTTONstandard">IMPORT</div>
      </div>   
   </div>`;
}

function IMPvmdFiles(){
   let Files = [];
   if(IMPtoFile   && IMPtoFile.trim()   !== "") Files.push(IMPtoFile);
   if(IMPfromFile && IMPfromFile.trim() !== "") Files.push(IMPfromFile);
   return Files;
}

function IMPgetLib(LibName) {
   let IMPlibrary = {
      "HL7 2.7"   : HL7_2_7,
      "HL7 2.6"   : HL7_2_6,
      "HL7 2.5.1" : HL7_2_5_1,
      "HL7 2.5"   : HL7_2_5,
      "HL7 2.4"   : HL7_2_4,
      "HL7 2.3.1" : HL7_2_3_1,
      "HL7 2.3"   : HL7_2_3,
      "HL7 2.2"   : HL7_2_2,
      "HL7 2.1"   : HL7_2_1,
      "X12 5010"  : X12_5010,
      "X12 4010"  : X12_4010,
   };
   return IMPlibrary[LibName];
}

function IMPupdateGlobals(){
   let LibraryName = IMPfromLibrary;
   let FileData = IMPfileData();
   if( FileData === undefined ) IMPtoFile   = "";
   if( FileData === undefined ) IMPfromFile = "";
   if( IMPtoFile   != "" && FileData[IMPtoFile]   === undefined ) IMPtoFile = "";
   if( IMPfromFile != "" && FileData[IMPfromFile] === undefined ) IMPfromFile = "";
   if(IMPtoFile && IMPtoFile !== "")     IMPtoFileData = FileData[IMPtoFile];
   else IMPtoFileData = {};
   if(IMPfromFile && IMPfromFile !== "") IMPfromFileData = FileData[IMPfromFile];
   else if(LibraryName !== "") IMPfromFileData = IMPgetLib(LibraryName);
   else IMPfromFileData = {};
}