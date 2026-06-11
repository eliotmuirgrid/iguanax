function STGsortPurgeDatasetByColumn(Dataset, Column) {
   if (!STGpurgeColumnSortOrder[Column] || STGpurgeColumnSortOrder[Column] === 'asc') {
      Dataset.sort((a, b) => {
         const valueA = typeof a[Column] === 'string' ? a[Column].toLowerCase() : a[Column];
         const valueB = typeof b[Column] === 'string' ? b[Column].toLowerCase() : b[Column];
         if (valueA < valueB) return -1;
         if (valueA > valueB) return 1;
         return 0;
      });
      STGpurgeColumnSortOrder[Column] = 'desc';
   } else {
      Dataset.sort((a, b) => {
         const valueA = typeof a[Column] === 'string' ? a[Column].toLowerCase() : a[Column];
         const valueB = typeof b[Column] === 'string' ? b[Column].toLowerCase() : b[Column];
         if (valueA > valueB) return -1;
         if (valueA < valueB) return 1;
         return 0;
      });
      STGpurgeColumnSortOrder[Column] = 'asc';
   }
}

function STGpurgeTestRemoveArrow(){
   COL_TRC('STGpurgeTestRemoveArrow');
   document.querySelectorAll('.STGsortAsc').forEach(function(el){ el.classList.remove('STGsortAsc'); }   );
   document.querySelectorAll('.STGsortDsc').forEach(function(el){ el.classList.remove('STGsortDsc'); });
}

function STGpurgeTestAddSortArrow(Order, Target){
   Order == "asc" ? Target.classList.add('STGsortAsc') : Target.classList.add('STGsortDsc');
}

function STGupdatePurgeTestHeader(Column){
   STGpurgeTestRemoveArrow();
   let Order = STGpurgeColumnSortOrder[Column];
   if (Column == "name"){
      STGpurgeTestAddSortArrow(Order, document.querySelector('.STGtestPurgeHeadingComponent'));
   } else if (Column == "size_raw"){
      STGpurgeTestAddSortArrow(Order, document.querySelector('.STGtestPurgeHeadingTotalSize'));
   } else if (Column == "rule"){
      STGpurgeTestAddSortArrow(Order, document.querySelector('.STGtestPurgeHeadingRule'));
   } else if (Column == "freed_raw"){
      STGpurgeTestAddSortArrow(Order, document.querySelector('.STGtestPurgeHeadingFreed'));
   }
}

function STGpurgeSortAndUpdate(Column){
   STGsortPurgeDatasetByColumn(STGpurgeDataSet.Dataset, Column);
   STGupdatePurgeTestHeader(Column);
   STGupdatePurgeTableRows();
}
