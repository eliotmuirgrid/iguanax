function SRCHfind() {
	SRCHclearResults();
	const Data = SRCHserializeState();
	if (!Data?.query) {
		return SRCHsetCount(-1);
	}
	APIcall("ide/search/find", Data, function (R) {
		const container = document.querySelector(".SRCHsidebarSearchList");
		if (container) {
			container.innerHTML = SRCHsidebarSearchRowHtml(R);
			SRCHsetCount(SRCHgetNumResults());
			if (R?.data?.max_results) {
				SRCHsetMaxResults();
			}
			if(R?.data?.too_large.length) {
				SRCHsetLargeFiles(R.data.too_large);
			}
		}
		// IX-4330 Need to reload the current file to make sure we pick up the code change.
		TRANSfileLoad(function(R){
			TRANSonFileLoad(R);
		});
	});
}

function SRCHreplace() {
	APIcall("ide/search/replace", SRCHgetUnlocked(), function (R) {
		SRCHfind();
	});
}

function SRCHreplaceAllConfirm() {
	const Data = SRCHserializeState();
	if (!TRANSdevelopment() || !Data.query || !Data.replace || SRCHgetNumResults() === 0) {
		return;
	}
	let Popout = POPpopoutOpen({
		title: "Confirm Replacement",
		content: SRCHreplaceAllConfirmHtml(Data.replace),
		width: "450px",
		esc_close: true,
	});
	document.querySelector(".SRCHreplaceAllConfirm").addEventListener("click", function () {
		POPpopoutClose(Popout);
		SRCHreplace();
		TRANSfileCurrentReload();
	});
	document.querySelector(".SRCHreplaceAllCancel").addEventListener("click", function () {
		POPpopoutClose(Popout);
	});
}

function SRCHreplaceAllConfirmHtml(replacement) {
	const NumFiles = document.querySelectorAll(
		".SRCHsidebarSearchRow:not(.SRCHreadOnlyResults) .SRCHbuttonFileName"
	).length;

	let Results = SRCHgetUnlocked()?.matches;
	const NumResults = Results ? Results.length : 0;
	return /*html*/ `
	<p> This will not replace results in locked files. </p>
   <p>Replace ${NumResults} occurrence(s) across ${NumFiles} file(s) with
      <span class="SRCHhighlightReplacement">'${replacement}'</span>?
   </p>
	<br>
   <div class="BUTTONgroup ACTdeleteButtons">
      <div class='SRCHreplaceAllCancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      <div class='SRCHreplaceAllConfirm BUTTONaction BUTTONstandard'>CONFIRM</div>
   </div>`;
}
