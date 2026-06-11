/**
 * Copyright (c) 2010-2022 iNTERFACEWARE Inc.  All rights reserved.
 * Model for dashboard
 */
 function DASHmodelInit(){
   MODELpollerAdd('DASHmodel', 'DASHcolumnData',        2,  'instance/columns', {});
   MODELpollerAdd('DASHmodel', 'DASHcomponentListData', 2,  'component/list',     DASHlistDataArgs);
   MODELpollerAdd('DASHmodel', 'DASHrunningComponents', 2,  'component/run_list', {});
   MODELpollerAdd('DASHmodel', 'DASHcomponentMapData',  2,  'component/map',      DASHmapDataArgs);
   MODELpollerAdd('DASHmodel', 'DASHuserPermission',    19, 'component/permission', {});
   MODELpollerAdd('DASHmodel', 'DASHuserPermissionAll', 10, 'component/permission/all', {});
   MODELpollerAdd('DASHmodel', 'DASHtagData',           10, 'filter_tags/load', {});
   MODELpollerAdd('DASHmodel', 'DASHpinData',           5,  'pins/load', {});
   MODELviewAdd('DASHmodel', 'DASHsortListData');
   MODELviewAdd('DASHmodel', 'DASHcolumns');
   MODELviewAdd('DASHmodel', 'DASHupdateArrows');
   MODELviewAdd('DASHmodel', 'DASHrenderList');
   MODELviewAdd('DASHmodel', 'DASHcreateButtonUpdate');
   MODELviewAdd('DASHmodel', 'DASHrenderListError');
   MODELviewAdd('DASHmodel', 'DASHupdateNameWidth');
   MODELviewAdd('DASHmodel', 'DASHupdateCardArrows');

   MODELviewAdd('DASHmodel', 'CYTOrenderInit');
   MODELviewAdd('DASHmodel', 'CYTOtoolbarRender');
   MODELviewAdd('DASHmodel', 'CYTOtagsUpdateSelected');

   MODELforcePoll('DASHmodel', function(){
      DASHcheckMapErrors(); // TODO - move to a view
      DASHupdate();
   });
   MODELstart('DASHmodel');
}

function DASHuserPermissionAll(){
   return MODELdata?.DASHuserPermissionAll?.data?.components || {};
}

function DASHpermissionData(){
   return MODELdata?.DASHuserPermission?.data?.create_component || false;
}

function DASHtriggerRefresh(){
   if(MODELdata.DASHgitCacheRefresh === undefined) MODELforcePoll('DASHtemplatesModel');
}

function DASHlistDataArgs(){
   let Args = {};
   const checkedOnly = DASHshowOnlyCheckedRows();
   const checked = DASHlistOfCheckedComponents();
   Args.filter       = PAGEhashParameter("filter") ? PAGEhashParameter("filter") + ' '       : undefined;   // + ' ' is important for some reason
   Args.order        = PAGEhashParameter("sort");
   Args.checked      = checkedOnly ? checked : undefined;
   Args.checked_only = checkedOnly;
   Args.whole_word   = PAGEhashParameter("whole_word");
   Args.ascending    = PAGEhashParameter("order") == "Dsc" ? 0 : 1;
   return Args;
}

function DASHmapDataArgs(){
   let Args = {};
   const checkedOnly = DASHshowOnlyCheckedRows();
   const checked = DASHlistOfCheckedComponents();
   Args.filter       = PAGEhashParameter("filter") ? PAGEhashParameter("filter") + ' ' : undefined;
   Args.checked      = checkedOnly ? checked : undefined;
   Args.whole_word   = PAGEhashParameter("whole_word");
   Args.checked_only = checkedOnly;
   return Args;
}

function DASHlistData(){
   return MODELdata?.DASHcomponentListData?.data.component_data || [];
}

function DASHmapData(){
   return MODELdata?.DASHcomponentMapData?.data || {};
}

function DASHrunningComponentsData(){
   return MODELdata?.DASHrunningComponents?.data;
}

function DASHcomponentCount(){
   return MODELdata?.DASHcomponentListData?.data?.total_components;
}

function DASHtagData(){
   return MODELdata?.DASHtagData?.data || [];
}

function DASHpinData(){
   return MODELdata?.DASHpinData?.data || {};
}

function DASHcolumnData(){
   return MODELdata?.DASHcolumnData?.data || [];
}
