let GINrestoreCard;

function GINrestoreHtml(){
   return /*html*/`
   <div class="FORMlabel GINmarginTop">Reset Instance:</div>
   <div class="GINrestoreValue GINmarginTop">This will replace the current configuration with the repository contents.</div>
   <div class="GINeditRestore BUTTONgroup"></div>`;   
}


function GINrestore(){
   GINrestoreCard = POPpopoutOpen({
      title: "Reset Instance",
      width: "650px",
      content: GINeditRestoreHtml(),
		disabledEscape: true,
   });
   let div = document.querySelector(".GINrestoreURL");
   FORMfocus(div);
   FORMenterListener(div, GINrestoreConfirm);
   div = document.querySelector(".GINrestoreModal");
   div.addEventListener("click", GINrestoreClick);
}

function GINrestoreClick(e){
   if(e.target.closest(".GINrestoreCancel"))  return GINrestoreClose();
   if(e.target.closest(".GINrestoreConfirm")) return GINrestoreConfirm();
}

function GINSvalidateCheckboxes() {
	const checkboxValidations = [
		{
			selector: ".GINrestoreWarningCheckbox",
			message: "You must confirm that you understand that this will reset the instance and delete all data.",
		},
		{
			selector: ".GINrestoreWarningCheckbox2",
			message:
				"You must confirm you have set the Push/Pull user to one with the correct permissions to the repository.",
		},
		{
			selector: ".GINrestoreWarningCheckbox3",
			message: "You must confirm you understand components and libraries without an upstream will lose their code.",
		},
	];

	for (const validation of checkboxValidations) {
		const checkbox = document.querySelector(validation.selector);
		if (!checkbox?.checked) {
			GINrestoreError(validation.message);
			return false;
		}
	}
	return true;
}

function GINSupdateButtonStates(disable) {
	const confirmbtn = document.querySelector(".GINrestoreConfirm");
	const cancelbtn = document.querySelector(".GINrestoreCancel");
	if (disable) {
		confirmbtn.classList.add("BUTTONdisabled");
		confirmbtn.classList.remove("BUTTONaction");
		cancelbtn.classList.add("BUTTONdisabled");
		const closeBtn = document.querySelector(".POPpopoutCard--close");
		if (closeBtn) closeBtn.remove();
	} else {
		confirmbtn.classList.remove("BUTTONdisabled");
		confirmbtn.classList.add("BUTTONaction");
	}
}

function GINcreateStatusDiv() {
	const statusDiv = document.createElement("div");
	statusDiv.className = "GINrestoreStatus";
	const spinner = document.createElement("div");
	spinner.className = "CARDloading-spinner";
	const statusText = document.createElement("div");
	statusText.className = "GINrestoreStatusText";
	statusText.textContent = "Resetting instance - this may take several minutes depending on repository size";
	statusDiv.appendChild(statusText);
	statusDiv.appendChild(spinner);
	document.querySelector(".GINrestoreProgress").appendChild(statusDiv);
}

function GINrestoreConfirm() {
	if (!GINSvalidateCheckboxes()) return;
	const confirmbtn = document.querySelector(".GINrestoreConfirm");
	if (confirmbtn && confirmbtn.classList.contains("BUTTONdisabled")) {
		return;
	}
	const div = document.querySelector(".GINrestoreURL");
	const URL = div.value;
	if (!URL) return;

	GINSupdateButtonStates(true);
	GINcreateStatusDiv();

	APIcall("instance/reset", { git: URL }, function (Response) {
		if (Response.error){
			GINSupdateButtonStates(false);
			const statusDiv = document.querySelector('.GINrestoreStatus');
			if (statusDiv) statusDiv.remove();
			return GINrestoreError(Response.error);
		}

		GINrestorePollerStart();
	});
}

function GINrestoreErrorConfirm(error) {
	GINrestoreError(error + "<br><div>A restart is still required.</div>");
	const confirmbtn = document.querySelector(".GINrestoreConfirm");
	const cancelbtn = document.querySelector(".GINrestoreCancel");
	cancelbtn.remove();
	let div = document.querySelector(".GINrestoreURL");
	FORMenterListener(div, GINrestart);
	confirmbtn.removeEventListener('click', GINrestoreConfirm);
	confirmbtn.addEventListener('click', GINrestart);
}

function GINrestoreClose() {
	const cancelbtn = document.querySelector(".GINrestoreCancel");
	if (cancelbtn && cancelbtn.classList.contains("BUTTONdisabled")) {
		return;
	}
	POPpopoutClose(GINrestoreCard);
}

function GINrestoreError(error){
   let div = document.querySelector(".GINrestoreError");
   if(!div) return;
   div.innerHTML = error;
}  

function GINeditRestoreHtml(){
   return /*html*/`
   <div class="GINrestoreModal">
      <b>This is a beta feature and may not work as expected.</b>
      <br>
      <br>
      <div class="GINrestoreText">This will replace the current configuration with the repository contents.</div>
      <br>
      <a class="GINrestoreLink" href="${LINKsettingsResetInstance}" target="_blank">Read more about the Reset Instance feature.</a>
      <br>
      <br>
      <div class="GINrestoreWarning">
         <input type="checkbox" class="GINrestoreWarningCheckbox FORMinput">
         <div class="GINrestoreWarningText">I confirm that I understand that this will reset the instance and delete all data.</div>
         <input type="checkbox" class="GINrestoreWarningCheckbox2 FORMinput">
         <div class="GINrestoreWarningText">I have set the Push/Pull user to one with the correct permissions to the repository.</div>
         <input type="checkbox" class="GINrestoreWarningCheckbox3 FORMinput">
         <div class="GINrestoreWarningText">I understand components and libraries without an upstream will lose their code.</div>
      </div>   
      <br>
      <br>
      <div class="GINrestoreGrid">
         <div class="FORMlabel GINgitURL GINmarginTop">Git URL:</div>
         <input type="text" class="GINrestoreURL FORMinput" placeholder="">
      </div>
      <br>
      <div class="GINrestoreError"></div>
      <br>
		<div class='GINrestoreProgress'></div>
      <div class="GINeditRestore BUTTONgroup">
         <div class="GINrestoreCancel BUTTONdefault BUTTONstandard">CANCEL</div>
         <div class="GINrestoreConfirm BUTTONaction BUTTONstandard">CONFIRM</div>
      </div>
   </div>`;   
}