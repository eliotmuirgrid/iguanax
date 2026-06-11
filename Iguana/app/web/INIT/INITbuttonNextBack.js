// Helpful to have back/next button routines here.

function INITcreateBtnGroupAndStatusDivs(NextText) {
   if(!NextText) { 
      NextText = 'NEXT &#10132;'; 
      COL_TRC("Using default ", NextText);
   }
   return `
<div class="INITfooter">
	<div class="INITsuccess"></div>
	<div class="INITerror"></div>
	<div class="INITbtnGroup">
		<div class="INITbackBtn BUTTONstandard BUTTONdefault BUTTONaction">BACK</div>
		<div class="INITnextBtn BUTTONstandard BUTTONdisabled">${NextText}</div>
	</div>
</div>`
}

function INITbackNextBtns(backHash, nextHash) {
   INITsetBackBtn(backHash);
   INITsetNextBtn(nextHash);
}

function INITsetBackBtn(backHash){
   let BackBtn = document.querySelector('.INITbackBtn');
   if(!BackBtn) { return; }
   BackBtn.addEventListener('click', function() { window.location.hash = backHash; })
}

function INITsetNextBtn(nextHash){
   let NextBtn = document.querySelector('.INITnextBtn');
   if(!NextBtn) { return; }
   NextBtn.addEventListener('click', function() {
      if(NextBtn.classList.contains('BUTTONdisabled')) { return; }
      window.location.hash = nextHash;
   })
}

function INITdisableNext(){
   let NextBtn = document.querySelector('.INITnextBtn');
   if(!NextBtn) { return; }
   NextBtn.classList.add('BUTTONdisabled');
   NextBtn.classList.remove('BUTTONaction');
}

function INITenableNext() {
   let NextBtn = document.querySelector('.INITnextBtn');
   if(!NextBtn) { return; }
   NextBtn.classList.remove('BUTTONdisabled');
   NextBtn.classList.add('BUTTONaction');
}

function INITbuttonNextEnabled(Enabled){
   let NextBtn = document.querySelector('.INITnextBtn');
   // Assume this function only get's called by the right place so no check - errors are helpful for seeing flaws in app logic.
   if (Enabled){
      if (NextBtn.classList.contains('BUTTONdisabled')){
         COL_TRC("Enabled =", Enabled);
         COL_TRC("Contains button disabled status =", NextBtn.classList.contains('BUTTONdisabled'));
         COL_TRC("Enabling next button.");
         NextBtn.classList.remove('BUTTONdisabled');
         NextBtn.classList.add('BUTTONaction');
         COL_TRC("Contains button disabled status =", NextBtn.classList.contains('BUTTONdisabled'));
      }
      return;
   } else {
      if (NextBtn.classList.contains('BUTTONaction')){
         COL_TRC("Disabling next button.");
         NextBtn.classList.add('BUTTONdisabled');
         NextBtn.classList.remove('BUTTONaction');
      }
   }
  
}