function CONFIRMaction(Action, Func, Button, Func2, callback, Options){
   var cb = callback;
   var options = Options;
   if (cb && typeof cb === "object" && !Array.isArray(cb)) {
      options = cb;
      cb = null;
   }
   options = (options && typeof options === "object") ? options : {};
   var destructive = (options.destructive === true || String(options.variant || "").toLowerCase() === "destructive");
   var label = String(Button || "CONFIRM");
   var actionClass = destructive
      ? "CONFIRMactionButton BUTTONstandard BUTTONaction BUTTONwarning CONFIRMactionButtonDestructive"
      : "CONFIRMactionButton BUTTONstandard BUTTONaction";

   let H = /*html*/`
   <div class="CONFIRMactionDescription">
   ${Action}
   </div>
	<div class="BUTTONgroup CONFIRMbuttonGroup">
      <div class="CONFIRMcancelButton BUTTONstandard BUTTONdefault">CANCEL</div>
      <div class="${actionClass}">${label}</div>
   </div>
   `;
   let Card = POPpopoutOpen({title : "Confirm Action", content : H, className : "CONFIRMmodal"});  
   document.querySelector('.CONFIRMcancelButton').addEventListener('click', function(){
      if(Func2) Func2();
      POPpopoutClose(Card);
   });
   document.querySelector('.CONFIRMactionButton').addEventListener('click', function(){
      Func();
      POPpopoutClose(Card);
   });
   if (typeof cb === "function") cb();
}
