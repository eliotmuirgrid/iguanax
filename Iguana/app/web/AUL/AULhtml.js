function AULhtml(){
   return /*html*/`
   <div class="AULmodal">
      <div class="AULhelp">
         Auto Layout requires an #autoorganize utility. Install the official Auto Layout utility below, or add your own.
      </div>
      <div class="AULcontainer NRLcontainer">
         <input class="AULgit FORMinput" placeholder="Git url" title="Git url of the repository you are wanting to create the Auto Layout utility from.">
         <div class="AULrepoFeedback NRLrepoFeedback"></div>
         <input class="AULbranch NRLbranch FORMinput" placeholder="Branch (optional)" title="Branch of the repository you are wanting to create the Auto Layout utility from (optional).">
         <input class="AULname NRLname FORMinput" placeholder="Name" title="Name of the Auto Layout utility you are creating.">
         <div class="AULnameExistsFeedback NRLnameExistsFeedback"></div>
         <textarea class="AULdescription NRLdescription" placeholder="Description - what does this utility do?" title="Description of the Auto Layout utility you are creating."></textarea>
         <div class="AULtagsRow FORMinput" title="Required tags are locked. You can add optional tags.">
            <span class="AULtagToken">#autoorganize</span>
            <span class="AULtagToken">#run</span>
            <input class="AULtagsExtra" spellcheck="false" placeholder="Optional tags">
         </div>
      </div>
      <div class="AULbottomRow">
         <div class="AULworking">Working...<div class="SPINspinner AULspinner"></div></div>
         <div class="BUTTONgroup AULbuttonsRow">
            <div class="AULclose BUTTONstandard BUTTONdefault">Cancel</div>
            <div class="AULadd BUTTONdisabled BUTTONstandard">Add & Run</div>
         </div>
      </div>
   </div>`;
}
