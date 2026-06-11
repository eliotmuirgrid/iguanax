let STGpurgeDataSet = ""
let STGpurgeColumnSortOrder = {}

function STGclearPurgeTestGlobals(){
   STGpurgeDataSet = ""
   STGpurgeColumnSortOrder = {}
}

function STGpurgeTestModelInit(){
   STGclearPurgeTestGlobals();
   STGpurgeSetupColumnSortOrders();
   MODELpollerAdd('STGpurgeTestModel', 'STGpurgeData',      10, 'log/purge_rules/test',  {});
   MODELpollerAdd('STGpurgeTestModel', 'STGpurgeOtherDirs', 10, 'log/purge_rules/test/other',  {});
   MODELpollerAdd('STGpurgeTestModel', 'STGstatsData',      10, 'log/component/usage',  {});
   MODELviewAdd  ('STGpurgeTestModel', 'STGcheckData');
   MODELviewAdd  ('STGpurgeTestModel', 'STGmakeDataset');
   MODELviewAdd  ('STGpurgeTestModel', 'STGinsertPurgeTable');
   MODELviewAdd  ('STGpurgeTestModel', 'STGupdatePurgeTableRows');
   MODELviewAdd  ('STGpurgeTestModel', 'STGupdatePurgeOtherDirs');
   MODELforcePoll('STGpurgeTestModel');
   // We intentionally do not start this model.
}

function STGpurgeSetupColumnSortOrders(){
   STGpurgeColumnSortOrder["name"]      = "dsc";
   STGpurgeColumnSortOrder["size_raw"]  = "asc";
   STGpurgeColumnSortOrder["rule"]      = "asc";
   STGpurgeColumnSortOrder["freed_raw"] = "asc";
}

function STGcheckData(){
   if (STGpurgeTestGetStatsData() == {}){
      POPpopoutClose(STGtestPurgeCard);
      return SNCKsnackbar("error", "Failed to get log disk usage");
   }
   if (STGpurgeTestGetPurgeData() == {}){
      POPpopoutClose(STGtestPurgeCard);
      return SNCKsnackbar("error", "Failed to test out purge rules");
   }
}

function STGpurgeTestGetPurgeData(){ return MODELdata.STGpurgeData?.data || {}; }
function STGpurgeTestGetStatsData(){ return MODELdata.STGstatsData?.data || {}; }
function STGpurgeTestOthersData()  { return MODELdata.STGpurgeOtherDirs?.data || []; }

function STGmakeDataset(){
   STGpurgeDataSet = STGbuildPurgeDataset(STGpurgeTestGetStatsData(), STGpurgeTestGetPurgeData());
}
