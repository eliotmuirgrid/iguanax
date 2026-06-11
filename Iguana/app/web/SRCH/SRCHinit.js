const SRCHtabElements = {
	Search: ".PRJsearch",
	Files: ".PRJfiles",
	SearchContent: ".PRJglobalSearch",
	FilesContent: ".PRJprojectTreeBody",
};

function SRCHopen() {
	SRCHtoggleTabs(
		SRCHtabElements.Search,
		SRCHtabElements.Files,
		SRCHtabElements.SearchContent,
		SRCHtabElements.FilesContent
	);
	SRCHcreateHtml();
	SRCHstateHandlers().restoreState(SRCHstate);
	SRCHlisteners();
	SRCHfocus()
}

function SRCHclose() {
	SRCHtoggleTabs(
		SRCHtabElements.Files,
		SRCHtabElements.Search,
		SRCHtabElements.FilesContent,
		SRCHtabElements.SearchContent
	);
}

function SRCHcreateHtml() {
	const Element = document.querySelector(".PRJglobalSearch");
	if (Element) Element.innerHTML = SRCHsidebarGui();
}

function SRCHtoggleTabs(activeTab, inactiveTab, showSelector, hideSelector) {
	const ActiveTab = document.querySelector(activeTab);
	const InactiveTab = document.querySelector(inactiveTab);
	if (ActiveTab?.classList.contains("PRJtabActive")) return;
	InactiveTab?.classList.remove("PRJtabActive");
	ActiveTab?.classList.add("PRJtabActive");
	SRCHshow(showSelector);
	SRCHhide(hideSelector);
	MODELupdate("TRANSmodel");

	// When hiding search tab, save state
	if (hideSelector === SRCHtabElements.SearchContent) {
		SRCHstate = SRCHstateHandlers().saveState();
	}
}
