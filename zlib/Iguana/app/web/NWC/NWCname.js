function NWCcomponentNameCount(){
   NWCnameMap = {};
   let Names = document.querySelectorAll('.NWCname');
   if(!Names) return;
   for(let i = 0; i<Names.length; i++){
      let Name = Names[i].value.trim();
      if(!NWCnameMap[Name]){ 
         NWCnameMap[Name] = 1; 
      }
      else NWCnameMap[Name] += 1;
   }
}

function NWCnameErrorMessage(Name){
   let ErrorMessage = "";
   let ComponentNamesData = MODELdata?.NWCcomponentNamesData?.data;
   if(ComponentNamesData[Name] == true) ErrorMessage = "Name already exists, please provide a unique name.";
   if(NWCnameMap[Name] != 1)            ErrorMessage = "Cannot create multiple components with the same name";
   if(Name.length > 60)                 ErrorMessage = "Name cannot exceed 60 characters";
   if(Name.trim() == "")                ErrorMessage = "Name cannot be empty";
   return ErrorMessage;
}

function NWCupdateComponentFeedback(ComponentCard){
   if(!ComponentCard) return;
   let NameDiv = ComponentCard.querySelector('.NWCname');
   if(!NameDiv) return;
   let Name = NameDiv.value.trim();
   let ErrorMessage = NWCnameErrorMessage(Name);
   let ErrorDiv = ComponentCard.querySelector('.NWCnameExistsFeedback');
   if(!ErrorDiv) return;
   if(ErrorDiv.innerHTML !== ErrorMessage) ErrorDiv.innerHTML = ErrorMessage;
   return ErrorMessage == "";
}

function NWCupdateNameFeedback(){
   let ComponentDivs = document.querySelectorAll('.NWCcomponentDiv');
   if(!ComponentDivs) return;
   NWCnameError = false;
   for(let i = 0; i<ComponentDivs.length; i++){
      if(!NWCupdateComponentFeedback(ComponentDivs[i])) NWCnameError = true;
   }
}

function NWCcomponentNames(){
   let Names = document.querySelectorAll('.NWCname') || [];
   let NameData = [];
   for(Name of Names){
      NameData.push(Name.value.trim());
   }
   return NameData;
}