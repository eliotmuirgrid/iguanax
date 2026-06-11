function STLlicenseScreen(){
   SETrender();
	document.querySelector('.STLlicense').classList.add('selected');
   STLmodelInit();
   STLlisteners();
}

function STLrenderLicense(){
   let Response = STLlicenseData();
   if(Response["success"]){ SETlicenseUpdateView(Response.data); } 
   else {
      document.querySelector('.SETlicenseInfo').innerHTML = "Error: unable to retrieve any license or ID information."
   }
}

function SETupdateIguanaId(Id){
   let IdSelector = document.querySelector('.SETlicenseIguanaId');
   if (IdSelector) IdSelector.innerHTML = ESChtmlEscape(Id);
}

function SETshowLicenseError(ErrorMessage){
   return /*html*/ `<div class="SETlicenseError">${ESChtmlEscape(ErrorMessage)}</div>`;
}

// Either renders the license detail table or renders the license error message
function SETlicenseUpdateView(Data){
   SETupdateIguanaId(Data.iguana_id);
   let InsertPos = document.querySelector('.SETlicenseContent');
   if (Data.license_error) {
      InsertPos.innerHTML = SETshowLicenseError(Data.license_error);
   }
   else if (Data.license && Data.license.license_status == "No license found") {
      InsertPos.innerHTML = SETshowLicenseError(Data.license_message);
   }
   else if (Data.license && Data.license.license_status == "License expired")  {
      InsertPos.innerHTML = SETshowLicenseError(Data.license_message) +  SETlicenseDataTableHtml(Data);
   }
   else {
      InsertPos.innerHTML = SETlicenseDataTableHtml(Data); + SETlicenseDisplayMessage(Data);
   }
}

function STLeditButtonHtml(){
   let Edit = STLeditPermission();
   let Classes =  Edit ? "SETeditLicense BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? `title="You do not have permission to edit this."` : "";
   return /*html*/`<div class="${Classes} BUTTONstandard" ${title}>EDIT</div>`;
}

function SETiguanaIdHtml(){
   return /* html */ `
   <div class="SETlicenseTable">
      <div class="SETlicenseTableRow">
         <div class="SETlicenseTableData">Iguana Id</div>
         <div class="SETlicenseTableData">
            <div class="ITMcopyFlexDivTable">
               <div class="ITMcopyContentIcon"></div>
               <div class="ITMcopyContent">Copy</div>
            </div>
            <div class="SETlicenseIguanaId"></div>
         </div>
      </div>
   </div>`;
}

function SETlicenseHtml(){
   return /* html */ `
   <div class="SETsettingsPaneLayout SETsettingsPane">
      <div class="SETlicenseTitle">
         <div class="SETlicenseMainTitle">License</div>
         <div class="SETlicenseActions BUTTONgroup">
            ${STLeditButtonHtml()}
         </div>
      </div>
      <div class="SETpageDescription">Log in to your iNTERFACEWARE Members Account to retrieve a license. [<a href="https://my-iguana.interfaceware.com/?a=#Page=getlicense" target="_blank">Retrieve license</a>] or [<a href="${LINKsettingsLicenseHeader}" target="_blank">Learn more</a>]</div>
      ${SETiguanaIdHtml()} 
      <br>
      <div class="SETlicenseLabel SETlicenseSection">License Details</div> 
      <div class="SETlicenseContent"></div>
   </div>`;
}

function STLformatTimeStampCustom(date){
   // We need to use the low level methods to format the date to "yyyy-MM-ddThh:mm"
   // Can't rely on methods as they don't give the right format, or in the case of toISOString, give UTC time.
   const year = date.getFullYear();
   const month = (date.getMonth() + 1).toString().padStart(2, '0'); // Months are zero-based
   const day = date.getDate().toString().padStart(2, '0');
   const hours = date.getHours().toString().padStart(2, '0');
   const minutes = date.getMinutes().toString().padStart(2, '0');
   return `${year}-${month}-${day} ${hours}:${minutes}`;
}

function STLprintDaysLeft(Timestamp){
   const Now = new Date();
   const Difference = Timestamp * 1000 - Now.getTime();
   if (Difference < 0){
      return STLformatTimeStampCustom(new Date(Timestamp*1000)) + " (Expired)";
   }
   // Calculate the number of days
   const DaysDifference = Math.floor(Difference / (24 * 60 * 60 * 1000));
   let Out = STLformatTimeStampCustom(new Date(Timestamp*1000)) + " (" + DaysDifference + " Day" + (DaysDifference != 1 ? "s" : "") + ")";
   return Out;
}

function SETlicenseDataTableHtml(Data){
   let federatedDashboard = STLfederatedDashboardEnabled(Data) ? "Enabled" : "Disabled";
   let TableHtml =  /*html*/`
   <div class="SETlicenseTable SETlicenseData">
      <div class="SETlicenseTableRow">
         <div class="SETlicenseTableData">License Expiry</div>
         <div class="SETlicenseTableData">${Data.license.expiry == 0 ? "No Expiry" : STLprintDaysLeft(Data.license.expiry) }</div>
      </div>
      <div class="SETlicenseTableRow">
         <div class="SETlicenseTableData">Maximum Number of Components</div>
         <div class="SETlicenseTableData">${Data.license.component_limit == 0 ? "Unlimited" : Data.license.component_limit }</div>
      </div>
      <div class="SETlicenseTableRow">
         <div class="SETlicenseTableData">Log Search Limit</div>
         <div class="SETlicenseTableData">${Data.license.search_limit == 0 ? "Unlimited" : Data.license.search_limit + " Days" }</div>
      </div>
      <div class="SETlicenseTableRow">
         <div class="SETlicenseTableData">Single-Sign-On (SSO)</div>
         <div class="SETlicenseTableData">${Data.license.single_sign_on === true ? "Enabled" : "Disabled" }</div>
      </div>
            <div class="SETlicenseTableRow">
         <div class="SETlicenseTableData">High Availability (HA)</div>
         <div class="SETlicenseTableData">${Data.license.high_availability === true ? "Enabled" : "Disabled" }</div>
      </div>
      <div class="SETlicenseTableRow">
         <div class="SETlicenseTableData">Command Center</div>
         <div class="SETlicenseTableData">${federatedDashboard}</div>
      </div>
   </div>
`
   return TableHtml;
}

function STLfederatedDashboardEnabled(Data) {
   if (typeof CORglobalDashboardLicenseStateFromPayload === "function") {
      return CORglobalDashboardLicenseStateFromPayload(Data) === true;
   }
   return Data?.license?.global_dashboard === true;
}

function SETlicenseDisplayMessage(Data){
   return `<div class="SETlicenseMessage">${Data.license_message}</div>`
}

function STLlicenseCopy() {
   const IdSelector = document.querySelector('.SETlicenseIguanaId');
   if(!IdSelector) { return; }
   // Get only the text node content, excluding child elements
   const idText = IdSelector.childNodes[0].nodeValue.trim();
   ITMsafeCopy(idText, IdSelector.closest(".SETlicenseTableData"));
}
