function DOC2formHtml(){
   const Today = DOC2datetimeLocalValue();
   return /*html*/ `
   <div class="DOC2uploadForm">
      <div class="DOC2uploadField">
         <label class="DOC2uploadLabel">Date</label>
         <input class="DOC2uploadInfo DOC2uploadDate" type="datetime-local" value=${Today}>
      </div>
      <div class="DOC2uploadField">
         <label class="DOC2uploadLabel">File</label>
         <input class="DOC2uploadInfo DOC2uploadFile" type="file">
      </div>
      <div class="DOC2uploadField">
         <label class="DOC2uploadLabel">Title</label>
         <input class="DOC2uploadInfo DOC2uploadTitle" type="text">
      </div>
      <div class="DOC2uploadField">
         <label class="DOC2uploadLabel">Description</label>
         <input class="DOC2uploadInfo DOC2uploadDesc" type="text">
      </div>
     
      <div class="DOC2uploadField">
         <label class="DOC2uploadLabel">Tags (space sep.)</label>
         <input class="DOC2uploadInfo DOC2uploadTags" type="text">
      </div>
      <button class="BUTTONaction BUTTONstandard DOC2uploadSubmit DOC2buttonDisabled">SUBMIT</button>
   </div>`;
}

function DOC2datetimeLocalValue(DateStamp = new Date()) {
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

function DOC2stripExtension(Name) {
   return Name.slice(0, Name.lastIndexOf(".")) || Name;
 }

 function DOC2mimeSubtype(Mime) {
   return Mime.substring(Mime.indexOf("/") + 1);
 }

 function DOC2toEpoch(DateStr) {
   const d = new Date(DateStr);
   return Math.floor(d.getTime() / 1000);
 }

 function DOC2fileToBase64(File, Callback) {
   const Reader = new FileReader();
   Reader.onload = () => Callback(String(Reader.result).split(',')[1]);
   Reader.readAsDataURL(File);
 }

function DOC2uploadSubmit(E){
   const Form = E.target.closest('.DOC2uploadForm');
   const File = Form.querySelector(".DOC2uploadFile").files[0];
 
   let D = {
      title:      (Form.querySelector(".DOC2uploadTitle")?.value.trim() || "") !== "" ? Form.querySelector(".DOC2uploadTitle").value.trim() : DOC2stripExtension(File.name),
      source:     DOC2mimeSubtype(File.type),
      created:    DOC2toEpoch(Form.querySelector(".DOC2uploadDate").value),
      tags:       Form.querySelector(".DOC2uploadTags").value.replace(/#/g, '') .trim().split(/\s+/),
      markdown:   Form.querySelector(".DOC2uploadDesc").value,
      component:  PAGEhashParameter("component")
   };

   DOC2fileToBase64(File, function(b64){
     D.data = b64;
     APIcall("doc/upload", D, function(R){ DOC2uploadSubmitResult(R); });
   });
 }

 function DOC2uploadSubmitResult(R){
   if (!R.success){
      document.querySelector(".DOC2uploadForm").innerText = R.error;
   } else {
      document.querySelector(".DOC2uploadForm").innerHTML = "File uploaded successfully.";
      PAGEsetHashParameter("id",R.data.id);
      PAGEsetHashParameter("created",R.data.created);
   }
}

function DOC2uploadDialog(){
   let H = DOC2formHtml();
   POPpopoutOpen({title : "Upload new documents", content : H, callback : DOC2adapterRefresh});
   document.querySelector(".DOC2uploadForm").addEventListener('keydown', function(E){ DOC2uploadbuttonControl();});
   document.querySelector(".DOC2uploadSubmit").addEventListener('click', function(E){ DOC2uploadSubmit(E); });
}

function DOC2uploadbuttonControl(){
   let Div = document.querySelector(".DOC2uploadSubmit");
   if(!Div) return;
   let File = document.querySelector(".DOC2uploadFile").value;
   let Desc = document.querySelector(".DOC2uploadDesc").value;
   let Date = document.querySelector(".DOC2uploadDate").value;
   let Tags = document.querySelector(".DOC2uploadTags").value;
   let Enabled = ! (File == "") && !(Desc == "") && !(Date == "") && !(Tags == "");
   if(!Enabled) Div.classList.add   ("DOC2buttonDisabled");
   else         Div.classList.remove("DOC2buttonDisabled");
}

