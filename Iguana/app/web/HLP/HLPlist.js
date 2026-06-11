let HLPinputTimeout = null;

function HLPfilter(Data, Keyword) {
   clearTimeout(HLPinputTimeout);
   HLPinputTimeout = setTimeout(() => {
      Data.forEach(item => {
         item.hide = !item.name.toLowerCase().includes(Keyword.toLowerCase());
      });
      HLPgenerateList(Data);
      HLPjumpToFunction(HLPgetSelected())
   }, 400);
}

function HLPlistAll(FunctionName) {
   if(FunctionName && FunctionName.substring(0, 3) === '_G.') { FunctionName = FunctionName.substring(3); }
   COL_TRC(FunctionName);
   if(!HLPisWindowOpen()) {
      HLPopenWindow('HLPfunctionPage', HLPcontentHtml(), "Help Index", "WNDdockRight");
   }
   HLPpopulateLinks(FunctionName);
   HLPsearchFocus();
   HLPupdateList();
}

function HLPsearchFocus(){
   let div = document.querySelector(".HLPsearch");
   FORMfocusDelay(div);
}

function HLPpopulateLinks(FunctionName) {
   COL_TRC(FunctionName);
   APIcall("help/index", HLPcreateRequestData(FunctionName), HLPhandleResponse);
}

function HLPcreateRequestData(FunctionName) {
   return {
      component    : TRANScomponentGuid(),
      name         : FunctionName,
      development  : TRANSdevelopment()
   };
}

function HLPhandleResponse(R) {
   if(!R.success) {
      HLPcloseWindow();
      return SNCKsnackbar('error', `${R.error} Cannot load HELP index.`);
   }

   const {name: functionName, list: listData} = R.data;
   const sortedData                           = HLPsortHelpEntries(listData);
   const exists                               = HLPfindHelpEntry(sortedData, functionName);
   HLPgenerateList(sortedData);
   const targetName = HLPdetermineTargetName(functionName, exists, sortedData);
   HLPjumpToFunction(targetName)
   HLPsetupHelpComponents(sortedData);
}

function HLPsortHelpEntries(listData) {
   const deprecatedFunctions = MODELdata?.DEPdata?.data;
   return [...listData].sort((a, b) => {
      const aDeprecated = deprecatedFunctions?.hasOwnProperty(a.name) || false;
      const bDeprecated = deprecatedFunctions?.hasOwnProperty(b.name) || false;
      if(aDeprecated !== bDeprecated) { return aDeprecated ? -1 : 1; }
      const aName = a.name.toLowerCase();
      const bName = b.name.toLowerCase();
      if (aName < bName) return -1;
      if (aName > bName) return 1;
      return 0;
   });
}

function HLPfindHelpEntry(listData, functionName) { return listData.find(item => item.name === functionName); }

const HLPlistListener = (e) => {
   if(!e.target.classList.contains('HLPlink')) return;
   HLPhandleClick(e.target, true);
}

function HLPgenerateList(Data) {
   const functionsList = document.querySelector('.HLPlistBody');

   HLPgetAllFunctions().forEach(el => { delete el.dataset.location; });

   const fragment = document.createDocumentFragment(); // better performance
   const selected = HLPgetSelected();

   Data.forEach(help => {
      const linkItem            = document.createElement('div');
      linkItem.innerText        = help.name;
      linkItem.dataset.location = help.location;
      linkItem.classList.add('HLPlink');

      if(help.name === selected) {
         linkItem.classList.add('HLPstrong');
      }

      if(help.hide) {
         linkItem.classList.add('HLPhide');
      }

      fragment.appendChild(linkItem);
   });
   functionsList.innerHTML = '';
   functionsList.appendChild(fragment);

   functionsList.removeEventListener('click', HLPlistListener);
   functionsList.addEventListener('click', HLPlistListener);
   HLPtoggleDeprecated();
}

function HLPdetermineTargetName(functionName, exists, listData) {
   const deprecatedFunctions = MODELdata?.DEPdata?.data;
   if((!functionName || !exists) && listData.length > 0) {
      if(HLPnavigationHistory.length > 0) return HLPnavigationHistory.slice(-1)[0].name;
      for(let i = 0; i < listData.length; i++) {
         if(!deprecatedFunctions?.hasOwnProperty(listData[i].name)) {
            return listData[i].name;
         }
      }
   }
   return functionName;
}

function HLPsetupHelpComponents(listData) {
   SPLTsetupSplitter('.HLPcontent', '.HLPresizeControl', '.HLPindexContainer', 'HLPratio');
   HLPsetupSearchListener(listData);
   HLPsetupDepToggleListener();
   HLPsetupNavListeners();
}