function RPOturnToRepo(On, Permission){
   let TurnTo;
   if (On) {
      TurnTo = `<div title="Cannot convert to a repository while the component is running." class="RPOturnToRepoDisabled">Convert to repository</div>`;
   }
   else if (!Permission) {
      TurnTo = `<div title="You do not have permission to convert to a repository." class="RPOturnToRepoDisabled">Convert to repository</div>`;
   }
   else {
      TurnTo = `<div class="RPOturnToRepo">Convert to repository</div>`;
   }
   return /*html*/`
   <div class="RPOturnToPlainGrid">
      <div class="RPOturnToPlainGridItem">[</div>
      ${TurnTo}
      <div class="RPOturnToPlainGridItem">]</div>
   </div>`;
}

function RPOturnToPlain(On, Permission){
   let TurnTo;
   if(On) { TurnTo = `<div title="Cannot turn to a plain folder while the component is running." class="RPOturnToPlainDisabled">Turn to a Plain Folder</div>`; }
   else if(!Permission) { TurnTo = `<div title="You do not have permission to turn to a plain folder." class="RPOturnToPlainDisabled">Turn to a Plain Folder</div>`; }
   else { TurnTo = `<div class="RPOturnToPlain">Turn to a Plain Folder</div>`; }
   return /*html*/`
   <div class="RPOturnToPlainGrid">
      <div class="RPOturnToPlainGridItem">[</div>
      ${TurnTo}
      <div class="RPOturnToPlainGridItem">]</div>
   </div>`;
}

function RPOplainHtml(){
   return /*html*/`<div class="RPOplain">Nothing. This is a plain folder.</div>`;
}

function RPOremoveUpstreamHtml(Component, UpstreamClass){
   return /*html*/`
   ${Component ? `<div class="RPOactionsOr">or</div>` : ""}
   <div class="RPOturnToPlainGrid">
      <div class="RPOturnToPlainGridItem">[</div>
      <div class="RPOremoveUpstream ${UpstreamClass}">Remove Upstream</div>
      <div class="RPOturnToPlainGridItem">]</div>
   </div>`;
}

function RPOactionsRow(Component, UpstreamClass, GitUrl, Plain, Dev){
   let On = false;
   let Permission = true;
   let TurnToPlain = Plain ? RPOturnToRepo(On, Permission) : RPOturnToPlain(On, Permission);
   let RemoveUpstream = GitUrl ? RPOremoveUpstreamHtml(Component, UpstreamClass) : "";
   if(!Component && !GitUrl) return "";
   if(!Dev) return "";
   return /*html*/`
   <br>
   <div class='RPOrepoRow'>
      <div class="FORMlabel">Actions: </div>
      ${Component ? "<div>" + TurnToPlain + "</div>" : ""}
      <div class="RPOactions">
         ${RemoveUpstream}
         <div></div>
      </div>
   </div>`;
}

function RPOregularUpstreamHtml(T){
   let GitUrl = T?.GitUrl || "";
   let Repo = T?.Repo || "";
   let Plain = T?.Plain || false;
   let Component = T?.Component || false;
   let UpstreamClass = T?.UpstreamClass || "";
   let Html = GitUrl ? RPOrepoHtml(GitUrl, Repo) : RPOnoRepoHtml();
   let RepoHtml = Plain ? RPOplainHtml() : Html;
   let Dev = T?.Development === false ? false : true;
   return /*html*/`
   <div class='RPOlabel RPOrepoRow'>
      <div class="FORMlabel">Connected to: </div>
      <div>${RepoHtml}</div>
      <div></div>
   </div>
   ${RPOactionsRow(Component, UpstreamClass, GitUrl, Plain, Dev)}`;
}

function RPOlibraryHtml(LibraryName){
   return /*html*/`   
   <div class='RPOrepoRow'>
      <div class="FORMlabel">Library: </div>
      <div class="">${LibraryName}</div>
   </div>
   <br>`;
}

function RPOrepoHtml(Link, Repo){
   let ShowUrl = Repo;
   if(ShowUrl === undefined) ShowUrl = Link;
   return /*html*/`<a class="RPOlink" target="_blank" href="${Link}">${ShowUrl}</a>`;
}

function RPOnoRepoHtml(){
   return /*html*/`<div class="RPOnoRepo RPOnoRepoWarning">There is no connected repository</div> `;
}

function RPOshowProgress(){
   let Div = document.querySelector('.RPOfeedback');
   if(Div && RPOmidOnSubmitRequest) Div.innerHTML = `<div class="RPOsuccessFeedback">Successfully created a repository, now setting it as the upstream ... </div>`;
}