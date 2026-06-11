function DOCloadNewestArgs(){
   return{
      component : PAGEhashParameter("component"),
      seetags   : PAGEhashParameter("seetags"),
      search    : PAGEhashParameter("search"),
      tag       : PAGEhashParameter("tag"),
      id        : MODELdata.DOCscrollbarNewest.data[0].id,
      created   : MODELdata.DOCscrollbarNewest.data[0].created
   };
}

function DOCloadOldestArgs(){
   return{
      component : PAGEhashParameter("component"),
      seetags   : PAGEhashParameter("seetags"),
      search    : PAGEhashParameter("search"),
      tag       : PAGEhashParameter("tag"),
      id        : MODELdata.DOCscrollbarOldest.data[0].id,
      created   : MODELdata.DOCscrollbarOldest.data[0].created
   };
}

function DOCscrollbarNewestArgs(){
   return{
      component      : PAGEhashParameter("component"),
      start          : "0",
      limit          : 1,
      search_forward : true,
      search         : PAGEhashParameter("search")
   };
}

function DOCscrollbarOldestArgs(){
   return{
      component      : PAGEhashParameter("component"),
      start          : "ffffffffffffffffff",
      limit          : 1,
      search_forward : false,
      search         : PAGEhashParameter("search")
   };
}

function DOCrenderMoreDownArgs(Item){
   return{
      component      : PAGEhashParameter("component"),
      search_forward : true,
      limit          : 50,
      start          : Item.created,
      id             : Item.id,
      search         : PAGEhashParameter("search")
   };
}

function DOCrenderMoreUpArgs(Item){
   return{
      component      : PAGEhashParameter("component"),
      search_forward : false,
      limit          : 50,
      start          : Item.created || "000000",
      //id             : Item.id || "-1",
      id             : Item.id,
      search         : PAGEhashParameter("search")
   };
}

function DOCrenderNewestListArgs(){
   return{
      component      : PAGEhashParameter("component"),
      search_forward : true,
      limit          : 50,
      start          : PAGEhashParameter("created") || "000000",
      id             : PAGEhashParameter("id"),
      search         : PAGEhashParameter("search")
   };
}

function DOCinitRenderDownArgs(Created, Id){
   return{
      component      : PAGEhashParameter("component"),
      search_forward : true,
      limit          : 50,
      start          : Created,
      id             :Id,
      search         : PAGEhashParameter("search")
   };
}

function DOCinitRenderUpArgs(Created, Id){
   return{
      component      : PAGEhashParameter("component"),
      search_forward : false,
      limit          : 50,
      start          : Created || "000000",
      id             :Id,
      search         : PAGEhashParameter("search")
   };
}
