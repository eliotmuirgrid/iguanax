let SMPLselectedLanguage = {};

const SMPLtoolbarFileTypeHtml = /*html*/`
<select class='SMPLfileTypeSelect FORMselectBox'>
   <option value="default" disabled>-Language-</option>
   <option value='css'>CSS</option>
   <option value='dbs'>DBS</option>
   <option value='hl7'>HL7</option>
   <option value='htmlmixed'>HTML</option>
   <option value='http'>HTTP</option>
   <option value='javascript'>JSON</option>
   <option value='lua'>Lua</option>
   <option value='plain' selected="selected">Plain Text</option>
   <option value='sql'>SQL</option>
   <option value='xml'>XML</option>
</select>`;

function SMPLbracketMatch(Message){
   COL_TRC(Message);
   let BracketDepth = 0;
   for(let i=0; i<Message.length; i++){
      if(Message[i] == '{'){
         BracketDepth++;
      }
      else if(Message[i]  == '}'){
         BracketDepth--;
         if(BracketDepth < 0){
            return false;
         }
      }
   }
   COL_TRC(BracketDepth);
   return BracketDepth == 0;
}

function SMPLjsonMatch(Message){
   let Copy = Message.replace(/\{\{IFWTOKEN\}\}.*?\{\{IFWTOKEN\}\}/g, "");
      try{
         JSON.parse(Copy);
         COL_TRC("JSON parse worked - so it's JSON!");
         return "json";
      }
      catch(e){
         COL_TRC("JSON parse failed - so it's not JSON");
         return "plain";
      }
}

// Eliot TODO - that this function returns SMPLhl7Mode is a red flag to me that
// the concerns are not separated enough.
function SMPLautoDetectFileType(Code) {
   COL_TRC(Code);
   if(!TRANSfileValid) return "lua";
   let Hl7Regex = /^(MSH|FHS|BHS)/i;
   let XmlRegex = /(<\?XML.*\?>|\!DOCTYPE HTML)/i;
   let HttpRegex = /HTTP/i;
   let JsonRegex = /\s*\{+|\[+/i;
   if (Hl7Regex. test(Code)) { COL_TRC("HL7 detected"); return "hl7"; }
   if (XmlRegex. test(Code)) { COL_TRC("XML detected"); return "xml";}
   if (HttpRegex.test(Code)) { COL_TRC("HTTP detected"); return "http";}
   if (JsonRegex.test(Code)) { COL_TRC("JSON detected"); return "javascript";} // why do we call it javascript?
   if (SMPLbracketMatch(Code)) {
      COL_TRC("Brackets matched so...")
      return SMPLjsonMatch(Code);
   }
   COL_TRC("Unable to auto detect file format, returning plain.");
   return "plain";
}

function SMPLsetLanguageMode(Type, Code) {
   // COL_TRC("Type=", Type, "Code=", Code);
   switch (Type) {
      case "txt":
      case "other":  Type = "null";       break;
      case "html":   Type = "htmlmixed";  break;
      case "hl7":    Type = "hl7";        break;
      default:                            break;
   }
   if (Type === "null") {
      COL_TRC("We are going to try and auto sniff the file type");
      Type = SMPLautoDetectFileType(Code);
   }
   COL_TRC(Type);
   // set code mirror mode
   if(TRANSeditor?.options?.mode !== Type){
      TRANSeditor.setOption("mode", Type);
   }
   // set file type select
   let Div = document.querySelector(".SMPLfileTypeSelect");
   if(!Div) return;
   if(Div.value !== Type) Div.value = Type;
}

function SMPLtoolbarRenderFileType() {
   if (!TRANSeditor){
      COL_TRC("Calling this before editor is set up");
      return;
   }
   SMPLsetLanguageMode(SMPLselectedLanguage[TRANSselectedFile()] || TRANScurrentFileType(), TRANSeditor.getValue());
}

// TODO - THIS IS BEING CALLED BEFORE THE EDITOR IS INITIALIZED.
function SMPLtoolbarSetUpFileType() {
   COL_TRC("SMPLtoolbarSetUpFileType");
   document.querySelector(".SMPLtoolbarFileType").innerHTML = SMPLtoolbarFileTypeHtml;
   document.querySelector(".SMPLfileTypeSelect").addEventListener('change', function() {
      SMPLselectedLanguage[TRANSselectedFile()] = document.querySelector(".SMPLfileTypeSelect").value;
      MODELupdate('TRANSmodel');
      TRANSsetCode();
   });
   if (TRANSeditor){
      COL_TRC("Setting the language mode here.  This feels more like a view function - separate concerns.");
      SMPLsetLanguageMode(TRANScurrentFileType(), TRANSeditor.getValue());
   } else {
      COL_TRC("No TRANSeditor - probably not set up yet.");
   }
}
