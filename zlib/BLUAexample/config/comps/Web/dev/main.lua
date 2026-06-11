function main()
	local R1 = net.http.get{url="https://www.google.com"}
	local R2 = net.http.get{url="http://localhost:12321"}
	assert(R2 == "hi there")
end
