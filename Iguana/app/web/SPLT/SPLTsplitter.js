function SPLTwidth(ContainerWidth, NewWidth, Minimum) {
   const Maximum  = ContainerWidth - 100;
   if(NewWidth > Maximum) NewWidth = Maximum;
   if(NewWidth < Minimum) NewWidth = Minimum;
   return NewWidth;
}

function SPLTsetupSplitter(Container, ResizeControl, LeftPane, LeftRightRatio, MinListWidth = 0,
   Collapsed = function () { return false }, UpdateDockDimensions = function () {}) {
   const Handle = document.querySelector(ResizeControl);
   if (!Handle) { throw "NEED TO SET UP PROJECT PANEL BEFORE CALLING THIS FUNCTION"; }
   const Left    = document.querySelector(LeftPane);
   const Minimum = Math.max(220, MinListWidth);
   if (STORElocalRead(LeftRightRatio)) { 
      const Width = STORElocalRead(LeftRightRatio);
      Left.style.width = SPLTwidth(document.querySelector(Container).offsetWidth, Width, Minimum) + 'px';
   }
   let Width = Left.offsetWidth;
   let Dragging = false;
   const ContainerElement = document.querySelector(Container);
   
   Handle.addEventListener('mousedown', function (e) {
      if (e.which != 1) return;
      if (Collapsed()) return;
      e.preventDefault();
      Dragging = true;
      let MinWidth   = Minimum;//Math.max(ContainerElement.offsetWidth * 0.15, Minimum);
      const MaxWidth = Math.min(ContainerElement.offsetWidth * 0.95, ContainerElement.offsetWidth - 100);
      const Offset = Left.offsetWidth - e.clientX;
      LeftPane === '.HLPindexContainer' ? MinWidth = 0 : -1;
      const onMouseMove = function(e) { SPLTmouseMoveHandler(e, Offset, MinWidth, MaxWidth); };
      document.addEventListener('mousemove', onMouseMove);
      document.addEventListener('mouseup', function onMouseUp(e) {
         if (Dragging) {
            e.preventDefault();
            document.removeEventListener('mousemove', onMouseMove);
            document.removeEventListener('mouseup', onMouseUp);
            STORElocalCreate(LeftRightRatio, Left.offsetWidth);
            Dragging = false;
            CYTOresize();
         }
      });
   });

   function SPLTmouseMoveHandler(e, Offset, MinWidth, MaxWidth) {
      e.preventDefault();
      Width = e.clientX + Offset;
      if (Width < MinWidth) { Width = MinWidth; }
      if (Width > MaxWidth) { Width = MaxWidth; }
      Left.style.width = Width + 'px';
      UpdateDockDimensions();
   }
}