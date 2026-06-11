let INITtitles = {
   ""         : "Welcome",
   "prepare"  : "Prepare",
   "git"      : "Install Git",
   "location" : "Location",
   "config"   : "Configure",
   "user"     : "Create User",
   "server"   : "Server Setup",
   "finish"   : "Finish"
};

let INITsteps = ["Welcome", "Prepare", "Install Git", "Location", "Configure", "Create User", "Server Setup", "Finish"];

function INITmenuModelInit(){
   MODELpollerAdd("INITmenuModel", "INITversion",   10000, "product/version", {});
   MODELviewAdd  ("INITmenuModel", "INITupdateVersion");
   MODELviewAdd  ("INITmenuModel", "INITupdateMenuHtml");
   MODELforcePoll("INITmenuModel");
   MODELstart    ("INITmenuModel");
}

function INITtitle(){
   let hash = PAGEhash();
   let title;
   if(hash !== undefined) title = INITtitles[hash];
   return title || "";
}

function INITstepNumber(Title){
   return INITsteps.indexOf(Title);
}

function INITupdateMenuHtml(){
   let Div = document.querySelector(".INITmodalSidebar");
   if(!Div) return;
   let Html = INITmenuHtml();
   if(Div.innerHTML !== Html) Div.innerHTML = Html;
}

function INITversionNumber(){
   return MODELdata?.INITversion?.data?.version;
}

function INITupdateVersion(){
   let Div = document.querySelector(".INITversionDiv");
   if(!Div) return;
   let Version = INITversionNumber();
   let Html = "";
   if(Version) Html = `IguanaX - Version ${Version}`;
   if(Div.innerHTML !== Html) Div.innerHTML = Html;
}

function INITmenuHtml(){
   let currentTitle = INITtitle();
   let currentStep = INITstepNumber(currentTitle);
   let html = "";
   for(let i = 0; i < INITsteps.length; i++){
      html += INITmenuItemHtml(INITsteps[i], i, INITextraClass(currentStep, i));
   }
   return html;
}

function INITextraClass(currentStep, Number){
   let ExtraClass = '';
   if (currentStep == Number){
      ExtraClass = ' INITactive';         
   }
   if (currentStep > Number){
      ExtraClass = ' INITcompleted';
   }
   return ExtraClass;
}

function INITmenuItemHtml(Label, Number, ExtraClass){
   return /*html*/`
   <div class="INITstep${ExtraClass}">
      <div class="INITverticalStepper">
         <div class="INITcircle">${Number + 1}</div>
         <div class="INITline"></div>
      </div>
      <div class="INITstepLabel">${Label}</div>
   </div>`;
}
