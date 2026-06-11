function LBRverifyName(){
   let ErrorMessage = "";
   let NameDiv = document.querySelector('.LBRname');
   if(!NameDiv) return "There is an issue rendering the name input field";
   let Name = NameDiv.value.trim();
   let NameExistsError = LBRnameExistsError();
   if(NameExistsError) ErrorMessage = "There was an issue verifying whether the name already exists: " + NameExistsError;
   if(Name.length === 0)  ErrorMessage = "Name must not be empty";
   let GitData = LBRgitData();
   if(Name.length > 240 && GitData.platform === "bitbucket") ErrorMessage = "Bitbucket name cannot exceed 240 characters";
   if(Name.length > 100 && GitData.platform === "github")    ErrorMessage = "Github name cannot exceed 100 characters";
   return ErrorMessage;
}

function LBRupdateNameExists(){
   let Div = document.querySelector('.LBRname');
   if(!Div) return;
   let NameExists = LBRnameExists();
   if(NameExists) Div.classList.add   ("LBRnameExistsError");
   else           Div.classList.remove("LBRnameExistsError");
   let Error = document.querySelector('.LBRnameExistsFeedback');
   if(!Error) return;
   let ErrorHtml = ""; 
   if(NameExists) ErrorHtml = `Folder or file already exists with that name.`;
   if(Error.innerHTML !== ErrorHtml) Error.innerHTML = ErrorHtml;
}

function LBRupdateNameError(){
   let ErrorMessage = LBRverifyName();
   let Error = document.querySelector('.LBRnameFeedback');
   if(!Error) return;
   LBRnameError = ErrorMessage !== "";
   if(LBRnameError) document.querySelector('.LBRnameExistsFeedback').innerHTML = "";
   if(Error.innerHTML !== ErrorMessage) Error.innerHTML = ErrorMessage;
}