function CARDurlInTemplatesCollection() {
   let Templates = CARDtemplates();
   if(!Templates) { return; }
   let Ssh = CARDgitRepoSsh();
   let Branch = CARDgitBranch();
   for(let i = 0; i < Templates.length; i++) {
      let GitUrl     = Templates[i].git;
      let Collection = Templates[i].collection;
      let TemplateBranch     = Templates[i].branch;
      if(GitUrl === Ssh && TemplateBranch === Branch) { return Collection; }
   }
   return "";
}