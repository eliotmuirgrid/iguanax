function PERdetails(){
   PERview();
   PERdetailsPopout();
}

function PERdetailsPopout(){
   if(document.querySelector(".PERdetails")) return;
   POPpopoutOpen({
      title: 'Peer details', 
      content: PERaddHtml(), 
      esc_close : true,
      width : "600px",
      callback : PERdefault
   });
   document.querySelector(".PERnew").addEventListener("click", PERnew);
   FORMfocus(document.querySelector(".PERnewAddress"));
   FORMenterListener(document.querySelector(".PERnewAddress"), PERnew);
}