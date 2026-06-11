function CARDescapeHtml(s) {
   if (s == null) return "";
   return String(s)
      .replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
      .replace(/"/g, "&quot;").replace(/'/g, "&#39;");
}

function CARDescapeAttr(s) {
   return CARDescapeHtml(s).replace(/`/g, "&#96;");
}

function CARDrepoSet() {
   return !!CARDgitRepo();
}

function CARDhasPermEditUpstream() {
   return !!CARDpermissionData()?.edit_upstream;
}

function CARDnoPermTitle() {
   return "You do not have permission to do this.";
}

function CARDgetSourceModel() {
   const repo   = CARDgitRepo() || "";
   const link   = (typeof CARDgitLink === "function") ? (CARDgitLink() || "") : "";
   const branch = CARDgitBranch() || "";
   const plain  = !!CARDgitPlain();
   const Data   = CARDgitRepoData();

   const hasGitUrl = !!(repo || link);

   return {
      repo, link, plain,
      isTemplate  : !!CARDdata()?.template,
      hasGitUrl,
      hasBranch   : !!branch,
      branchLabel : branch,
      url         : { web_branch : (Data?.urls?.web_branch || "") }
   };
}

function CARDplainSourceLineHtml() {
   const title = CARDescapeAttr("This component is a plain folder. Connect a Git repository to enable syncing.");
   return `<div class="CARDrepoBranchRow"><span class="CARDplainRepo" title="${title}">Plain folder — using local files only.</span></div>`;
}

function CARDrepoBranchSourceLineHtml() {
   const M = CARDgetSourceModel();
   if (M.plain) return "";

   // Repo / Git URL label (real or placeholder)
   let repoHtml = "";
   if (M.hasGitUrl) {
      const repoTxt  = CARDescapeHtml(M.repo || "Repository");
      const repoHref = CARDescapeAttr(M.link);
      repoHtml = M.link
         ? `<a target="_blank" class="CARDcurrentRepo" href="${repoHref}">${repoTxt}</a>`
         : `<span class="CARDcurrentRepo">${repoTxt}</span>`;
   } else {
      const title = CARDescapeAttr("Git URL is not set. Use [Set Git URL] to enable syncing.");
      repoHtml = `<span class="CARDcurrentRepo CARDgitUrlNotSet" title="${title}">Git URL not set</span>`;
   }

   // Branch (optional)
   if (!M.hasBranch) {
      return `<div class="CARDrepoBranchRow">${repoHtml}</div>`;
   }

   const brTxt = CARDescapeHtml(M.branchLabel);
   let brHtml  = `<span class="CARDbranchName">${brTxt}</span>`;

   if (M.url?.web_branch) {
      const brHref = CARDescapeAttr(M.url.web_branch);
      brHtml       = `<a target="_blank" class="CARDbranchName" href="${brHref}">${brTxt}</a>`;
   }

   return `<div class="CARDrepoBranchRow">${repoHtml}<span class="CARDatSymbol"> @ </span>${brHtml}</div>`;
}

function CARDsourceLineHtml() {
   const M = CARDgetSourceModel();
   if (M.plain) return CARDplainSourceLineHtml();
   return CARDrepoBranchSourceLineHtml();
}

function CARDupstreamRowHtml() {
   return `<div class="CARDcurrentRepoGrid"><span class="CARDrepoSourceRow">${CARDsourceLineHtml()}</span></div>`;
}

function CARDbracketAction(label, cssClass, title) {
   const t = title ? ` title="${CARDescapeAttr(title)}"` : "";
   return `<span class="CARDmakeCopyBrackets">[</span><span class="${cssClass}"${t}>${CARDescapeHtml(label)}</span><span class="CARDmakeCopyBracketsEnd">]</span>`;
}

function CARDjoinActions(actions) {
   const xs = (actions || []).filter(Boolean);
   const n  = xs.length;
   if (n === 0) return "";
   if (n === 1) return xs[0];
   if (n === 2) {
      return xs[0] + `<span class="CARDorDiv">or </span>` + xs[1];
   }
   // n >= 3
   const head = xs.slice(0, -1).join(`<span class="CARDdotDiv">• </span>`);
   const tail = xs[n - 1];
   return head + `<span class="CARDorDiv">or </span>` + tail;
}

function CARDeditSourceActionHtml() {
   return CARDbracketAction("Make Editable", "CARDremoveUpstream");
}

function CARDchangeGitUrlActionHtml() {
   return CARDhasPermEditUpstream()
      ? CARDbracketAction("Set Git URL", "CARDeditUpstream")
      : CARDbracketAction("Set Git URL", "CARDdisableLink", CARDnoPermTitle());
}

function CARDchangeBranchActionHtml() {
   const M = CARDgetSourceModel();
   const useSetLabel = (!M.hasBranch || !M.hasGitUrl);
   const label = useSetLabel ? "Set Source Branch" : "Switch Source Branch";
   // Gate branch selection when:
   // - branch is effectively "default" (no explicit branch set / shown), AND
   // - Git URL is not set (no remote context to resolve branches against)
   const gateBecauseNoGitUrl = (!M.hasGitUrl && !M.hasBranch);
   // Permission always wins
   if (!CARDhasPermEditUpstream()) {
      return CARDbracketAction(label, "CARDdisableLink", CARDnoPermTitle());
   }
   // Disable (nudge) state
   if (gateBecauseNoGitUrl) {
      // If you'd rather HIDE instead of disable, return "" here.
      return CARDbracketAction(
         label,
         "CARDdisableLink",
         "Set a Git URL first to choose a source branch."
      );
   }
   // Normal enabled state
   return CARDbracketAction(label, "CARDchangeBranch");
}

function CARDseeInCollectionHtml() {
   return `<span class="CARDmakeCopyBrackets">[</span><span class="CARDseeInCollection">See in Collection</span><span class="CARDmakeCopyBracketsEnd">]</span>`;
}

function CARDaddToCollectionHtml() {
   const ok    = !!CARDpermissionData()?.edit_collections;
   const cls   = ok ? "CARDaddToCollection" : "CARDdisableLink";
   const title = ok ? "" : ` title="${CARDescapeAttr("You do not have permission to do this.")}"`;
   return `<span class="CARDmakeCopyBrackets">[</span><span class="${cls}"${title}>Add to Collection</span><span class="CARDmakeCopyBracketsEnd">]</span>`;
}

function CARDcollectionHtml() {
   return CARDurlInTemplatesCollection() ? CARDseeInCollectionHtml() : CARDaddToCollectionHtml();
}

function CARDsourceActionsHtml() {
   const M = CARDgetSourceModel();
   let a = "", b = "", c = "";

   if (M.plain) {
      a = `${RPOturnToRepo(CARDcomponentOn(), CARDhasPermEditUpstream())}`;

   } else if (M.isTemplate) {
      // Template: no source branch switch.
      a = CARDeditSourceActionHtml();

   } else {
      // Non-plain, non-template: ALWAYS show Set Git URL + Source Branch action.
      // If Git URL is not set, do NOT show collection actions.
      a = CARDchangeGitUrlActionHtml();
      b = CARDchangeBranchActionHtml();
      c = M.hasGitUrl ? CARDcollectionHtml() : "";
   }

   const joined = CARDjoinActions([a, b, c]);
   if (!joined) return "";
   return `<div class="CARDsourceActionsButtons">${joined}</div>`;
}

function CARDupdateRepo() {
   const div = document.querySelector(".CARDsourceDiv");
   if (!div) return;

   const html = CARDupstreamRowHtml();
   if (div.innerHTML !== html) div.innerHTML = html;
}

function CARDupdateSourceActions() {
   const div = document.querySelector(".CARDsourceActions");
   if (!div) return;

   const html = CARDsourceActionsHtml() || "";
   if (div.innerHTML !== html) div.innerHTML = html;
}

function CARDsourceHtml() {
   return `<div class="CARDdetails">` +
      `<div class="CARDrunningCommit">${CARDrunningCommitHtml()}</div>` +
      `<div class="CARDrepoRow"><div class="FORMlabel">Source:</div><div class="CARDsourceDiv"></div></div>` +
      `<div class="CARDactionsRow"><div class="FORMlabel"></div><div class="CARDsourceActions"></div></div>` +
      `<div class="CARDpermissionDiv">${CARDpermissionHtml()}</div>` +
   `</div>`;
}

function CARDturnToRepo(){
   CARDdisableOpenEdit();
   return RPOturnRepo(CARDeditUpstreamCallback);
}
