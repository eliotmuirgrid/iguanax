// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMimportQuerySql
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 16 February 2023 - 04:18PM
// ---------------------------------------------------------------------------
#include <DBIM/DBIMimportQuerySql.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBIMmysqlGetTableData(const COLstring& DatabaseName){
   COL_FUNCTION(DBIMmysqlGetTableData);
   return "SELECT table_name, column_name, data_type, column_key "
          "FROM information_schema.columns "
          "WHERE table_schema = '" + DatabaseName + "' "
          "ORDER BY table_name, ordinal_position;";
}

COLstring DBIMsqlServerGetTableData(const COLstring& DatabaseName){
   COL_FUNCTION(DBIMsqlServerGetTableData);
   return "SELECT COLS.TABLE_NAME, COLS.COLUMN_NAME, COLS.DATA_TYPE, KEYS.CONSTRAINT_NAME "
          "FROM INFORMATION_SCHEMA.COLUMNS AS COLS "
          "LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE AS KEYS "
               "ON COLS.TABLE_NAME = KEYS.TABLE_NAME AND COLS.COLUMN_NAME = KEYS.COLUMN_NAME "
          "WHERE COLS.TABLE_CATALOG = '" + DatabaseName + "' "
          "ORDER BY COLS.TABLE_NAME, COLS.ORDINAL_POSITION;";
}

COLstring DBIMsqliteGetTableData(){
   COL_FUNCTION(DBIMsqliteGetTableData);
   return "SELECT m.name as table_name, p.name as column_name, p.type as data_type, p.pk as column_key "
          "FROM sqlite_master AS m "
          "JOIN pragma_table_info(m.name) AS p "
          "ORDER BY m.name, p.cid;";
}

COLstring DBIMoracleGetTableData(){
   COL_FUNCTION(DBIMoracleGetTableData);
   return "SELECT COLS.TABLE_NAME, COLS.COLUMN_NAME, COLS.DATA_TYPE, CONS.CONSTRAINT_TYPE "
          "FROM USER_TAB_COLUMNS COLS "
          "LEFT JOIN USER_CONS_COLUMNS CONS_COLS "
               "ON COLS.TABLE_NAME = CONS_COLS.TABLE_NAME "
               "AND COLS.COLUMN_NAME = CONS_COLS.COLUMN_NAME "
          "LEFT JOIN USER_CONSTRAINTS CONS "
               "ON CONS_COLS.CONSTRAINT_NAME = CONS.CONSTRAINT_NAME "
          "ORDER BY COLS.TABLE_NAME, COLS.COLUMN_ID;";
}

COLstring DBIMpostgresGetTableData(){
   COL_FUNCTION(DBIMpostgresGetTableData);
   return "SELECT COLS.TABLE_NAME, COLS.COLUMN_NAME, COLS.DATA_TYPE, CASE WHEN PK.CONSTRAINT_NAME IS NOT NULL THEN 'Yes' ELSE 'No' END AS column_key "
          "FROM INFORMATION_SCHEMA.COLUMNS COLS "
          "LEFT JOIN (SELECT TC.TABLE_NAME, KCU.COLUMN_NAME, TC.CONSTRAINT_NAME "
               "FROM INFORMATION_SCHEMA.TABLE_CONSTRAINTS TC JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE KCU ON TC.CONSTRAINT_NAME = KCU.CONSTRAINT_NAME "
               "AND TC.TABLE_SCHEMA = KCU.TABLE_SCHEMA "
               "AND TC.CONSTRAINT_TYPE = 'PRIMARY KEY' "
           ") PK ON COLS.TABLE_NAME = PK.TABLE_NAME AND COLS.COLUMN_NAME = PK.COLUMN_NAME "
           "WHERE COLS.TABLE_SCHEMA = 'public' "
           "ORDER BY COLS.TABLE_NAME, COLS.ORDINAL_POSITION;";
}
