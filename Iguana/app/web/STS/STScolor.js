let STScolorObj;
let STScolorPopout;
let STSdefaultColor =  "#00609f";

function STScolorChanged(){
   let Div = document.querySelector(".STScolor");
   let Input = document.querySelector(".STScolorInput");
   if(!Div || !Input) return;
   Div.style.backgroundColor = Input.value;
}

function STScolorClicked(){
   STSselectColorPopout();
   STSinitializeColorWheel();
}

function STSinitializeColorWheel(){
   let Input = document.querySelector(".STScolorInput");
   let Color = STSdefaultColor;
   if(Input && Input.value) Color = Input.value;
   STScolorObj = new iro.ColorPicker(".STScolorSelectDiv", { width: 200, color: Color});
}

function STSselectColorHtml(){
   return /*html*/`\
   <p class="STScolorInfo">Please select a color to identify the server, as it will be used to style the sidebar: <br>[<a class="STSresetColor">Reset</a>]</p>
   <br>
   <div class="STScolorSelectDiv"></div>
   <br>
   <div class="BUTTONgroup CARDtemplateRemoveButtons">
      <div class='STSselectColorCancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      <div class='STSselectColorButton BUTTONaction BUTTONstandard'>CONFIRM</div>
   </div>`;
}

function STSselectColorPopout(){
   let Content = STSselectColorHtml();
   STScolorPopout = POPpopoutOpen({title: 'Server Color', content: Content, width : "555px"});
   document.querySelector('.STSresetColor')       .addEventListener('click', function() { STSresetColor();  });
   document.querySelector('.STSselectColorButton').addEventListener('click', function() { STSsetServerColor(); });
   document.querySelector('.STSselectColorCancel').addEventListener('click', function() { POPpopoutClose(STScolorPopout);  });
}

function STSresetColor(){
   STScolorObj.color.set(STSdefaultColor);
}

function STSsetServerColor(){
   let Color = STScolorObj.color.hexString;
   let Input = document.querySelector(".STScolorInput");
   if(!Input) return;
   Input.value = Color;
   POPpopoutClose(STScolorPopout); 
   STScolorChanged();
}

