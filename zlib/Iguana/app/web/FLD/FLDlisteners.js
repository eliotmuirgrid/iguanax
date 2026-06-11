function FLDlisteners(e){
   if(e.target.closest('.FLDeditFields'))         FLDedit();
   if(e.target.closest('.FLDaddField'))           FLDaddBlankCustomField();
   if(e.target.closest('.FLDcancelConfig'))       FLDview();
   if(e.target.closest('.FLDdeleteRow'))          e.target.closest('.DRGdragSortListItem').remove();
   if(e.target.closest('.FLDsaveConfig'))         FLDsave();
   if(e.target.closest('.FLDfileBrowseCustom'))   FLDfileBrowseCustomize(e.target);
   if(e.target.closest('.FLDfolderBrowseCustom')) FLDfolderBrowseCustomize(e.target);
}

function FLDchangeListeners(e){
   if(e.target.closest('.FLDtypeSelect') ) FLDrerenderCustomizeRow(e.target);
}