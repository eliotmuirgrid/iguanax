function DOCformHtml(){
   const Today = DOCdatetimeLocalValue();
   return /*html*/ `
   <div class="DOCuploadForm">
      <div class="DOCuploadField">
         <label class="DOCuploadLabel">Date</label>
         <input class="DOCuploadInfo DOCuploadDate" type="datetime-local" value=${Today}>
      </div>
      <div class="DOCuploadField">
         <label class="DOCuploadLabel">File</label>
         <input class="DOCuploadInfo DOCuploadFile" type="file">
      </div>
      <div class="DOCuploadField">
         <label class="DOCuploadLabel">Title</label>
         <input class="DOCuploadInfo DOCuploadTitle" type="text">
      </div>
      <div class="DOCuploadField">
         <label class="DOCuploadLabel">Description</label>
         <input class="DOCuploadInfo DOCuploadDesc" type="text">
      </div>
     
      <div class="DOCuploadField">
         <label class="DOCuploadLabel">Tags (space sep.)</label>
         <input class="DOCuploadInfo DOCuploadTags" type="text">
      </div>
      <button class="BUTTONaction BUTTONstandard DOCuploadSubmit DOCuploadDisabled">SUBMIT</button>
   </div>`;
}

function DOCdatetimeLocalValue(DateStamp = new Date()) {
   const pad = n => String(n).padStart(2, "0");
   return (
     DateStamp.getFullYear() + "-" +
     pad(DateStamp.getMonth() + 1) + "-" +
     pad(DateStamp.getDate()) + "T" +
     pad(DateStamp.getHours()) + ":" +
     pad(DateStamp.getMinutes()) + ":" +
     pad(DateStamp.getSeconds())
   );
 }

function DOCstripExtension(Name) {
   return Name.slice(0, Name.lastIndexOf(".")) || Name;
 }

 function DOCmimeSubtype(Mime) {
   return Mime.substring(Mime.indexOf("/") + 1);
 }

 function DOCtoEpoch(DateStr) {
   const d = new Date(DateStr);
   return Math.floor(d.getTime() / 1000);
 }

 function DOCfileToBase64(File, Callback) {
   const Reader = new FileReader();
   Reader.onload = () => Callback(String(Reader.result).split(',')[1]);
   Reader.readAsDataURL(File);
 }

function DOCuploadSubmit(E){
   const Form = E.target.closest('.DOCuploadForm');
   const File = Form.querySelector(".DOCuploadFile").files[0];
 
   let D = {
      title:      (Form.querySelector(".DOCuploadTitle")?.value.trim() || "") !== "" ? Form.querySelector(".DOCuploadTitle").value.trim() : DOCstripExtension(File.name),
      source:     DOCmimeSubtype(File.type),
      created:    DOCtoEpoch(Form.querySelector(".DOCuploadDate").value),
      tags:       Form.querySelector(".DOCuploadTags").value.replace(/#/g, '') .trim().split(/\s+/),
      markdown:   Form.querySelector(".DOCuploadDesc").value,
      component:  PAGEhashParameter("component")
   };

   DOCfileToBase64(File, function(b64){
     D.data = b64;
     APIcall("doc/upload", D, function(R){ DOCuploadSubmitResult(R); });
   });
 }

 function DOCuploadSubmitResult(R){
   if (!R.success){
      document.querySelector(".DOCuploadForm").innerText = R.error;
   } else {
      document.querySelector(".DOCuploadForm").innerHTML = "File uploaded successfully.";
      PAGEsetHashParameter("id",R.data.id);
      PAGEsetHashParameter("created",R.data.created);
   }
}

function DOCuploadClick(){
   document.querySelector(".DOCupload").addEventListener('click', function(E){DOCuploadDialog();});
}

function DOCuploadDialog(){
   let H = DOCformHtml();
   POPpopoutOpen({title : "Upload new documents", content : H, callback : DOCadapterRefresh});
   document.querySelector(".DOCuploadForm").addEventListener('keydown', function(E){ DOCuploadbuttonControl();});
   document.querySelector(".DOCuploadSubmit").addEventListener('click', function(E){ DOCuploadSubmit(E); });
}

function DOCuploadbuttonControl(){
   let Div = document.querySelector(".DOCuploadSubmit");
   if(!Div) return;
   let File = document.querySelector(".DOCuploadFile").value;
   let Desc = document.querySelector(".DOCuploadDesc").value;
   let Date = document.querySelector(".DOCuploadDate").value;
   let Tags = document.querySelector(".DOCuploadTags").value;
   let Enabled = ! (File == "") && !(Desc == "") && !(Date == "") && !(Tags == "");
   if(!Enabled) Div.classList.add   ("DOCuploadDisabled");
   else         Div.classList.remove("DOCuploadDisabled");
}

