function CMTmodel(){
   MODELpollerAdd('CMTmodel', 'CMTcomponentList', 5, 'component/list', {});
   MODELpollerAdd('CMTmodel', 'CMTcommits',       2, 'component/commits', CMTcommitsArgs, function(){return !CMTshowAllSelected()});
   MODELpollerAdd('CMTmodel', 'CMTinstCommits',   2, 'instance/git_history', {},          function(){return  CMTshowAllSelected()});
   MODELviewAdd('CMTmodel', 'CMTupdateComponents');
   MODELviewAdd('CMTmodel', 'CMTupdateCommits');
   // MODELviewAdd('CMTmodel', 'CMTupdateRunFromComponentCheckBox');
   MODELforcePoll('CMTmodel');
}

function CMTcomponentListData(){
   return MODELdata?.CMTcomponentList?.data?.component_data || [];
}

function CMTcommitsArgs(){
   let Args = {};
   Args.component = CMTcomponentSelected;
   return Args;
}

function CMTinstCommitsData(){
   return MODELdata?.CMTinstCommits?.data?.commits || [];
}

function CMTcommitsData(){
   return MODELdata?.CMTcommits?.data || [];
}