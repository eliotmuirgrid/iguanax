function SLBupdateName(Data, Key){
   let Name = SLBname(Data?.name, Key);
   SLBupdate(".SLBdetName", Name);
}

function SLBupdateDesc(Data){
   let Desc = HASHTAGrenderHashtags(Data?.description) || `<div class="SLBnotSet">There is no desctiption set.</div>`;
   SLBupdate(".SLBdetDesc", Desc);
}

function SLBupdateUrl(Data){
   let Url  = Data?.urls?.url || "";
   let Link = Data?.urls?.web;
   let Html = /*html*/`<a class="SLBrepoLink" href="${Link}" target="_blank">${Url}</a>`;
   SLBupdate(".SLBdetUrl", Html);
}

function SLBupdateCol(Data){
   let Url  = Data?.collection?.url || "";
   let Link = Data?.collection?.web;
   let Html = /*html*/`<a class="SLBrepoLink" href="${Link}" target="_blank">${Url}</a>`;
   if(!Url) Html = /*html*/`<div class="SLBnotSet">Not found in any collection.</div>`;
   SLBupdate(".SLBdetCol", Html);
}
