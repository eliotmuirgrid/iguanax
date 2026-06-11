// TODO - refactor this
function CORconnectionRequestCardHtml(guid, description, passcode){
   return /*html*/`
   <div class="subsection-pane">
   <div id="passcode-list">
      {{#.}}<p>${description}(${guid}) is trying to connect, passcode is: <b>${passcode}</b></p>{{/.}}
   </div>
   </div>
   <div id="dismiss_btn" class="BUTTONaction BUTTONstandard">Dismiss</div>
   <div id="clear_btn" class="BUTTONcancel BUTTONstandard">Clear All</div>`;
}