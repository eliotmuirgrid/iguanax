let SRCHstate = null; // access this via SRCHsaveStateHandler()

function SRCHserializeState() {
	const State = SRCHstateHandlers();
	return {
		component: TRANScomponentGuid(),
		development: TRANSdevelopment(),
		query: State.getQuery(),
		replace: State.getReplace(),
		match_case: State.getCaseMatch(),
		match_full_word: State.getWordMatch(),
		path: State.isGlobal() ? "" : TRANSfile(),
		matches: SRCHgetFileSelections(),
	};
}

// Use this to access the state
function SRCHsaveStateHandler() {
	return SRCHgetStateHandler();
}

function SRCHgetStateHandler() {
	return {
		query: SRCHgetElementState(".SRCHsidebarSearchInput", "value"),
		replace: SRCHgetElementState(".SRCHreplaceWord", "value"),
		caseMatch: SRCHgetElementState(".SRCHmatchCase", "SRCHmatchCaseOn"),
		wordMatch: SRCHgetElementState(".SRCHmatchWord", "SRCHmatchWordOn"),
		global: SRCHgetElementState(".SRCHglobal", "checked"),
	};
}

function SRCHrestoreStateHandler(state) {
	if (!state) return;
	SRCHsetElementState(".SRCHsidebarSearchInput", "value", state.query);
	SRCHsetElementState(".SRCHreplaceWord", "value", state.replace);
	if (state.replace) {
		document.querySelector(".SRCHtoggleReplace").click();
	}
	SRCHsetElementState(".SRCHglobal", "checked", state.global);

	SRCHupdateToggleHandler(".SRCHmatchCase", "SRCHmatchCaseOn", "SRCHmatchCaseOff", state.caseMatch);
	SRCHupdateToggleHandler(".SRCHmatchWord", "SRCHmatchWordOn", "SRCHmatchWordOff", state.wordMatch);
	if (state.query) SRCHfind();
}

// Main state handlers
function SRCHstateHandlers() {
	return {
		getQuery: () => SRCHgetElementState(".SRCHsidebarSearchInput", "value"),
		getReplace: () => SRCHgetElementState(".SRCHreplaceWord", "value"),
		getCaseMatch: () => SRCHgetElementState(".SRCHmatchCase", "SRCHmatchCaseOn"),
		getWordMatch: () => SRCHgetElementState(".SRCHmatchWord", "SRCHmatchWordOn"),
		isGlobal: () => SRCHgetElementState(".SRCHglobal", "checked"),
		saveState: SRCHsaveStateHandler,
		restoreState: SRCHrestoreStateHandler,
	};
}

function SRCHgetElementState(selector, check) {
	const El = document.querySelector(selector);
	return El
		? (() => {
				switch (check) {
					case "value":
						return El.value;
					case "checked":
						return El.checked;
					default:
						return El.classList.contains(check);
				}
		  })()
		: false;
}

function SRCHsetElementState(selector, type, value, addClass = true) {
	const El = document.querySelector(selector);
	if (El) {
		switch (type) {
			case "value":
				El.value = value;
				break;
			case "checked":
				El.checked = value;
				break;
			case "class":
				addClass ? El.classList.add(value) : El.classList.remove(value);
				break;
		}
	}
}

function SRCHupdateToggleHandler(selector, onClass, offClass, condition) {
	SRCHsetElementState(selector, "class", condition ? onClass : offClass, true);
	SRCHsetElementState(selector, "class", condition ? offClass : onClass, false);
}
