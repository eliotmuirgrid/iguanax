let GITHorgsCard;

function GITHeditOrgsHtml(){
   let Data = GITHowners();
   let List = Data?.owners;
   let Source = Data?.organizations;
   let Orgs = Source ? List : [];
   let html = GITHallOrgHtml(Orgs);
   return /*html*/`
   <div class="GITHorgsEdit">
      <div>Leave the list blank to use auto-detected organizations, or add entries to set your own.</div> 
      <br>
      <div class="GITHorgsTable">
         <div class="GITHheading">
            <div>Organizations</div>
            <div></div>
         </div>
         ${html}
      </div>
      <div class="SETaddRow"> 
         <svg class="SETaddIcon" width="14" height="14">
            <use href="SET/SETiconAdd.svg#SETiconAdd"></use>
         </svg>
         ADD ORGANIZATION
      </div>
      <div class="GITHerror GITHorgsError"></div>
      <div class="BUTTONgroup GITHbutton">
         <div class="GITHorgsCancel BUTTONstandard BUTTONcancel">CANCEL</div>
         <div class="GITHorgsSave BUTTONstandard BUTTONaction">SAVE</div>
      </div>
   </div>`;
}

function GITHorgsError(Error){
   document.querySelector('.GITHorgsError').innerHTML = Error;
}

function GITHaddOrgRow(){
   let html = GITHorgRowHtml("");
   const table = document.querySelector('.GITHorgsTable');
   if(table) table.insertAdjacentHTML('beforeend', html);
   SETfocusLast(".GITHorgRow .FORMinput");
}

function GITHremoveOrgRow(target){
   let div = target.closest(".GITHorgRow");
   div.remove();
   let divs = GITHorgRows();
   if(divs.length == 0) GITHaddOrgRow();
}

function GITHorgRows(){
   return document.querySelectorAll(".GITHorgRow");
}

function GITHallOrgHtml(OrgData){
   let html = "";
   for(org of OrgData) html = html + GITHorgRowHtml(org);
   if(html == "") html = GITHorgRowHtml("");
   return html;
}

function GITHorgRowHtml(OrgData){
   return /*html*/`
   <div class="GITHorgRow">
      <div>
         <input class="FORMinput GITHorgsValue" value="${ESChtmlEscape(OrgData)}" placeholder="Start typing an organization name…">
      </div>
      <div class="GITHdeleteOrg">
         <div class="FORMbutton BUTTONcancel BUTTONwarning SETdeleteRow">
            <svg class="SETtrashIcon" width="16" height="16">
               <use href="SET/SETtrashIcon.svg#SETtrashIcon"></use>
            </svg>
         </div>
      </div>
   </div>`;
}  

function GITHsetOrgsPopout(){
   GITHorgsCard = POPpopoutOpen({
      title:"Set Git Organizations",
      width: "650px",
      content: GITHeditOrgsHtml()
   });
   SETfocusLast(".GITHorgRow .FORMinput");
   FORMenterListener(document.querySelector(".GITHorgsTable"), GITHsetOrgs);
   GITHorgsListeners();
}

function GITHorgsListeners(){
   let div = document.querySelector(".GITHorgsEdit");
   if(!div) return;
   div.addEventListener("click", GITHorgsClick);
}

function GITHorgsClick(e){
   if(e.target.closest(".SETaddRow"))    GITHaddOrgRow();
   if(e.target.closest(".SETdeleteRow")) GITHremoveOrgRow(e.target);
   if(e.target.closest(".GITHorgsSave")) GITHsetOrgs();
   if(e.target.closest(".GITHorgsCancel")) POPpopoutClose(GITHorgsCard);
}

function GITHsetOrgs(){
   let orgs = GITHgetOrgs();
   APIcall("git/organizations/set", orgs, function(R){
      if(!R?.success) return GITHorgsError(R?.error || "");
      POPpopoutClose(GITHorgsCard);
      MODELforcePoll("GITHmodel");
      let Message = GINsetOrggs(R?.data || []);
      GINaddAndCommit({message: Message, users: true});
   });
}

function GITHgetOrgs(){
   let orgs = [];
   let divs = GITHorgRows();
   if(!divs || divs.length == 0) return orgs;
   for(let i=0; i < divs.length; i++){
      let row = divs[i];
      let val = row.querySelector(".GITHorgsValue")?.value || "";
      if(val) orgs.push(val);
   }
   return orgs;
}