function GSVeditRowsHtml(Rules){
   let Rows = GSVrowEditHtml(Rules);
   return /*html*/`
   <div class="GSVimportant">To use the <b>default</b> ports, simply set both the <b>HTTP/S Port</b> and <b>SSH Port</b> values to <b> 0</b>. Only specify these ports if they are set to a non-standard number.</div>
   ${SETresetToDefaultHtml("Add the Default Git Servers", "GSVresetToDefaultLink")}
   <br>
   <div class="GSVruleTable">
      <div class="GSVeditRuleHeading">
         <div class="GSVheading GSVpaddingLeft">HTTP/S</div>
         <div class="GSVheading"></div>
         <div class="GSVheading">URL</div>
         <div class="GSVheading"></div>
         <div class="GSVheading GSVportHeading">HTTP/S Port</div>
         <div class="GSVheading GSVand"></div>
         <div class="GSVheading GSVportHeading">SSH Port</div>
         <div class="GSVheading GSVon"></div>
         <div class="GSVheading GSVproviderHeading">Provider</div>
         <div class="GSVheading"></div>
      </div>
      ${Rows}
   </div>
   ${GSVaddRuleHtml()}`;
}

function GSVprotocolSelectHtml(Secure){
   let SelectedOne =  Secure ? " selected" : "";
   let SelectedTwo = !Secure ? " selected" : "";
   return /*html*/`
   <select class="FORMselectBox GSVprotocol GSVruleVal">
      <option ${SelectedOne}>https</option>
      <option ${SelectedTwo}>http</option>
   </select>`;
}

function GSVproviderSelectHtml(Provider){
   let AzureCloud      = Provider == "Cloud - Azure Devops"  ? " selected" : "";
   let AzureServer     = Provider == "Server - Azure Devops" ? " selected" : "";   
   let BitbucketCloud  = Provider == "Cloud - Bitbucket"     ? " selected" : "";
   let BitbucketServer = Provider == "Server - Bitbucket"    ? " selected" : "";
   let GitHubCloud     = Provider == "Cloud - GitHub"        ? " selected" : "";
   let GitHubServer    = Provider == "Server - GitHub"       ? " selected" : "";
   let GitLabCloud     = Provider == "Cloud - GitLab"        ? " selected" : "";
   let GitLabServer    = Provider == "Server - GitLab"       ? " selected" : "";
   return /*html*/`
   <select class="FORMselectBox GSVprovider GSVruleVal GSVproviderDiv">
      <option ${AzureCloud}     >Cloud - Azure Devops</option>
      <option ${BitbucketCloud} >Cloud - Bitbucket</option>
      <option ${GitHubCloud}    >Cloud - GitHub</option>
      <option ${GitLabCloud}    >Cloud - GitLab</option>
      <option ${AzureServer}    >Server - Azure Devops</option>
      <option ${BitbucketServer}>Server - Bitbucket</option>
      <option ${GitHubServer}   >Server - GitHub</option>
      <option ${GitLabServer}   >Server - GitLab</option>
   </select>`;
}

function GSVeditRowHtml(Link, Data){
   let Provider  = Data?.type      || "";
   let Http_port = Data?.http_port || 0;
   let Ssh_port  = Data?.ssh_port  || 0;
   return /*html*/`
   <div class="GSVeditRuleRow">
      ${GSVprotocolSelectHtml(Data?.secure)}
      <div class="GSVcolon GSVruleVal GSVport">://</div>
      <input class="FORMinput GSVruleVal GSVaddress" value="${ESChtmlEscape(Link || "")}">
      <div class="GSVcolon GSVruleVal GSVport">:</div>
      <input class="FORMinput GSVruleVal GSVhttpPort"  min="0" step="1" value="${Http_port}" type="number">
      <div class="GSVcolon GSVand GSVruleVal">and</div>
      <input class="FORMinput GSVruleVal GSVsshPort"  min="0" step="1" value="${Ssh_port}" type="number">
      <div class="GSVcolon GSVruleVal GSVon">on</div>
      ${GSVproviderSelectHtml(Provider)}
      <div class="GSVdeleteRow GSVpaddingRight">
         <div class="FORMbutton BUTTONcancel BUTTONwarning GSVdelete">
            ${SETtrashIcon()}
         </div>
      </div>
   </div>`;
}

function GSVrowEditHtml(Servers){
   let Html = ``;
   for (let Link in Servers) Html += GSVeditRowHtml(Link, Servers[Link]);
   if(!Html) return GSVeditRowHtml();
   return Html;
}

function GSVaddRuleHtml(){
   return /*html*/`
   <div class="GSVadd">
      <svg class="SETaddIcon" width="14" height="14">
         <use href="SET/SETiconAdd.svg#SETiconAdd"></use>
      </svg>
      GIT SERVER
   </div>`;
}