function PRJdrawerHtml() {
   let Hide = TRANScomponentHide();
   return /*html*/`
   <div class="TRANSswitchDiv">
      ${PRJprojectNameAndSwitchHtml()}
      <div class="PRJswitchDiv${Hide}">
         ${PRJswitchHtml()}     
         <div class="PRJstatusText"></div>
      </div>
   </div>
   `;
}

function PRJupdateStatusText(){
  const el = document.querySelector('.PRJstatusText');
  if(!el) return;

  const data = TRANScomponentData();
  if(!data) return;

  const on = PRJcomponentRunning();
  const light = (data.light || "").toUpperCase();
  const status = TOGswitchDescription(light, on);
  if(el.textContent !== status) el.textContent = status;

  const b="PRJstatusText",
        mods=[`${b}--idle`,`${b}--running`,`${b}--changing`,`${b}--error`],
        desired = light==="RED"    ? `${b}--error`
                : light==="YELLOW" ? `${b}--changing`
                : on               ? `${b}--running`
                :                    `${b}--idle`,
        current = mods.find(m=>el.classList.contains(m));

  if(current !== desired){
    if(current) el.classList.remove(current);
    el.classList.add(desired);
  }
}



function PRJprojectNameAndSwitchHtml(){
   return /*html*/`
   <div class='PRJprojectNameAndSwitch'>
      <select class="PRJcomponentSelect FORMselectBox" title="Switch components"></select>
      <a class="PRJcomponentLink BUTTONdefault BUTTONstandard" title="Go to dashboard"></a>
   </div>
   `;
}
