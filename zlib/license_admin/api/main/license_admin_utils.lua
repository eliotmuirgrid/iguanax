do
   local dbconn
   local CRMparams = admin_config.CRMparams
   initDb = function()
      if not dbconn or not dbconn:check() then
         dbconn = db.connect(CRMparams)
      end
      if not dbconn:check() then
         throwError(licensing_error.InternalError, 'Operation could not be completed due to a database error.')
      end
      return dbconn
   end
end

--wrap database calls with pcall, rethrow error but sanitized, and log any errors
getmetatable(db)._submeta.db_connection.__index.pQuery = function(conn, ...)  
   local success, result = pcall(conn.query, conn,...)
   
   if not success then
      local param = ...
      if type(param) == 'table' then
         param = json.serialize{data = param}
      end            
      logIssue('Error', 'Issue: ' .. tostring(result) .. '\n\nIn query: \n' .. param)
      
      throwError(licensing_error.InternalError, 'Operation could not be completed due to a database error.')
   else 
      return result      
   end
end

getmetatable(db)._submeta.db_connection.__index.pTransaction = function(conn, method)
   conn:begin()
   local success, result = pcall(method)
   if not success then
      conn:rollback()
      
      --Re-throw "licensing errors". More proper way would be to set and check a metatable.
      if type(result) == 'table' and result.code then
         error(result)
      else --Assume it's an internal error.
         logIssue('Error', 'Issue in pTransaction: ' .. tostring(result))     
         throwError(licensing_error.InternalError, 'Operation could not be completed due to a database error.')   
      end
   else
      conn:commit()
      return result      
   end   
end


function checkRequiredParams(params, reqparams)
   for i=1,#reqparams do
      if not params[reqparams[i]] then      
         throwError(licensing_error.InvalidParams, 'Missing required parameter: ' .. reqparams[i] .. '.')   
      end
      if #params[reqparams[i]] == 0 then
         throwError(licensing_error.InvalidParams, 'Required parameter cannot be empty: ' .. reqparams[i] .. '.')   
      end
   end   
end

function checkParamNonempty(name, value)
   if value and #value == 0 then
      throwError(licensing_error.InvalidParams, 'Parameter cannot be empty: ' .. name .. '.')   
   end
end

function resultSetToTable(res)    
   local t = {}   
   for i = 1,#res do
      t[i] = {}
      for j = 1,#res[i] do
         t[i][res[i][j]:nodeName()] = res[i][j]:nodeValue()
      end
   end
   
   return t
end

function SQLinsertFromTable(TableName, ValueList)
   local conn = initDb()
   
   local names, vals = '',''
   
   for k,v in pairs(ValueList) do
      if names ~= '' then
         names = names .. ','
      end
      if vals ~= '' then
         vals = vals .. ','
      end
      
      names = names .. k
      if type(v) == 'string' then
         vals = vals .. conn:quote(v)
      else
         vals = vals .. v
      end
   end
   
   return 'INSERT INTO ' .. TableName .. ' (' .. names .. ') VALUES (' .. vals .. ')'   
end

local function SQLkeyConditions(KeyValues)
   local conn = initDb()
   
   local sql = ' WHERE '
   local first = true
   for k,v in pairs(KeyValues) do
      if not first then
         sql = sql .. ' AND '         
      else 
         first = false
      end
      
      if type(v) == 'string' then
         sql = sql .. k .. ' = ' .. conn:quote(v)
      else
         sql = sql .. k .. ' = ' .. v         
      end
   end   
   return sql
end

function SQLupdateFromTable(TableName, ValueList, KeyConditions)   
   local conn = initDb()
   
   local sql = 'UPDATE ' .. TableName .. ' SET'
   local first = true
   
   for k,v in pairs(ValueList) do
      if not first then
         sql = sql .. ','
      else
         first = false
      end      
      
      if type(v) == 'string' then
         sql = sql .. ' ' .. k .. ' = ' .. conn:quote(v)
      else
         sql = sql .. ' ' .. k .. ' = ' .. v         
      end
   end
   
   if KeyConditions then
      sql = sql .. SQLkeyConditions(KeyConditions)
   end
   
   return sql
end

function logIssue(IssueType, Description, Data)   
   local Text = Description .. '\n\n' .. debug.traceback()   
      
   if Data then
      Text = Text .. '\n\nData ' .. json.serialize{data = Data}
   end
   
   local Method = iguana['log' .. IssueType] or iguana.logError      
   Method(Text)   
end

function hasPermission(Session, PermissionName)   
   local conn = initDb()
   
   local restable = conn:pQuery(
      [[SELECT * FROM UserPermissions P
      JOIN PermissionTypes T
      ON P.PermissionID = T.PermissionID
      WHERE P.UserID = ]]
      .. Session.data.UserID ..
      [[ AND T.Name = ]]
      .. conn:quote(PermissionName)
   )
   
   return #restable > 0
end









