function UPGRADEaccessErrorHtml(Url, Error){return /*html*/`
   <div class="UPGRADEaccessErrorDiv">
      <b><div class="UPGRADEerror">Error: </div> Access to ${Url} unavailable. <br> ${Error} </b>[<a href=${LINKwarningSecurityPatch} target="_blank">Learn more</a>]
   </div>`;
}

function UPGRADEdisplayAccessError(Url, Error){
   let Div = document.querySelector(".UPGRADEguidInputContainer");
   if(!Div) return;
   let Html = UPGRADEaccessErrorHtml(Url, Error);
   if(Div.innerHTML !== Html) Div.innerHTML = Html;
}

function UPGRADEcheckForAccess(){
   APIcall("upgrade/access", {}, function(R){
      if(!R.success) return;
      let Code  = R?.data?.code;
      let Error = R?.data?.error || "(Trouble capturing error details)";
      let Url   = R?.data?.url   || "(Trouble capturing the requested url)";
      if(Code !== 200) return UPGRADEdisplayAccessError(Url, Error)
   });
}

function UPGRADEcheckForPermission(){
   APIcall("component/permission", {}, function(R){
      if(!R.success) return;
      let Perm = R?.data?.upgrade;
      if(Perm) return;
      let Div = document.querySelector(".UPGRADEguidInputContainer");
      if(!Div) return;
      Div.innerHTML = "Error - You do not have permission to do this.";
   });
}

function UPGRADEupgradeOld(){
   COL_TRC("Upgrade screen."); 
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon('CORsettingsIcon');
   document.querySelector('.CORmain').innerHTML = 
   /*html*/`<div class="UPGRADEspecial">
	<div class="UPGRADEspecialContainer">
	<div class="UPGRADEmain">
   <h3>Upgrade Special</h3>
   <p>
   This screen allows you to upgrade to an unreleased snapshot build. 
   [<a target="_blank" href=${LINKupgradeBuild}>Learn more</a>]
   </p>
	<br/>
   <div class="UPGRADEguidInputContainer">
    Guid to upgrade to: <input class="FORMinput UPGRADEguid" type="text"><div class="BUTTONaction BUTTONstandard UPGRADEdoit">UPGRADE</div>
   </div>
   </div>
	</div>
	</div>`;

   document.querySelector(".UPGRADEdoit").addEventListener("click", function(E) { UPGRADEexecuteSpecial(); });
   COL_TRC("Set upgrade listener.");
   UPGRADEcheckForAccess();
   UPGRADEcheckForPermission();
   //APIcall("upgrade/replace", {"guid" : "3154141c8"}, function(R) { UPGRADEdone(R); });
}
