local scope = {}

local function trace(a,b,c,d) end

local function assertNumberEqual(Expected, Actual, Context)
   if Expected ~= Actual then
      error('Mismatch: '..(Context or '')..'\n'
         ..'Expected: '..Expected..'\n'
         ..'Actual  : '..Actual..'\n'
         ..'Diff    : '..Actual-Expected..'\n'
         ,2)
   end
end

local function tsExtRangeLow(Platform)
   return not (Platform.win or Platform.rhel3 or Platform.aix
		or Platform.hpux)
end

function scope.testDiffTimeImp(Imp, Ts1, Ts2, Delta, Context)
   if not Imp then return end
   trace(Imp.name)
   if Ts1 then trace(Imp.date('%Y-%m-%d %H:%M:%S', Ts1)) end
   if Ts2 then trace(Imp.date('%Y-%m-%d %H:%M:%S', Ts2)) else trace('nil') end
   assertNumberEqual(Delta, Imp.difftime(Ts1, Ts2), Context)
end

function scope.testDiffTime(Imp)
   scope.testDiffTimeImp(Imp,
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=0},
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=5},
      -5,
      '2012-08-13 00:00:00 - 2012-08-13 00:00:05'
   )
   scope.testDiffTimeImp(Imp,
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=0},
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=0},
      0,
      '2012-08-13 00:00:00 - 2012-08-13 00:00:00'
   )
   scope.testDiffTimeImp(Imp,
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=5},
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=0},
      5,
      '2012-08-13 00:00:05 - 2012-08-13 00:00:00'
   )
   scope.testDiffTimeImp(Imp,
      Imp.time{year=2012, month=01, day=13, hour=0, min=0, sec=0},
      nil,
      (Imp.name=='gm' and 1326412800) or 1326430800,
      '2012-01-13 00:00:00 - nil'
   )
   scope.testDiffTimeImp(Imp,
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=0},
      nil,
      (Imp.name=='gm' and 1344816000) or (Imp.linear and 1344834000) or 1344830400,
      '2012-08-13 00:00:00 - nil'
   )
   scope.testDiffTimeImp(Imp,
      Imp.time{year=2012, month=08, day=13, hour=0, min=0, sec=0},
      Imp.time{year=2012, month=01, day=01, hour=0, min=0, sec=0},
      (Imp.linear and 19440000) or 19436400,
      '2012-08-13 00:00:05 - 2012-01-01 00:00:00'
   )
   -- The "gm" variety of this test is a bit odd, as it mixes Lua datetime stuff with
   -- ours. This is certain to fail on Windows.
   if (Imp.name ~= 'ts' and not Imp.platform.win)
     or tsExtRangeLow(Imp.platform) then
      scope.testDiffTimeImp(Imp,
         Imp.time{year=1959, month=08, day=13, hour=0, min=0, sec=0},
         Imp.time{year=1959, month=01, day=01, hour=0, min=0, sec=0},
         (Imp.linear and 19353600) or 19350000,
         '1959-08-13 00:00:05 - 1959-01-01 00:00:00'
      )
   end
end

local platform = {
   win = os.getenv('OS')=='Windows_NT',
   rhel3 = os.getenv('CHM_AUTOBUILD_TOKEN_RHEL3')=='rhel3',
   aix = os.getenv('CHM_AUTOBUILD_TOKEN_AIX')=='aix' or os.getenv('IFW_PF_AIX')=='aix',
   hpux = os.getenv('CHM_AUTOBUILD_TOKEN_HPUX') == 'hpux',
}

-- an "Imp" table represents one of the sets of datetime functions and other hints which may define behaivor
local ImpDf = { name='df', date=os.date, time=os.time, difftime=os.difftime, platform=platform, linear=true }
local ImpTs = { name='ts', date=os.ts.date, time=os.ts.time, difftime=os.ts.difftime, platform=platform }
local ImpGm = { name='gm', date=os.ts.gmdate, time=os.ts.gmtime, difftime=os.ts.difftime, platform=platform, linear=true }

return {
   ['testDiffTime.df']=function() scope.testDiffTime(ImpDf) end,
   ['testDiffTime.ts']=function() scope.testDiffTime(ImpTs) end,
   ['testDiffTime.gm']=function() scope.testDiffTime(ImpGm) end,
}
