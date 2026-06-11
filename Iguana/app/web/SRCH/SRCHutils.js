// This how we capture Ctrl+F or Cmd+F
function SRCHcheckForCommandF(Event) {
	FORMcheckForCommandPlusKey(Event, "f", function(){
		SRCHopen();
		SRCHfocus();
	});
}

function SRCHsearchOpen() {
	let Div = document.querySelector(".PRJsearch");
	return Div && Div.classList.contains("PRJtabActive");
}

function SRCHfocus() {
	const input = document.querySelector(".SRCHsidebarSearchInput");
	FORMfocus(input);
}

function SRCHshow(Selector) {
	const Element = document.querySelector(Selector);
	Element?.style.setProperty("display", "block");
}

function SRCHhide(Selector) {
	const Element = document.querySelector(Selector);
	Element?.style.setProperty("display", "none");
}

function SRCHgetNumResults() {
	const Results = document.querySelectorAll(".SRCHsidebarCollapsableResults");
	return Results ? Results.length : 0;
}

function SRCHsetCount(count) {
	const CountDiv = document.querySelector(".SRCHmatchCount");
	if (CountDiv) {
		if(count == null) { return CountDiv.innerHTML = ""; }
		const NumFiles = count === 0 ? 0 : SRCHgetNumFiles();
		CountDiv.innerHTML =
			count !== -1
				? `${count} ${count == 1 ? "result" : "results"} in ${NumFiles} ${NumFiles == 1 ? "file" : "files"}`
				: "";
	}
}

function SRCHsetMaxResults() {
	const Div = document.querySelector(".SRCHmaxResults");
	if (Div) {
		Div.innerText =
			"The result set only contains a subset of all matches. Be more specific in your search to narrow down results";
	}
}

function SRCHsetLargeFiles(files) {
	const Div = document.querySelector(".SRCHmaxResults");
	if(Div) {
		Div.innerText = "";
		Div.innerText += "\nSome files were skipped due to size:\n" +
			(files.length > 10
				? files.slice(0, 10).join("\n") + `\n...and ${files.length - 10} more`
				: files.join("\n"));
	}
}

function SRCHparseJumpText(jumpText) {
	const match = jumpText.match(/([^:]+):(\d+):(\d+)/);
	if (!match) return null;
	return {
		file: match[1],
		line_number: parseInt(match[2]) - 1,
		position: parseInt(match[3]) - 1,
	};
}

// this gives absolute file path since backend cache is also absolute
function SRCHgetFileSelections() {
	const selections = [];
	const fileRows = document.querySelectorAll(".SRCHsidebarSearchRow");
	fileRows.forEach((row) => {
		const filenameElem = row.querySelector(".SRCHbuttonFileName");
		if (!filenameElem) return;
		const results = [];
		for (const obj of row.querySelectorAll(".SRCHsidebarJumpToSearch")) {
			const parsed = SRCHparseJumpText(obj.textContent.trim());
			if (parsed) {
				results.push({
					...parsed,
					file: TRANSprojectRoot() + parsed.file,
					line_number: parsed.line_number + 1,
					position: parsed.position + 1,
				});
			}
		}
		if (results.length > 0) {
			selections.push(...results);
		}
	});
	return selections;
}

function SRCHclearResults() {
	document.querySelectorAll(".SRCHsidebarSearchRow").forEach((row) => row.remove());
}

function SRCHgetNumFiles() {
	const Files = document.querySelectorAll(".SRCHbuttonFileName");
	return Files ? Files.length : 0;
}

function SRCHgetUnlocked() {
	const data = SRCHserializeState();
	const filteredMatches = data.matches.filter((match) => {
		const lib = LIBisInLibrary(match.file);
		return !lib || LIBlibraryUnlocked(lib);
	});
	return { ...data, matches: filteredMatches };
}

function SRCHreset() {
	// Clear input fields
	let search = document.querySelector(".SRCHsidebarSearchInput");
	let replace = document.querySelector(".SRCHreplaceWord");
	if(search) search.value = "";
	if(replace) replace.value = "";
	// Reset search state
	document.querySelectorAll(".SRCHmatchCaseOn, .SRCHmatchWordOn").forEach((btn) => {
		btn.classList.replace("On", "Off");
	});
	// Toggle global search off
	SRCHclearResults();
	SRCHsetCount();
}

function SRCHgetSelected() { return document.querySelector(".SRCHsidebarCollapsableResults.SRCHresultSelected"); }