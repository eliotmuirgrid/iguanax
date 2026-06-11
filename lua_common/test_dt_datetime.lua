local scope = {}

local function trace(a,b,c,d,e,f) end

local function assertTrueMessage(Expected, Message)
   if not Expected then
      error('Assertion failed: '..(Message or ''), 2)
   end
end

local function assertNumberEqual(Expected, Actual)
   if Expected ~= Actual then
      error('Expected: '..Expected..' Actual: '..Actual..' Diff: '..Actual-Expected, 2)
   end
end

local function assertStringEqual(Expected, Actual, Context)
   if Expected ~= Actual then
      error('Mismatch: '..(Context or '')..'\n'
         ..'Expected: '..tostring(Expected)..'\n'
         ..'Actual  : '..tostring(Actual)..'\n'
         ,2)
   end
end

local TM_NOW = os.ts.date('*t')

function scope.testDateCurrent(Imp)
   -- better ideas for expecting stuff of current datetime welcome
   trace(Imp.name)
   assertStringEqual('string', type(Imp.date()))
   assertStringEqual('string', type(Imp.date(nil)))
   assertStringEqual('string', type(Imp.date(nil, nil)))
   assertStringEqual('string', type(Imp.date('%Y-%m-%d %H:%M:%S')))
   assertStringEqual('table', type(Imp.date('*t')))
   assertStringEqual('table', type(Imp.date('!*t')))
   local CurrentTm = Imp.date('*t')
   local ImpTs = Imp.time(CurrentTm)
   local ImpTm = Imp.date('*t', ImpTs)
   assertNumberEqual(CurrentTm.year, ImpTm.year)
   assertNumberEqual(CurrentTm.month, ImpTm.month)   
   assertNumberEqual(CurrentTm.day, ImpTm.day)
   assertNumberEqual(CurrentTm.hour, ImpTm.hour)
   assertNumberEqual(CurrentTm.min, ImpTm.min)
   assertNumberEqual(CurrentTm.sec, ImpTm.sec)
   assertNumberEqual(CurrentTm.isdst, ImpTm.isdst)
end

function scope.testTimeCurrent(Imp)
   trace(Imp.name)
   Imp.time()
   Imp.time(nil)
   local CurrentTs = Imp.time()
   local ImpTm = Imp.date('*t', CurrentTs)
   local ImpTs = Imp.time(ImpTm)
   assertTrueMessage(CurrentTs == ImpTs, "ts->tm->ts roundtrip fails")
end

function scope.testDateSpecificImp(Imp, Tm, Str, ExpectedTs)
   trace(Imp.name)
   local Ts = Imp.time(Tm)
   assertTrueMessage(Ts, 'For '..Str..' Ts is nil = invalid')
   assertStringEqual(Str, Imp.date('%Y-%m-%d %H:%M:%S', Ts), Str)
   if ExpectedTs then assertStringEqual(ExpectedTs, Ts, Str) end
 end

local function tsExtRangeLow(Platform)
   return not (Platform.win or Platform.rhel3 or Platform.aix)
end

function scope.testDateSpecific(Imp)
   trace(Imp.name)
   scope.testDateSpecificImp(Imp, { year=2012, month=09, day=30, hour=13, min=14, sec=15 }, '2012-09-30 13:14:15')
   scope.testDateSpecificImp(Imp, { year=2012, month=10, day=01, hour=13, min=14, sec=15 }, '2012-10-01 13:14:15')
   scope.testDateSpecificImp(Imp, { year=2012, month=10, day=02, hour=13, min=14, sec=15 }, '2012-10-02 13:14:15')
   scope.testDateSpecificImp(Imp, { year=2012, month=11, day=04, hour=1, min=30, sec=0 }, '2012-11-04 01:30:00') --ambig
   scope.testDateSpecificImp(Imp, { year=2012, month=11, day=07, hour=13, min=14, sec=15 }, '2012-11-07 13:14:15')
   if Imp.name == 'gm' or Imp.name == 'df' or (Imp.name=='ts' and tsExtRangeLow(Imp.platform)) then
      -- low (negative) timestamp values are accepted  by standard imp only on some platforms
      scope.testDateSpecificImp(Imp, {year=1959, month=08, day=13, hour=12, min=13, sec=14}, '1959-08-13 12:13:14')
      scope.testDateSpecificImp(Imp, {year=1935, month=09, day=30, hour=16, min=17, sec=18}, '1935-09-30 16:17:18')
      scope.testDateSpecificImp(Imp, {year=1902, month=03, day=05, hour=09, min=31, sec=29}, '1902-03-05 09:31:29')
   end
   if Imp.name == 'gm' or Imp.name == 'df' then
      -- high (over 32 bit) timestamp values are not known to be accepted by standard imp
      scope.testDateSpecificImp(Imp, { year=2038, month=01, day=19, hour=13, min=14, sec=15 }, '2038-01-19 13:14:15')
   end
   if Imp.name == 'gm' then
      scope.testDateSpecificImp(Imp, { year=1738, month=01, day=19, hour=13, min=14, sec=15 }, '1738-01-19 13:14:15', -7319587545)
      scope.testDateSpecificImp(Imp, { year=1838, month=01, day=19, hour=13, min=14, sec=15 }, '1838-01-19 13:14:15', -4163913945)
      scope.testDateSpecificImp(Imp, { year=1938, month=01, day=19, hour=13, min=14, sec=15 }, '1938-01-19 13:14:15', -1008240345)
      scope.testDateSpecificImp(Imp, { year=2038, month=01, day=19, hour=13, min=14, sec=15 }, '2038-01-19 13:14:15', 2147519655)
      scope.testDateSpecificImp(Imp, { year=2138, month=01, day=19, hour=13, min=14, sec=15 }, '2138-01-19 13:14:15', 5303193255)
      scope.testDateSpecificImp(Imp, { year=2238, month=01, day=19, hour=13, min=14, sec=15 }, '2238-01-19 13:14:15', 8458866855)
   end
end

local function maxDay(Year, Month)
   local NextYear = Year
   local NextMonth = Month+1
   if NextMonth > 12 then
      NextYear = NextYear + 1
      NextMonth = 1
   end
   local TsDiff = os.ts.gmtime{year=NextYear, month=NextMonth, day=1} - os.ts.gmtime{year=Year, month=Month, day=1}
   return TsDiff/(3600*24)
end

local function dateStr(Tm)
   local Res = Tm.year..'-'
   if Tm.month < 10 then
      Res = Res..'0'
   end
   Res = Res..Tm.month..'-'
   if Tm.day < 10 then
      Res = Res..'0'
   end
   Res = Res..Tm.day
   return Res
end

function scope.testDateEvery(Imp)
   --if iguana.isTest() then return end
   trace(Imp.name)
   local StartYear, EndYear = 1902, 2098
   if Imp.name == 'ts' then
      EndYear = 2036
      if not tsExtRangeLow(Imp.platform) then
         StartYear = 1971
      end
   end
   trace(StartYear, EndYear)
   for Year = StartYear, EndYear do
      for Month = 1, 12 do
         local MaxDay = maxDay(Year,Month)
         trace(MaxDay)
         for Day = 1, MaxDay do
            trace(Year, Month, Day)
            local Tm = { year=Year, month=Month, day=Day, hour=13, min=14, sec=15 }
            local Ts = Imp.time(Tm)
            scope.testDateSpecificImp(Imp, Tm, dateStr(Tm)..' 13:14:15')
         end
      end
   end
end

function scope.testOffsetImp(Imp, Tm, ExpectedDst, ExpectedOffset)
   local Str = os.ts.gmdate('%Y-%m-%d %H:%M:%S', os.ts.gmtime(Tm))
   trace(Imp.name, Str, Imp.date==os.ts.date, Imp.date==os.ts.gmdate, Imp.date==os.date, Imp.date)
   local ImpTs = Imp.time(Tm)
   local ImpTm = Imp.date('*t', ImpTs)
   trace(ImpTm)
   if Imp.name == 'ts' then -- os.ts.date() is the only imp that provides isdst
      trace('DST=', ImpTm.isdst)
      local Offset = os.ts.gmtime(Tm)-ImpTs
      trace('Offset=', Offset)
      assertStringEqual(ExpectedDst, ImpTm.isdst, Str)
      if ExpectedOffset then
         assertStringEqual(ExpectedOffset, Offset, Str)
      end
   else
      assertTrueMessage(ImpTm.isdst == nil, "isdst should be nil: "..Imp.date('%Y-%m-%d %H:%M:%S', ImpTs))
   end
end

function scope.testOffset(Imp)
   scope.testOffsetImp(Imp, {year=2012, month=11, day=3, hour=0}, true, -14400)
   scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=0, min=30}, true, -14400)

   -- DST cross-over is on a different edge for some platforms.
   if Imp.platform.hpux or Imp.platform.win then
   	scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=0, min=59, sec=59}, true, -14400)
   else
	scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=1, min=0}, true, -14400)
   end

   scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=1, min=0, isdst=true}, true, -14400)

   -- DST ends at 1AM on this day for some platforms
   local tm = {year=2012, month=11, day=4, hour=1, min=30}
   if Imp.platform.hpux or Imp.platform.win or (Imp.platform.aix and not TM_NOW.isdst) then
      -- hypothesis - on AIX, this boundary case can be treated as either DST or not depending on current DST
   	scope.testOffsetImp(Imp, tm, false, -18000)
   else
   	scope.testOffsetImp(Imp, tm, true, -14400)
   end

   scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=1, min=30, isdst=true}, true, -14400)
   scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=1, min=0, isdst=false}, false, -18000)
   scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=1, min=30, isdst=false}, false, -18000)
   scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=2, min=0}, false, -18000)
   scope.testOffsetImp(Imp, {year=2012, month=11, day=4, hour=2, min=30}, false, -18000)
   scope.testOffsetImp(Imp, {year=2012, month=11, day=8, hour=0}, false, -18000)
end

local function addDaysTm(Tm, Days)
   local LocalTs = os.ts.gmtime(Tm)
   assert(LocalTs)
   local NewTm = os.ts.gmdate('*t', LocalTs+3600*24*Days)
   assert(NewTm)
   return NewTm
end

local function addDaysOsTs(Ts, Days)
   local Tm = os.date('*t', Ts)
   local NewTm = addDaysTm(Tm, Days)
   return os.time(NewTm)
end

local function addDaysImpTs(Imp, Ts, Days)
   if Imp.name == 'df' then
      return addDaysOsTs(Ts, Days)
   end
   local Tm = Imp.date('*t', Ts)
   local NewTm = addDaysTm(Tm, Days)
   return Imp.time(NewTm)
end

function scope.testAddDaysImp(Imp, Tm, Days, ExpectedStr)
   local Ts = Imp.time(Tm)
   local Str = Imp.date('%Y-%m-%d', Ts)
   trace(Imp.name, Str, Days, ExpectedStr)

   local NewTs = addDaysImpTs(Imp, Ts, Days)
   assertStringEqual(ExpectedStr, Imp.date('%Y-%m-%d', NewTs))
   assertStringEqual(Imp.date('%H:%M:%S', Ts), Imp.date('%H:%M:%S', NewTs))
   
   local NewTm = addDaysTm(Tm, Days)
   assertStringEqual(ExpectedStr, dateStr(NewTm))
   assertNumberEqual(Tm.hour, NewTm.hour)
   assertNumberEqual(Tm.min, NewTm.min)
   assertNumberEqual(Tm.sec, NewTm.sec)
end

function scope.testAddDays(Imp)
   scope.testAddDaysImp(Imp, { year=2012, month=11, day=13, hour=13, min=14, sec=15}, -14, '2012-10-30')
   scope.testAddDaysImp(Imp, { year=2012, month=11, day=13, hour=13, min=14, sec=15}, -4, '2012-11-09')
   scope.testAddDaysImp(Imp, { year=2012, month=11, day=13, hour=13, min=14, sec=15}, 0, '2012-11-13')
   scope.testAddDaysImp(Imp, { year=2012, month=11, day=13, hour=13, min=14, sec=15}, 4, '2012-11-17')
   scope.testAddDaysImp(Imp, { year=2012, month=11, day=13, hour=13, min=14, sec=15}, 24, '2012-12-07')
end

local platform = {
   win = os.getenv('OS')=='Windows_NT',
   rhel3 = os.getenv('CHM_AUTOBUILD_TOKEN_RHEL3')=='rhel3',
   aix = os.getenv('CHM_AUTOBUILD_TOKEN_AIX')=='aix' or os.getenv('IFW_PF_AIX')=='aix',
   hpux = os.getenv('CHM_AUTOBUILD_TOKEN_HPUX')=='hpux',
}

-- an "Imp" table represents one of the sets of datetime functions and other hints which may define behaivor
local ImpDf = { name='df', date=os.date, time=os.time, difftime=os.difftime, platform=platform, linear=true }
local ImpTs = { name='ts', date=os.ts.date, time=os.ts.time, difftime=os.ts.difftime, platform=platform }
local ImpGm = { name='gm', date=os.ts.gmdate, time=os.ts.gmtime, difftime=os.ts.difftime, platform=platform, linear=true }

return {
   ['testDateCurrent.df']=function() scope.testDateCurrent(ImpDf) end,
   ['testDateCurrent.ts']=function() scope.testDateCurrent(ImpTs) end,
   ['testDateCurrent.gm']=function() scope.testDateCurrent(ImpGm) end,
   ['testDateSpecific.df']=function() scope.testDateSpecific(ImpDf) end,
   ['testDateSpecific.ts']=function() scope.testDateSpecific(ImpTs) end,
   ['testDateSpecific.gm']=function() scope.testDateSpecific(ImpGm) end,
   ['testDateEvery.df']=function() scope.testDateEvery(ImpDf) end,
   ['testDateEvery.ts']=function() scope.testDateEvery(ImpTs) end,
   ['testDateEvery.gm']=function() scope.testDateEvery(ImpGm) end,
   ['testTimeCurrent.df']=function() scope.testTimeCurrent(ImpDf) end,
   ['testTimeCurrent.ts']=function() scope.testTimeCurrent(ImpTs) end,
   ['testTimeCurrent.gm']=function() scope.testTimeCurrent(ImpGm) end,
   ['testOffset.df']=function() scope.testOffset(ImpDf) end,
   ['testOffset.ts']=function() scope.testOffset(ImpTs) end,
   ['testOffset.gm']=function() scope.testOffset(ImpGm) end,
   ['testAddDays.df']=function() scope.testAddDays(ImpDf) end,
   ['testAddDays.ts']=function() scope.testAddDays(ImpTs) end,
   ['testAddDays.gm']=function() scope.testAddDays(ImpGm) end,
   
}
