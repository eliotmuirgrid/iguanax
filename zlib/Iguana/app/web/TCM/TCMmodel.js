let TCMcommitList;
let TCMrenderedCommitMap;

function TCMcommitListOpen(){
   let div = document.querySelector(".SPLTcollapse");
   return div ? false : true;
}

function TCMmodelInit(){
   // COMresetData();
   MODELpollerAdd("TCMmodel", 'COMcommitData', 3, 'component/git_history', TCMargs, TCMcommitListOpen);
   MODELviewAdd("TCMmodel", "COMformatData");
   MODELviewAdd("TCMmodel", "TCMlistUpdate");
   MODELforcePoll("TCMmodel");
   MODELstart("TCMmodel");
   TCMlistUpdate();
}

function TCMargs(){
   let Args = {};
   Args.component = TRANScomponentGuid();
   Args.development = TRANSdevelopment();
   return Args;
}

function TCMresetData(){
   // TCMcommitList = [];
   TCMrenderedCommitMap = {};
}

