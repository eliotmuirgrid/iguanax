let ESRgitmodulesUrl;

function ERSscreen(MenuItem) {
   const Module         = MenuItem.path;
   const ModuleUpstream = TRGTlibraryList()[Module].url;
   const Data           = {component: TRANScomponentGuid(), development: TRANSdevelopment(), path: MenuItem.path};
   APIcall("component/get_repository", Data, function(Response) {
      if(!Response.success) {
         console.log("Could not get upstream repository: " + Response.error);
      }
      const Url  = Response.data?.repo;
      const Link = Response.data?.link;
      ERSgetUrls(Module, ModuleUpstream, Url, Link);
   });
}

function ERSgetUrls(Path, ModuleUpstream, Url, Link) {
   APIcall("git/all_urls", {url: ModuleUpstream}, function(R) {
      if(!R.success) {
         console.log("Could not get upstream repository for library: " + R.error);
      }
      let Popout = POPpopoutOpen({
         title    : "Error with the upstream",
         content  : ESRscreenHtml(Url, Path, Link, ModuleUpstream, R?.data?.web),
         width    : '700px',
         esc_close: true
      });
      document.querySelector('.ESRviewRemote').addEventListener("click", function() {
         LPRrender(Path, ESRgitmodulesUrl);
         POPpopoutClose(Popout);
      });
      document.querySelector('.ESRviewGitModules').addEventListener("click", function() {
         TRANSfileChange(".gitmodules");
         POPpopoutClose(Popout);
      });
      document.querySelector('.ESRclearMessage').addEventListener("click", function() { POPpopoutClose(Popout); });
   });
}

function ESRscreenHtml(FolderUpstream, Path, FolderLink, GitModulesUpstream, LibraryLink){
   ESRgitmodulesUrl = GitModulesUpstream;
   return /*html*/`
   <div class="ESRinformation">The ${Path} library's remote repository differs from the git URL mentioned in the .gitmodules file.</div>
   <br>
   <div class="ESRinformation">To fix the error, both the git URLs <b>must match each other.</b></div>
   <br>
   <div class="ESRexampleDiv">
      <div class='ESRrepoRow'>
         <div class="FORMlabel">Remote:</div>
         <div>${ESRrepoLink(FolderUpstream, FolderLink)}</div>
         <div class="ESRviewRemote BUTTONstandard BUTTONdefault ">EDIT</div>
      </div>
      ${ESRhelpUpstream(GitModulesUpstream)}
   </div>
   <div class="ESRexampleDiv">
      <div class='ESRrepoRow'>
         <div class="FORMlabel">.gitmodules:</div>
         <div>${ESRrepoLink(GitModulesUpstream, LibraryLink)}</div>
         <div class="ESRviewGitModules BUTTONstandard BUTTONdefault ">EDIT</div>
      </div>
      ${ESRhelpGitmodules(Path, FolderUpstream, GitModulesUpstream)}
   </div>
   <br>
   <div class='BUTTONgroup ESRbuttonGroup'>
      <div class='ESRclearMessage BUTTONstandard BUTTONaction'>OKAY</div>
   </div>
   `;
}

function ESRhelpUpstream(GitModulesUpstream){
   if(!GitModulesUpstream) return "";
   return /*html*/`
   <br>
   <div class="ESRinformation"><i>Click <b>EDIT</b> to set the remote url and automatically paste the .gitmodules link.</i></div>
   `;
}

function ESRhelpGitmodules(Path, FolderUpstream, GitModulesUpstream){
   if(!FolderUpstream) return "";
   return /*html*/`
   <br>
   <div class="ESRinformation"><i>Paste this snippet into your <b>.gitmodules</b> file if it looks correct:</i></div>
   <textarea class="FORMinput ESRexample" disabled>${ESRexampleContent(Path, FolderUpstream, GitModulesUpstream)}</textarea>`;
}

function ESRexampleContent(Path, FolderUpstream, GitModulesUpstream){
   let Upstream = FolderUpstream || GitModulesUpstream || "<git-link-here>";
   return `
[submodule "${Path}"]
   path = ${Path}
   url = ${Upstream}`;
}

function ESRrepoLink(Repo, Link){
   if(!Repo) return ESRnoRepoHtml();
   return /*html*/`<a target="_blank" href="${Link}">${Repo}</a>`;
}

function ESRnoRepoHtml(){
   return /*html*/`
   <div class="ESRnoRepoWarning">No git URL found</div> `;
}