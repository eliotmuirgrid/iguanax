let LBRshowAllCollections = "LBRshowAllCollections123random123r4ndom";

function LBRupdateCollections(){
   let Collections = LBRcollections();
   if(!Collections) return;
   if( DEEPequal(Collections, LBRcollectionsRendered) ) return;
   let Div = document.querySelector('.LBRfilterByCollectionSelect');
   if(!Div) return;
   Div.innerHTML = LBRcollectionRowsHtml();
   LBRupdateSelectedCollections();
	LBRupdateEmojis();
   LBRcollectionsRendered = JSON.parse(JSON.stringify(Collections));
}

function LBRupdateSelectedCollections(){
   let SelectedCollections = STORElocalRead("LBRselectedCollections");
   let PlainFolder = STORElocalRead("LBRcheckedPlainFolder");
   if(SelectedCollections) {
      SelectedCollections = JSON.parse(SelectedCollections);
      for(let collection of SelectedCollections) {
         let Div = document.querySelector(`.LBRfilterByCollectionSelect .LBRcollectionCheck[value="${collection}"]`);
         if(Div) Div.checked = true;
      }
   }
   if(PlainFolder) {
      let Div = document.querySelector('.LBRfilterByCollectionSelect .LBRcollectionCheck[value="LBRshowAllCollections123random123r4ndom"]');
      if(Div) Div.checked = true;
   }
	let Selected = document.querySelector('.LBRfilterByCollectionSelect .LBRcollectionCard:has(.LBRcollectionCheck:checked)');
	if(!Selected) return;
	const container = document.querySelector('.LBRfilterByCollectionSelect');
	if(!container) return;
	container.scrollTop = Selected.offsetTop - container.clientHeight / 2 + Selected.clientHeight / 2;
}

function LBRcollectionRowsHtml() {
	let collections = LBRcollections();
	let html = "";
	for (let i = 0; i < collections.length; i++) html += LBRcollectionRow(collections[i]);
	return html;
}

function LBRcollectionRow(Data){
	let git = Data?.git || '';
	let name = ESChtmlEscape(Data?.name || git || '');
	let description = Data?.description || "";
	return /*html*/`
	<div class="LBRcollectionCard">
		<div class="LBRheaderGrid">
			<input type="checkbox" class="LBRcollectionCheck" value="${git}">
			<div class="LBRcollectionName">${name}</div>
		</div>
		<div class="LBRcollectionDescription">${description}</div>
	</div>`;
}

function LBRfilterByCollectionHtml(){
	return /*html*/`
	<div class="LBRfilterByCollectionDiv">
		<div class="LBRheaderTitle">COLLECTIONS</div>
		<div class="LBRfilterByCollectionSelect"></div>
	</div>
	`;
}

function LBRcardClicked(target){
	const card = target.closest(".LBRcollectionCard");
	if (!card) return;
	const check = card.querySelector(".LBRcollectionCheck");
	if (!check) return;
	if (check.disabled) return;
	if (!target.closest(".LBRcollectionCheck")) {
	  check.checked = !check.checked;
	}
	const State = check.checked ? "true" : "false";
	STORElocalCreate("LBRcheckedPlainFolder", State);
	const SelectedCollections = LBRselectedCollections();
	STORElocalCreate("LBRselectedCollections", JSON.stringify(SelectedCollections));
	MODELupdate("LBRmodel");
}

function LBRupdateEmojis() {
	document.querySelectorAll('.LBRcollectionName').forEach(nameEl => {
	  const rawText = nameEl.textContent.trim();
 
	  // leading emoji match (handles VS16 + ZWJ sequences)
	  const m = rawText.match(
		 /^(\p{Extended_Pictographic}|\p{Emoji_Presentation})(?:\uFE0F|\u200D\p{Extended_Pictographic})*\s*/u
	  );
 
	  const header = nameEl.closest('.LBRheaderGrid') || nameEl.parentElement;
	  if (!header) return;
 
	  let wrap = header.querySelector('.LBRemojiWrap');
 
	  if (!m) {
		 // no emoji: ensure grid is 2-col and remove lane if it exists
		 header.classList.remove('has-emoji');
		 if (wrap) wrap.remove();
		 return;
	  }
 
	  const emoji = m[0].trim();
	  const rest  = rawText.slice(m[0].length).trim();
 
	  // update title text
	  nameEl.textContent = rest;
 
	  // ensure 3-col grid
	  header.classList.add('has-emoji');
 
	  // create lane if missing
	  if (!wrap) {
		 wrap = document.createElement('div');
		 wrap.className = 'LBRemojiWrap';
		 header.insertBefore(wrap, nameEl);
	  } else {
		 wrap.innerHTML = '';
	  }
 
	  const span = document.createElement('span');
	  span.className = 'LBRemoji';
	  span.textContent = emoji;
	  wrap.appendChild(span);
	});
 }
 