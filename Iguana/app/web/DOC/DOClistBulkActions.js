function DOCbulkActions(){
   const BulkActions = document.createElement("div");
   BulkActions.classList.add("DOCbulkActions");
   document.querySelector(".DOCtree").appendChild(BulkActions);
   document.querySelector(".DOCbulkActions").innerHTML=`
   <div class="BUTTONaction BUTTONstandard DOCbulkAddTag"> ADD TAG </div>
   <div class="BUTTONaction BUTTONstandard DOCbulkDelTag"> REMOVE TAG </div>
   `;
}
