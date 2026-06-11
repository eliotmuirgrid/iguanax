async function TRANShandleFileData(Str, Type){
   const Data = TRANScomponentData();
   const Name = Data?.name || "";
   const Description = Data?.description ? `, Description: "${Data.description}"` : "";
   const Repo = Data?.run_repo ? `, Git: ${Data.run_repo}` : "";
   // const Prefix = (Type == "Source Code") ? "Source code for" : "Sample data for";
   let Text = `-- ${Type} for the ${Name} component${Description}${Repo}.\n${Str}`;
   await navigator.clipboard.writeText(Text);
   SNCKsnackbar('info', `${Type} contents copied to clipboard`, 6000);
}

function TRANSsampleContentClick(){
   let Data = TRANScomponentArgs();
   APIcall("export/dev/samples", Data, function(R){
      if(!R.success) return SNCKsnackbar('error', R.error, 6000);
      TRANShandleFileData(R.data.strdata, "Sample Data");
   });
}

function TRANSprojectContentClick(){
   let Data = TRANScomponentArgs();
   APIcall("export/dev/files", Data, function(R){
      if(!R.success) return SNCKsnackbar('error', R.error, 6000);
      TRANShandleFileData(R.data.strdata, "Source Code");
   });
}