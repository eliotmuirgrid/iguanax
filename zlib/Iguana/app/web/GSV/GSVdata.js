let GSVserversData;

function GSVdefaultData(){
   return {
      "bitbucket.org": {
          "http_port": 0,
          "secure": true,
          "ssh_port": 0,
          "type": "Cloud - Bitbucket",
          "web": "https://bitbucket.org"
      },
      "dev.azure.com": {
          "http_port": 0,
          "secure": true,
          "ssh_port": 0,
          "type": "Cloud - Azure Devops",
          "web": "https://dev.azure.com"
      },
      "github.com": {
          "http_port": 0,
          "secure": true,
          "ssh_port": 0,
          "type": "Cloud - GitHub",
          "web": "https://github.com"
      },
      "gitlab.com": {
          "http_port": 0,
          "secure": true,
          "ssh_port": 0,
          "type": "Cloud - GitLab",
          "web": "https://gitlab.com"
      }
   }
}

function GSVupdateServers(Data){
   let div = document.querySelector(".GSVrulesTarget");
   if(!div) return;
   let Rules;
   if(Data) {
      Rules = Data;
      GSVserversData = undefined;
   } else {
      Rules = GSVdata();
   }
   let Html = GSVrulesHtml(Rules);
   if(!DEEPequal(GSVserversData, Rules)) div.innerHTML = Html;
   GSVserversData = Rules;
}

function GSVpurgeRuleRowHtml(Link, Data) {
   const Protocol = Data.secure ? "https" : "http";
   const Type = Data.type;
   let href = Data?.web;
   let HttpPort = Data?.http_port ? Data.http_port : "";
   let SshPort  = Data?.ssh_port  ? Data.ssh_port  : "";
   return /*html*/`
   <div class="GSVruleRow">
      <div class="GSVruleData">${Protocol}</div>
      <a href="${ESChtmlEscape(href)}" target="_blank" class="GSVruleData GSVpaddingLeft">${ESChtmlEscape(Link)}</a>
      <div class="GSVruleData">${HttpPort}</div>
      <div class="GSVruleData">${SshPort}</div>
      <div class="GSVruleData">${Type}</div>
   </div>`;
}

function GSVrowsHtml(Servers){
   let Html = "";
   for(Link in Servers) Html += GSVpurgeRuleRowHtml(Link, Servers[Link]);
   return Html;
}

function GSVrulesHtml(Rules){
   let View  = !GSVeditPage();
   if(View && !Object.keys(Rules).length) return /*html*/ `<div class="SETemptyTable">No git servers defined - click EDIT to define the list of git servers</div>`;
   if(!View) return GSVeditRowsHtml(Rules);
   let Rows = GSVrowsHtml(Rules);
   return /*html*/`
   <div class="GSVruleTable">
      <div class="GSVruleHeading">
         <div class="GSVheading GSVpaddingLeft">HTTP/S</div>
         <div class="GSVheading GSVpaddingLeft">URL</div>
         <div class="GSVheading">HTTP/S Port</div>
         <div class="GSVheading">SSH Port</div>
         <div class="GSVheading">Provider</div>
      </div>
      ${Rows}
   </div>`;
}