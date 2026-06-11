function WNDstringSlider(id){
   let Div = document.querySelector(`#${id}`);
   if(!Div) return;
   return Div.querySelector(".string_slider");
}

function WNDfirstLineWidth(id){
   let Div = document.querySelector(`#${id}`);
   if(!Div) return 0;
   let Content = Div.querySelector(".string_slider .content .chunk");
   if(!Content) return 0;
   let WidthChar = WNDfindLongestRowLength(Content.innerHTML, true) * 7.2;
   let SideWidth = 160;
   let CopyButton = 98;
   return WidthChar + SideWidth + CopyButton;
}

function WNDresizeStringSliderWidth(id){
   let Div = document.querySelector(`#${id}`);
   if(!Div) return 0;
   let Content = Div.querySelector(".string_slider .content .chunk");
   if(!Content) return 0;
   let WidthChar = WNDfindLongestRowLength(Content.innerHTML, false) * 7.2;
   let SideWidth = 160;
   return WidthChar + SideWidth;
}

function WNDresizeStringSliderHeight(id){
   let Div = document.querySelector(`#${id}`);
   if(!Div) return 0;
   let Content = Div.querySelector(".string_slider .content .chunk");
   if(!Content) return 0;
   let HeightChar = WNDcountRows(Content.innerHTML) * 16;
   COL_TRC(HeightChar, "Client Height=", Content.clientHeight);
   let SideHeight = 90;
   return HeightChar + SideHeight;
}

function WNDfindLongestRowLength(inputString, firstOnly) {
   const lines = inputString.split("\n");
   let longestRowLength = 0;
   for (const line of lines) {
     const lineLength = line.length;
     if (lineLength > longestRowLength) {
       longestRowLength = lineLength;
     }
     if(firstOnly) return longestRowLength;
   }
   return longestRowLength;
}

function WNDcountRows(inputString) {
   const lines = inputString.split("\n");
   return lines.length;
}

function WNDhandleResize(id, force){
   let Div = document.querySelector(`#${id}`);
   if(!Div) return;
   if(!Div.classList.contains("WNDwindow"))     return;
   if(Div.classList.contains("WNDautoResized") && !force) return;
   let Resized = WNDautoSizeWindow(id, force);
   if(Resized) Div.classList.add("WNDautoResized");
}

function WNDautoSizeWindow(id, force){
   let Div = document.querySelector(`#${id}`);
   if(!Div) return;
   let ContentDiv = Div.querySelector(".WNDcontent");
   if(!ContentDiv) return;
   if(!WNDstringSlider(id)) return;
   let Width  = WNDresizeStringSliderWidth(id);
   let FirstWidth = WNDfirstLineWidth(id);
   if(FirstWidth >= Width) Width = FirstWidth;
   let Height = WNDresizeStringSliderHeight(id);
   let k = document.querySelector(".TRANSwindowEdit");
   let TransWidth = k.offsetWidth;
   let LeftPos = parseInt(Div.style.left, 10);
   if(force) LeftPos = 40;
   let LeftShift = (TransWidth - LeftPos) - Width;
   let NewLeftPos = LeftPos;
   if(LeftShift < 0) {
      NewLeftPos = LeftPos + LeftShift;
      if(NewLeftPos < 10) NewLeftPos = 10;
      Div.style.left = NewLeftPos + "px";
   }
   if(Width < 300)  Width  = 300;
   if(Width > 2000) Width  = 2000;
   if(Width > TransWidth - NewLeftPos - 15) Width = TransWidth - NewLeftPos - 15;
   if(Height < 200) Height = 200;
   if(Height > 1000) {
      COL_TRC("1000 is the maximum height");
      Height = 1000;
   }
   Div.style.width  = Width  + "px";
   Div.style.height = Height + "px";
   return true;
}
