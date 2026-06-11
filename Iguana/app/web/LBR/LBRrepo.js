function LBRverifyRepo(){
   let ErrorMessage = "";
   let Div = document.querySelector('.LBRrepoLinkInput');
   if(!Div) return "There was an issue rendering";
   let Repo = Div.value.trim();
   let GitData = LBRgitData();
   if(Repo.length > 62  && GitData.platform === "bitbucket") ErrorMessage = "Bitbucket repo cannot exceed 62 characters";
   if(Repo.length > 100 && GitData.platform === "github")    ErrorMessage = "Github repo cannot exceed 100 characters";
   if(Repo.length === 0) ErrorMessage = "Repo must not be empty";
   return ErrorMessage;
}

function LBRupdateRepo(){
   let ErrorMessage = LBRverifyRepo();
   let RepoError = document.querySelector('.LBRrepoFeedback');
   if(!RepoError) return;
   LBRrepoError = ErrorMessage !== "";
   if(LBRrepoError) document.querySelector('.LBRrepoDoesntExistFeedback').innerHTML = "";
   if(RepoError.innerHTML !== ErrorMessage) RepoError.innerHTML = ErrorMessage;
}
