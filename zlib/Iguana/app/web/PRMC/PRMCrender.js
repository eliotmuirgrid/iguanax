let PRMCdata;
let PRMCcard;
let PRMCinline = false;

function PRMCrender(){
  PRMCinline = false;
  window.location.hash = "dashboard";
  SNCKsnackbar("warning", "Promote Changes is available from the editable translator view.", 4000);
}

function PRMCopenInline(){
  if(!PRMCtranslatorContext()){
    return SNCKsnackbar("warning", "Promote Changes is available from the editable translator view.", 4000);
  }
  PRMCinline = true;
  POPtooltipRemoveByClass('CLDmodal');
  APIcall("component/get_repository", PRMCapiData(), PRMConRepoLoaded);
}

function PRMConRepoLoaded(R){
  if(!R.success){
    if(!PRMCinline) window.location.hash = "dashboard";
    return SNCKsnackbar("error", "Could not get upstream repository details: " + R.error);
  }
  PRMCdata = R?.data || {};
  PRMCpopout();
  PRMClisteners();
}

function PRMCpopout(){
  if(document.querySelector(".PRMCmodal")) return;

  PRMCcard = POPpopoutOpen({
    title     : "Promote Changes (Beta)",
    width     : "800px",
    content   : PRMChtml(),
    esc_close : true,
    callback  : PRMCclose
  });
}

function PRMCclose(){
  const inline = !!PRMCinline;
  PRMCinline = false;
  PRMCcard = null;

  if(inline) return;

  let Data = {};
  Data.component = PRMCcomponentId();
  Data.filter    = PRMCfilterId();
  window.location.hash = "dashboard/details" + PAGEformatParams(Data);
}

function PRMCsetError(msg){
  const el = document.querySelector(".PRMCmodal .PRMCerror");
  if(!el) return;
  el.textContent = msg || "";
  el.style.display = msg ? "block" : "none";
}
function PRMCclearError(){ PRMCsetError(""); }

function PRMCgitUrlHtml(){
  const web = (PRMCrepoWeb && PRMCrepoWeb()) || "";
  const ssh = (PRMCrepoSsh && PRMCrepoSsh()) || "";
  if(!web && !ssh){
    const title = CARDescapeAttr("Git URL is not set. Promotion can still work for local branches, but pushing requires a remote.");
    return `<span class="CARDcurrentRepo CARDgitUrlNotSet" title="${title}">Git URL not set</span>`;
  }
  return RPOrepoHtml(web, ssh);
}

function PRMCbranchSummaryHtml(branchName, branchUrl, defaultBranch){
  if(!branchName){
    const d = CARDescapeHtml(defaultBranch || "");
    return `<span><code>${d}</code> <span class="PRMCmode">(default)</span></span>`;
  }
  const b = CARDescapeHtml(branchName);
  const u = CARDescapeAttr(branchUrl || "");
  return u ? `<a target="_blank" href="${u}">${b}</a>` : `<span>${b}</span>`;
}

function PRMCtargetBranchSuggestion(source){
  const lowerSource = (source || "").toLowerCase();
  const pairs = [
    ["-main", "-test"],
    ["-master", "-test"],
    ["-dev", "-test"],
    ["-test", "-prod"],
    ["-qa", "-prod"],
    ["/main", "/test"],
    ["/master", "/test"],
    ["/dev", "/test"],
    ["/test", "/prod"]
  ];
  for(const pair of pairs){
    if(lowerSource.endsWith(pair[0])) return source.slice(0, -pair[0].length) + pair[1];
  }
  const names = {
    main        : "test",
    master      : "test",
    trunk       : "test",
    dev         : "test",
    develop     : "test",
    development : "test",
    test        : "prod",
    qa          : "prod",
    staging     : "production"
  };
  return names[lowerSource] || "";
}

function PRMCintroHtml(){
  return /*html*/`
    <div class="PRMCintro">
      <div class="RPOdescription">
        <div><b>Review changes before promoting them.</b></div>
        <div class="PRMCintroDetail">
          Choose a source branch and target branch. Iguana will stage the merge on the target branch,
          then open Commit Changes so you can review the diffs before committing.
        </div>
      </div>
    </div>`;
}

function PRMCcurrentSourceHtml(){
  const BranchName = PRMCbranchName();
  const DefaultBranch = PRMCdefaultBranch();
  const Branch = PRMCbranchSummaryHtml(BranchName, PRMCrepoWebBranchUrl(), DefaultBranch);

  return /*html*/`
    <div class="RPOheader">CURRENT SOURCE:</div>
    <div class="RPOcomponentDiv">
      <div class="RPOrepoRow">
        <div class="FORMlabel">Branch:</div>
        <div class="PRMCcell">${Branch}</div>
      </div>
      <div class="RPOrepoRow">
        <div class="FORMlabel">Repository:</div>
        <div class="PRMCcell">${PRMCgitUrlHtml()}</div>
      </div>
    </div>`;
}

function PRMCbranchFieldsHtml(){
  const Source = PRMCbranchName();
  const Target = PRMCtargetBranchSuggestion(Source);

  return /*html*/`
    <div class="RPOrepoRow PRMCbranchRow">
      <div class="FORMlabel">From:</div>
      <div class="PRMCinputCol">
        <input class="FORMinput PRMCsourceInput" placeholder="Source branch..." value="${CARDescapeAttr(Source)}">
        <div class="PRMCdefaultHint">Branch containing the changes.</div>
      </div>
    </div>
    <div class="RPOrepoRow PRMCbranchRow">
      <div class="FORMlabel">To:</div>
      <div class="PRMCinputCol">
        <input class="FORMinput PRMCtargetInput" placeholder="Target branch..." value="${CARDescapeAttr(Target)}">
        <div class="PRMCdefaultHint">Branch that should receive the changes.</div>
      </div>
    </div>
    ${PRMCpromotionTypeHtml()}
    <div class="PRMCnotice">
      Beta workflow: review staged changes carefully before committing and pushing.
    </div>
    <div class="PRMCerror" role="alert" aria-live="polite" style="display:none;"></div>`;
}

function PRMCpromotionTypeHtml(){
  return /*html*/`
    <div class="RPOrepoRow PRMCpromotionTypeRow">
      <div class="FORMlabel">Promotion Type:</div>
      <div class="PRMCpromotionOptions" role="radiogroup" aria-label="Promotion Type">
        <label class="PRMCpromotionOption">
          <input type="radio" name="PRMCpromotionType" value="squash" checked>
          <span>Compress changes (Squash Merge)</span>
        </label>
        <label class="PRMCpromotionOption">
          <input type="radio" name="PRMCpromotionType" value="standard">
          <span>Keep full history (Standard Merge)</span>
        </label>
      </div>
    </div>`;
}

function PRMCbuttonsHtml(){
  return /*html*/`
    <div class="BUTTONgroup PRMCbuttons">
      <div class="PRMCcancel BUTTONstandard BUTTONdefault">Cancel</div>
      <div class="PRMCsubmit BUTTONstandard BUTTONaction BUTTONdisabled">Review &amp; Promote</div>
    </div>`;
}

function PRMCpromoteHtml(){
  return /*html*/`
    <div class="RPOheader">PROMOTION SETUP:</div>
    <div class="RPOcomponentDiv">
      ${PRMCbranchFieldsHtml()}
      ${PRMCbuttonsHtml()}
    </div>`;
}

function PRMChtml(){
  return /*html*/`
    <div class="PRMCmodal">
      ${PRMCintroHtml()}<br>
      ${PRMCcurrentSourceHtml()}<br>
      ${PRMCpromoteHtml()}
    </div>`;
}
