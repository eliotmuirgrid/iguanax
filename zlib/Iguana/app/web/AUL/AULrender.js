let AULcard;
let AULinProgress = false;
let AULnameTyped = false;
let AULguessNameTimer = 0;

const AUL_OFFICIAL_AUTO_LAYOUT_URL = "https://bitbucket.org/interfaceware/component-organizer.git";
const AUL_REQUIRED_TAGS = ["#autoorganize", "#run"];

function AULcomponentHasAutoOrganizeTag(Component) {
   const tags = Array.isArray(Component?.tags) ? Component.tags : [];
   return tags.some(function(Tag) { return String(Tag || "").toLowerCase() === "#autoorganize"; });
}

function AULhasAutoLayoutUtility() {
   const listData = DASHlistData();
   const mapData  = DASHmapData() || {};
   const mapItems = Array.isArray(mapData) ? mapData : Object.values(mapData || {});
   return listData.some(AULcomponentHasAutoOrganizeTag) || mapItems.some(AULcomponentHasAutoOrganizeTag);
}

function AULhandleAutoLayoutSuccess(){
   SNCKsnackbar("success", "Auto Layout applied successfully.", 10000);
}

function AULcomponentsFromListResponse(Response){
   const Data = Response?.data;
   if(Array.isArray(Data)) return Data;
   if(Array.isArray(Data?.component_data)) return Data.component_data;
   if(Array.isArray(Response?.component_data)) return Response.component_data;
   return [];
}

function AULfindAutoLayoutGuid(Components){
   const List = Array.isArray(Components) ? Components : [];
   for(let i = 0; i < List.length; i++){
      if(AULcomponentHasAutoOrganizeTag(List[i])) return List[i]?.guid || "";
   }
   return "";
}

function AULrunAutoLayout(){
   APIcall("component/list", {filter: "#autoorganize", whole_word: "true", ascending: 1, order: "", checked_only: false}, function(ListResponse){
      if(!ListResponse?.success){
         SNCKsnackbar("error", "Could not run Auto Layout: " + (ListResponse?.error || "Unknown error"));
         return;
      }
      const Components = AULcomponentsFromListResponse(ListResponse);
      const Guid = AULfindAutoLayoutGuid(Components);
      if(!Guid) return AULnewComponentScreen();
      const Params = {
         component: Guid,
         function: "main",
         data: "INIT",
         start_if_stopped: true
      };
      APIcall("component/function/call", Params, function(CallResponse){
         if(!CallResponse?.success){
            SNCKsnackbar("error", "Could not run Auto Layout: " + (CallResponse?.error || "Unknown error"));
            return;
         }
         AULhandleAutoLayoutSuccess();
      });
   });
}

function AULnewComponentScreen(){
   if(document.querySelector('.AULmodal')) return;
   AULinProgress = false;
   AULnameTyped = false;
   clearTimeout(AULguessNameTimer);
   AULcard = POPpopoutOpen({
      title: "Add Auto Layout Utility",
      width: "800px",
      content: AULhtml(),
      esc_close : true,
      callback : DASHcloseModal
   });
   AULlisteners();
   AULmodelInit();
   let Input = document.querySelector(".AULgit");
   if(Input) FORMfocus(Input);
   let Container = document.querySelector(".AULcontainer");
   if(Container) FORMenterListener(Container, AULaddComponent);
}

function AULcloseModal(){
   MODELremove(AUL_MODEL_NAME);
   if(AULcard) {
      POPpopoutClose(AULcard);
      AULcard = null;
      return;
   }
   DASHcloseModal();
}

function AULrequiredTags(){
   return AUL_REQUIRED_TAGS.slice();
}

function AULbaseName(Name){
   return String(Name || "").trim().replace(/\s\(\d+\)$/, "");
}

function AULnameExistsBool(Name){
   let Value = String(Name || "").trim();
   if(!Value) return false;
   return AULnameData()[Value] === true;
}

function AULnextAvailableName(Name){
   let Value = AULbaseName(Name);
   if(!Value) return "";
   if(!AULnameExistsBool(Value)) return Value;
   let int = 1;
   while(AULnameExistsBool(`${Value} (${int})`) && int < 20){
      int++;
   }
   return `${Value} (${int})`;
}

function AULsplitTags(Input){
   return String(Input || "")
      .trim()
      .split(/\s+/)
      .map(function(Tag){ return Tag.trim(); })
      .filter(Boolean);
}

function AULtagsUnique(Tags){
   let Unique = [];
   let Seen = {};
   for(let i = 0; i < Tags.length; i++){
      let Tag = (Tags[i] || "").trim();
      if(!Tag) continue;
      if(Seen[Tag]) continue;
      Seen[Tag] = true;
      Unique.push(Tag);
   }
   return Unique;
}

function AULtagsForSubmit(){
   let Required = AULrequiredTags();
   let ExtraInput = document.querySelector(".AULtagsExtra");
   let Extra = AULsplitTags(ExtraInput ? ExtraInput.value : "");
   return AULtagsUnique(Required.concat(Extra));
}

function AULgitBranchInputs(){
   return {
      gitInput: document.querySelector(".AULgit"),
      branchInput: document.querySelector(".AULbranch")
   };
}

function AULsyncParsedGitInput(){
   let Inputs = AULgitBranchInputs();
   let RawGit = NRLinputValue(Inputs.gitInput);
   let RawBranch = NRLinputValue(Inputs.branchInput);
   let Parsed = NRLparseGitInput(RawGit, RawBranch);
   let Pending = NRLparseUrlWithPendingBranchMarker((RawGit || "").trim());
   let CleanGit = Pending ? Pending.url : Parsed.url;
   if(Inputs.gitInput && Inputs.gitInput.value !== CleanGit) Inputs.gitInput.value = CleanGit;
   if(Inputs.branchInput && Parsed.split && Inputs.branchInput.value !== Parsed.branch) Inputs.branchInput.value = Parsed.branch;
   return {
      url: Parsed.url,
      branch: Parsed.branch,
      split: Parsed.split,
      cleanGit: CleanGit
   };
}

function AULcreateSuccess(Response){
   if(Response?.success === true) return true;
   if(Response?.data?.success === true) return true;
   return false;
}

function AULcreateGuid(Response){
   let Components = Response?.data?.components;
   if(!(Components instanceof Array) || !Components.length) return "";
   return Components[0]?.guid || "";
}
