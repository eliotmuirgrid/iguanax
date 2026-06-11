function CLDrender(){ CLDupdate(true); }

function CLDsyncOn(){
   const t = CLDtarget();
   if(!t) return;
   t.classList.toggle("on", !!CLDpopover());
}

function CLDupdate(autoShow = false){
   const div = CLDtarget();
   if(!div) return;

   POPtooltip({
      target       : div,
      event        : POPtooltipEvent.CLICK,
      autoShow     : !!autoShow,
      keepExisting : true,
      position     : "bottom",
      yOffset      : -14,
      xOffset      : 16,
      content      : `<div class="CLDtarget">Loading...</div>`,
      style        : "CLDmodal",
      callback     : function(tooltip){
         if(!tooltip) return;
         CLDsyncOn();
         CLDrenderHtml();
         CLDreposition();
         CLDbindActions(tooltip);
      },
      onHide       : function(){
         setTimeout(CLDsyncOn, 100);
      }
   });
}

function CLDbindActions(tooltip){
   if(!tooltip || tooltip.dataset.cldBound === "1") return;
   tooltip.dataset.cldBound = "1";

   tooltip.addEventListener("click", function(e){
      if(e.target.closest(".CLDsetGitUrl"))        return CLDopenSetGitUrl();
      if(e.target.closest(".CLDsetSourceBranch")) return CLDopenSetSourceBranch();
      if(e.target.closest(".CLDpromoteChanges"))  return CLDopenPromoteChanges();
      if(e.target.closest(".CLDcopyButton"))       return CLDcopyUrl();
   });
}

function CLDopenSetGitUrl(){
   // existing flow for setting repo URL in this screen
   if(typeof CPRrender === "function") return CPRrender();
}

function CLDopenSetSourceBranch(){
   // IMPORTANT: wire this to the branch modal opener for *this* screen (no hash changes).
   if(typeof BRNCopenInline === "function") return BRNCopenInline();
}

function CLDopenPromoteChanges(){
   const ActivePromotion = (typeof TRANSpromotionCurrent === "function") ? TRANSpromotionCurrent() : null;
   if(ActivePromotion?.active || ActivePromotion) {
      return SNCKsnackbar("warning", "Finish or abort the current promotion before starting a new one.", 5000);
   }
   if(typeof PRMCopenInline === "function") return PRMCopenInline();
}

function CLDcopyText(){
   const D = TRGTgitRepo() || {};
   const git = (D.repo || D.link || "").trim();
   const br  = (D.branch || "").trim();
   return (git && br) ? `${git} @ ${br}` : git;
}

function CLDcopyUrl(){
   const Text = CLDcopyText();
   navigator.clipboard.writeText(Text);
   const Div = document.querySelector(".CLDcopyButton");
   if(!Div) return;
   Div.innerHTML = "Copied!";
   Div.classList.add("CLDcopyButtonCopied");
}

function CLDtarget(){ return document.querySelector(".TRGTrepoButton"); }
function CLDpopover(){ return document.querySelector(".CLDmodal"); }

function CLDrenderHtml(){
   const div = document.querySelector(".CLDtarget");
   if(!div) return;
   const html = CLDhtml();
   if(div.innerHTML !== html) div.innerHTML = html;
}

function CLDreposition(){
   const tooltip = CLDpopover();
   const target  = CLDtarget();
   if(!tooltip || !target) return;
   // Clear any previous transform so measurements are stable
   tooltip.style.transform = "";
   const tr = target.getBoundingClientRect();
   const tt = tooltip.getBoundingClientRect();
   const targetCenter = tr.left + (tr.width / 2);
   // Desired left so tooltip is centered under the target
   let desiredLeft = targetCenter - (tt.width / 2);
   // Clamp within viewport
   const margin = 8;
   desiredLeft = Math.max(margin, Math.min(desiredLeft, window.innerWidth - tt.width - margin));
   // Compute how far we need to shift from current position
   const currentLeft = tt.left;
   const dx = currentLeft - desiredLeft;
   tooltip.style.transform = `translateX(-${dx}px)`;
}

/* ===== run/dev mode helpers ===== */
function CLDisRunMode(){
   try{
      return (typeof TRANSdevelopment === "function" && TRANSdevelopment() === false);
   }catch(e){}
   return false;
}

function CLDsourceTitleText(){
   return CLDisRunMode() ? "Run Source:" : "Source:";
}

/* ===== source model (TRGT-based, CARD-style fields) ===== */
function CLDgetSourceModel(){
   const D = TRGTgitRepo() || {};
   const repo   = D.repo || "";
   const link   = D.link || "";
   const branch = D.branch || "";
   const plain  = !!D.plain;
   const hasGitUrl = !!(repo || link);
   return {
      repo, link, plain,
      isTemplate  : false,                // adjust if TRGT exposes this
      hasGitUrl,
      hasBranch   : !!branch,
      branchLabel : branch,
      url         : { web_branch : (D?.urls?.web_branch || "") }
   };
}

function CLDrepoBranchSourceLineHtml(){
   const M = CLDgetSourceModel();
   if(M.plain) return "";
   // Repo / Git URL label (real or placeholder)
   let repoHtml = "";
   if(M.hasGitUrl){
      const repoFull  = (M.repo || M.link || "").trim();
      const repoLabel = CLDrepoLabel(repoFull);
      const repoTxt   = CARDescapeHtml(repoLabel);
      const repoTitle = CARDescapeAttr(repoFull);
      const repoHref  = CARDescapeAttr(M.link);
      repoHtml = M.link
         ? `<a target="_blank" class="CARDcurrentRepo" href="${repoHref}" title="${repoTitle}">${repoTxt}</a>`
         : `<span class="CARDcurrentRepo" title="${repoTitle}">${repoTxt}</span>`;
   } else {
      const title = CARDescapeAttr("Git URL is not set. Use [Set Git URL] to enable syncing.");
      repoHtml = `<span class="CARDcurrentRepo CARDgitUrlNotSet" title="${title}">Git URL not set</span>`;
   }
   // Branch (optional)
   if(!M.hasBranch) return `<div class="CARDrepoBranchRow">${repoHtml}</div>`;
   const brTxt = CARDescapeHtml(M.branchLabel);
   let brHtml  = `<span class="CARDbranchName">${brTxt}</span>`;
   if(M.url?.web_branch){
      const brHref = CARDescapeAttr(M.url.web_branch);
      brHtml = `<a target="_blank" class="CARDbranchName" href="${brHref}">${brTxt}</a>`;
   }
   return `<div class="CARDrepoBranchRow">${repoHtml}<span class="CARDatSymbol"> @ </span>${brHtml}</div>`;
}

function CLDsourceLineHtml(){
   const M = CLDgetSourceModel();
   if(M.plain) return CARDplainSourceLineHtml();
   return CLDrepoBranchSourceLineHtml();
}

function CLDupstreamRowHtml(){
   return `<div class="CARDcurrentRepoGrid"><div class="CARDrepoSourceRow">${CLDsourceLineHtml()}</div></div>`;
}

/* ===== actions ===== */
function CLDjoinActions(actions){
   const xs = (actions || []).filter(Boolean);
   const n  = xs.length;
   if(n === 0) return "";
   if(n === 1) return xs[0];
   // 2 actions: "a or b"
   if(n === 2){
      return xs[0] + `<span class="CARDorDiv">or </span>` + xs[1];
   }
   // 3+ actions: "a · b · ... or last"
   const head = xs.slice(0, -1).join(`<span class="CLDsep">·</span>`);
   return head + `<span class="CARDorDiv">or </span>` + xs[n - 1];
}

function CLDsourceActionsHtml(){
   const M = CLDgetSourceModel();
   const useSetBranchLabel = (!M.hasBranch || !M.hasGitUrl);
   const branchLabel = useSetBranchLabel ? "Set Source Branch" : "Switch Source Branch";
   // Hide the Git source actions when TRANSdevelopment() is false
   if(CLDisRunMode()) return "";
   // Plain folder: branch selection is not meaningful here.
   if(M.plain){
      return `<div class="CLDactions">
         <span class="CLDaction CLDsetGitUrl">Set Git URL</span>
      </div>`;
   }
   // Templates (if ever exposed here): no branch action.
   if(M.isTemplate){
      return `<div class="CLDactions">
         <span class="CLDaction CLDsetGitUrl">Set Git URL</span>
      </div>`;
   }
   // Non-plain, non-template:
   // Disable source branch actions when:
   // - branch is effectively "default" (no explicit branch set / shown), AND
   // - Git URL is not set (no remote context)
   const gateBecauseNoGitUrl = (!M.hasGitUrl && !M.hasBranch);
   const a = `<span class="CLDaction CLDsetGitUrl">Set Git URL</span>`;
   const b = gateBecauseNoGitUrl
      ? `<span class="CLDaction CLDdisableLink" title="${CARDescapeAttr("Set a Git URL first to choose a source branch.")}">${branchLabel}</span>`
      : `<span class="CLDaction CLDsetSourceBranch">${branchLabel}</span>`;
   const canPromote = (typeof TRANSpermissionData !== "function") || !!TRANSpermissionData()?.edit_script;
   const promotionActive = (typeof TRANSpromotionCurrent === "function") ? !!TRANSpromotionCurrent() : false;
   const c = !canPromote
      ? `<span class="CLDaction CLDdisableLink" title="${CARDescapeAttr("You do not have permission to do this.")}">Promote Changes</span>`
      : promotionActive
         ? `<span class="CLDaction CLDdisableLink" title="${CARDescapeAttr("Finish or abort the current promotion before starting a new one.")}">Promote Changes</span>`
      : (gateBecauseNoGitUrl
         ? `<span class="CLDaction CLDdisableLink" title="${CARDescapeAttr("Set a Git URL or source branch first.")}">Promote Changes</span>`
         : `<span class="CLDaction CLDpromoteChanges">Promote Changes</span>`);
   return `<div class="CLDactions">${CLDjoinActions([a, b, c])}</div>`;
}


/* ===== copy button (keep) ===== */
function CLDcopyButtonHtml(){
   const D = TRGTgitRepo() || {};
   const Empty = !D.link && !D.repo;
   if(Empty) return "";
   return /*html*/`
      <div class="BUTTONdefault BUTTONstandard CLDcopyButton" title="Copy source">
         Copy
      </div>
   `;
}

/* ===== tooltip html ===== */
function CLDhtml(){
   const SourceHtml  = CLDupstreamRowHtml();
   const ActionsHtml = CLDsourceActionsHtml();
   const CopyHtml    = CLDcopyButtonHtml();
   const TitleTxt    = CLDsourceTitleText();

   const SingleRow = !(ActionsHtml && ActionsHtml.trim());
   const GridClass = `CLDgrid CLDpad${SingleRow ? " CLDsingleRow" : ""}`;

   return /*html*/`
      <div class="${GridClass}">
         <div class="CLDsourceBlock">
            <div class="CLDtitle">${CARDescapeHtml(TitleTxt)}</div>
            ${SourceHtml}
            ${ActionsHtml}
         </div>
         ${CopyHtml}
      </div>
   `;
}


function CLDrepoLabel(repo){
   if(!repo) return "";
   const s = String(repo).trim();
   let path = s;
   // ssh/http(s) URLs: keep path after host
   let m = s.match(/^[a-z]+:\/\/[^/]+\/(.+)$/i);
   if(m) path = m[1];
   // scp-style: git@host:owner/repo.git
   m = s.match(/^[^@]+@[^:]+:(.+)$/);
   if(m) path = m[1];
   const parts = String(path).split("/").filter(Boolean);
   if(parts.length >= 2) return parts.slice(-2).join("/");
   return path;
}
