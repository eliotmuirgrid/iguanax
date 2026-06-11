function PRJhandleProjectPanelClick(e) {
	// if ( e.target.closest(".PRJresizeButton") )   { PRJcollapseButton(); } // TODO - implement this
	if (e.target.closest(".PRJtabsSection")) {
		PRJchangeTab(e);
	}
	if (e.target.closest(".PRJbuttonGroup")) {
		return PRJbuttonClick(e);
	}
	if (e.target.closest(".PRJlibraryButton")) {
		TRANSsaveCurrentModuleCursorPosition();
		LBRrender();
	}
	if (e.target.closest(".MENUtooltip")) {
		return MENUlisteners(e);
	}
	if (e.target.closest(".PRJprojectManager")) {
		PRJprojectTreeClick(e);
	}
	if (e.target.closest(".SRCHsidebarFindReplace")) return;
	if (e.target.closest(".PRJcomponentSelect")) return;
	if (!e.target.closest(".PRJfileTree")) {
		TRANSsetCode();
		MENUremoveTooltip();
	}
}

function PRJchangeTab(e) {
	e.preventDefault();
	let target = e.target;
	let classList = target.classList;
	if (classList.contains("PRJfiles")) {
		SRCHclose(e);
	}
	if (classList.contains("PRJsearch")) {
		SRCHopen();
	}
}

function PRJbuttonClick(e) {
	TRANSsaveCurrentModuleCursorPosition();
	if (e.target.closest(".PRJaddFileButton")) {
		PRJcreateFile(e);
	}
	if (e.target.closest(".PRJaddFolderButton")) {
		PRJcreateDir(e);
	}
}
