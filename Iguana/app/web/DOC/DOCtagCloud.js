let FilteredCount;

function DOCtagCloudUpdate(){
   COL_TRC("Cloud update");   
   document.querySelector(".DOCtagCloud").innerHTML = DOCtransformCloudTags(MODELdata.DOClist.data.tags);
}

function DOCtreeTagsEventRegister(){
   document.querySelector(".DOCtagCloud").addEventListener('click', function(E){ DOCtagCloudClick(E); });
}

function DOCtransformCloudTags(Tags){
   let HTags = [];
   FilteredCount = 0;
   const DateTagPat = /^\d{4}_\d{2}-[a-z]{3}_\d{2}$/i;
   for (let i=0; i<Tags.length; i++){
      if (DateTagPat.test(Tags[i])){  FilteredCount+=1; continue;};
      HTags.push("#" + Tags[i]);
   }
   let R = HASHTAGrenderTags(HTags, "DOCtagCloudDiv");
   return R;
}


function DOCtagCloudClick(E){
   const Target = E.target;
    if (Target.closest('.DOCtagCloudDiv')){ return DOCtagDivClick(Target); }  
}

function DOCtagDivClick(Target){
   let Idx = DOCaddressOf(Target, "DOCtagCloudDiv");
   let Data = MODELdata.DOClist.data.tags || [];
   let Tag = Data[Idx[0]+FilteredCount];
   // COL_TRC(Tag);
    DOCtagAddTag("DOCsearchInput", Tag);
    //DOCsearch();
}