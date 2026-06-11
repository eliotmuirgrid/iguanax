function HLPrenderHelpEdit(FuncData){
   const HelpContainer = document.createElement('div')
   HelpContainer.classList.add('HLPcontainer')
   HelpContainer.classList.add('HLPeditMode')
   HelpContainer.innerHTML = HLPeditContent(FuncData)
   return HelpContainer
}

function HLPeditContent(help){
   return  `${HLPeditHeaderHtml(HLPnameResolve(help), help.SummaryLine, help.is_core)}                             
            ${HLPeditParametersHtml(help.Parameters, help.ParameterTable)}
            ${HLPeditDescriptionHtml()}`
}

function HLPeditHeaderHtml(title, summary, isCore){
   const heading = ESChtmlEscape(title.replace(/\//g, "."));
   let headingHtml;
   if(isCore) {
      headingHtml = `<h1 class="HLPheading HLPfunctionName">${heading}</h1>`
   }else {
      headingHtml = `<input class="HLPheading HLPinput HLPeditName" type="text" value="${heading}"/>`;
   }
   return /*html*/`
   <div class="HLPheaderSection">
      <div class="HLPheadingContainer">
         ${headingHtml}
         <input class="HLPcell HLPinput HLPeditSummary" type="text" value="${ESChtmlEscape(summary)}" placeholder="Function summary">
      </div>
      <div class="BUTTONgroup HLPbuttonGroup">
         <div class="BUTTONstandard BUTTONaction HLPsaveButton">   SAVE   </div>
         <div class="BUTTONstandard BUTTONcancel HLPcancelButton"> CANCEL </div>
      </div>
   </div>`
}

function HLPeditDescriptionHtml(){
   return /*html*/`
   <div class="HLProwHeader HLPdescriptionHeader">
      <div class="FORMlabel">DESCRIPTION</div>
   </div>
   <div class="HLPdescriptionContent HLPedit">
      <textarea class="HLPdescription HLPinput"></textarea>
   </div>`
}

const HLPaddListParameterHtml = 
   /*html*/`<div class="HLProwSubHeader"><div class="HLPaddRow HLPaddListParameter"> + NEW PARAMETER </div></div>`

const HLPaddTableParameterHtml = 
   /*html*/`<div class="HLProwSubHeader"><div class="HLPaddRow HLPaddTableParameter"> + NEW PARAMETER </div></div>`

function HLPeditTableBlankParameterRowHtml(){
   let Parameter = {}
   Parameter.Name = '';
   Parameter.Description = '';
   Parameter.Required = false;
   return HLPeditTableParameterRowHtml(Parameter);
}

function HLPeditListBlankParameterRowHtml(){
   return /*html*/`
   <div class="HLProw HLPeditListParameters HLPlistParamRow" draggable="true">
      <div class="HLPcell HLPmoveRow">⋮⋮</div>
      <div class="HLPcell HLPinput HLPparameterDesc"></div>
      <div class="HLPcell HLPdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
   </div>`
}

function HLPeditTableParameterRowHtml(Parameter){
   return /*html*/`
   <div class="HLProw HLPeditTableParameters HLPtableParamRow" draggable="true">
      <div class="HLPcell HLPmoveRow">⋮⋮</div>
      <div class="HLPcell HLPcheckBox"><input class="HLPrequired" type="checkbox" ${Parameter.Required ? "checked" : ""}></div>
      <input class="HLPcell HLPinput HLPparameter" type="text" value="${ESChtmlEscape(Parameter.Name)}" placeholder="Name">
      <div class="HLPcell HLPinput HLPparameterDesc"></div>
      <div class="HLPcell HLPdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
   </div>`;
}

function HLPeditListParameterRowHtml(){
   return /*html*/`
   <div class="HLProw HLPeditListParameters HLPlistParamRow" draggable="true">
      <div class="HLPcell HLPmoveRow">⋮⋮</div>
      <div class="HLPcell HLPinput HLPparameterDesc"></div>
      <div class="HLPcell HLPdeleteRow"><img src="/DASH/DASHclearSearch.svg"></div>
   </div>`;
}

function HLPeditParameterHeaderHtml(ParameterTable){
   return /*html*/` 
   <div class="HLProwHeader HLProwHeaderWithToggle">
      <div class="FORMlabel">PARAMETERS</div>
		<div class="HLPparameterSwitcherToggle">
       DOES THE FUNCTION TAKE A SINGLE TABLE ARGUMENT?
			<div class="TOGtoggle">
            <input type="checkbox" class="TOGcheck HLPisTable" ${ParameterTable ? "checked" : ""} title="Toggle edit table parameters">
				<div class="TOGswitch"></div>
				<div class="TOGtrack"></div>
			</div>
		</div>
   </div>`
}

function HLPeditParametersHtml(Parameters = [], ParameterTable){
   return /*html*/`
      <div class=${ParameterTable ? "HLPeditTableParameters" : "HLPeditListParameters"}>
         ${ HLPeditParameterHeaderHtml(ParameterTable) }
         <div class="HLPtable" style="display: ${ParameterTable ? "block" : "none"}">
            <div>
               <div class="HLProwSubHeader HLPeditTableParameters">
                  <div></div>
                  <div class="FORMlabel" title="Required">REQ</div>
                  <div class="FORMlabel">NAME</div>
                  <div class="FORMlabel">DESCRIPTION</div>
                  <div></div>
               </div>
               <div class="HLPtableParameterRows"> ${ !Parameters ? HLPeditTableBlankParameterRowHtml() :
                  Parameters.map(function(Parameter){ return HLPeditTableParameterRowHtml(Parameter)}).join('')}
               </div> 
            </div>
            ${ HLPaddTableParameterHtml }
         </div>
         <div class="HLPlist" style="display: ${ParameterTable ? "none" : "block"}">
            <div>
               <div class="HLProwSubHeader HLPeditListParameters">
                  <div></div>
                  <div class="FORMlabel">DESCRIPTION</div>
               </div>
               <div class="HLPlistParameterRows"> ${ !Parameters ? HLPeditListBlankParameterRowHtml() :
                  Parameters.map(function(Parameter){ return HLPeditListParameterRowHtml(Parameter)}).join('')}
               </div>
            </div>
            ${ HLPaddListParameterHtml }
         </div>
      </div>`
}

