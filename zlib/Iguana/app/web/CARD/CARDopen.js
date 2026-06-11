
function CARDscreen(){
   CARDdisableOpenEdit();
   PAGEresetPageSpecial("CARDdetailedCardId");
   DASHdashboardScreen(false);
   if(!CARDcomponentId()) {
      let Url = "#dashboard";
      let Filter = PAGEhashParameter("filter");
      if(Filter) Url = Url + "?filter=" + Filter;
      return window.location.hash = Url;
   }
   DASHbridgeNavigateEmit(CARDcomponentId());
   CARDmodelInit();
}
