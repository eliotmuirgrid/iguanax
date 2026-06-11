const LGE_THRESHOLD = 5000; // 5 seconds
// Functions for rendering the on hover tooltips/popover for the logs and errors on CARD screen.

function LGElogRecentEnough(LatestTime, CurrentTime){
   if (isNaN(CurrentTime)) return false
   return (LatestTime - CurrentTime) < LGE_THRESHOLD;
}

// Concatenate an array of content into safe html to display
function LGEprocessPopoverContent(ContentArray, ClassName){
   let HtmlOut = "";
   let classHtml = ClassName ? ` class="${ClassName}"` : "";
   for (let i = 0; i < ContentArray.length; i++){
      HtmlOut += `<div${classHtml}>${ContentArray[i]}</div>`
   }
   return HtmlOut;
}

// Takes the response from /logs/retrieve and returns html to put into the popover/tooltip
function LGEgetPopoverContent(Data, ClassName){
   let Output = [];
   if (!Data[0]?.content) return "";
   Output.push(Data[0]?.content);
   let LatestTime = Number(Data[0]?.time)
   if (isNaN(LatestTime)) return Output;
   // Add to Output all the entries recent enough to display
   for (let i = 1; i < Data.length; i++){
      if (!LGElogRecentEnough(LatestTime, Number(Data[i]?.time))) break;
      if (Data[i]?.content.length) Output.push(Data[i]?.content);
   }
   return LGEprocessPopoverContent(Output, ClassName);;
}

function LGEconvertLogErrorToUrl(guid, dev, content) {
   if(!content) { return content; }
   const prefix = dev ? `dev${FILsep()}` : `run${FILsep()}`;
   return content.replace(/([a-zA-Z0-9_\-.\/\\]+):(\d+)/g, (match, filePath, lineNumber) => {
      let editPath    = filePath;
      const devPrefix = `${prefix}`;
      if(filePath.includes(devPrefix)) {
         editPath = filePath.substring(filePath.indexOf(devPrefix) + devPrefix.length);
      }
      const guidDevPrefix = guid + `${FILsep()}${prefix}`;
      if(filePath.includes(guidDevPrefix)) {
         editPath = filePath.substring(filePath.indexOf(guidDevPrefix) + guidDevPrefix.length);
      }
      const href = `${LGEformatUrlClean(guid, dev, editPath)}`;
      return `<a href="${href}" class="STOREfileLink">${filePath}:${lineNumber}</a>`;
   });
}

function LGEformmattedContent(Data, guid, dev, errors){
   let Content = LGEgetPopoverContent(Data);
   let Html = LGEconvertLogErrorToUrl(guid, dev, Content);
   if(errors) Html = `<div><a class="LGEclearErrors">Clear Errors</a></div>` + Html;
   return Html;
}

function LGEformatUrlClean(component, dev, file) {
   if(!component) { return; }
   if(!file) { file = "main.lua"; }
   file = file.replaceAll('\\', '/');
   let Url    = "#translator";
   const Data = {component: component, development: dev, edit: file};
   return Url + PAGEformatParams(Data, true);
}