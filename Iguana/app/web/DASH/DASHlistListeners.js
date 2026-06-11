let DASHrowHoverTimer = null;

function DASHsetComponentListListeners(){
   DASHbindCytoBridgeListeners();
   let table = document.querySelector('.DASHdashboardGrid');
   table.addEventListener('dblclick',   function(e) { DASHcomponentListDoubleClicked(e);} );
   table = document.querySelector('.DASHtableContainer');
   table.addEventListener('click',      function(e) { DASHcomponentListClicked(e);} );
   table.addEventListener('mouseover',  function(e) { DASHcomponentListMouseover(e) });
   table.addEventListener('mouseleave', function(e) { DASHcomponentListMouseOut(e);} );
}

function DASHgetEventDetails(e){
   e.stopPropagation();
   let Details = {};
   Details.target = e.target;
   Details.classList = Details.target.classList;
   Details.componentRow = Details.target.closest('.DASHcomponentRow');
   Details.guid = DASHrowGuid( DASHrowIndex(Details.componentRow) );
   Details.targetClass = function(ClassName){ return Details.classList.contains(ClassName); }
   Details.parentClass = function(ClassName){ return Details.target.closest(ClassName) !== null; }
   return Details;
}

function DASHrowIndex(targetRow){ return targetRow ? Array.from(targetRow.parentElement.children).indexOf(targetRow) : -1; }

function DASHrowGuid(rowIdx){ return rowIdx > 0 ? DASHrenderedRows[rowIdx] : ''; }

function DASHcomponentListClicked(e){
   let T = DASHgetEventDetails(e);
   if( e.target.closest('.DASHselectAllRow') ) return DASHselectAll();
   if( e.target.closest('.DASHselectRow') )    return DASHupdateCheckedComponents([], {emitToCyto: true});
   if( e.target.closest('.DASHtableHeader') )  return DASHheaderClick(T.target);
   if( e.target.closest('.DASHhashtag') )      return DASHselectHashtag(T.target);
   if( e.target.closest('.DASHcomponentSwitch .TOGbutton') ) return DASHcomponentSwitch( e.target.closest('.DASHcomponentSwitch'), T.guid);
   if( e.target.closest('.DASHcomponentRow') )    {
      if(e.target.closest(".DASHcomponentName")){ 
         e.preventDefault();
         return DASHopenComponent(T.guid); 
      }
      return DASHrowSelected( T.guid );
   }
}

function DASHrowSelected(guid) {
   DASHclearSelectedRows();
   DASHbridgeNavigateEmit(guid);
}

function DASHcomponentListDoubleClicked(e){
   let T = DASHgetEventDetails(e);
   if( e.target.closest('.TOGbutton') || e.target.closest('input')) return;
   if( e.target.closest('.DASHcomponentRow') ) return DASHopenComponent(T.guid);
}

function DASHcomponentListMouseover(e) {
   e.stopPropagation();
   if(e.target.closest('.DASHcomponentRow') )         { DASHrowHover(e.target); } // TODO
   if(e.target.classList.contains('DASHserverLabel')) { POPtooltipRemoveByClass('LGEtooltipWrap'); }
   if(e.target.classList.contains('DASHqueCount'))    { DASHqueueCountTooltip(e.target); }
}

function DASHrowHoverPopups(target, guid){
   if (target.closest('.DASHcomponentName'))  return DASHrowHoverDisplayEdit(target, guid);
   if (target.closest('.DASHlatestActivity')) return DASHrowHoverDisplayLog(target, guid);
   if (target.closest('.DASHerrorCount'))     return DASHrowHoverDisplayError(target, guid);
}

function DASHrowHover(target){
   let guid = DASHrowGuid( DASHrowIndex(target.closest('.DASHcomponentRow')) );
   DASHcomponentHover(guid);
   clearTimeout(DASHrowHoverTimer);
   DASHrowHoverPopups(target, guid);
}

function DASHcomponentHover(guid) {
   DASHcomponentRemoveHover();
   if(!guid) { return DASHbridgeHoverClearEmit(); }
   let ComponentRow = DASHrowSelector(guid);
   if(ComponentRow) { ComponentRow.classList.add('DASHrowHighlighted'); }
   DASHbridgeHoverEmit(guid);
}

function DASHcomponentRemoveHover() {
   document.querySelectorAll('.DASHcomponentRow.DASHrowHighlighted')?.forEach(row => row.classList.remove('DASHrowHighlighted'));
   DASHbridgeHoverClearEmit();
}

function DASHhighlightKeywords(description) {
   description = ESChtmlEscape(description)
   let queryText = ESChtmlEscape(DASHsearchText().trim());
   if(queryText) {
      let pattern;
      try {
         let keywords = queryText.split(/\s/).filter(word => {return word.trim().length;});
         pattern = new RegExp(`(${keywords.join('|')})`, 'gi');
      } catch(e) {pattern = queryText;}
      description = description.replace(pattern, match => `<span class="DASHfilter" style="background-color: yellow">${match}</span>`);
   }
   return description;
}

function DASHcomponentListMouseOut(e) {
   COL_TRC(e);
   e.stopPropagation();
   clearTimeout(DASHrowHoverTimer);
   DASHcomponentRemoveHover();
}

function DASHenableCheckOnlyRows(){
   if(document.querySelector('.DASHonlyChecked').checked == false) document.querySelector('.DASHonlyChecked').checked = true;
}

function DASHselectHashtag(target) {
   let div = document.querySelector('.EDITfilter');
   let hashtag = target.getAttribute('word');
   if(hashtag == null) return;
   let Search = div.value.includes(hashtag) ? "" : hashtag;
   PAGEsetHashParameter("filter", Search);
}

function DASHheaderClick(Target) {
   let Classes = Target.classList;
   if (Classes.contains('DASHcheckbox')) {return; }
   let Header = Target.innerHTML;
   let Args = PAGEhashParameters();
   if(Args.sort != Header){
      Args.order = "";
      Args.sort  = Header;
   }
   else if(Args.order == "Dsc"){
      Args.order = "";
      Args.sort  = "";
   }
   else if(Args.sort){
      Args.order = "Dsc";
   }
   else{
      Args.sort = Header;
   }
   let Url = "#dashboard" + PAGEformatParams(Args)
   window.location.hash = Url;
}

function DASHcomponentSwitch(Target, ComponentId) {
   let Command = TOGswitchApi(Target);
   let Arg     = TOGswitchArg(Target, [ComponentId] );
   let Direction = Target.closest(".TOGgrey") || Target.closest(".TOGred") ;
   if(DASHisComponentRunning(ComponentId)=== Direction) return;
   APIcall( Command, Arg, function(Response){
      if (!LICcheckComponentStartError(Command, Response, function(){ MODELforcePoll('DASHmodel'); })){ return; }
      if (0 != Response.data.errors.length) SNCKsnackbar('error', Response.data.errors[0], 3000);
      MODELforcePoll('DASHmodel');
   });
}

function DASHviewLogPermissionCheck(Guid){
   let Perm = DASHuserPermissionAll()[Guid]?.view_logs;
   if(!Perm) return false;
   return true;
}

function DASHrenderLogTooltip(guid, div, message, type = POPtooltipType.INFO, position=POPtooltipPosition.RIGHT) {
   POPtooltipRemoveByClass('LGEtooltipWrap');
   if(parseInt(div.innerText) === 0) {
      return;
   }
   if(type === POPtooltipType.INFO) {
      POPtooltip({
         target: div, 
         content: message, 
         type: type, 
         style: 'LGEtooltipWrap', 
         flowDirection: POPtooltipFlowDirection.DOWN,
         position: position
      });
   } else {
      POPtooltip({
         target  : div,
         content : message,
         persist: true,
         type    : type,
         style   : 'LGEtooltipWrap',
         position: position,
         flowDirection: POPtooltipFlowDirection.DOWN,
         callback: () => {
            STOREcreateErrorJumpStore();
            DASHclearErrorTooltip(guid);
         }
      });
   }
}

function DASHrowHoverDisplayLog(div, guid){
   if(!div) return;
   if(!DASHviewLogPermissionCheck(guid)) return DASHrenderLogTooltip(guid, div, "You do not have permission to view this.");
   DASHgetLastLogs(guid, div);
}

function DASHrowHoverDisplayEdit(div, guid){
   if(!div) return;
   let Edit = !DASHlistData().find(d => d.guid === guid)?.template;
   let Perm = DASHuserPermissionAll()[guid]?.edit_script;
   if(!Perm || !Edit) return;
   let Url = TRANSformatUrl(guid, true);
   const tooltiphtml = DASHrenderedComponentData[guid]?.tooltip;
   let Html = `${Perm ? `<div><a class="DASHeditScript" href="${Url}">Edit</a></div>` : ''}${tooltiphtml}`
   POPtooltipRemoveByClass('LGEtooltipWrap');
   POPtooltip({
      target  : div,
      content : Html,
      type    : POPtooltipType.INFO,
      style   : 'LGEtooltipWrap',
      position: POPtooltipPosition.RIGHT,
      flowDirection: tooltiphtml ? POPtooltipFlowDirection.DOWN : POPtooltipFlowDirection.CENTER,
      callback: function() {
         const tooltip = document.querySelector('.LGEtooltipWrap');
         if(!tooltip) return;
         let Width = div.offsetWidth;
         let parentWidth = div.closest(".DASHnameHeader")?.offsetWidth || 0;
         let delta = parentWidth - Width;
         if(delta > 0) delta = 0;
         delta = delta - 8;
         tooltip.style.transform = `translateX(${delta}px)`;
      }
   });
}

function DASHrowHoverDisplayError(div, guid) {
	if (div) DASHgetLastErrors(guid, div);
}

function DASHgetLastErrors(guid, div){
   APIcall('logs/retrieve', LGEargs({guid : guid, error : true}), function(R) {
      let Data = R?.data || [];
      const dev = !DASHrenderedComponentData[guid]?.commit;
      let Html = "";
      if(!R.success)             Html = "Failed to get last error log. Details: " + R.error;
      else if(Data.length === 0) Html = "No errors to display.";
      else                       Html = LGEformmattedContent(Data, guid, dev, true);
      DASHrenderLogTooltip(guid, div, Html, Data.length === 0 ? POPtooltipType.INFO : POPtooltipType.ERROR);
   });
}

function DASHgetLastLogs(guid, div){
   APIcall('logs/retrieve', LGEargs({guid : guid}), function(R) {
      let Data = R?.data || [];
      let Html = "";
      if(!R.success)            Html = "Failed to get last activity log. Details: " + R.error;
      else if(Data.length == 0) Html = "No logs to display. Run this component to see logs";
      else                      Html = LGEgetPopoverContent(Data, "CARDlastActivityPopoverEntry");
      DASHrenderLogTooltip(guid,div, Html, POPtooltipType.INFO, POPtooltipPosition.RIGHT);
   });
}

function DASHclearErrorTooltip(guid) {
   const clearLink = document.querySelector('.LGEclearErrors');
   if(clearLink) {
      const clearErrorFunc = () => {
         POPtooltipRemoveByClass('LGEtooltipWrap');
         CARDclearErrorModal([guid]);
      };
      clearLink.removeEventListener('click', clearErrorFunc);
      clearLink.addEventListener('click', clearErrorFunc);
   }
}
