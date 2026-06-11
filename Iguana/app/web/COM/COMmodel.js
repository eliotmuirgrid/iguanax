let COMcommitList = {};
let COMrenderedCommitMap;

function COMcommitListOpen(){
   let div = document.querySelector(".SPLTcollapse");
   return div ? false : true;
}

function COMmodelInit(){
   COMresetData();
   MODELpollerAdd("COMmodel", 'COMcommitData',     13, 'instance/git_history', {}, COMcommitListOpen);
   MODELpollerAdd("COMmodel", 'COMgitCredentials', 7, 'git/credentials', {},  COMcommitListOpen);
   MODELviewAdd("COMmodel", "COMformatData");
   MODELviewAdd("COMmodel", "COMlistUpdate");
   MODELviewAdd("COMmodel", "COMerrorWarning");
   MODELforcePoll("COMmodel");
   MODELstart("COMmodel");
}

function COMresetData(){
   // COMcommitList = [];
   COMrenderedCommitMap = {};
}

function COMcommitDataError(){
   return MODELdata?.COMcommitData?.error || "";
}

function COMcommitData(){
   return MODELdata?.COMcommitData?.data?.commits || {};
}

function COMpath(){
   return MODELdata?.COMcommitData?.data?.path || "";
}

function COMformatData(){
   let Data = COMcommitData();
   let Array = [];
   for(commit in Data) Array.push(Data[commit]);
   COMcommitList[COMscreen()] = Array.reverse();
}
