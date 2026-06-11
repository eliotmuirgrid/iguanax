function USERdeleteHtml(UserObject) {
   let Roles = HASHTAGrenderTags(UserObject.roles);
   return /*html*/`
   <div class="USERaddUserCard">
      <div class="">
         <div class="USERreadDiv">
            <div class="SETuserInfo">Username:</div>
            <div class="USERname USERreadOnly SETpageDescription">${ESChtmlEscape(UserObject.username)}</div>
         </div>
         <div class="USERreadDiv">
            <div class="SETuserInfo">Roles:</div>
            <div class="USERtags SETpageDescription">
            ${Roles ? Roles : "<i>None</i>"}
            </div>
         </div>
      </div>
      <div class="USERdeleteMessage">User account: <b>${ESChtmlEscape(UserObject.username)}</b> will be deleted permanently</div>
      <br>
      <div class="USERbuttonGroup">
         <div class="USERcancel USERbutton USERbuttonCancel">CANCEL</div>
         <div class="USERdelete USERbutton USERbuttonWarning">DELETE</div>
      </div>
   </div>`;
};

function USERdelete(Username, Card) {
   APIcall('user/delete', {'username': Username}, function(Response) {
      MODELforcePoll('USERmodel');
      if(!Response.success) { return SNCKsnackbar('error', Response['error'], 6000); }
      POPpopoutClose(Card);
      let Message = GINdeleteUser(Username);
      GINaddAndCommit({message: Message, users: true});
   });
}

function USERdeleteCard(UserObject) {
   let Card = POPpopoutOpen({title: 'Delete User', content: USERdeleteHtml(UserObject), esc_close : true});
   document.querySelector('.USERdelete').addEventListener('click', function(){USERdelete(UserObject.username, Card)});
   document.querySelector('.USERcancel').addEventListener('click', function(){POPpopoutClose(Card); });
}