function Callback(UserCall)
   UserCall('Ring ring')
end

function MyCall(Line)
   print("Called with:", Line)
end

function HelloWorld(Line)
   print(Line)
   Callback(MyCall)
end

function OnError(Log)
   print(Log.Body)
end

