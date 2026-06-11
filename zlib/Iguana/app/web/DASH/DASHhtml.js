function DASHsearchDiv(){
	return /*html*/`		
	<div class="DASHsearchDiv">	
		<div class="DASHsearch">
			<div class="DASHsearchInput">
				<div class="editor--container">
					<input class="EDITfilter" spellcheck="false" value="${ PAGEhashParameter("filter") || ""}" placeholder="Filter Components">
				</div>
			</div>
			<div class="BUTTONcancel DASHclearSearch"><img src="/DASH/DASHclearSearch.svg"></div>
		</div>
		<div class="DASHsearchFooterRow">
			<div class="DASHfilterCheckbox"><input type="checkbox" class="DASHfilterResultsCheckbox" ${PAGEhashParameter("whole_word") === "true" ? 'checked' : ''}>Match Whole Word</div>
			<div class="DASHresultCount"></div>
		</div>
	</div>`;
}

function DASHtitleBar(){
	return /*html*/`
	<div class="DASHsideHeader">
	<div class="DASHheading">Dashboard</div>
	<div class="DASHheadingButtons">
		<div class="DASHaddFromUrlButton BUTTONdisabled">
			+ From URL
			</div>
			<div class="DASHaddComponentButton BUTTONdisabled">
				<svg class="DASHiconAddStyle14px" width="14" height="14"><use href="DASH/DASHaddIcon14px.svg#DASHiconAdd14px"></use></svg>
				Component
			</div>
		</div>
	</div>`;
}

function DASHheadersHtml(){
	return /*html*/`
	<div class="DASHtableHeader DASHcheckbox"><input type="checkbox" class="DASHselectAllRow"></div>
	<div class="DASHtableHeader DASHstatusHeader" title="Status">Run</div>
	<div class="DASHtableHeader DASHnameHeader" title="Name of Component">Name</div>
	<div class="DASHtableHeader DASHlastAct" title="Last Activity">Last</div>
	<div class="DASHtableHeader DASHcenterText DASHque" title="Queue Count">Que</div>
	<div class="DASHtableHeader DASHcenterText DASHerr" title="Error Count">Err</div>
	<div class="DASHtableHeader DASHcenterText DASHin" title="Inbound Messages/Second">In</div>
	<div class="DASHtableHeader DASHcenterText DASHout" title="Outbound Message/Second">Out</div>
	`;
}

function DASHlayout(ShowOnlyChecked) {
	return /*html*/ `
	<div class="DASHdashboardGrid">
		<div class="DASHdashboard">
			<div class="DASHlistView">
				<div class="DASHresizeControl">
					<div class="DASHresizeShadow"></div>
				</div>
				${DASHtitleBar()}
				${DASHsearchDiv()}
				<div class="DASHtableOverflow">
					<div class="DASHtableContainer">
						<div class="DASHtableRow DASHtableHeading">
							${DASHheadersHtml()}
						</div>
						<div class="DASHlistDataError"></div>
						<div class="DASHrowBuffer"></div>
					</div>
					<div class="DASHbulkActionFade"></div>
				</div>
				<div class="DASHselectToolbar">
					<select class="FORMselectBox DASHselectMenu" disabled>
						<option value="DASHselect">Bulk Actions</option>
						<option value="DASHselectStart">Start</option>
						<option value="DASHselectStop">Stop</option>
						<option value="DASHselectEdit">Edit Tags</option>
						<option value="DASHselectClearQueue">Clear Queue</option>
						<option value="DASHselectClearErrors">Clear Errors</option>
						<option value="DASHselectDelete">Delete</option>
					</select>
					<div class="DASHshowOnlyCheckedSwitchBox">
					<label class="DASHonlyCheckedContainer">
						<input type="checkbox" class="DASHonlyChecked" ${ShowOnlyChecked ? 'checked' : ''}>
						<div class="DASHonlyCheckedIcon">Only show selected</div>
					</label>
					</div>
				</div>
			</div>
			${DASHgitHistorySplitter()}
			<div class="CYTOcontainer"></div>
		</div>
	</div>`;
}

function DASHgitHistorySplitter(){
	return /*html*/`
	<div class="DASHgitHistorySplitter">
		<div class="DASHcommitTarget"></div>
		<div class="DASHgitHistoryFadeSection"></div>
		<div class="DASHresizeControl2">
      	<div class="DASHresizeShadow2"></div>
			<div class="SPLTresizeButtonContainer DASHresizeButtonContainer">
				<div class="SPLTresizeButton DASHresizeButton">
					<svg class="SPLTresizeIcon DASHresizeIcon" width="16" height="16">
						<use href="SPLT/SPLTiconCollapseSplitter.svg#SPLTiconCollapseSplitter"></use>
					</svg>
				</div>
			</div>
      </div>
	</div>`;
}

function DASHlastActivityTime(EpochNumber, Guid){
	let Date = LOGyearMonthDateTime({
      epoch: EpochNumber * 1000,
      includeTime: true,
		includeDay : false,
		includeYear : false,
		includeSeconds : true,
      simpleTime: true,
      shortDayLabel: true
   });
	let LastTime = EpochNumber == 0 ? "N/a" : Date;
	let Perm = DASHuserPermissionAll()[Guid]?.view_logs;
	if(Perm) return /*html*/`<a class="DASHlatestActivity" href="#logs?component=${encodeURIComponent(Guid)}">${LastTime}</a>`;
	return /*html*/`<div class="DASHlatestActivityNoPerm">${LastTime}</div>`;
}

function DASHerrorCountHtml(errorCount, Guid){
	let RedClass = errorCount ? "DASHredError" : "";
	return `<a class="DASHerrorCount ${RedClass}" href="#logs?component=${encodeURIComponent(Guid)}&tags=%23error&filter=true">${errorCount}</a>`;
}

function DASHisComponentHighlighted(guid){
	const hovered = CYTOgetHoveredId();
	if(hovered && hovered === guid) return true;
}

function DASHcomponentRowHtml(data, running) {
	let PermAll = DASHuserPermissionAll();
	let Guid = data.guid;
	let Perm = PermAll[Guid];
	let Columns = DASHnewColumns(data?.columns || {});
	return /*html*/`
	<div class="DASHtableRow DASHcomponentRow  ${DASHisComponentHighlighted(Guid) ? "DASHrowHighlighted" : ""} ${DASHisComponentChecked(Guid) ? 'DASHrowChecked' : ''} ${data.selected ? 'DASHrowSelected' : ''}">
   	<div class="DASHcheckbox"><input type="checkbox" class="DASHselectRow" ${DASHisComponentChecked(Guid) ? 'checked' : ''}></div>
		<div class="DASHcenterText DASHstatusHeader">${TOGswitchHtml("DASHcomponentSwitch", data.light, running, Perm?.stop, Perm?.start)}</div>
   	<div class="DASHnamePlusLabel DASHnameHeader">
      	<a class="DASHcomponentName" href="#dashboard/details?component=${encodeURIComponent(Guid)}">${DASHhighlightKeywords(data.name)}</a>
			<div class="DASHserverLabelHtml"><div class="DASHlabel">${HASHTAGrenderTags(data.tags, "", {}, {}, DASHsearchText())}</div></div>
   	</div>
   	<div class="DASHlabel DASHlastActivity DASHlastAct">${DASHlastActivityTime(data.last_activity, Guid)}</div>
   	<div class="DASHcenterText DASHqueCount DASHque">${data?.queue_count || 0}</div>
   	<div class="DASHcenterText DASHerrCount DASHerr">${DASHerrorCountHtml(data.error_count, Guid)}</div>
   	<div class="DASHcenterText DASHmpsCount DASHin">${data.mps_in}</div>
   	<div class="DASHcenterText DASHmpsCount DASHout">${data.mps_out}</div>
		${Columns}
	</div>`
}
