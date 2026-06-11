function GSVlisteners(){
   let div = document.querySelector(".GSVmodal");
   if(!div) return;
   div.removeEventListener("click",  GSVclickListeners);
   div.addEventListener   ("click",  GSVclickListeners);
   div.removeEventListener("change", GSVchangeListeners);
   div.addEventListener   ("change", GSVchangeListeners);
}

function GSVclickListeners(e){
   if(e.target.closest(".GSVadd"))    return GSVaddRow();
   if(e.target.closest(".GSVdelete")) return GSVdeleteRow(e.target);
   if(e.target.closest(".GSVsave"))   return GSVsave();
   if(e.target.closest(".GSVresetToDefaultLink")) return GSVresetToDefault();
}

function GSVchangeListeners(e){
   if(e.target.closest(".GSVprovider")) return GSVproviderChanged(e.target);
}

function GSVresetToDefault(){
   let ExistingData = GSVgetData();
   let DefaultData = GSVdefaultData();
   GSVupdateServers({...DefaultData, ...ExistingData});
}

function GSVaddRow(){
   let div = document.querySelector(".GSVruleTable");
   if(!div) return;
   SETappendTo(div, GSVeditRowHtml("", {secure : true}));
   SETfocusLast(".GSVaddress");
}

function GSVdeleteRow(target){
   let row = target.closest(".GSVeditRuleRow");
   if(row) row.remove();
}

function GSVgetData(){
   const GSVruleRows = document.querySelectorAll('.GSVeditRuleRow');
   const Data = {};
   GSVruleRows.forEach(function(row) {
      const Address = row.querySelector('.GSVaddress').value.trim();
      let Entry = {};
      let Protocol = row.querySelector('.GSVprotocol').value.trim();
      let Secure = Protocol === "https" ? true : false;
      Entry.secure = Secure;
      let Provider = row.querySelector('.GSVprovider').value.trim();
      Entry.type = Provider;
      const Http_port = row.querySelector('.GSVhttpPort').value.trim() || 0;
      Entry.http_port = Http_port;
      const Ssh_port  = row.querySelector('.GSVsshPort').value.trim() || 0;
      Entry.ssh_port  = Ssh_port;
      if(Address != "") Data[Address] = Entry;
   });
   return Data;
}

function GSVsave(){
   let Data = GSVgetData();
   APIcall('git/servers/save', Data, function(R) {
      if(!R.success) { SNCKsnackbar('error', R?.error); }
      else           { 
         let Message = GINsaveServers(Data);
         GINaddAndCommit({message: Message, settings: true}); 
      }
      window.location = "#settings/git_servers";
   });
}

function GSVproviderChanged(target){
   let Row = target.closest(".GSVeditRuleRow");
   if(!Row) return;
   let div = target.closest(".GSVprovider");
   if(div?.value === "Server - Bitbucket") return GSVproviderBitbucketServer(Row);
   else GSVproviderElse(Row);
}

function GSVproviderBitbucketServer(Row){
   let HttpDiv = Row.querySelector(".GSVhttpPort");
   let HttpPort = parseInt(HttpDiv?.value || "0", 10);
   if(HttpPort == 0) HttpDiv.value = 7990;
   let SshDiv = Row.querySelector(".GSVsshPort");
   let SshPort = parseInt(SshDiv?.value || "0", 10);
   if(SshPort == 0) SshDiv.value = 7999;
}

function GSVproviderElse(Row){
   let HttpDiv = Row.querySelector(".GSVhttpPort");
   let HttpPort = parseInt(HttpDiv?.value || "7990", 10);
   if(HttpPort == 7990) HttpDiv.value = 0;
   let SshDiv = Row.querySelector(".GSVsshPort");
   let SshPort = parseInt(SshDiv?.value || "7999", 10);
   if(SshPort == 7999) SshDiv.value = 0;
}