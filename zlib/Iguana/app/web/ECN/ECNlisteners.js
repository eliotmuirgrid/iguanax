function ECNlisteners(){
   let div = document.querySelector(".ECNmodal");
   if(!div) return;
   div.removeEventListener("change", ECNchangeListener);
   div.addEventListener   ("change", ECNchangeListener);
   div.removeEventListener("click",  ECNclickListener);
   div.addEventListener   ("click",  ECNclickListener);
}

function ECNclickListener(e){
   if(e.target.closest(".ECNaddSourceBtn"))   return ECNaddSource();
   if(e.target.closest(".ECNaddReceiverBtn")) return ECNaddReceiver();
   if(e.target.closest(".ECNdeleteSource"))   return ECNdeleteSource(e.target);
   if(e.target.closest(".ECNdeleteReceiver")) return ECNdeleteReceiver(e.target);
   if(e.target.closest(".ECNclose"))          return DASHcloseModal();
}

function ECNchangeListener(e){
   if(e.target.closest(".ECNtargetSelect")) return ECNtargetChanged();
}

function ECNtargetChanged(){
   let div = document.querySelector(".ECNtargetSelect");
   if(!div) return;
   let Component = div?.value || "";
   PAGEsetHashParameter("component", Component);
}

function ECNaddSource(){
   let div = document.querySelector(".ECNaddSourceComp");
   if(!div) return;
   let Component = ECNcomponent();
   let SourceComponent = div?.value || "";
   if(!SourceComponent) return;
   let Data = {};
   Data.source = SourceComponent;
   Data.component = Component;
   ECNshowError("");
   APIcall("component/source/add", Data, function(R){
      if (R.success) {
         MODELforcePoll("ECNmodel");
         MODELforcePoll("DASHmodel");
         let Message = GINaddConn(ECNname(Data.source), ECNname(Data.component));
         GINaddAndCommit({message: Message, component: Component});
      } else {
         ECNshowError("Unable to add source: " + R.error);
      }
   });
}

function ECNshowError(message) {
   let Div = document.querySelector(".ECNerror");
   if (Div) Div.innerHTML = message;
}

function ECNaddReceiver(){
   let div = document.querySelector(".ECNaddReceiverComp");
   if(!div) return;
   let Component = ECNcomponent();
   let ReceiverComponent = div?.value || "";
   if(!ReceiverComponent) return;
   let Data = {};
   Data.source = Component;
   Data.component = ReceiverComponent;
   ECNshowError("");
   APIcall("component/source/add", Data, function(R){
      if (R.success) {
         MODELforcePoll("ECNmodel");
         MODELforcePoll("DASHmodel");
         let Message = GINaddConn(ECNname(Data.source), ECNname(ReceiverComponent));
         GINaddAndCommit({message: Message, component: ReceiverComponent});
      } else {
         ECNshowError("Unable to add destination: " + R.error);
      }
   });
}

function ECNdeleteSource(target){
   let Guid = ECNtargetToItem(target, ECNsourcesData());
   if(!Guid) return;
   let Component = ECNcomponent();
   let Data = {};
   Data.remove_source = Guid;
   Data.component = Component;
   ECNshowError("");
   APIcall("component/source/delete", Data, function(R){
      if(!R.success) return ECNshowError("Could not delete connection: " + R.error);
      MODELforcePoll("ECNmodel");
      MODELforcePoll("DASHmodel");
      let Message = GINremoveConn(ECNname(Data.remove_source), ECNname(Component));
      GINaddAndCommit({message: Message, component: Component});
   });
}

function ECNdeleteReceiver(target){
   let Guid = ECNtargetToItem(target, ECNreceiversData());
   if(!Guid) return;
   let Component = ECNcomponent();
   let Data = {};
   Data.remove_source = Component;
   Data.component = Guid;
   ECNshowError("");
   APIcall("component/source/delete", Data, function(R){
      if(!R.success) return ECNshowError("Could not delete connection: " + R.error);
      MODELforcePoll("ECNmodel");
      MODELforcePoll("DASHmodel");
      let Message = GINremoveConn(ECNname(Data.remove_source), ECNname(Guid));
      GINaddAndCommit({message: Message, component: Guid});
   });
}

function ECNtargetToItem(target, Data){
   if(!target) return;
   let ListData = Data;
   const Row = target.closest('.ECNrow');
   if(!Row) { return; }
   const parentElement = Row.parentNode;
   const allRows = Array.from(parentElement.querySelectorAll('.ECNrow'));
   const Index = allRows.indexOf(Row);
   if(Index >= ListData.length) { return; }
   const Item = ListData[Index];
   return Item;
}
