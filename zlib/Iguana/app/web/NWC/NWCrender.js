let NWCcard;
let NWCcomponentList;

function NWCnewComponentScreen(){
	DASHdashboardScreen();
	if(document.querySelector('.NWCmodal')) return;
   NWCcard = POPpopoutOpen({
      title: 'Add components', 
      width: '95%', 
		height: '95%',
      minWidth: '600px', 
      content: NWChtml(),  
		className: 'NWCcomponent',
      esc_close : true,
      callback : DASHcloseModal
   });
   document.querySelector('.NWCcomponent .POPpopoutCard-content').classList.add('NWCcatalogueModalContainer');
   NWCmodelInit();
	// NWCslowModelInit();
	DASHtriggerRefresh();
	NWCrenderComponentList();
	NWCupdateSelectedOptions();
	FORMfocusDelay(document.querySelector(".NWCfilter"));
}

function NWCupdateButtonAdd(){
   let Button = document.querySelector('.NWCadd');
   if(!Button) return;
	let Components = document.querySelectorAll('.NWCcomponentDiv');
	if( NWCrepoError || NWCnameError || !Components.length) {
		Button.classList.add('BUTTONdisabled');
		Button.classList.remove('BUTTONaction');
	}
   else {
		Button.classList.add('BUTTONaction');
		Button.classList.remove('BUTTONdisabled');
	}
}

function NWCgitError(){
	return /*html*/`	
	<div class="NWCgitInformation NWChidden">
		<div class="NWCgitFeedback"></div>
		<br>
		<div class="BUTTONgroup NWCbuttons">
			<div class="NWCcancel BUTTONstandard BUTTONaction">DISMISS</div>
		</div>
	</div>`;
}

function NWChtmlTemplates(){
	return /*html*/`			
	<div class="NWCtemplateDiv">
		<div class="NWCheaderTitle">COMPONENTS</div>
		<div class="NWCsourceComponentDiv"></div>
		<div class="NWCheaderDiv">	
			<input class="NWCfilter FORMinput" placeholder="Search">
		</div>
		<div class="NWCcardContainer">
			<div class="NWCcardLoading">Syncing components <a target="_blank" href="#settings/git">from bitbucket...</a></div>
			<div class='NWCnewCardErrorDiv'></div>
			<div class="NWCcardContent-hide"></div>
		</div>
	</div>`;
}

function NWCcomponentListHtml(){
	return /*html*/`
	<div class="NWCcomponentListDiv">
		<div class="NWClistContainer">
			<div class="NWCcomponentListAnchor"></div>
		</div>
			<div class="NWCcollectionCard">
				<div class="NWCheaderGrid">
					<input type="checkbox" class="NWCcollectionCheck NWCconnectComponentsCheck">
					<div class="NWCcollectionName NWCeditableLabel">Connect components</div>
				</div>
				<div class="NWCcollectionDescription">Automatically connect created components together.</div>
			</div>
			<div class="NWCcollectionCard">
				<div class="NWCheaderGrid">
					<input type="checkbox" class="NWCcollectionCheck NWCkeepGitTrackingCheck">
					<div class="NWCcollectionName NWCeditableLabel">Create without Git repository</div>
				</div>
				<div class="NWCcollectionDescription">Converts components and their libraries into plain folders.</div>
			</div>
	</div>`;
}

function NWCbuttonHtml(){
	return /*html*/`
	<div class="BUTTONgroup NWCbuttonsRow">
		<div class="NWCrefreshCacheGrid"><div class="NWCrefreshCache">Refresh cache</div><div class="NWCrefreshInfo"></div></div>
		<div class="NWCaccessWarning"></div>
		<div class="NWCsourceComponentDiv"></div>
		<div></div>
		<div class="NWCcancel BUTTONstandard BUTTONdefault">CANCEL</div>
		<div class="NWCadd BUTTONdisabled BUTTONstandard">ADD</div>
	</div>`;
}

function NWChtml(){
   return /*html*/`
	${NWCgitError()}
	<div class="NWCmodal NWChidden">
		<div class="NWCgrid">
			${NWCfilterByCollectionHtml()}
			${NWChtmlTemplates()}
			<div class="NWCaddComponentsGrid">
				<div class="NWCheaderTitle">DETAILS</div>
				${NWCcomponentListHtml()}
			</div>
		</div>
		${NWCbuttonHtml()}
	</div>`;
}

function NWCrenderComponentList(){
	NWCcomponentList  = DRGcreateDragSortList({'anchor': '.NWCcomponentListAnchor'});
}
