let CMTdevelopmentRow = {
   hash: "DEVELOPMENT",
   message: "This will run what is currently in the translator.",
   date: ""
};

function CMTshowAllSelected(){
   return CMTcomponentSelected === CMTshowAllOptionId;
}  

function CMTcommits(){
   let Commits = CMTshowAllSelected() ? CMTinstCommitsData() : CMTcommitsData();
   let Data = [];
   for(let Idx in Commits){
      Data.push(Commits[Idx]);
   }
   if(CMTshowAllSelected())   Data.reverse();
   if(CMTdevelopmentOption()) Data.unshift(CMTdevelopmentRow);
   return Data;
}

function CMTdevelopmentOption(){
   return CARDwritable() || CARDgitPlain();
}

function CMTupdateCommits(){
   let Commits = CMTcommits();
   let Html = CMTcommitHelpRowsHtml(Commits);
   document.querySelector('.CMTcommitTarget').innerHTML = Html;
}
