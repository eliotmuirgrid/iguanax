function DBMupdateFromFileName(){
   let FromFileName = document.querySelector('.DBMfromFilePath');
   if(!FromFileName) return;
   let FromName = DBMfromFile;
   if(FromName === "") FromName = DBMfromDatabase;
   if(FromName === "") FromName = "Select a VDB source  →";
   if(FromFileName.innerHTML !== FromName) FromFileName.innerHTML = FromName;
}

function DBMfromFileHtml(Data){
   if(Object.keys(DBMfromFileData).length === 0) return DBMselectFileMessage();
   let MaxName = DBMdetermineMaxNameLength(Data);
   let Html = '';
   for(TableName in Data) Html += DBMeditTableRow(TableName, Data, MaxName); 
   return /*html*/`${Html}`;
}

