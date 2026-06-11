let NRLcard;

function NRLnewComponentUrlScreen(){
   DASHdashboardScreen();
	if(document.querySelector('.NRLmodal')) return;
   NRLcard = POPpopoutOpen({
      title: 'Add component', 
      width: '800px', 
      content: NRLhtml(),  
      esc_close : true,
      callback : DASHcloseModal
   });
   NRLlisteners();
   NRLmodelInit();
   FORMfocus(document.querySelector(".NRLgit"));
   FORMenterListener(document.querySelector(".NRLcontainer"), NRLaddComponent);
}

function NRLhtml(){
   return /*html*/`
	<div class="NRLmodal">
      <div class="NRLheader">FROM GIT URL:</div>
      <br>
      <div class="NRLcontainer">
         <input class="NRLgit FORMinput" placeholder="Git url" title="Git url of the repository you are wanting to create a component from.">
         <div class="NRLrepoFeedback"></div>
         <input class="NRLbranch FORMinput" placeholder="Branch (optional)" title="Branch of the repository you are wanting to create a component from (optional).">
         <input class="NRLname FORMinput" placeholder="Name" title="Name of the component you are creating.">
         <div class="NRLnameExistsFeedback"></div>
         <textarea class="NRLdescription" placeholder="Description - what does this component do?" title="Description of the component you are creating."></textarea>
         <input class="NRLtags FORMinput" placeholder="Tags - what groups does this belong to?">
         <br>
      </div>
      <div class='NRLbottomRow'>
         <div class='NRLworking'>Working...<div class="SPINspinner NRLspinner"></div></div>
         <div class="BUTTONgroup NRLbuttonsRow">
            <div class="NRLclose BUTTONstandard BUTTONdefault">CANCEL</div>
            <div class="NRLadd BUTTONdisabled BUTTONstandard">ADD</div>
         </div>
      </div>
	</div>`;
}

function NRLrenderGitError(){
   let div = document.querySelector(".NRLmodal");
   if(!div) return;
   let Html = GITHerrorHtml("create a component");
   div.innerHTML = Html;
}
