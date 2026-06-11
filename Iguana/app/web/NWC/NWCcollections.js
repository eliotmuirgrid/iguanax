let NWCshowAllCollections = "NWCshowAllCollections123random";

function NWCupdateCollections(){
   let Collections = NWCcollections();
   if(!Collections) return;
   if( DEEPequal(Collections, NWCcollectionsRendered) ) return;
   let Div = document.querySelector('.NWCfilterByCollectionSelect');
   if(!Div) return;
   Div.innerHTML = NWCcollectionRowsHtml();
	NWCupdateSelectedCollections();
	NWCupdateEmojis();
   NWCcollectionsRendered = JSON.parse(JSON.stringify(Collections));
}

function NWCupdateSelectedCollections(){
   let SelectedCollections = STORElocalRead("NWCselectedCollections");
   if(SelectedCollections) {
      SelectedCollections = JSON.parse(SelectedCollections);
      for(let collection of SelectedCollections) {
         let Div = document.querySelector(`.NWCfilterByCollectionSelect .NWCcollectionCheck[value="${collection}"]`);
         if(Div) Div.checked = true;
      }
   }
	let Selected = document.querySelector('.NWCfilterByCollectionSelect .NWCcollectionCard:has(.NWCcollectionCheck:checked)');
	if(!Selected) return;
	const container = document.querySelector('.NWCfilterByCollectionSelect');
	if(!container) return;
	container.scrollTop = Selected.offsetTop - container.clientHeight / 2 + Selected.clientHeight / 2;
}

function NWCcollectionRowsHtml() {
	let collections = NWCcollections();
	let html = "";
	for (let i = 0; i < collections.length; i++) html += NWCcollectionRow(collections[i]);
	return html;
}

function NWCcollectionRow(Data){
	let git = Data?.git || '';
	let name = ESChtmlEscape(Data?.name || git || '');
	let description = Data?.description || "";
	return /*html*/`
	<div class="NWCcollectionCard">
		<div class="NWCheaderGrid">
			<input type="checkbox" class="NWCcollectionCheck" value="${git}">
			<div class="NWCcollectionName">${name}</div>
		</div>
		<div class="NWCcollectionDescription">${description}</div>
	</div>`;
}

function NWCfilterByCollectionHtml(){
	return /*html*/`
	<div class="NWCfilterByCollectionDiv">
		<div class="NWCheaderTitle">COLLECTIONS</div>
		<div class="NWCfilterByCollectionSelect"></div>
	</div>
	`;
}

function NWCcardClicked(target){
	const card = target.closest(".NWCcollectionCard");
	if (!card) return;
	const mainCheck = card.querySelector(".NWCcollectionCheck");
	if (!target.closest(".NWCcollectionCheck")) {
	  mainCheck.checked = !mainCheck.checked;
	}
	const connectCheck = card.querySelector(".NWCconnectComponentsCheck");
	if (connectCheck) {
	  STORElocalCreate("NWCcheckedConnectComponent",
		 connectCheck.checked ? "true" : "false"
	  );
	}
	const gitCheck = card.querySelector(".NWCkeepGitTrackingCheck");
	if (gitCheck) {
	  STORElocalCreate("NWCcheckedPlainFolder",
		 gitCheck.checked ? "true" : "false"
	  );
	}
	const SelectedCollections = NWCselectedCollections();
	STORElocalCreate("NWCselectedCollections", JSON.stringify(SelectedCollections));
	MODELupdate("NWCmodel");
 }

function NWCupdateEmojis() {
	document.querySelectorAll('.NWCcollectionName').forEach(nameEl => {
	  const rawText = nameEl.textContent.trim();
 
	  // leading emoji match (handles VS16 + ZWJ sequences)
	  const m = rawText.match(
		 /^(\p{Extended_Pictographic}|\p{Emoji_Presentation})(?:\uFE0F|\u200D\p{Extended_Pictographic})*\s*/u
	  );
 
	  const header = nameEl.closest('.NWCheaderGrid') || nameEl.parentElement;
	  if (!header) return;
 
	  let wrap = header.querySelector('.NWCemojiWrap');
 
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
		 wrap.className = 'NWCemojiWrap';
		 header.insertBefore(wrap, nameEl);
	  } else {
		 wrap.innerHTML = '';
	  }
 
	  const span = document.createElement('span');
	  span.className = 'NWCemoji';
	  span.textContent = emoji;
	  wrap.appendChild(span);
	});
 }
 