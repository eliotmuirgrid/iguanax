let DOC2FilteredCount;
function DOC2tagCloud(){
   let Tags = MODELdata.DOClist.data.tags;
   if(!Tags) return;
   let Div = document.querySelector(".DOC2tagCloud");
   if (!Div) return;
   Div.innerHTML = DOC2transformCloudTags(Tags);
}

function DOC2transformCloudTags(Tags){
   let HTags = [];
   DOC2FilteredCount = 0;
   const DateTagPat = /^\d{4}_\d{2}-[a-z]{3}_\d{2}$/i;
   for (let i=0; i<Tags.length; i++){
      if (DateTagPat.test(Tags[i])){  DOC2FilteredCount+=1; continue;};
      HTags.push("#" + Tags[i]);
   }
   let R = HASHTAGrenderTags(HTags, "DOC2tagCloudDiv");
   return R;
}

function DOC2treeTagsEventRegister(){
   document.querySelector(".DOC2tagCloud").addEventListener('click', function(E){ DOC2tagCloudClick(E); });
}

function DOC2tagCloudClick(E){
   const Target = E.target;
    if (Target.closest('.DOC2tagCloudDiv')){ return DOC2tagDivClick(Target); }  
}

function DOC2tagDivClick(Target){
   let Idx = DOC2addressOf(Target, "DOC2tagCloudDiv");
   let Data = MODELdata.DOClist.data.tags || [];
   let Tag = Data[Idx[0]+DOC2FilteredCount];
    DOC2tagAddTag("DOC2searchInput", Tag);
}