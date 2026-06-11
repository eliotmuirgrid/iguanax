function RELrender(){
   MODELshutdownAll();
   PAGEresetPageSpecial("RELwindow");
   LOGscreenInit();
   if(!document.querySelector(".RELwindow")){
      POPpopoutOpen({
         title:'Context view', 
         content: RELrelatedHtml(),
         callback : RELclose,
         className: 'RELwindow',
         width: '85%', 
         height: '90%'
      });
   }
   RELinitModelInit();
   RELsetListeners();
   LOGparamsPast = PAGEhashParameters();
}

function RELclose(){
   let Args = RELargs();
   window.location.hash = "#logs" + PAGEformatParams(Args);
}

function RELargs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.from      = PAGEhashParameter("from");
   Args.to        = PAGEhashParameter("to");
   Args.time      = PAGEhashParameter("time");
   Args.id        = PAGEhashParameter("id");
   Args.search    = PAGEhashParameter("search");
   Args.exact     = PAGEhashParameter("exact");
   Args.tags      = PAGEhashParameter("tags");
   Args.filter    = PAGEhashParameter("filter");
   return Args;
}

function RELrelatedHtml(){
   let href=`#logs/details${PAGEformatParams(RELargs())}`
   return /*html*/`
      <div class="RELmodal">
         <div class="RELdiv2">
            <div class="RELgrey">[<a class="RELlink" href="${href}"> Go to log details </a>]</div>
            <br>
            <br>
            <div class="RELtitleDiv">
               <div class="RELtitle">UPSTREAM MESSAGES:</div>
            </div>
            <div class="RELupstreamDiv RELnoBorder"></div>
            <br>
            <br>
            <div class="RELtitle">RELATED LOGS:</div>
            <div class="RELlistDiv"></div>
      </div>
   </div>`;
}