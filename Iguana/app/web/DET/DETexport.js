let DETexportPopout;
let DETexportEditor;
let DETphiMaskPopout;
let DETanonymizeUndoStack = [];

const DET_PHI_MASK_TAG = "#phimask";
const DET_PHI_MASK_URL = "https://bitbucket.org/interfaceware/phimaskutility.git";
const DET_PHI_MASK_REQUIRED_TAGS = [DET_PHI_MASK_TAG, "#run", "#utility"];
const DET_PHI_MASK_FUNCTION = "maskForExport";

function DETcomponentHasTag(Component, Tag){
   let Needle = String(Tag || "").toLowerCase();
   let Tags = Array.isArray(Component?.tags) ? Component.tags : [];
   for(let i = 0; i < Tags.length; i++){
      if(String(Tags[i] || "").toLowerCase() === Needle) return true;
   }
   return false;
}

function DETfindTaggedComponentGuid(Components, Tag){
   let List = Array.isArray(Components) ? Components : [];
   for(let i = 0; i < List.length; i++){
      if(DETcomponentHasTag(List[i], Tag)) return List[i]?.guid || "";
   }
   return "";
}

function DETcomponentsFromListResponse(Response){
   let Data = Response?.data;
   if(Array.isArray(Data)) return Data;
   if(Array.isArray(Data?.component_data)) return Data.component_data;
   if(Array.isArray(Response?.component_data)) return Response.component_data;
   return [];
}

function DETguessPhiMaskDataType(Message){
   let Value = String(Message || "").trim();
   if(!Value) return "HL7";
   if(Value.slice(0, 3) === "MSH") return "HL7";
   if(Value.slice(0, 500).indexOf("ClinicalDocument") >= 0) return "CDA";
   if(Value.slice(0, 500).match(/^\s*\{/)) return "FHIR";
   return "HL7";
}

function DETphiMaskDataTypeSelectHtml(){
   let Message = DETrawData()?.content || "";
   let Selected = DETguessPhiMaskDataType(Message);
   let Types = ["HL7", "CDA", "FHIR"];
   let Options = Types.map(function(Type){
      let Attr = Type === Selected ? "selected" : "";
      return `<option value="${Type}" ${Attr}>${Type}</option>`;
   }).join("");
   return /*html*/`
      <div class="DETphiMaskTypeInline">
         <span class="DETphiMaskTypeInlineLabel">As</span>
         <select class="FORMselectBox DETphiMaskType" title="Anonymize message format">${Options}</select>
      </div>`;
}

function DETexportLineFeedSelect(){
   return document.querySelector(".DETexportLFDButtons .LFDlineFeedSelect");
}

function DETexportSelectedLineFeed(){
   return DETexportLineFeedSelect()?.value || LFDlf;
}

function DETexportConvertFeeds(Content){
   let lineFeed = DETexportSelectedLineFeed();
   let scr = LFDscrambledChar();
   let newContent = Content;
   newContent = newContent.replaceAll(LFDcrlf, scr);
   newContent = newContent.replaceAll(LFDcr, scr);
   newContent = newContent.replaceAll(LFDlf, scr);
   newContent = newContent.replaceAll(scr, lineFeed);
   return newContent;
}

function DETapplyLineFeedPattern(SourceContent, TargetContent){
   let Source = String(SourceContent || "");
   let Target = String(TargetContent || "");
   let Delimiters = Source.match(/\r\n|\r|\n/g) || [];
   if(Delimiters.length === 0) return Target;
   let Lines = Target.split(/\r\n|\r|\n/);
   if(Lines.length <= 1) return Target;
   let Out = Lines[0];
   for(let i = 1; i < Lines.length; i++){
      let Delim = Delimiters[Math.min(i - 1, Delimiters.length - 1)];
      Out += Delim + Lines[i];
   }
   return Out;
}

function DETsampleArgs(ComponentId){
   let Params = {};
   Params.component   = ComponentId;
   Params.path        = SMPLrootFolder();
   Params.development = true;
   return Params;
}

function DETdefaultExportSampleDataName(){
   let Params = DETsampleArgs(DETexportToComponent());
   APIcall('component/dir', Params, function(R){
      SMPLfileListArray = [];
      if (R.data.exists) SMPLpopulateFileList(R.data.content);
      let Name = SMPLgenerateUniqueName();
      let input = document.querySelector(".DETexportName");
      if(Name) input.value = Name;
      FORMfocus(input);
   });
}

function DETexportButtonHtml(){
   let Perm = DETpermissionData()?.export_sample_data;
   let guid = DETexportToComponent();
   let Selected = DETpermissionAll()[guid]?.edit_script
   let Classes = Perm && Selected? "DETexportConfirm BUTTONaction" : "BUTTONdisabled";
   let title   = Perm && Selected ? "" : `title="You do not have permission to do this."`;
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>Export</div>`;
}

function DETanonymizeButtonHtml(){
   let Perm = DETpermissionData()?.export_sample_data;
   let guid = DETexportToComponent();
   let Selected = DETpermissionAll()[guid]?.edit_script;
   let Classes = Perm && Selected ? "DETanonymize BUTTONdefault" : "BUTTONdisabled";
   let title = Perm && Selected ? "title=\"Run PHI Mask utility\"" : `title="You do not have permission to do this."`;
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>Anonymize</div>`;
}

function DETundoAnonymizeButtonHtml(){
   let Perm = DETpermissionData()?.export_sample_data;
   let guid = DETexportToComponent();
   let Selected = DETpermissionAll()[guid]?.edit_script;
   let Enabled = Perm && Selected && DETanonymizeUndoStack.length > 0;
   let Classes = Enabled ? "DETundoAnonymize BUTTONdefault" : "DETundoAnonymize BUTTONdisabled";
   let title = !Perm || !Selected
      ? `title="You do not have permission to do this."`
      : (Enabled ? "title=\"Restore message before anonymize\"" : "title=\"Nothing to undo yet.\"");
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>Undo</div>`;
}

function DETcomponentListHtml(List, IncludeServer, NoSelected = false){
   let SelectedGuid = DETcomponentId();
   let H = IncludeServer ? `<option value="" ${!SelectedGuid ? "selected" : ""}>Iguana Service</option>` : "";
   for (let i=0; i < List.length; i++){
      let Option = List[i];
      let Guid = Option.guid;
      let Perm = DETpermissionAll()[Guid]?.edit_script ? "" : "disabled";
      let Selected = !NoSelected && (Guid == SelectedGuid) ? "selected" : "";
      H += `<option ${Selected} ${Perm} value="${Guid}">${Option.name}</option>`;
   }
   return H;
}

function DETexportHtml(){
   return/*html*/`
   <div class="DETexportModal">
      <div class="DETexportComponentCard">
         <div class='FORMlabel DETexportLabel'>Destination component</div>
         <select class="DETtoComponent FORMselectBox">
            ${DETcomponentListHtml(DETcomponentListData(), false)}
         </select>
      </div>
      <div class="DETexportNameCard DETexportNameCardTight">
         <div class='FORMlabel DETexportLabel'>Sample data file name</div>
         <input class="FORMinput DETexportName"/>
      </div>
      <div class="DETexportEditorCard">
         <div class="DETexportEditorDiv">
         <div class="DETexportEditorRow">
            <div class="FORMlabel DETexportLabel">Message content</div>
            <div class="DETexportEditorControls">
               <div class="DETexportControlGroup DETexportControlGroupMask">
                  <div class="DETanonymizeButtonTarget"></div>
                  <div class="DETundoAnonymizeButtonTarget"></div>
                  ${DETphiMaskDataTypeSelectHtml()}
               </div>
               <div class="DETexportControlGroup DETexportControlGroupLfd">
                  <div class="DETexportLFDButtons">${LFDselectHtml()}</div>
               </div>
            </div>
         </div>
         <div class="DETlogEditor LFDsampleData DETexportMessageEditor">
            <textarea class="DETexportMessage FORMinput" placeholder="Message"></textarea>
         </div>
         </div>
      </div>
      <div class="DETerrorFeedback"></div>
      <div class="DETbuttons BUTTONgroup">
         <div class="DETexportCancel BUTTONstandard BUTTONdefault">Cancel</div>
         <div class="DETexportButtonTarget"></div>
      </div>
   </div>`;
}

function DETexportScreen(){
   DETexportPopout = POPpopoutOpen({
      title : "Export Sample Data",
      content : DETexportHtml(),
      className : "DETmodalExportEditable",
      width : "75%",
      height : "82%",
      minHeight : "460px"
   });
   let div = document.querySelector(".DETexportModal");
   if(!div) return;
   div.addEventListener("click", DETexportClickListeners);
   div.addEventListener("change", DETexportChangeListeners);
   let input = document.querySelector(".DETexportName");
   FORMenterListener(input, DETexportExecutePreCall);
   DETanonymizeUndoStack = [];
   DETexportEditorInit();
   DETupdateExportButton();
   DETdefaultExportSampleDataName();
}

function DETexportEditorInit(){
   let TextArea = document.querySelector(".DETexportMessage");
   if(!TextArea) return;
   let EditorConfig = {
      lineNumbers    : true,
      lineWrapping   : true,
      autofocus      : true,
      indentUnit     : 3,
      specialChars   : LFDspecialChars(),
      specialCharPlaceholder: LFDspecialCharPlaceholder,
      tabSize        : 3,
      viewportMargin : 20
   };
   DETexportEditor = CodeMirror.fromTextArea(TextArea, EditorConfig);
   let Data = DETrawData()?.content || "  ";
   let Decoded = LFDloadDecoded(Data);
   DETexportEditor.setValue(Decoded);
   DETexportEditor.setOption("mode", SMPLautoDetectFileType(Data));
   DETexportEditor.off("keydown", SMPLonKeyDown);
   DETexportEditor.on ("keydown", SMPLonKeyDown);
   DETexportEditor.off("keypress", SMPLonKeyDown);
   DETexportEditor.on ("keypress", SMPLonKeyDown);
   DETexportEditor.off("cursorActivity", SMPLonCursorAct);
   let div = document.querySelector(".DETexportLFDButtons");
   div?.removeEventListener("click", DETexportLFDclick);
   div?.addEventListener("click", DETexportLFDclick);
   div?.removeEventListener("change", DETexportLFDchange);
   div?.addEventListener("change", DETexportLFDchange);
   let Select = DETexportLineFeedSelect();
   let DefaultLF = LFDlf;
   if(Select) Select.value = DefaultLF;
   DETexportLFDconvertClick(true);
}

function DETexportLFDclick(e){
   if(e.target.closest(".LFDshowHide")) return DETexportLFDshowHideClick();
   if(e.target.closest(".LFDconvert"))  return DETexportLFDconvertClick(true);
}

function DETexportLFDchange(e){
   if(e.target.closest(".LFDlineFeedSelect")) {
      return DETexportLFDconvertClick(false);
   }
}

function DETexportLFDshowHideClick(){
   let div = document.querySelector(".DETexportMessageEditor");
   if(!div) return;
   div.classList.toggle("LFDhide");
}

function DETexportLFDconvertClick(defaultConvert){
   if(!DETexportEditor) return;
   const doc = DETexportEditor.getDoc();
   const selectionStartIdx = doc.indexFromPos(doc.getCursor('start'));
   const selectionEndIdx   = doc.indexFromPos(doc.getCursor('end'));
   const selectedText = doc.getSelection();
   if (selectedText && selectedText.length > 0) {
      const newSelectedText = DETexportConvertFeeds(selectedText);
      doc.replaceRange(
         newSelectedText,
         doc.posFromIndex(selectionStartIdx),
         doc.posFromIndex(selectionEndIdx)
      );
      doc.setSelection(
         doc.posFromIndex(selectionStartIdx),
         doc.posFromIndex(selectionStartIdx + newSelectedText.length)
      );
   } else if (defaultConvert) {
      const content = doc.getValue();
      const newContent = DETexportConvertFeeds(content);
      doc.setValue(newContent);
      doc.setCursor(doc.posFromIndex(selectionStartIdx));
   } else {
      doc.setCursor(doc.posFromIndex(selectionStartIdx));
   }
}

function DETexportClickListeners(e){
   if(e.target.closest(".DETexportCancel"))  return POPpopoutClose(DETexportPopout);
   if(e.target.closest(".DETexportConfirm")) return DETexportExecutePreCall();
   if(e.target.closest(".DETanonymize"))     return DETanonymizeMessage();
   if(e.target.closest(".DETundoAnonymize")) return DETundoAnonymizeMessage();
}

function DETexportChangeListeners(e){
   if(e.target.closest(".DETtoComponent"))  {
      DETdefaultExportSampleDataName();
      DETupdateExportButton();
   }
}

function DETupdateExportButton(){
   let Html = DETexportButtonHtml();
   let div = document.querySelector(".DETexportButtonTarget");
   if(!div) return;
   if(div.innerHTML != Html) div.innerHTML = Html;
   DETupdateAnonymizeButton();
}

function DETupdateAnonymizeButton(){
   let Html = DETanonymizeButtonHtml();
   let div = document.querySelector(".DETanonymizeButtonTarget");
   if(!div) return;
   if(div.innerHTML != Html) div.innerHTML = Html;
   DETupdateUndoAnonymizeButton();
}

function DETupdateUndoAnonymizeButton(){
   let Html = DETundoAnonymizeButtonHtml();
   let div = document.querySelector(".DETundoAnonymizeButtonTarget");
   if(!div) return;
   if(div.innerHTML != Html) div.innerHTML = Html;
}

function DETexportToComponent(){
   return document.querySelector('.DETtoComponent').value;
}

function DETexportExecutePreCall(){
   let DestId = DETexportToComponent();
   APIcall("component/detail", {component : DestId}, function(R){
      if(!R.success) return SNCKsnackbar('error', R.error);
      let Data = R?.data || {};
      if(!Data.template) return DETexportExecute();
      else DETmakeEditablePop(DestId)
   });
}

function DETmakeEditableArgs(ComponentId, GitUrl){
   let Args = {};
   Args.component = ComponentId;
   Args.url       = GitUrl;
   Args.callback  = DETexportExecute;
   Args.message   = "To add sample data to this component, you must make it editable.";
   return Args;
}

function DETmakeEditablePop(ComponentId){
   APIcall("component/get_repository", {component : ComponentId, development : true}, function(R){
      if(!R.success) return SNCKsnackbar('error', R.error);
      let Data = R?.data || {};
      let Repo = Data?.repo || "";
      MKEmakeEditablePopout(DETmakeEditableArgs(ComponentId, Repo));
   });
}

function DETreportError(Error){
   let div = document.querySelector(".DETerrorFeedback");
   if(!div) return;
   div.innerHTML = Error;
   FORMfocus(document.querySelector(".DETexportName"));
}

function DETexportExecute(){
   let DestId = DETexportToComponent();
   let Name   = document.querySelector('.DETexportName').value.trim();
   let Message = DETexportEditor ? LFDsaveEncoded(DETexportEditor.getValue()) : (DETrawData()?.content || "");
   let Vars={
      to        : DestId,
      component : PAGEhashParameter('component'),
      id        : PAGEhashParameter('id'),
      time      : PAGEhashParameter('time'),
      name      : Name,
      message   : Message
   };
   APIcall("log/export/sample", Vars, function(R){
      if (!R.success) return DETreportError(R.error);
      DETgetNewIndex(R.data, DestId);
      POPpopoutClose(DETexportPopout);
   });
}

function DETgetNewIndex(Value, ComponentId){
   let Params = DETsampleArgs(ComponentId);
   let ReplaceValue = FILsep() == "\\" ? "/" : "\\";
   let OsFileName = Value.replace(ReplaceValue, FILsep());
   APIcall('component/dir', Params, function(R){
      let Data = R.data.content;
      SMPLfileListArray = [];
      if (R.data.exists) SMPLpopulateFileList(Data);
      let Index = SMPLgetSampleIndex(SMPLrootFolder() + Value, Data);
      let Args = {};
      Args.component = ComponentId;
      Args.development = true;
      if(Index !== undefined) Args.sample = Index;
      Args.edit = SMPLrootFolder() + OsFileName;
      let Url = "#translator" + PAGEformatParams(Args);
      window.location.hash = Url;
   });
}

function DETanonymizeMessage(){
   let Message = DETexportEditor ? LFDsaveEncoded(DETexportEditor.getValue()) : (DETrawData()?.content || "");
   let DataType = document.querySelector(".DETphiMaskType")?.value || "HL7";
   let LookupParams = {
      filter: DET_PHI_MASK_TAG,
      whole_word: "true",
      ascending: 1,
      order: "",
      checked_only: false
   };
   APIcall("component/list", LookupParams, function(ListResult){
      if(!ListResult?.success) return SNCKsnackbar("error", ListResult?.error || "Could not load component list.");
      let Components = DETcomponentsFromListResponse(ListResult);
      let Guid = DETfindTaggedComponentGuid(Components, DET_PHI_MASK_TAG);
      if(!Guid) return DETopenPhiMaskPopout();
      DETanonymizeRun(Guid, Message, DataType);
   });
}

function DETanonymizeRun(ComponentGuid, Message, DataType){
   let StructuredData = {
      content: Message,
      format: DataType
   };
   let Params = {
      component: ComponentGuid,
      function: DET_PHI_MASK_FUNCTION,
      data: StructuredData,
      start_if_stopped: true
   };
   APIcall("component/function/call", Params, function(R){
      if(!R?.success) return SNCKsnackbar("error", R?.error || "Could not run PHI Mask utility.");
      DETanonymizeApplyResult(R, DataType);
   });
}

function DETanonymizeApplyResult(Response, DataType){
   let Masked = Response?.data?.[0];
   if(Masked === undefined) Masked = Response?.data?.result;
   if(Masked === undefined) Masked = Response?.data?.resut;
   if(Masked === undefined) Masked = Response?.result;
   if(Masked === undefined) Masked = Response?.data?.returns?.[0];
   if(Masked !== undefined && Masked !== null && DETexportEditor) {
      let Value = typeof Masked === "string" ? Masked : String(Masked);
      let Previous = LFDsaveEncoded(DETexportEditor.getValue());
      let PatternPreserved = DETapplyLineFeedPattern(Previous, Value);
      DETexportEditor.setValue(LFDloadDecoded(PatternPreserved));
      DETexportEditor.setOption("mode", SMPLautoDetectFileType(PatternPreserved));
      DETanonymizeUndoStack.push(Previous);
      DETupdateUndoAnonymizeButton();
      SNCKsnackbar("success", `Anonymized as ${DataType}.`, 7000);
      return;
   }
   SNCKsnackbar("success", "PHI Mask utility ran, but did not return data. Verify the utility function returns the masked message.", 9000);
}

function DETundoAnonymizeMessage(){
   if(DETanonymizeUndoStack.length === 0) return;
   if(!DETexportEditor) return;
   let Value = DETanonymizeUndoStack.pop();
   DETexportEditor.setValue(LFDloadDecoded(Value));
   DETexportEditor.setOption("mode", SMPLautoDetectFileType(Value));
   DETupdateUndoAnonymizeButton();
   SNCKsnackbar("success", "Undid last anonymize step.", 5000);
}

function DETphiMaskHelpHtml(){
   return /*html*/`
   <div class="DETphiMaskModal">
      <div class="DETphiMaskHelp">
         Anonymize requires a utility component tagged <code>${DET_PHI_MASK_TAG}</code>. Add the PHI Mask component now?
      </div>
      <div class="DETphiMaskUrlRow">
         <div class="DETphiMaskUrlLabel">Source</div>
         <div class="DETphiMaskUrlValue">${DET_PHI_MASK_URL}</div>
      </div>
      <div class="BUTTONgroup">
         <div class="BUTTONstandard BUTTONdefault DETphiMaskCancel">Cancel</div>
         <div class="BUTTONstandard BUTTONaction DETphiMaskAdd">Add Utility</div>
      </div>
   </div>`;
}

function DETclearPhiMaskPopout(){
   DETphiMaskPopout = null;
}

function DETopenPhiMaskPopout(){
   if(document.querySelector(".DETphiMaskModal")) return;
   DETphiMaskPopout = POPpopoutOpen({
      title: "Add PHI Mask Utility",
      content: DETphiMaskHelpHtml(),
      maxWidth: "620px",
      callback: DETclearPhiMaskPopout
   });
   let Card = DETphiMaskPopout?.element;
   if(!Card) return;
   Card.removeEventListener("click", DETphiMaskClickListeners);
   Card.addEventListener("click", DETphiMaskClickListeners);
}

function DETphiMaskClickListeners(e){
   if(e.target.closest(".DETphiMaskCancel")) return DETclosePhiMaskPopout();
   if(e.target.closest(".DETphiMaskAdd")) return DETcreatePhiMaskUtility();
}

function DETclosePhiMaskPopout(){
   let Popout = DETphiMaskPopout;
   if(!Popout) return;
   DETphiMaskPopout = null;
   POPpopoutClose(Popout);
}

function DETnextPhiMaskName(){
   let Base = "PHI Mask";
   let List = DETcomponentListData() || [];
   let Exists = {};
   for(let i = 0; i < List.length; i++){
      let Name = (List[i]?.name || "").trim();
      if(Name) Exists[Name] = true;
   }
   if(!Exists[Base]) return Base;
   let Index = 1;
   while(Exists[`${Base} (${Index})`] && Index < 50) { Index++; }
   return `${Base} (${Index})`;
}

function DETcreatePhiMaskUtility(){
   let Button = document.querySelector(".DETphiMaskAdd");
   if(!Button) return;
   if(Button.classList.contains("BUTTONdisabled")) return;
   Button.classList.add("BUTTONdisabled");
    Button.textContent = "Adding...";
   let CreateData = {
      name: DETnextPhiMaskName(),
      branch: "",
      description: "Masks input content using a provided format and returns masked content.",
      git: DET_PHI_MASK_URL,
      tags: DET_PHI_MASK_REQUIRED_TAGS.slice(),
      editable: false,
      template: true
   };
   APIcall("component/create", CreateData, function(R){
      if(!R?.success && !R?.data?.success) {
         Button.classList.remove("BUTTONdisabled");
         Button.textContent = "Add Utility";
         return SNCKsnackbar("error", R?.error || R?.data?.error || "Could not add PHI Mask utility.");
      }
      let Guid = R?.data?.components?.[0]?.guid || "";
      let finish = function(){
         DETclosePhiMaskPopout();
         MODELforcePoll("DASHmodel");
         if(Guid) {
            let Message = GINaddFromUrl(CreateData.name, CreateData.git);
            GINaddAndCommit({message: Message, component: Guid, all: true});
         }
         SNCKsnackbar("success", "PHI Mask utility added with #phimask tag. Run Anonymize again in a moment.", 9000);
      };
      if(!Guid) return finish();
      APIcall("component/tags/set", {component: Guid, value: DET_PHI_MASK_REQUIRED_TAGS.slice()}, function(TagResult){
         if(!TagResult?.success) {
            SNCKsnackbar("warning", "Utility created, but could not force #phimask tag. Please verify tags.", 10000);
         }
         finish();
      });
   });
}
