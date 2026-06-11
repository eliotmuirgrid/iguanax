let LBRcard;

function LBRrender(){
	if(document.querySelector('.LBRmodal')) return;
   LBRcard = POPpopoutOpen({
      title: 'Add library', 
      width: '95%', 
      minWidth: '600px', 
      content: LBRhtml(),  
		className: 'LBRcomponent',
      esc_close : true,
      callback : LBRclose
   });
   document.querySelector('.LBRcomponent .POPpopoutCard-content').classList.add('LBRcatalogueModalContainer');
   LBRmodelInit();
	LBRslowModelInit();
	FORMfocusDelay(document.querySelector(".LBRfilter"));
}

function LBRupdateButton(){
   let Button = document.querySelector('.LBRcreate');
   if(!Button) return;
   if(LBRrepoError || LBRnameExists() || LBRnameError) {
		Button.classList.add('BUTTONdisabled');
		Button.classList.remove('BUTTONaction');
	}
   else {
		Button.classList.add('BUTTONaction');
		Button.classList.remove('BUTTONdisabled');
	}
}

function LBRhtmlDetails(){
   return /*html*/`
	<div class="LBRdetailsDiv">
		<div class="LBRheaderTitle">DETAILS</div>
		<div></div>
		${LBRcreateAsPlainFolderHtml()}
		<div class="LBRfilterContainer">
			<input class="LBRname FORMinput" placeholder="Name" title="Name of the library you are importing." value="">
			<div class="LBRnameFeedback"></div>
			<div class="LBRnameExistsFeedback"></div>
			<br>
			<div class="LBRrepoLinkHrefDiv">
				<input type="text" value="" placeholder="Git url - https://host.org/owner/repository.git" class="LBRrepoLinkInput FORMinput"  title="Git url of the library you are importing.">
				<div class="LBRrepoLink BUTTONdefault BUTTONstandard">
					<div class="LBRopenLinkDiv" title="View repository on web"></div>
				</div>
			</div>
			<div class="LBRrepoFeedback"></div>
			<div></div>
			<div class="LBRrepoDoesntExistFeedback"></div>
			<br>
		</div>
	</div>`;
}

function LBRcreateAsPlainFolderHtml(){
  let PlainFolder = STORElocalRead("LBRcheckedPlainFolder") === 'true';
  let checkboxAttrs = '';
  if (TRANSplainFolder()) {
    checkboxAttrs = 'checked disabled';
  } else if (PlainFolder) {
    checkboxAttrs = 'checked';
  }
  return /*html*/`
  <div class="LBRcollectionCard">
    <div class="LBRheaderGrid">
      <input ${checkboxAttrs} type="checkbox" class="LBRcollectionCheck LBRkeepGitTrackingCheck">
      <div class="LBRcollectionName LBReditableLabel">Create without Git repository</div>
    </div>
    <div class="LBRcollectionDescription">Adds the contents of the folder without the Git repository.</div>
  </div>`;
}


function LBRhtml(){
   return /*html*/`
	<div class="LBRgitInformation LBRhidden">
         <div class="LBRgitFeedback"></div>
         <br>
         <div class="BUTTONgroup LBRbuttons">
            <div class="LBRcancel BUTTONstandard BUTTONaction">DISMISS</div>
         </div>
      </div>
	<div class="LBRmodal LBRhidden">
		${LBRfilterByCollectionHtml()}	
		${LBRhtmlTemplates()}
		${LBRhtmlDetails()}
		<div class="BUTTONgroup LBRbuttonsRow">
			<div class="LBRrefreshCacheGrid">
				<div class="LBRrefreshCache">Refresh cache</div>
				<div class="LBRrefreshCacheInfo"></div>
			</div>
			<div class="LBRcancel BUTTONstandard BUTTONdefault">CANCEL</div>
			<div class="LBRcreate BUTTONdisabled BUTTONstandard">ADD</div>
		</div>
	</div>`;
}

function LBRclose(){
	if(POPpopoutClose(LBRcard)) LBRcard = null; 
	LBRmodelDestroy();
	TRANSsetCode();
}

// needs to handle if the collection file is empty - this is handled
// needs to handle if there are no templates in any of the collection files
