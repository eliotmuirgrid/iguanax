function ENVeditableButtonHtml(showDelete){
   let Editable = ENVcreatePermission();
   let Classes =  Editable ? "ENVsave BUTTONaction" : "BUTTONdisabled";
   let title   = !Editable ? `title="You do not have permission to do this."` : "BUTTONdisabled";
   return /*html*/`
   <div class="${Classes} BUTTONstandard" ${title}>SAVE</div>
   <div class="ENVcancel BUTTONstandard BUTTONcancel">CANCEL</div>
   ${showDelete && Editable? /*html*/`<div class="ENVdelete BUTTONstandard BUTTONcancel BUTTONwarning">DELETE</div>` : ''}`;
}

function ENVeditHtml(key, value, showDelete) {
   return /*html*/`
   <div class="ENVeditCard">
      <div class="ENVeditForm">
         <div class="ENVformFullWidth">
            <div class="ENVinfo">Name:</div>
            <input class="FORMinput" type="text" value="${ESChtml(key)}"/>
            <div class="ENVfeedback"></div>
         </div>
         <div class="ENVformFullWidth">
            <div class="ENVinfo">Value:</div>
            <input class="FORMinput" type="text" value="${ESChtml(value)}"/>
            <div class="ENVfeedback"></div>
         </div>
      </div>
      <div class="ENVbuttonGroup">
         ${ENVeditableButtonHtml(showDelete)}
      </div>
      <div class="ENVapiFeedback"></div>
   </div>`;
}

function ENVcardActions(envCard, originalKey) {
   const $saveBtn = document.querySelector('.ENVsave');
   const $cancelBtn = document.querySelector('.ENVcancel');
   const $deleteBtn = document.querySelector('.ENVdelete');

   if($saveBtn)  $saveBtn.addEventListener('click',   function() { ENVsetLocal(envCard, originalKey); })
   if($cancelBtn)$cancelBtn.addEventListener('click', function() { POPpopoutClose(envCard) });
   if($deleteBtn) { $deleteBtn.addEventListener('click', function() { ENVdeleteLocal(envCard, originalKey); }) }
}

function ENVsetCardApiFeedback(feedback) { return document.querySelector('.ENVapiFeedback').textContent = feedback; }