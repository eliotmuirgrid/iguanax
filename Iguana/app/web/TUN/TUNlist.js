function TUNlistHtml(){
   return /*html*/`		
   <div class="TUNlistView ">
      <div class="TUNsideHeader">
         <div class="TUNheading">Cluster</div>
         <div class="TUNheadingButtons">
            <div class="TUNaddFromUrlButton">
					+ MY KEY
            </div>
            <div class="TUNaddPeer">
               <svg class="TUNiconAddStyle14px" width="14" height="14"><use href="TUN/TUNaddIcon14px.svg#TUNiconAdd14px"></use></svg>
               ADD PEER
            </div>
         </div>
      </div>
      <div class="TUNtableOverflow">
         <div class="TUNtableContainer">
            <div class="TUNtableRow TUNtableHeading">
               <div class="TUNtableHeader TUNcenterText" title="Status">RUN</div>
               <div class="TUNtableHeader" title="Name of Component">NAME</div>
            </div>
            <div class="TUNlistDiv"></div>
            <div class="TUNlistDataError"></div>
            <div class="TUNrowBuffer"></div>
         </div>
      </div>
      <div class="TUNconnectedDiv"></div>
   </div>`;
}

function TUNlistRowHtml(name, connected){
	let data = {};
   return /*html*/`
	<div class="TUNtableRow TUNcomponentRow">
      <div class="TUNcenterText">
         <div class="TOGtoggle TUNlight">
            <div class="TOGtrack ${connected ? "TUNswitchOn" : "TUNswitchErr" } "></div>
         </div>
      </div>
      <div class="TUNnamePlusLabel">
         <div class="TUNcomponentName">${name}</div>
         <div class="TUNserverLabelHtml"><div class="TUNlabel">${HASHTAGrenderHashtags(data.description)}</div></div>
      </div>
      <div>${connected ? `<a href="/peer/${name}/#dashboard">[ Open Dashboard ]</a>` : ""} </div>
   </div>`;
}

function TUNinsertHeadingHtml() {
   let Container = document.querySelector('.TUNtableContainer');
   if(!Container || Container.firstElementChild.classList.contains('TUNtableHeading')) { return; }
   Container.innerHTML = `
   <div class="TUNtableRow TUNtableHeading">
      <div class="TUNtableHeader TUNcenterText" title="Status">RUN</div>
      <div class="TUNtableHeader" title="Name of Component">NAME</div>
   </div>
   <div class="TUNlistDiv"></div>
   <div class="TUNlistDataError"></div>
   <div class="TUNrowBuffer"></div>`;
}

function TUNpeerSetupHtml() {
   COL_TRC("Setup instructions.");
   let Container = document.querySelector('.TUNtableContainer');
   if(!Container || Container.firstElementChild.classList.contains('TUNstepList')) { return; }
   Container.innerHTML = `
    <div class="TUNstepList">
    <div class="TUNstepTitle">HOW TO TUNNEL TO REMOTE IGUANA SERVERS</div>
    <ul>
      <li>Step 1: Add a peer and paste their public key</li>
      <li>Step 2: Have the remote Iguana do the same with your key found by pressing My Key</li>
      <li>Step 3: Once both ends have access you should see a list appear where you can click Open Dashboard on the remote</li>
    </ul>
  </div>`
}
