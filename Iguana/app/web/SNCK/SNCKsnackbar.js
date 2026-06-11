// Create a snackbar to display feedback to the user.
// Type is one of warning, error, info or success and controls the appearance of the snackbar.
// Message is the text to be displayed.
// Fade is the lifetime (in milliseconds) of the snackbar before it fades out. Setting Fade to 0, or creating an error type snackbar
// means that the snackbar will persist and must be closed manually (default behaviour).

function SNCKgenerateGuid(){ 
   let X = "EDI" + ([1e7]+-1e3+-4e3+-8e3+-1e11).replace(/[018]/g, c => (c ^ crypto.getRandomValues(new Uint8Array(1))[0] & 15 >> c / 4).toString(16) ); 
   COL_TRC('Generated GUID=', X);
   return X;
}

function SNCKsnackbarHtml(Params){
   return /*html*/`
   <div id="${Params.Id}" class="SNCKsnackbar SNCKsnackbar--${Params.Type}">
      <div class="SNCKmessage"></div>
      <div class="SNCKclosePanel">
         <svg class="SNCKsnackbarCloseIcon" width="16" height="16" viewBox="0 0 16 16">
            <use href="SNCK/SNCKiconClose.svg#SNCKiconClose"></use>
         </svg>
      </div>
   </div>`;
}

function SNCKsnackbar(Type, Message, Fade = 5000, EscMessage=true) {
   COL_TRC(Message, "Type=", Type, "Fade=", Fade);
   const Id = SNCKgenerateGuid();
   if(Type !== 'warning' && Type !== 'error' && Type !== 'info' && Type !== 'success') { Type = 'info'; }
   let html = SNCKsnackbarHtml({Id: Id, Type: Type});
   let div = document.querySelector('.CORmain');
   if(!div) div = document.querySelector('.DOCmain');
   div.insertAdjacentHTML('beforeend', html);
   let MessageHtml = EscMessage ? ESChtmlEscape(Message) : Message;
   document.querySelector('#' + Id + ' > .SNCKmessage').innerHTML = MessageHtml;
   document.querySelector('div#' + Id + ' > .SNCKclosePanel').addEventListener('click', function() { SNCKdismissSnackbar(Id); });
   if(Fade !== 0 && Type !== "error") { 
      setTimeout(function() {
         const element = document.getElementById(Id);
         if(element) element.style.transition = 'opacity 2s';
         if(element) element.style.opacity = '0';
         setTimeout(function() { SNCKdismissSnackbar(Id); }, 2000);
      }, Fade);
   }
}

function SNCKdismissSnackbar(Id) { 
   const element = document.getElementById(Id);
   if(element) element.remove();
}
