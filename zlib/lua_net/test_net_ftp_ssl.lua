require('common.testutils')

local TEST_HOST = os.getenv('IFW_TEST_FTP_SSL_HOST') or 'ftp-test.interfaceware.com'
local TEST_USER = os.getenv('IFW_TEST_FTP_SSL_USER') or 'ftp-test'
local TEST_PASS = os.getenv('IFW_TEST_FTP_SSL_PASS') or 'pdbRm2v5Kw8p'
local TEST_CERT = os.getenv('IFW_TEST_FTP_SSL_CERT') or '../lua_net/cert.ftp-test.pem'

local function testErrorHost()
   local function attempt()
      local Conn = net.ftps.init{
         server="nohost.ifware.net",
         username=TEST_USER,
         password="FIXME",
         port=990,
         ssl_version="tls-v1",
         verify_peer=false,
         verify_host=false,
         live=true,
      }
      Conn:list{remote_path='/files/'}
   end
   testutils.AssertError( attempt, nil, "Unable to connect to server", true)
end

local function testErrorPort()
   -- Expect this to take as long as the timeout value
   -- This test may also timeout instead of returning the correct error
   local function attempt()
      local Conn = net.ftps.init{
         server=TEST_HOST,
         username=TEST_USER,
         password=TEST_PASS,
         port=995,
         timeout=30,
         ssl_version="tls-v1",
         verify_peer=false,
         verify_host=false,
         live=true,
      }
      Conn:list{remote_path='/files/'}
   end
   testutils.AssertError( attempt, nil, "Unable to connect to server", true)
end

local function testErrorNoSsl()
   local function attempt()
      local Conn = net.ftps.init{
         server=TEST_HOST,
         username=TEST_USER,
         password=TEST_PASS,
         port=990,
         live=true,
      }
      Conn:list{remote_path='/files/'}
   end
   testutils.AssertError( attempt, nil, "Could not get directory listing", true)
   -- testutils.AssertError( attempt, nil, "must use encryption", true)
end

local function testErrorVerify()
   local function attempt()
      local Conn = net.ftps.init{
         server=TEST_HOST,
         username=TEST_USER,
         password="FIXME",
         port=990,
         ssl_version="tls-v1",
         ca_file=TEST_CERT,
         live=true,
      }
      Conn:list{remote_path='/files/'}
   end
   testutils.AssertError( attempt, nil, "SSL certificate problem", true)
end

local function testErrorPassword()
   local function attempt()
      local Conn = net.ftps.init{
         server=TEST_HOST,
         username=TEST_USER,
         password="FIXME",
         port=990,
         ssl_version="tls-v1",
         verify_peer=false,
         verify_host=false,
         live=true,
      }
      Conn:list{remote_path='/files/'}
   end
   testutils.AssertError( attempt, nil, "Invalid username and/or password", true)
end


local function testOk()
  local Conn = net.ftps.init{
      server=TEST_HOST,
      username=TEST_USER,
      password=TEST_PASS,
      port=990,
      ssl_version="tls-v1",
      verify_peer=false,
      verify_host=false,
      live=true,
  }
  local FileList = Conn:list{remote_path='/files/'}
  assert(#FileList==1)
  local Body = Conn:get{remote_path='/files/hello.txt'}
  assert(Body=='hello\n')
end

local function testVerify()
  local Conn = net.ftps.init{
      server=TEST_HOST,
      username=TEST_USER,
      password=TEST_PASS,
      port=990,
      ssl_version="tls-v1",
	   ca_file="../DBD/ca-bundle/ca-bundle.crt",
      verify_host=false,
      live=true,
  }
  local FileList = Conn:list{remote_path='/files/'}
  assert(#FileList==1)
  local Body = Conn:get{remote_path='/files/hello.txt'}
  assert(Body=='hello\n')
end

return {
	['testErrorHost']=testErrorHost,
	['testErrorPort']=testErrorPort,
	['testErrorNoSsl']=testErrorNoSsl,
	['testErrorVerify']=testErrorVerify,
	['testErrorPassword']=testErrorPassword,
	['testOk']=testOk,
	['testVerify']=testVerify,
}
