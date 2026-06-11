function NRLverifyRepo(){
   let ErrorMessage = "";
   let Div = document.querySelector('.NRLgit');
   if(!Div) return "There was an issue rendering";
   let Repo = Div.value.trim();
   if(Repo.length === 0) {
      ErrorMessage = "Repo must not be empty";
      return ErrorMessage;
   }
   let Url = MODELdata.NRLcomponentRepoValid.data;
   if (!Url.valid) ErrorMessage = "This is not a valid repository";
   return ErrorMessage;
}

function NRLupdateRepo(){
   let ErrorMessage = NRLverifyRepo();
   let RepoError = document.querySelector('.NRLrepoFeedback');
   if(!RepoError) return;
   NRLrepoError = ErrorMessage !== "";
   if(RepoError.innerHTML !== ErrorMessage) RepoError.innerHTML = ErrorMessage;
}

function NRLgetRepoString(){
   let Div = document.querySelector('.NRLgit');
   if(!Div) return "There was an issue rendering";
   let Call = {}
   Call.url = Div.value.trim();
   return Call;
}

function NRLworkingFeedback(){
   let Div = document.querySelector('.NRLworking');
   if (!Div) return;
   if (NRLinProgress){ Div.style.visibility = 'visible'; } 
   else              { Div.style.visibility = 'hidden';  }
}