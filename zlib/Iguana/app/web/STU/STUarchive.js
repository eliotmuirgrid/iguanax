function STUarchivePathUpdate(){
   let Data = STUarchiveData();
   let Path = Data?.dir || "";
   let div = document.querySelector(".STUarchivePath");
   if(!div) return;
   if(div.innerHTML != Path) div.innerHTML = Path;
}

function STUarchiveSizeUpdate(){
   let Data = STUarchiveData();
   let Val = Data?.size || undefined;
   let Size = STUfuzzySize(Val);
   let div = document.querySelector(".STUarchiveSize");
   if(!div) return;
   if(div.innerHTML != Size) div.innerHTML = Size;
}