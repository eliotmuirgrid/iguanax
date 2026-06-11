function SRCHlisteners() {
	const SearchPanel = document.querySelector(".PRJglobalSearch");
	// Cleanup existing listeners by replacing panel
	const newPanel = SearchPanel.cloneNode(true);
	SearchPanel.parentNode.replaceChild(newPanel, SearchPanel);

	const SearchInput = document.querySelector(".SRCHsidebarSearchInput");
	const debouncedInputHandler = SRCHsearchInputListener(); // Create single debounced handler instance
	newPanel.addEventListener("click", handlePanelClick);
	SearchInput.addEventListener("input", debouncedInputHandler);
	SearchInput.addEventListener("keydown", (e) => {
		if (e.key === "Enter") {
			e.preventDefault();
			SRCHfind();
		}
	});

	function handlePanelClick(e) {
		const targetSelector = Object.keys(Handlers).find((selector) => e.target.closest(selector));
		if (!targetSelector) return;
		const { handler, preventDefault = true } = Handlers[targetSelector];
		preventDefault && e.preventDefault();
		handler(e);
	}

	const Handlers = {
		".SRCHglobal": {
			handler: SRCHfind,
			preventDefault: false,
		},
		".SRCHsidebarClearSearch": {
			handler: SRCHclearSearch,
		},
		".SRCHsidebarHideOrShowTabs": {
			handler: SRCHtoggleCollapseAll,
		},
		".SRCHsidebarRefreshSearch": {
			handler: SRCHfind,
		},
		".SRCHsidebarRemoveFile": {
			handler: SRCHremoveFileResults,
		},
		".SRCHbuttonFileName": {
			handler: SRCHcollapse,
		},
		".SRCHsidebarRemoveResult": {
			handler: SRCHremoveSingleResult,
		},
		".SRCHbuttonFileCollapse, .SRCHbuttonFileExpand": {
			handler: SRCHtoggleFileCollapse,
		},
		".SRCHtoggleReplace": {
			handler: SRCHreplaceToggleListener,
		},
		".SRCHsidebarCollapsableResults": {
			handler: SRCHjumpToResult,
		},
		".SRCHmatchCase": {
			handler: (e) => SRCHtoggleWordCaseState(e, "SRCHmatchCase"),
		},
		".SRCHmatchWord": {
			handler: (e) => SRCHtoggleWordCaseState(e, "SRCHmatchWord"),
		},
		".SRCHsearchPrevious": {
			handler: () => SRCHnavigateResults(-1),
		},
		".SRCHsearchNext": {
			handler: () => SRCHnavigateResults(1),
		},
		".SRCHreplaceAllInComponent": {
			handler: SRCHreplaceAllConfirm,
		},
		".SRCHreplaceSingle": {
			handler: SRCHreplaceSingle,
		},
	};
}

function SRCHcollapse(e) {
	const collapseButton = e.target.closest(".SRCHsidebarControlFileName").querySelector(".SRCHbuttonCollapseToggle");
	collapseButton?.click();
}

function SRCHtoggleCollapseAll(e) {
	const fileRows = document.querySelectorAll(".SRCHsidebarSearchRow");
	const anyExpanded = Array.from(fileRows).some((row) => row.querySelector(".SRCHbuttonFileCollapse"));

	fileRows.forEach((row) => {
		const btn = row.querySelector(anyExpanded ? ".SRCHbuttonFileCollapse" : ".SRCHbuttonFileExpand");
		btn?.click();
	});
}

function SRCHreplaceToggleListener(e) {
	const Toggle = document.querySelector(".SRCHtoggleReplace");
	const ReplaceContainer = document.querySelector(".SRCHsidebarInputs");
	const ReplaceCells = ReplaceContainer.querySelectorAll(".SRCHreplaceCell");
	if (Toggle.classList.contains("SRCHtoggleReplaceOn")) {
		Toggle.classList.remove("SRCHtoggleReplaceOn");
		Toggle.classList.add("SRCHtoggleReplaceOff");
		ReplaceCells.forEach((cell) => (cell.style.display = "none"));
		ReplaceContainer.style.gridTemplateRows = "auto auto";
	} else {
		Toggle.classList.remove("SRCHtoggleReplaceOff");
		Toggle.classList.add("SRCHtoggleReplaceOn");
		ReplaceCells.forEach((cell) => (cell.style.display = ""));
		ReplaceContainer.style.gridTemplateRows = "";
	}
	SRCHfocus();
}

function SRCHsearchInputListener() {
	let timeoutId = null;
	return function debouncedSearch() {
		clearTimeout(timeoutId);
		timeoutId = setTimeout(() => {
			SRCHfind();
		}, 300);
	};
}

function SRCHtoggleWordCaseState(e, baseClass) {
	const btn = e.target;
	const offClass = `${baseClass}Off`;
	const onClass = `${baseClass}On`;
	if (btn.classList.contains(offClass)) {
		btn.classList.remove(offClass);
		btn.classList.add(onClass);
	} else {
		btn.classList.remove(onClass);
		btn.classList.add(offClass);
	}
	SRCHfind();
	SRCHfocus();
}

function SRCHtoggleFileCollapse(e) {
	const button = e.target;
	const parentRow = button.closest(".SRCHsidebarSearchRow");
	const resultsContainer = parentRow.querySelector(".SRCHsidebarSearchResult");

	if (button.classList.contains("SRCHbuttonFileCollapse")) {
		button.classList.remove("SRCHbuttonFileCollapse");
		button.classList.add("SRCHbuttonFileExpand");
		resultsContainer.style.display = "none";
	} else {
		button.classList.remove("SRCHbuttonFileExpand");
		button.classList.add("SRCHbuttonFileCollapse");
		resultsContainer.style.display = "";
	}
}

function SRCHremoveFileResults(e) {
	const removeButton = e.target.closest(".SRCHsidebarRemoveFile");
	if (removeButton) {
		const fileRow = removeButton.closest(".SRCHsidebarSearchRow");
		if (fileRow) {
			fileRow.remove();
			SRCHsetCount(SRCHgetNumResults());
		}
	}
}

function SRCHremoveSingleResult(e) {
	const removeButton = e.target.closest(".SRCHsidebarRemoveResult");
	if (removeButton) {
		const resultRow = removeButton.closest(".SRCHsidebarCollapsableResults");
		if (resultRow) {
			const fileSection = resultRow.closest(".SRCHsidebarSearchRow");
			resultRow.remove();
			SRCHsetCount(SRCHgetNumResults());
			// Check remaining results in the same file section
			const resultsContainer = fileSection.querySelector(".SRCHsidebarSearchResult");
			const hasResults = resultsContainer.querySelector(".SRCHsidebarCollapsableResults");
			if (!hasResults) {
				fileSection.remove();
			}
		}
	}
}

function SRCHjumpToResult(e) {
	const resultRow = e.target.closest(".SRCHsidebarCollapsableResults");
	// Remove active class from all results
	document.querySelectorAll(".SRCHsidebarCollapsableResults").forEach((r) => r.classList.remove("SRCHresultSelected"));
	// Add active class to clicked result
	resultRow.classList.add("SRCHresultSelected");
	const jumpText = resultRow.querySelector(".SRCHsidebarJumpToSearch").textContent;
	const posInfo = SRCHparseJumpText(jumpText);
	if (!posInfo) return;

	const doc = TRANSeditor.getDoc();
	const jumpPosition = { line: posInfo.line_number, ch: posInfo.position };
	const setEditorPosition = () => {
		TRANSeditor.focus();
		const startPos = doc.posFromIndex(doc.indexFromPos(jumpPosition));
		const queryLength = SRCHstateHandlers().getQuery().length;
		const endPos = doc.posFromIndex(doc.indexFromPos(startPos) + queryLength);

		doc.setSelection(startPos, endPos);
		TRANSeditor.scrollIntoView(startPos, 100);
	};
	if (posInfo.file === TRANSfile()) {
		return setEditorPosition();
	}
	const cursorHandler = () => {
		setEditorPosition();
		TRANSeditor.off("cursorActivity", cursorHandler);
	};
	TRANSeditor.on("cursorActivity", cursorHandler);
	TRANSfileChange(posInfo.file);
}

function SRCHnavigateResults(direction) {
	const results = Array.from(document.querySelectorAll(".SRCHsidebarCollapsableResults"));
	// Filter out results in collapsed containers
	const visibleResults = results.filter(
		(result) => result.closest(".SRCHsidebarSearchResult")?.style.display !== "none"
	);
	if (!visibleResults.length) return;
	const currentActive = SRCHgetSelected();
	let currentIndex = currentActive ? visibleResults.indexOf(currentActive) : -1;

	// Handle initial state where no result is selected
	let newIndex = currentIndex;
	if (currentIndex === -1) {
		newIndex = direction > 0 ? 0 : visibleResults.length - 1;
	} else {
		newIndex = currentIndex + direction;
		if (newIndex < 0) newIndex = visibleResults.length - 1;
		else if (newIndex >= visibleResults.length) newIndex = 0;
	}

	visibleResults.forEach((r) => r.classList.remove("SRCHresultSelected"));
	const targetResult = visibleResults[newIndex];
	targetResult.classList.add("SRCHresultSelected");
	const clickEvent = new MouseEvent("click", {
		// gives valid event chain for cursorActivity in SRCHjumpToResult
		bubbles: true, // Triggers ancestor handlers
		cancelable: true, // Allows preventDefault()
		view: window, // Links to browser context
	});
	targetResult.dispatchEvent(clickEvent);
	TRANSeditor.focus();
}

function SRCHgetReplacementDetails() {
	const selectedResult = SRCHgetSelected();
	if (!selectedResult) return null;

	const replaceInput = document.querySelector(".SRCHreplaceWord");
	const replacement = replaceInput.value;
	if (!replacement) return null;

	const jumpText = selectedResult.querySelector(".SRCHsidebarJumpToSearch").textContent;
	const posInfo = SRCHparseJumpText(jumpText);
	if (!posInfo) return null;

	const fullPath = TRANSprojectRoot() + posInfo.file;
	const lib = LIBisInLibrary(fullPath);
	if (lib && !LIBlibraryUnlocked(lib)) return null;

	return { selectedResult, replacement, posInfo };
}

function SRCHperformEditorReplacement(posInfo, replacement, callback) {
	TRANSeditor.focus();
	TRANSeditor.replaceRange(
		replacement,
		{ line: posInfo.line_number, ch: posInfo.position },
		{ line: posInfo.line_number, ch: posInfo.position + SRCHstateHandlers().getQuery().length }
	);
	TRANSfileSave(TRANSrenderedFileName, TRANSeditor.getValue(), callback);
}

function SRCHrefreshSearchResults(posInfo, oldFileSection) {
	const fileNameButtons = Array.from(document.querySelectorAll(".SRCHbuttonFileName"));
	const fileSection = fileNameButtons
		.find((btn) => btn.textContent.includes(posInfo.file))
		?.closest(".SRCHsidebarSearchRow");

	if (!fileSection) return;

	const searchData = SRCHserializeState();
	searchData.path = posInfo.file;

	APIcall("ide/search/find", searchData, (R) => {
		const hasResults = R.data.matches.length > 0;
		if (hasResults) {
			fileSection.outerHTML = SRCHsidebarSectionHtml(
				posInfo.file,
				R.data.matches.map((m) => SRCHresultItemHtml(m, posInfo.file)).join("")
			);
		} else {
			fileSection.remove();
		}
		SRCHsetCount(SRCHgetNumResults());
		SRCHnavigateResults(1);
	});
}

function SRCHresultItemHtml(match, filename) {
	return /*html*/ `
		<div class="SRCHsidebarCollapsableResults">
			<div class="SRCHsidebarResultString fc fcin">
				${match.line}
				<div class="SRCHsidebarJumpToSearch">${filename}:${match.line_number}:${match.position}</div>
			</div>
			<svg class="SRCHsidebarRemoveResult BUTTONdefault SRCHcloseResult" width="10" height="10">
				<use href="SRCH/SRCHiconClose.svg#close"></use>
			</svg>
		</div>`;
}

function SRCHreplaceSingle(e) {
	const currentActive = SRCHgetSelected();
	if(!currentActive) { return SRCHnavigateResults(1); }
	const details = SRCHgetReplacementDetails();
	if (!details) return;

	SRCHperformEditorReplacement(details.posInfo, details.replacement, () => {
		SRCHrefreshSearchResults(details.posInfo, details.selectedResult.closest(".SRCHsidebarSearchRow"));
	});
}

function SRCHclearSearch(e) {
	SRCHreset();
	SRCHfind(); // Force search update with cleared state
}
