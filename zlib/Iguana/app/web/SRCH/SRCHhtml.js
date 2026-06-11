function SRCHheaderHtml() {
	return /*html*/ `        
    <div class="SRCHgroup">
        <div></div>
        <div class="SRCHsidebarHideOrShowTabs SRCHsidebarMinimizeTabs" title="Minimize/Maximize tabs"></div>
        <div class="SRCHsidebarRefreshSearch" title="Refresh Search Results"></div>
        <div class="SRCHsidebarClearSearch" title="Clear Search"></div>
    </div>`;
}

function SRCHsidebarGui() {
	return /*html*/ `
    <div class="SRCHsidebarFindReplace SRCHsidebarFindReplaceClosed">
        ${SRCHheaderHtml()}
        <div class="SRCHsidebarSearchFields">
            <a href="" class="SRCHtoggleReplace SRCHtoggleReplaceOff" title="Toggle Replace"></a>
            <div class="SRCHsidebarInputs">
                <div class="SRCHinputArea">
                    <input type="text" placeholder="Search" class="SRCHsidebarSearchInput" autocomplete="off" tabindex="1">
                    <a href="" class="SRCHmatchCase SRCHmatchCaseOff" title="Case Sensitive"></a>
                    <a href="" class="SRCHmatchWord SRCHmatchWordOff" title="Match Whole Word"></a>
                </div>
                <div class="SRCHactionBtnArea">
                    <a href="" class="SRCHsearchActionBtn connected enabled SRCHsearchPrevious" title="Find Previous"></a>
                    <a href="" class="SRCHsearchActionBtn connected enabled SRCHsearchNext" title="Find Next"></a>
                </div>
                <div class="SRCHinputArea SRCHreplaceCell">
                    <input type="text" placeholder="Replace" class="SRCHsidebarSearchInput SRCHreplaceWord" autocomplete="off" tabindex="2">
                </div>
                <div class="SRCHactionBtnArea SRCHreplaceCell">
                    <div class="SRCHsearchActionBtn SRCHreplaceInComponent SRCHreplaceSingle" title="Replace Current Selection"></div>
                    <div class="SRCHsearchActionBtn SRCHreplaceAllInComponent" title="Replace All"></div>
                </div>
            </div>
        </div>
        <div class="SRCHgridDiv">
            <div class="TOGtoggle">
                <input type="checkbox" class="TOGcheck SRCHglobal" title="Toggle global search" checked>
                <div class="TOGswitch"></div>
                <div class="TOGtrack"></div>
            </div>
            <label class="SRCHreplaceSliderLabel">Global</label>
            <div class="SRCHmatchCount"></div>
        </div>
        <div class="SRCHmaxResults"></div>
        <div class="SRCHsidebarSearchList"></div>
    </div>
    `;
}

function SRCHsidebarSearchRowHtml(apiResults) {
	const groupedMatches = apiResults.data.matches.reduce((acc, match) => {
		(acc[match.file] ??= []).push(match);
		return acc;
	}, {});

	return Object.entries(groupedMatches)
		.map(([filename, matches]) =>
			SRCHsidebarSectionHtml(
				filename,
				matches
					.map(
						(match, index) => /*html*/ `
                <div class="SRCHsidebarCollapsableResults">
                    <div class="SRCHsidebarResultString fc fcin">
                        ${match.line}
                        <div class="SRCHsidebarJumpToSearch">${filename}:${match.line_number}:${match.position}</div>
                    </div>
                    <svg class="SRCHsidebarRemoveResult BUTTONdefault SRCHcloseResult" width="10" height="10">
                        <use href="SRCH/SRCHiconClose.svg#close"></use>
                    </svg>
                </div>`
					)
					.join("")
			)
		)
		.join("");
}

function SRCHsidebarSectionHtml(filename, SubRowHtml) {
	const readOnly = TRANSisReadOnly(filename);
	return /*html*/ `
    <div class="SRCHsidebarSearchRow ${readOnly ? "SRCHreadOnlyResults" : ""}">
        <div class="SRCHsidebarControlFileName">
            <div class="SRCHbuttonCollapseToggle SRCHbuttonFileCollapse" title="Collapse/Expand File Results"></div>
            <div class="SRCHbuttonFileName">${filename}</div>
            ${readOnly ? '<div class="SRCHreadOnlyButton" title="This file is read only">Locked</div>' : `<div class="SRCHgrow"></div>`}
            <svg class="SRCHsidebarRemoveFile BUTTONdefault SRCHcloseResult" width="10" height="10" title="Remove Result">
                <use href="SRCH/SRCHiconClose.svg#close"></use>
            </svg>
        </div>
        <div class="SRCHsidebarSearchResult">
            ${SubRowHtml}
        </div>
    </div>`;
}

function SRCHsidebarSectionHtmlCollapsed(filename) {
	return /*html*/ `
    <div class="SRCHsidebarSearchRow">
        <div class="SRCHsidebarControlFileName">
            <div class="SRCHbuttonCollapseToggle SRCHbuttonFileExpand" title="Collapse/Expand File Results"></div>
            <div class="SRCHbuttonFileName">${filename}</div>
            <div class="SRCHgrow"></div>
            <svg class="SRCHsidebarRemoveFile BUTTONdefault SRCHcloseResult" width="10" height="10" title="Remove Result">
                <use href="SRCH/SRCHiconClose.svg#close"></use>
            </svg>
        </div>
        <div class="SRCHsidebarSearchResult"></div>
    </div>`;
}
