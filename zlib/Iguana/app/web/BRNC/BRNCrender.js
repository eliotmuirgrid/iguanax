let BRNCdata;
let BRNCcard;
let BRNCinline = false;

/* =========================
   Entry
   ========================= */
function BRNCrender(){
  BRNCinline = false;
  DASHdashboardScreen();
  APIcall("component/get_repository", BRNCapiData(), BRNConRepoLoaded);
}

// opener for CLD (no screen change / no hash change on close)
function BRNCopenInline(){
  BRNCinline = true;
  POPtooltipRemoveByClass('CLDmodal');
  APIcall("component/get_repository", BRNCapiData(), BRNConRepoLoaded);
}

function BRNConRepoLoaded(R){
  if(!R.success){
    if(!BRNCinline) window.location.hash = "dashboard";
    return SNCKsnackbar("error", "Could not get upstream repository details: " + R.error);
  }
  BRNCdata = R?.data || {};
  BRNCpopout();
  BRNClisteners();
}

/* =========================
   Popout
   ========================= */
function BRNCpopout(){
  if(document.querySelector(".BRNCmodal")) return;

  BRNCcard = POPpopoutOpen({
    title     : "Switch Source Branch",
    width     : "800px",
    content   : BRNChtml(),
    esc_close : true,
    callback  : BRNCclose
  });
}

function BRNCclose(){
  // callback from POPpopoutOpen (X/ESC/programmatic close)
  const inline = !!BRNCinline;
  BRNCinline = false;
  BRNCcard = null;

  if(inline) return;

  let Data = {};
  Data.component = BRNCcomponentId();
  Data.filter    = BRNCfilterId();
  window.location.hash = "dashboard/details" + PAGEformatParams(Data);
}

/* =========================
   Error helpers
   ========================= */
function BRNCsetError(msg, warning){
  const el = document.querySelector(".BRNCmodal .BRNCerror");
  if(!el) return;
  el.textContent = msg || "";
  el.style.display = msg ? "block" : "none";
  el.classList.toggle("BRNCwarning", !!warning);
}
function BRNCclearError(){ BRNCsetError(""); }

/* =========================
   HTML helpers
   ========================= */
function BRNCgitUrlHtml(){
    const web = (BRNCrepoWeb && BRNCrepoWeb()) || "";
    const ssh = (BRNCrepoSsh && BRNCrepoSsh()) || "";
    if (!web && !ssh) {
      const title = CARDescapeAttr("Git URL is not set. Use [Set Git URL] to enable syncing.");
      return `<span class="CARDcurrentRepo CARDgitUrlNotSet" title="${title}">Git URL not set</span>`;
    }
    return RPOrepoHtml(web, ssh);
  }
  

function BRNCbranchSummaryHtml(branchName, branchUrl, defaultBranch){
  if(!branchName){
    const d = CARDescapeHtml(defaultBranch || "");
    return `<span><code>${d}</code> <span class="BRNCmode">(default)</span></span>`;
  }
  const b = CARDescapeHtml(branchName);
  const u = CARDescapeAttr(branchUrl || "");
  return u ? `<a target="_blank" href="${u}">${b}</a>` : `<span>${b}</span>`;
}

function BRNCdefaultHintTextHtml(defaultBranch){
  return defaultBranch
    ? `Default: <code class="BRNCcode">${CARDescapeHtml(defaultBranch)}</code>`
    : `Default branch is not available.`;
}

/* =========================
   Section HTML
   ========================= */
function BRNCintroHtml(){
  return /*html*/`
    <div class="BRNCintro">
      <div class="RPOdescription">
        <div><b>Switch the branch this component loads from.</b></div>
        <div class="BRNCintroDetail">
          This replaces the component contents with the selected branch's contents.
          It does not merge history or promote changes between branches.
        </div>
        <ul>
          <li>Use this for one-component-per-branch organization.</li>
         <li>Use Promote Changes for dev, test, and prod merge workflows.</li>
        </ul>
      </div>
    </div>`;
}

function BRNCcurrentSourceHtml(){
  const BranchName = BRNCbranchName();
  const DefaultBranch = BRNCdefaultBranch();
  const Branch = BRNCbranchSummaryHtml(BranchName, BRNCrepoWebBranchUrl(), DefaultBranch);

  return /*html*/`
    <div class="RPOheader">CURRENT SOURCE:</div>
    <div class="RPOcomponentDiv">
      <div class="RPOrepoRow">
        <div class="FORMlabel">Branch:</div>
        <div class="BRNCcell">${Branch}</div>
      </div>
      <div class="RPOrepoRow">
        <div class="FORMlabel">Repository:</div>
        <div class="BRNCcell">${BRNCgitUrlHtml()}</div>
      </div>
    </div>`;
}

function BRNCbranchPickerRowHtml(){
  const BranchName = BRNCbranchName();
  const DefaultHint = BRNCdefaultHintTextHtml(BRNCdefaultBranch());

  return /*html*/`
    <div class="RPOrepoRow BRNCbranchRow">
      <div class="FORMlabel">Switch to:</div>
      <div class="BRNCinputCol">
        <input class="FORMinput BRNCbranchInput" placeholder="Select a branch..." value="${CARDescapeAttr(BranchName)}">
        <div class="BRNCdefaultHint">${DefaultHint}</div>
        <div class="BRNCswitchNotice">
          Switching source branches replaces this component's files. It does not merge changes from the current branch.
        </div>
        <div class="BRNCerror" role="alert" aria-live="polite" style="display:none;"></div>
      </div>
    </div>`;
}

function BRNCbuttonsHtml(){
  return /*html*/`
    <div class="BUTTONgroup BRNCbuttons">
      <div class="BRNCcancel BUTTONstandard BUTTONdefault">CANCEL</div>
      <div class="BRNCsubmit BUTTONstandard BUTTONaction BUTTONdisabled">SWITCH SOURCE</div>
    </div>`;
}

function BRNCchooseBranchHtml(){
  return /*html*/`
    <div class="RPOheader">SWITCH TO SOURCE BRANCH:</div>
    <div class="RPOcomponentDiv">
      ${BRNCbranchPickerRowHtml()}
      ${BRNCbuttonsHtml()}
    </div>`;
}

/* =========================
   Full modal HTML
   ========================= */
function BRNChtml(){
  return /*html*/`
    <div class="BRNCmodal">
      ${BRNCintroHtml()}<br>
      ${BRNCcurrentSourceHtml()}<br>
      ${BRNCchooseBranchHtml()}
    </div>`;
}
