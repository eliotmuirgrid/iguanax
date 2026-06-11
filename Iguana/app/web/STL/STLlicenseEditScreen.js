function SETlicenseEdit(){
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   div.innerHTML = SETlicenseEditHtml();
   let Response = STLlicenseData();
   SETupdateIguanaId(Response?.data?.iguana_id);
   FORMfocus(document.querySelector('.SETlicenseInput'));
}

function SETlicenseEditHtml(){
   return /* html */ `
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETlicenseTitle">
         <div class="SETlicenseMainTitle">License</div>
         <div class="SETlicenseActions BUTTONgroup">
            <div class="SETlicenseUpdate BUTTONaction BUTTONstandard">SAVE</div>
            <div class="SETlicenseCancelUpdate BUTTONcancel BUTTONstandard">CANCEL</div>
         </div>
      </div>
      <div class="SETpageDescription">
         Log in to your iNTERFACEWARE Members Account to retrieve a license. [<a href="https://my-iguana.interfaceware.com/?a=#Page=getlicense" target="_blank">Retrieve license</a>] or [<a href="${LINKsettingsLicenseHeader}" target="_blank">Learn more</a>]
      </div>
      ${SETiguanaIdHtml()} 
      <br>
      <div class="SETlicenseLabel SETlicenseSection">License Code:</div> 
      <textarea class="SETlicenseInput FORMinput"></textarea>
      <br>
      <br>
      <div class="SETlicenseInfoDiv">Please copy and paste the<a class="SETlink" href="${LINKsettingsLicenseHeader}" target="_blank">license code</a>you have received from iNTERFACEWARE into the area above and click <b>Save</b>.</div>
      <div class="SETnewLicenseError"></div>
   </div>`;
}

function SETlicenseUpdate(){
   let Data = {}
   let LicenseCodeInput = document.querySelector('.SETlicenseInput');
   if (LicenseCodeInput) { Data["license"] = LicenseCodeInput.value.trim(); }
   APIcall('license/set', Data, function(R){
      if (R.success) { 
         STLlicenseScreen(); 
         let Message = GINupdateLicense();
         GINaddAndCommit({message: Message, other: true});
      }
      else { document.querySelector('.SETnewLicenseError').innerHTML = R.error; }
   })
}
