function GINupdateKey() {
   let div = document.querySelector(".GINkey");
   if(div) {
      div.innerHTML = GINkeyHtml();
   }
}

function GINkeyHtml(){
   return /*html*/`
   <div class="FORMlabel GINmarginTop">Track Private Keys:</div>
   <div class="GINkeyValue GINmarginTop">${GINtrackKeys() === true ? "Disabled" : "<div class='GINvalueEmpty'>Enabled</div>"} - <i>Disabling is strongly recommended for security.</i></div>
   <div class="GINeditKey BUTTONgroup"></div>`;
}

function GINeditKeyTracking() {
   POPpopoutOpen({
      title: "Track Private Keys (disabled – recommended)",
      width: "600px",
      content: GINeditKeyTrackingHtml()
   });
   GINeditKeyTrackingListeners();
}

function GINeditKeyTrackingListeners() {
   let selectbox      = document.querySelector(".GINtrackSelect");
   let cancelButton  = document.querySelector(".GINkeyTrackingCancel");
   let confirmButton = document.querySelector(".GINkeyTrackingConfirm");
   cancelButton.addEventListener("click", () => { POPpopoutRemoveAll(); });
   confirmButton.addEventListener("click", () => {
      let ignore = selectbox?.value == "No";

      APIcall('instance/set_key_tracking', {ignore: ignore}, (R) => {
         if(!R.success) {
            return SNCKsnackbar('error', R.error);
         }
         POPpopoutRemoveAll();
         MODELforcePoll('GINmodel');
      });
   });
}

function GINeditKeyTrackingHtml() {
   let Track = GINtrackKeys() === false;
   let YesSel =  Track ? " selected" : "";
   let NoSel  = !Track ? " selected" : "";
   return /*html*/`
   <div class="GINeditKeyTrackingModal">
   <div>Choose whether private-key files should be committed to this repository.</div>
   <br>
   <div class="GINtrackKeysDiv">
      <div class="GINmarginTop FORMlabel">Include private-key files:</div>
      <div>
         <select class="FORMselectBox GINtrackSelect">
            <option value="Yes"${YesSel}>Yes – Include keys ⚠︎</option>
            <option value="No"${NoSel}>No – Exclude keys (recommended)</option>
         </select>
      </div>
      </div>
      <br>
      <br>
      <div class="GINgrey">When “No” is selected, key files will be ignored via .gitignore to prevent accidental exposure.</div>
      <br>
      <div class="GINrulesDiv">
<pre class="GINkeyValue GINmarginTop">
**/*.key
**/*.pem
**/*.ppk</pre>
<pre class="GINkeyValue GINmarginTop">
**/id_rsa
**/id_dsa
**/id_ecdsa</pre>
<pre class="GINkeyValue GINmarginTop">
**/id_ed25519
**/private_key
**/privatekey</pre>
      </div>
      <br>
      <br>
      <br>
      <div class="GINeditUserButtons BUTTONgroup">
         <div class="GINkeyTrackingCancel BUTTONstandard BUTTONdefault">CANCEL</div>
         <div class="GINkeyTrackingConfirm BUTTONstandard BUTTONaction">CONFIRM</div>
      </div>
   </div>
   `;
}

