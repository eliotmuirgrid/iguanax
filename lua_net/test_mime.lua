-- $Revision: 1.3 $
-- $Date: 2012-12-19 17:33:59 $

require('common.testutils')
require('mime')

local assertEquals = testutils.assertEquals

-- Big ugly string to make sure we can embed the CRLF pairs properly.
local expectedBody = '--xyzzy_0123456789_xyzzy\r\nContent-Type: text/plain; charset="utf8"\r\n\r\nDear internet,\r\nHere is a picture of a kitten.\r\n\r\n--xyzzy_0123456789_xyzzy\r\nContent-Type: image/jpeg; charset="B"; name="kittens.jpeg"\r\nContent-Disposition: inline; filename="kittens.jpeg"\r\nContent-Transfer-Encoding: base64\r\n/9j/4AAQSkZJRgABAQEAYABgAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRofHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwhMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/wAARCAACAAIDASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD3+iiigD//2Q==\r\n\r\n--xyzzy_0123456789_xyzzy--'

local OS = os.getenv('OS') or ''
local isWin = string.match(string.lower(OS), 'windows', 1, true) ~= nil 

local server      = 'openrelay.smrsh.tld'
local to          = {'xyzzy@example.com',}
local from        = 'fnord@example.com'
local header      = {['Subject'] = 'Attachment test'}
local body        = 'Dear internet,\r\nHere is a picture of a kitten.'
local attachments = isWin and {'..\\lua_net\\kittens.jpeg',} or {'../lua_net/kittens.jpeg',}
local live        = false
local debug       = true

-- Invoke send directly to load up our debug values.
local msgBody, msgHeader = mime.send{
    server=server,
    to=to,
    from=from,
    header=header,
    body=body,
    attachments=attachments,
    usename=username,
    password=password,
    timeout=timeout,
    use_ssl=use_ssl,
    live=live,
    debug=debug
  }

-- Test functions
local function bodyWithAttachment()
  assertEquals(msgBody, expectedBody)
end

local function checkHeaders()
  local count = 0
  local foundContentType = false
  local foundMIMEVersion = false
  for k, v in pairs(msgHeader) do
    count = count + 1
    if k == 'Content-Type' then
      assertEquals(v, 'multipart/mixed; boundary=xyzzy_0123456789_xyzzy')
      foundContentType = true
    end
    if (k == 'MIME-Version') then
      assertEquals(v, '1.0')
      foundMIMEVersion = true
    end
  end
  assertEquals(count, 3)
  assertEquals(foundContentType, true)
  assertEquals(foundMIMEVersion, true)
end

-- Roll-up tests and return them
return {
   ['bodyWithAttachment'] = bodyWithAttachment,
   ['checkHeaders'] = checkHeaders,
}
