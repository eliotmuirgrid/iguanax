// See IX-1262 for more ideas on what could be done for the help editor

function HLPrenderHelpView(help){
   return /*html*/`<div class="HLPcontainer"> ${HLPviewContent(help)} </div>`
}

function HLPviewContent(help){
   return `${HLPheaderHtml(HLPnameResolve(help), help.SummaryLine, help.is_core)}
            ${HLPparametersHtml(help.ParameterTable, help.Parameters)}
            ${HLPdescriptionHtml()}`;
}

function HLPheaderHtml(Title, Summary, IsCore) {
   return /*html*/`
   <div class="HLPheaderSection">
      <div class="HLPheadingContainer">
         <h1 class="HLPheading">${ESChtmlEscape(Title.replace(/\//g, "."))}</h1>
         <div class="HLPsummary">${ESChtmlEscape(Summary)}</div>
      </div>
      ${IsCore && APP_HASH ? `` : `<div class="BUTTONdefault BUTTONstandard HLPeditButton">EDIT</div>`}
   </div>`
}

function HLPparametersHtml(TableMode, Parameters){
   return TableMode ? HLPtableParametersHtml(Parameters) : HLPlistParametersHtml(Parameters)
}

function HLPdescriptionHtml(){
   return /*html*/`
   <div class="HLPdescriptionContent HLPview">
      <textarea class='HLPdescription HLPinput'></textarea>
   </div>`
}

const HLPtableParameterHeadersHtml = /*html*/`
   <div class="HLProwHeader HLPviewTableParameters">
      <div class="FORMlabel">PARAMETERS</div>
   </div>`

function HLPtableParameterRowHtml(Parameter){
   let Param = '';
   if (Parameter.Required){
      Param = "<b title='Required'>" + ESChtmlEscape(Parameter.Name) + "</b>";
   } else {
      Param = ESChtmlEscape(Parameter.Name);
   }
   return /*html*/`
   <div class="HLProw HLPviewTableParameters HLPtableParamRow">
      <div class="HLPcell HLPvalue">${Param}</div>
      <div class="HLPcell HLPinput HLPparameterDesc"></div>
   </div>`;
}

function HLPtableParametersHtml(Parameters = []){
   if (!Parameters) return '';
   return /*html*/`
         ${ HLPtableParameterHeadersHtml +
            Parameters.map(function(Parameter) {return HLPtableParameterRowHtml(Parameter)}).join('')}<br>`
}

const HLPlistParameterHeadersHtml = /*html*/`
   <div class="HLProwHeader HLPviewListParameters">
      <div class="FORMlabel">PARAMETERS</div>
   </div>`

function HLPlistParameterRowHtml(Parameter, Index){
   return /*html*/`
   <div class="HLProw HLPviewListParameters HLPlistParamRow">
      <div class="HLPcell HLPvalue HLPindex">${Index}</div>
      <div class="HLPcell HLPinput HLPparameterDesc"></div>
   </div>`;
}

function HLPlistParametersHtml(Parameters = []){
   if (!Parameters){ return ''; }
   let Index = 1
   return /*html*/`
         ${ HLPlistParameterHeadersHtml +
            Parameters.map(function(Parameter) {return HLPlistParameterRowHtml(Parameter, Index++)}).join('')}<br>`
}
