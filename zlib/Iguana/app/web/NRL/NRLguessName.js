function NRLrepoCutTokens(){
   return new Set([
      "-", // GitLab: /group/repo/-/tree/main
      "tree",
      "blob",
      "commit",
      "commits",
      "compare",
      "pull",
      "pulls",
      "merge",
      "merge_requests",
      "issues",
      "src",
      "browse",
      "branch",
      "branches",
   ]);
}

function NRLguessName(){
   if(NRLnameTyped) return;
   NRLqueueNameGuess();
}

function NRLqueueNameGuess(){
   clearTimeout(NRLupdateUrlTimer);
   NRLupdateUrlTimer = setTimeout(function() { 
      let parsed = NRLsyncParsedGitInput();
      NRLpopulateNameGuess(parsed.url, parsed.branch, ".NRLname"); 
   }, 100);
}

function NRLurl(){
   return NRLsyncParsedGitInput().url;
}

function NRLbranch(){
   return NRLsyncParsedGitInput().branch;
}

function NRLgitBranchInputs(){
   return {
      gitInput: document.querySelector('.NRLgit'),
      branchInput: document.querySelector('.NRLbranch')
   };
}

function NRLinputValue(Input){
   return Input ? Input.value : "";
}

function NRLsyncParsedGitInput(){
   let Inputs = NRLgitBranchInputs();
   let gitEl = NRLinputValue(Inputs.gitInput);
   let branchEl = NRLinputValue(Inputs.branchInput);
   let parsed = NRLparseGitInput(gitEl, branchEl);
   if(parsed.split) {
      if(Inputs.gitInput) Inputs.gitInput.value = parsed.url;
      if(Inputs.branchInput) Inputs.branchInput.value = parsed.branch;
   }
   return parsed;
}

function NRLnameData(){
   return MODELdata?.NRLcomponentNamesData?.data;
}

function NRLbuildNameGuess(parsed){
   let Guess = "";
   if(parsed.branch) Guess = NRLbranchNameToHuman(parsed.branch);
   if(!Guess) Guess = NRLrepoNameToHuman(parsed.url);
   return Guess;
}

function NRLensureNameIsUnique(Guess){
   let Names = NRLnameData();
   if(NWCnameExistsBool(Guess, Names)) return NWCnextAvailableName(Guess, Names);
   return Guess;
}

function NRLpopulateNameGuess(Git,Branch,ClassName){
   let parsed = NRLparseGitInput(Git,Branch);
   let Guess = NRLbuildNameGuess(parsed);
   Guess = NRLensureNameIsUnique(Guess);
   let div = document.querySelector(ClassName);
   if(!div || !Guess) return;
   div.value = Guess;
   FORMfocus(div, true);
}

function NRLparseUrlAndBranchFromGit(raw){
   let m = raw.match(/^(.*?)\s+@\s*(.+)$/);
   if(!m) return null;
   let urlPart = (m[1] || "").trim();
   let branchPart = (m[2] || "").trim();
   if(!urlPart || !branchPart) return null;
   return { url: urlPart, branch: branchPart };
}

function NRLparseUrlWithPendingBranchMarker(raw){
   let m = raw.match(/^(.*?)\s+@\s*$/);
   if(!m) return null;
   let urlPart = (m[1] || "").trim();
   if(!urlPart) return null;
   return { url: urlPart };
}

function NRLparseGitInput(Git,Branch){
   let raw = (Git || "").trim();
   let ParsedSplit = NRLparseUrlAndBranchFromGit(raw);
   if(ParsedSplit) return { url: ParsedSplit.url, branch: ParsedSplit.branch, split: true };
   let Pending = NRLparseUrlWithPendingBranchMarker(raw);
   if(Pending) {
      return {
         url: Pending.url,
         branch: (Branch || "").trim(),
         split: false
      };
   }
   return {
      url: raw,
      branch: (Branch || "").trim(),
      split: false
   };
}

function NRLtitleCaseWords(words){
   return words
      .filter(Boolean)
      .map(word => word.charAt(0).toUpperCase() + word.slice(1))
      .join(" ");
}

function NRLbranchNameToHuman(branch) {
   if (!branch) return "";
   try {
      let b = branch.trim();
      b = b.replace(/^refs\/heads\//, "");
      b = b.replace(/[-_\/]+/g, " ");
      return NRLtitleCaseWords(b.split(" ")).trim();
   } catch {
      return "";
   }
}

function NRLstripQueryAndFragment(input){
   return input.split("#")[0].split("?")[0].trim();
}

function NRLextractPathFromHostPath(raw){
   try {
      const u = new URL("https://" + raw);
      return u.pathname || "";
   } catch {
      return raw;
   }
}

function NRLextractPathFromGitInput(raw){
   const scp = raw.match(/^[^@]+@[^:]+:(.+)$/);
   if (scp) return scp[1] || "";
   if (/^[a-z]+:\/\//i.test(raw)) {
      const u = new URL(raw);
      return u.pathname || "";
   }
   if (/^[\w.-]+\.[\w.-]+\//.test(raw)) return NRLextractPathFromHostPath(raw);
   return raw;
}

function NRLnormalizeRepoPath(path){
   let normalized = path.replace(/^\/+/, "").replace(/\/+$/, "");
   return normalized.replace(/\.git$/i, "");
}

function NRLtrimSegmentsAtCutToken(segments){
   let CutTokens = NRLrepoCutTokens();
   for (let i = 0; i < segments.length; i++) {
      if (CutTokens.has(segments[i])) return segments.slice(0, i);
   }
   return segments;
}

function NRLrepoSegmentsFromPath(path){
   let segs = path.split("/").filter(Boolean);
   if (!segs.length) return [];
   if (segs[0] === "repos" && segs.length > 1) segs = segs.slice(1);
   return NRLtrimSegmentsAtCutToken(segs);
}

function NRLrepoNameFromSegments(segs){
   let repo = segs[segs.length - 1] || "";
   return repo.replace(/\.git$/i, "").trim();
}

function NRLhumanizeRepoName(repo){
   let name = repo
      .replace(/([a-z0-9])([A-Z])/g, "$1 $2")         // myRepo -> my Repo
      .replace(/([A-Z]+)([A-Z][a-z])/g, "$1 $2")      // XMLParser -> XML Parser
      .replace(/[-_.]+/g, " ")
      .replace(/\s+/g, " ")
      .trim();

   return name
      .split(" ")
      .filter(Boolean)
      .map(w => (/^[A-Z0-9]+$/.test(w) ? w : (w.charAt(0).toUpperCase() + w.slice(1))))
      .join(" ")
      .trim();
}

function NRLrepoNameToHuman(input) {
   if (!input) return "";
   try {
      let raw = NRLstripQueryAndFragment(String(input).trim());
      if (!raw) return "";

      let path = NRLextractPathFromGitInput(raw);
      path = NRLnormalizeRepoPath(path);
      let segs = NRLrepoSegmentsFromPath(path);
      if (!segs.length) return "";
      let repo = NRLrepoNameFromSegments(segs);
      if (!repo) return "";
      return NRLhumanizeRepoName(repo);
   } catch {
      return "";
   }
}
