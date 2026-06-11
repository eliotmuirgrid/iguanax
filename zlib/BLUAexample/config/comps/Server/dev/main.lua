function main(Data)
	if Data == "INIT" then
      net.http.listen{port=12321}
   else
      net.http.respond{body="hi there"}
   end
end
