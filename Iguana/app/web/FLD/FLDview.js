let FLDdataRendered;
let FLDwindowId = "FLDdefinitionWindow";

function FLDview(FilePath){ FLDloadFieldDefinition(FLDrenderView, FilePath); }

function FLDloadFieldDefinition(Callback, FilePath="config.json"){
   const Parameters = {
      "component"   : TRANScomponentGuid(),
      "development" : TRANSdevelopment()
   }
   APIcall("component/field/grammar", Parameters, function(Response){
      if (Response.success) {
         if(Callback) Callback(Response.data);
         return;
      }
      console.log("Cannot render custom field window, opening as normal file");
      SNCKsnackbar('error', "Unable to open in Custom Fields editor: " + Response.error, 6000);
      TRANSfileChange(FilePath);
      WNDcloseWin(FLDwindowId);
   });
}

function FLDrenderView(Data){ WNDwindow(FLDwindowId, FLDviewContent(Data), "Custom Fields", "", "PRJicon-config"); }

function FLDviewContent(Data){
   FLDdataRendered = Data;
   let FieldHtml = /*html*/`<div class="FLDmodal">`;
   if(!TRANSreadOnly()) FieldHtml += FLDeditPriviledgesHtml;
   FieldHtml += FLDviewHeaderTitlesHtml;
   if(Data !== null) { for(Field of Data){ FieldHtml += FLDregularFieldViewHtml(Field); } }
   if(Data === null || Data.length === 0){ FieldHtml += FLDblankFieldsViewHtml; }
   FieldHtml += /*html*/`</div>`;
   return FieldHtml;
}

const FLDeditPriviledgesHtml =
/*html*/`
<div class="FLDsaveConfigButton BUTTONgroup">
   <div class="FLDeditFields BUTTONdefault BUTTONstandard">EDIT</div>
</div>`;

const FLDviewHeaderTitlesHtml =
/*html*/`
<div class="FLDviewFieldsHeader">
   <div class="FORMlabel">FIELD NAME</div>
   <div class="FORMlabel">FIELD TYPE</div>
   <div class="FORMlabel">DEFAULT VALUE</div>
   <div class="FORMlabel">DESCRIPTION</div>
</div>`;

function FLDviewDefaultValueHtml(Field){
   if(Field.type === "list"){
      return /*html*/`
         <div class="FLDtableLastColumn">
            <div class="FLDcurrentValue">${ESChtml(Field.default)}</div>
            <div class="FLDcurrentValue">${Field.options? ESChtml(Field.options) : ''}</div>
         </div>`;
   }
   return /*html*/`<div class="FLDcurrentValue">${ESChtml(Field.default)}</div>`;
}

function FLDregularFieldViewHtml(Field){
   return /*html*/`
   <div class="FLDviewFields">
      <div class="FLDrowName">${ESChtml(Field.name)}</div>
      <div class="FLDcurrentValue">${Field.type}</div>
      ${FLDviewDefaultValueHtml(Field)}
      <div class="FLDcurrentValue">${Field.description ? ESChtml(Field.description) : ''}</div>
   </div>
   `;
}

const FLDblankFieldsViewHtml =
/*html*/`
<div class="FLDviewFieldsBlank">
   <div class="FLDblankRow">No custom fields are currently configured</div>
</div>`;
