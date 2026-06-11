function NRLupdateNameFeedback(){
   let NameDiv = document.querySelector('.NRLname');
   if(!NameDiv) return;
   let Name = NameDiv.value.trim();
   let ErrorMessage = NRLnameErrorMessage(Name);
   let ErrorDiv = document.querySelector('.NRLnameExistsFeedback');
   if(!ErrorDiv) return;
   if(ErrorDiv.innerHTML !== ErrorMessage) ErrorDiv.innerHTML = ErrorMessage;
   NRLnameError = ErrorMessage != ""
}

function NRLnameErrorMessage(Name){
   let Names = MODELdata.NRLcomponentNamesData.data;
   let ErrorMessage = "";
   if(Names[Name] == true) ErrorMessage = "Name already exists, please provide a unique name.";
   if(Name.length > 60)                    ErrorMessage = "Name cannot exceed 60 characters";
   if(Name.trim() == "")                   ErrorMessage = "Name cannot be empty";
   return ErrorMessage;
}
