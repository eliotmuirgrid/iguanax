function MKEmakeEditable(editOriginal, btn) { 
   if(!btn || btn.classList.contains('BUTTONdisabled')) { return; }
   APIcall("component/make_editable", {component: MKEcomponent(), edit_original : editOriginal}, function(Response) {
      if(!Response.success) { return MKEcustomizeError(Response.error); }
      MKEsetDevelopment(editOriginal);
   });
}

function MKEsetDevelopment(editOriginal) {
   let Name = MKEdata().name;
   let Message = editOriginal ? GINkeepOriginalMakeEditable(Name) : GINcreateCopyMakeEditable(Name);
   APIcall('component/set_commit', {component: MKEcomponent(), commit: 'DEVELOPMENT'}, function(Response) {
      if(!Response.success) { return SNCKsnackbar('error', Response.error, 6000);}
      GINaddAndCommit({message: Message, component: MKEcomponent()});
      POPpopoutClose(MKEpopout);
      MODELremove('MKEmodel');
      if(MKEcallback) { MKEcallback(); }
   });
}