function PERupdateServers(Data){
   let div = document.querySelector(".PERrulesTarget");
   if(!div) return;
   let Rules;
   if(Data) {
      Rules = Data;
      PERserversData = undefined;
   } else {
      Rules = PERdata();
   }
   let Html = PERrulesHtml(Rules);
   if(!DEEPequal(PERserversData, Rules)) div.innerHTML = Html;
   PERserversData = Rules;
}

function PERreconnectHtml(Connected){
   if(Connected) return "";
   return /*html*/`
   <a class="BUTTONdefault BUTTONstandard PERconnect" title="Try to establish a connection with the server.">
      RECONNECT
   </a>
   `;
}

function PERstatusHtml(Connected, Guid){
   let Status = Connected ? " PERconnected" : "";
   let title= Connected ? "Active" : "Connection error";
   let Html = /*html*/`
   <div class="PERstatus${Status}" title="${title}">
      <a class="PERdetails" href="#settings/peers/details?peer=${Guid}">${Guid}</a>
   </div>`;
   return Html;
}

function PERrowsHtml(Rules){
   let Rows = "";
   for(let Guid in Rules){
      let Rule = Rules[Guid];
      let Connected = Rule?.connected;
      let DeleteUrl = `#settings/peers/delete?peer=${Guid}`;
      let Error = Rule?.error;
      let Button = PERreconnectHtml(Connected);
      let InfoHtml = Error ? `
      <div class="PERinline">
         <div class="PERspace">-</div>
         <div class="PERerror">${Error}</div>
      </div>` : "";
      Rows += /*html*/`
      <div class="PERrow">
         <div class="PERcell PERpaddingTop">${PERstatusHtml(Connected, Guid)}</div>
         <div class="PERmessage">${InfoHtml}</div>
         <div class="PERbuttons">
            ${Button}
            <a class="FORMbutton BUTTONcancel BUTTONdefault BUTTONwarning PERdeleteButton" href="${DeleteUrl}">
               ${SETtrashIcon()}
            </a>
         </div>
      </div>`;
   }
   return Rows;
} 

function PERrulesHtml(Rules){
   if(!Object.keys(Rules).length) return /*html*/ `<div class="SETemptyTable">No peers defined - click ADD to add to the list of peers</div>`;
   let Rows = PERrowsHtml(Rules);
   return /*html*/`
   <div class="PERruleTable">
      <div class="PERruleHeading">
         <div class="PERheading GSVpaddingLeft">Linked Peers</div>
         <div class="PERheading"></div>
      </div>
      ${Rows}
   </div>`;
}
