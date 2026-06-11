
local function tostring(tbl,tab)
   if type(tbl) ~= 'table' then
      return _G.tostring(tbl)
   end
   local out = {}
   local function add(...)
      for _,x in ipairs{...} do table.insert(out, x) end
   end
   tab = tab or ''
   local ntab = tab..'\t'
   local nl = next(tbl) and '\n' or ''
   add('{', nl)
   for k,v in pairs(tbl) do
      k = type(k) == 'string' and ('%q'):format(k) or tostring(k,ntab)
      v = type(v) == 'string' and ('%q'):format(v) or tostring(v,ntab)
      add(ntab, '[', k, '] = ', v, ',', nl)
   end
   add(tab, '}')
   return table.concat(out)
end

local function trace(a,b,c,d) return end

local logfile, TestOutput
local function log(...)
   if iguana.isTest() then
      for i,v in ipairs{...} do
         TestOutput = TestOutput .. v
      end
   else
      logfile:write(...)
      logfile:flush()
   end
end

local function run(prefix, tests, failures)
   for k,v in pairs(tests) do      
      assert(not k:match('/'))
      local key = prefix..'/'..k
      if type(v) == 'table' then
         run(key, v, failures)
      else
         log('\nrunning ', key, '... ')
         local ok,err = pcall(v)
         if not err then
            log('OKAY')
         else
            log('FAILED TEST\n', tostring(err))
            failures[key] = err
         end
      end
   end
end

function main(TestNameList)
   TestOutput = ''
   --run once
   if not iguana.isTest() then
      if DBTESTS_ALREADY_RUN then
         return
      end
      DBTESTS_ALREADY_RUN = true
   else
      --return
   end
   
   if logfile then pcall(logfile.close, logfile) end
   logfile = io.open('alltests.log','w')
   iguana.stopOnError(false)
   local failures = {}
   local alltests = TestNameList or require('alltests')
   for _,suite in ipairs(alltests) do
      iguana.setTimeout(5*60)
      log('\nloading ', suite)
      local ok,err = pcall(function()
            local tests = require(suite)()
            run(suite, tests, failures)
         end)
      if not ok then
         log('FAILED LOAD\n', tostring(err))
         failures[suite] = err
      end
   end
   if next(failures) then
      local out = tostring(failures)
      iguana.logError('SOME TESTS FAILED: '..out)
   end
   log('\ntest complete\n')
   trace(TestOutput)
   return TestOutput
end

-- this is intended for editor debugging of a given module, without pcalls -
-- so as to help catch and weed out any errors with editor's assistance,
-- instead of looking in the log
-- FIXME maybe refactor and merge into the standard routine to allow editor mode
local function run_module_unsafe(Module)
   if type(Module)=='string' then
      Module = require(Module)
   end
   local TestNameFunctionList
   if type(Module)=='function' then
      TestNameFunctionList = Module()
   elseif type(Module)=='table' then
      TestNameFunctionList = Module
   else
      error('Module parameter must be a table of { testName=testFunction }, a function that returns such a table or a string with the name of the module that returns one of the former',2)
   end
   for TestName, TestFunc in pairs(TestNameFunctionList) do
      TestFunc()
   end
end

return {
   main=main,
   run=run,
   run_module_unsafe=run_module_unsafe,
}
