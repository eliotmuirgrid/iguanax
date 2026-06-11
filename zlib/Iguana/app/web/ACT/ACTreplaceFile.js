function ACTreplaceFileConfirmHtml(Filename){ 
   return /*html*/`
   <p class="ACTdeleteDirMargin ACTbreak">    A file named ${Filename} already exists in this folder. </p>
   <br>
   <p class="ACTdeleteDirMargin"><b class="ACTbreak"> Do you want to replace it? </b></p>
   <br>
   <br>
   <div class='BUTTONgroup ACTbuttons'>
      <div class='ACTclose BUTTONstandard BUTTONcancel'> CANCEL    </div>
      <div class='ACTreplace BUTTONstandard BUTTONdefault'> REPLACE   </div>
      <div class='ACTkeep BUTTONstandard BUTTONaction'> KEEP BOTH </div>
   </div>`
}

function ACTreplaceFileWindow(Filename, Filepath, ExistingFiles) {
   let Card = POPpopoutOpen({title: 'Confirm Replace File', 
      content: ACTreplaceFileConfirmHtml(Filename),
      width : "500px",
      esc_close : true,
      callback : TRANSsetCode
   });
   document.querySelector('.ACTreplace').addEventListener('click', function(){ ACTreplaceFile  (Filepath, Card)});
   document.querySelector('.ACTkeep')  .addEventListener('click', function(){ ACTkeepBothFiles(Filename, Filepath, ExistingFiles, Card)});
   document.querySelector('.ACTclose') .addEventListener('click', function(){ POPpopoutClose  (Card)});
}

function ACTkeepBothFiles(Filename, Filepath, ExistingFiles, Card){
   const NewPath = ACTparentFolder(Filepath) +  ACTgenerateReplaceName(Filename, ExistingFiles);
   ACTpasteApiCall(NewPath, function(){ MODELforcePoll('TRANSmodel'); });
   POPpopoutClose(Card);
}

function ACTgenerateReplaceName(Filename, ListOfNames = []){
   let   NewName = '';
   let   Name      = ACTsplitName(Filename).Name;
   const Extension = ACTsplitName(Filename).Ext;
   for(let i = 2; true; i++){ 
      NewName = `${Name} ${i}${Extension}`;
      if(!ListOfNames.includes(NewName)) return NewName;
   }
}

function ACTreplaceFile(FilePath, Popout){
   POPpopoutClose(Popout);
   ACTdeleteFile(FilePath, function(){
      ACTpasteApiCall(FilePath, function(){
         if (FilePath === TRANSfile()) TRANSfileCurrentReload();
         else                          MODELforcePoll('TRANSmodel');
      });
   })
}

function ACTdeleteFile(FilePath, Callback){
   APIcall('component/file/delete', { path : FilePath, component : TRANScomponentGuid(), development : TRANSdevelopment()}, function(Response){
      if(!Response.success) return SNCKsnackbar('error', Response['error']);
      TRANSfileCurrentReload();
      if(Callback) Callback();
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
}