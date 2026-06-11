require('common.testutils')

local TEST_HOST = os.getenv('IFW_TEST_FTP_PLAIN_HOST') or '192.168.0.47'
local TEST_USER = os.getenv('IFW_TEST_FTP_PLAIN_USER') or 'ftp-test'
local TEST_PASS = os.getenv('IFW_TEST_FTP_PLAIN_PASS') or 'pdbRm2v5Kw8p'

local function testErrorHost()
   local function attempt()
      local Conn = net.ftp.init{
         server="nohost.ifware.net",
         username=TEST_USER,
         password="FIXME",
         live=true,
      }
      Conn:list{remote_path='.'}
   end
   testutils.AssertError( attempt, nil, "Unable to connect to server", true)
end

local function testErrorPort()
   local function attempt()
      local Conn = net.ftp.init{
         server=TEST_HOST,
         username=TEST_USER,
         password="FIXME",
		 port=991,
         live=true,
      }
      Conn:list{remote_path='.'}
   end
   testutils.AssertError( attempt, nil, "Unable to connect to server", true)
end

local function testErrorPassword()
   local function attempt()
      local Conn = net.ftp.init{
         server=TEST_HOST,
         username=TEST_USER,
         password="FIXME",
         live=true,
      }
      Conn:list{remote_path='.'}
   end
   testutils.AssertError( attempt, nil, "Invalid username and/or password", true)
end


local function testOk()
  local Conn = net.ftp.init{
      server=TEST_HOST,
      username=TEST_USER,
      password=TEST_PASS,
      live=true,
  }
  local FileList = Conn:list{remote_path="."}
  assert(#FileList==2) --expecting the hello.txt file, as well as a directory called files
  local Body = Conn:get{remote_path='hello.txt'}
  assert(Body=='hello\n')
end


return {
	['testErrorHost']=testErrorHost,
	['testErrorPort']=testErrorPort,
	['testErrorPassword']=testErrorPassword,
	['testOk']=testOk,
}
