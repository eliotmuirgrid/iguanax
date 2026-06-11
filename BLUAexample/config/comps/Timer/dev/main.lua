function changeTarget(Message)
   if Message == "hi" then
      TimerFireTest.change()
   end
end

function checkTarget(Message)
   if Message == "exit" then
      TimerFireTest.check()
   end
end

function main(Data)
   if Data == 'INIT' then
      component.setTimer{delay=200, data="hi", callback=changeTarget}
      component.setTimer{delay=400, data="exit", callback=checkTarget}
   end 
end
