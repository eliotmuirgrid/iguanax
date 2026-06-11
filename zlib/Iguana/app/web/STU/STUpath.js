function STUpathUpdate(){
   let Data = STUlogData();
   let Path = Data?.log_path || "";
   let div = document.querySelector(".STUdirPath");
   if(!div) return;
   if(div.innerHTML != Path) div.innerHTML = Path;
}

function STUnotSetHtml(){
   return /*html*/`<div class="STUnotSet">No folder selected</div>`;
}

function STUaltPathUpdate(){
   let Data = STUbackupData();
   let Path = Data?.log_path || STUnotSetHtml();
   let div = document.querySelector(".STUaltDirPath");
   if(!div) return;
   if(div.innerHTML != Path) div.innerHTML = Path;
}

function STUsizeUpdate(){
   let Data = STUlogData();
   let Val = Data?.dir_size || undefined;
   let Size = STUfuzzySize(Val);
   let div = document.querySelector(".STUlogSize");
   if(!div) return;
   if(div.innerHTML != Size) div.innerHTML = Size;
}

function STUaltSizeUpdate(){
   let Data = STUbackupData();
   let Val = Data?.dir_size || undefined;
   let Size = STUfuzzySize(Val);
   let div = document.querySelector(".STUaltLogSize");
   if(!div) return;
   if(div.innerHTML != Size) div.innerHTML = Size;
}

function STUfuzzySize(Size) {
   if (Size === undefined) return "...";
   if (Size < 0)  { throw new Error('Size must be a positive number'); }
   let Count = 0;
   while (Size > 1000) {
      Size /= 1000;
      Count++;
   }
   const Units = ["B", "KB", "MB", "GB", "TB"];
   if (Count > 0) { Size = Size.toFixed(2); }
   return Size + " " + Units[Count];
}
