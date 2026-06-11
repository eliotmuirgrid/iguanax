function PRJcomponentRunning(){
   let RunningData = MODELdata?.TRANSrunningComponents?.data;
   if(!RunningData) return;
   return RunningData[TRANScomponentGuid()] ? true : false; 
}

function PRJswitchClick() {
   let Div = document.querySelector(".PRJscriptSwitch");
   let Command = TOGswitchApi(Div);
   let Arg     = TOGswitchArg(Div, [TRANScomponentGuid()] );
   APIcall(Command, Arg, function(Response) {
      if (!LICcheckComponentStartError(Command, Response, function(){ MODELforcePoll('TRANSmodel'); })){ return; }
      if (0 !== Response.data.errors.length) SNCKsnackbar('error', Response.data.errors[0], 3000);
      MODELforcePoll('TRANSmodel');
      MODELforcePoll('TRANSslowModel');
   });
}

function PRJlastErrorString(Light){
   if (Light == "RED"){
      let ErrorData = TRANSlastError();
      if (ErrorData.length){
         return ".\n " + ErrorData[0]?.content;
      }
   }
   return "";
}

function PRJrenderSwitchToolTip() {
   if(document.querySelector('.PRJinProgressInfo') !== null) return;
   let Data = TRANScomponentData();
   if(!Data) return;
   let Content = TOGswitchDescription(Data.light, PRJcomponentRunning());
   if(Data.light === 'RED') {
      if(document.querySelector('.PRJswitchLastErrorTooltip')) { return; }
      Content = Content + PRJlastErrorString(Data.light);
      let Div = document.querySelector('.PRJerrorTooltip');
      if(Div && Div.innerHTML == Content) return;
      POPtooltip({
         target  : document.querySelector('.TOGstatus'),
         content : `<div class="PRJerrorTooltip">` + Content + `</div>`,
         position: POPtooltipPosition.BOTTOM,
         style   : 'PRJswitchLastErrorTooltip',
         type    : POPtooltipType.ERROR
      });
   }
}

function PRJupdateSwitchTooltip(){
   let Data = TRANScomponentData();
   if(!Data) return;
   let Div = document.querySelector('.PRJerrorTooltip');
   if(!Div) return;
   let Content = TOGswitchDescription(Data.light, PRJcomponentRunning());
   Content = Content + PRJlastErrorString(Data.light);
   if(Div.innerHTML !== Content) Div.innerHTML = Content;
}

function PRJswitchHtml(){
   return TOGswitchHtml("PRJscriptSwitch", TRANScomponentData()?.light, PRJcomponentRunning(), TRANSpermissionData()?.stop, TRANSpermissionData()?.start);
}

function PRJupdateSwitch(){
   let Html = PRJswitchHtml();
   let Div = document.querySelector('.TRANSswitchDiv');
   let Inner = Div.querySelector(".PRJscriptSwitch");
   if(!Inner) return Div.innerHTML = Html;
   if(Inner.outerHTML != Html) {
      POPtooltipRemoveByClass('PRJswitchTooltip');
      Inner.outerHTML = Html;
   }
}

function PRJupdateName(){
   let ComponentData = TRANScomponentData();
   if(!ComponentData) return;
   let Name = ComponentData.name;
   let $ProjectName = document.querySelector('.PRJprojectName');
   if(!$ProjectName) { return; }
   if($ProjectName.textContent !== Name) {
      $ProjectName.textContent = Name;
      $ProjectName.title       = Name;
   }
}

function PRJupdateLink(){
   let div = document.querySelector(".PRJcomponentLink");
   if(!div) return;
   let href = DASHformatUrl(TRANScomponentGuid());
   if(div.href !== href) div.href = href;
}