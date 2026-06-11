let DASHrenderedColumns = undefined;

function DASHcolumnHeaderHtml(T){
   let Name  = T?.name || "";
   let Title = T?.title || "";
   if(Title) Title = ` title="${Title}"`;
   let Width = T?.width || "40";
   Width = ` style="width: ${Width}px"`;
   return /*html*/`<div class="DASHtableHeader DASHcenterText" ${Title}${Width}>${Name}</div>`;
}

function DASHcolumnValueHtml(T, Value){
   let Width = T?.width || "40";
   Width = ` style="width: ${Width}px"`;
   return /*html*/`<div class="DASHcenterText" ${Width}>${Value}</div>`;
}

function DASHnewColumns(Data){
   let NewCols = DASHcolumnData();
   let Html = "";
   for(let col of NewCols) {
      let Name = col?.name || "";
      let Value = Data[Name] || "";
      Html = Html + DASHcolumnValueHtml(col, Value);
   }
   return Html;
}

function DASHcolumns(){
   let Data = DASHcolumnData();
   if(DEEPequal(Data, DASHrenderedColumns)) return;
   DASHrenderedComponentData = {};
   let Html = DASHheadersHtml();
   let NewCols = "";
   for(let col of Data) NewCols = NewCols + DASHcolumnHeaderHtml(col);
   Html = Html + NewCols;
   let div = document.querySelector(".DASHtableHeading");
   if(!div) return;
   div.innerHTML = Html;
   DASHrenderedColumns = Data;
}