let HLPcurrentFile = '';

function HLPloadFunction(AbsoluteHelpPath) {
   COL_TRC(AbsoluteHelpPath);
   APIcall("component/help", {'component': TRANScomponentGuid(), 'path': AbsoluteHelpPath}, function(R) {
      if(!R.success) { return SNCKsnackbar('error', R["error"]); }
      HLPcurrentFile = AbsoluteHelpPath;
      HLPviewFunction(R.data);
   });
}

function HLPopenListWindow(Path){
   HLPloadFunction(HLPabsolutePath(Path));
   MODELforcePoll('TRANSmodel');
}

function HLPnameResolve(help) {
   let target = typeof help === 'string' ? help : help.Name || HLPcurrentFile;
   return target
      .replace(TRANSprojectRoot(), '')
      .replace(HLP_GLOBAL_DIR, '')
      .replaceAll("\\", "/")
      .replaceAll("/", '.')
      .replace(/\.help$/, "")
}

function HLPviewFunction(help){
   COL_TRC(help);
   let HelpContent = document.querySelector('.HLPcontainer')
   if(HelpContent) HelpContent.innerHTML = HLPviewContent(help)
   else { HLPopenWindow('HLPfunctionPage', HLPrenderHelpView(help), "Function Manual", "WNDdockRight") }
   HLPregisterViewListeners(help);
   HLPsetListeners2();
   HLPinitDescQuill (help.Description, true);
   HLPinitParamQuill(help.Parameters, '.HLPtableParamRow', true)
   HLPinitParamQuill(help.Parameters, '.HLPlistParamRow', true)
   let Right = document.querySelector('.HLPrightPane')
   if(Right) Right.classList.remove('HLPhidden')
}

function HLPeditFunction(help){
   const Right = document.querySelector('.HLPrightPane')
   if(Right){
      let HelpContent = Right.querySelector('.HLPcontainer')
      Right.removeChild(HelpContent)
      Right.appendChild(HLPrenderHelpEdit(help))
   } else {
      const Elem = document.createElement('div')
      Elem.appendChild(HLPrenderHelpEdit(help))
      HLPopenWindow('HLPfunctionPage', Elem.innerHTML, "Function Manual", "WNDdockRight")
   }
   HLPregisterEditListeners()
   HLPinitDescQuill(help.Description)
   HLPinitParamQuill(help.Parameters, '.HLPtableParamRow')
   HLPinitParamQuill(help.Parameters, '.HLPlistParamRow')
}

function HLPabsolutePath(Path) { return TRANSprojectRoot() + Path; }

function HLPopenWindow(WindowId, Content, Header, DockDirection){
   if(!WNDwindow(WindowId, Content, Header, DockDirection)) return;
   document.querySelector(`#${DockDirection}`).style.width = "750px";
   WNDupdateDockDimensions();
   HLPadjustHeaderPosition();
}

function HLPcloseWindow(WindowId = 'HLPfunctionPage') {
   if(WNDisWindow(WindowId)) { return WNDcloseWin(WindowId); }
   const dockedElement = document.getElementById(WindowId + "docked");
   if(dockedElement) { return WNDcloseTab(WindowId + "docked", dockedElement.closest('.WNDdock').id); }
}

function HLPisWindowOpen(){
   const WindowId = 'HLPfunctionPage'
   const Docked = document.querySelector(`#${WindowId}docked`)
   return !!(WNDisWindow(WindowId) || Docked);
}

function HLPupdateHelpButton(){
   const windowOpen = HLPisWindowOpen();
   document.querySelector('.HLPhelpButton')?.classList.toggle("on", windowOpen);
}

function HLPadjustHeaderPosition() {
   const HLPcontainer = document.querySelector('.HLPcontainer');
   if(HLPcontainer) {
      HLPcontainer.closest('.WNDcontent').style.paddingTop = "0 !important";
      HLPjumpToFunction(HLPfunctionName())
   }
}

function HLPinitDescQuill(Description, ReadOnly = false) {
   if(!document.querySelector('.HLPdescription')) return;
   let ToolbarOptions = [
      [{ 'header': [3, 4, false] }],
      ['bold', 'italic', 'underline', 'code'], // Added 'code' for inline snippets
      [
         { 'list': 'ordered' },
         { 'list': 'bullet' }
         // Removed indent buttons
      ],
      ['code-block'],
      ['link', 'image', 'video']
   ];
   if(ReadOnly) ToolbarOptions = false;
   let Options = {
      modules: { toolbar: ToolbarOptions },
      readOnly: ReadOnly,
      theme: 'snow',
      bounds: '.HLPcontainer'
   }
   new Quill('.HLPdescriptionContent', Options);
   const DescriptionElement = document.querySelector(".HLPdescriptionContent");
   DescriptionElement.querySelector('.ql-editor').innerHTML = Description;
}

function HLPparamQuillOptions(ReadOnly){
   let ToolbarOptions = ReadOnly ? false : ['bold', 'italic', 'underline']
   return {
      modules : {toolbar: ToolbarOptions},
      readOnly: ReadOnly,
      theme   : 'bubble',
      bounds  : '.HLPcontent'
   }
}

function HLPinitParamQuill(Parameters, ClassName, ReadOnly = false) {
   let ParameterElements = document.querySelectorAll(`${ClassName} .HLPparameterDesc`)
   for (let i = 0; i < ParameterElements.length; i++) {
      new Quill(ParameterElements[i], HLPparamQuillOptions(ReadOnly));
      if(Parameters) ParameterElements[i].querySelector('.ql-editor').innerHTML = Parameters[i].Description
   }
}
