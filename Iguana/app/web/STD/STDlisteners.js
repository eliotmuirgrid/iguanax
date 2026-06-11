function STDlisteners(){
   COL_TRC("STDlisteners");
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   div.addEventListener("click", function(e){ STDclickListener(e); } );
}

function STDclickListener(e){
   if(e.target.closest(".SETeditDatabase"))   return SETdatabaseScreenEdit();
   if(e.target.closest(".SETsaveDatabase"))   return SETdatabaseSave();
   if(e.target.closest(".SETcancelDatabase")) return STDdatabaseScreen();
   if(e.target.closest(".SETmysqlBrowse"))    return STDmysqlBrowse();
   if(e.target.closest(".SEToracleBrowse"))   return STDoracleBrowse();
   if(e.target.closest(".SETodbcBrowse"))     return STDodbcBrowse();
}

function STDmysqlBrowse(){
   COL_TRC("STDmysqlBrowse");
   FILbrowser({
      value : document.querySelector('.SETdatabaseMysqlEdit').value,
      new_folder : false,
      show_hidden: true,
      callback   : function(T) { document.querySelector('.SETdatabaseMysqlEdit').value = T; }
   });
}

function STDoracleBrowse(){
   FILbrowser({
      value : document.querySelector('.SETdatabaseOracleEdit').value,
      new_folder : false,
      show_hidden: true,
      callback   : function(T) { document.querySelector('.SETdatabaseOracleEdit').value = T; }
   });
}

function STDodbcBrowse(){
   FILbrowser({
      value : document.querySelector('.SETdatabaseOdbcEdit').value,
      new_folder : false,
      show_hidden: true,
      callback   : function(T) { document.querySelector('.SETdatabaseOdbcEdit').value = T; }
   });
}