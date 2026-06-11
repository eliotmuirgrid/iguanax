-- This old long gross script is used to read in the translator help documetation 
-- for a new Iguana version and output an HTML file that can then be uploaded to our 
-- website that powers api.interfaceware.com

function main(Data)
   -- THIS PATH MUST BE CHANGED TO POINT AT THE translator_help DIRECTORY
   -- IN THE IGUANA INSTALL FOLDER.
   base = '/path/to/install/directory/translator_help/'
   local b = io.popen('cd ' .. base .. ' && ls -d */')
   local dir_list = b:read('*a')
   b:close()
   local dir_T = string.split(dir_list,'\n')
   local v = iguana.version()
   local version = v.major .. v.minor .. v.build
   local dir = {
      [1] = '' -- needed for the base folder
   }
   for x=1, #dir_T do 
      if dir_T[x] ~= '' then
         dir[x+1] = dir_T[x]:gsub('/',''):gsub('\\','')
      end
   end
 
   
   api = {}
   api_intro = {}

   for y=2, #dir do
      api[dir[y]] = {}
      api_intro[dir[y]] = {}
   end
   
   for y=1, #dir do 
      
      
      local a = io.popen('cd ' .. base .. dir[y] .. ' && ls ')
      local file_list = a:read('*a')
      a:close()
      
      local files_T = string.split(file_list,'\n')
      
      
      
      for x=1, #files_T do 
         filename = files_T[x]
         if filename:find('coroutine') then
         else
            if filename:find('.html') then
               if filename:find('_overview.html') then
                  -- do nothing
               else
                  -- read the file info
                  local name = string.split(filename, '.html')
                  htmlFile = io.open(base  ..dir[y] .. '/' .. filename)   
                  text2 = htmlFile:read('*a')
                  if y > 1 then
                     --api[dir[y]][name[1]] = json.parse{data=text}
                     api_intro[dir[y]] = {}
                     api_intro[dir[y]][name[1]] = text2
                  else
                     api_intro[name[1]] = ''
                     api_intro[name[1]] = text2
                  end 
               end
               
               
            elseif filename:find('.json') then
               
               -- read the json info
               local name = string.split(filename, '.json')
               if filename ~= '' then
                  jsonFile = io.open(base  ..dir[y] .. '/' .. filename)    
                  text = jsonFile:read('*a')
                  if y > 1 then
                     api[dir[y]][name[1]] = json.parse{data=text}
                     
                     
                  else
                     api[name[1]] = json.parse{data=text}
                  end 
               end
               
            else
               
               
            end
         end
         
         
         
      end
   end
   
   
   jsonVersion = json.serialize{data=api}
   jsonVersion= jsonVersion:gsub('<div.*FunctionList.* />', '')
   jsonVersion= jsonVersion:gsub('<div.*FunctionDetails.* />', '')
   jsonTable = json.parse{data=jsonVersion}

   jsonAPI_intro = json.serialize{data=api_intro}
   jsonAPI_intro= jsonAPI_intro:gsub('<div.*FunctionList.* />', '')
   jsonAPI_intro= jsonAPI_intro:gsub('<div.*FunctionDetails.* />', '')
   api_intro = json.parse{data=jsonAPI_intro}
   
   
   local html = ''
   
   for name, line in pairsByKeys(jsonTable) do
      html = html .. doHxId(name, 1, name)
      if api_intro[name] then
         if #api_intro[name] >0 then
            html = html .. api_intro[name] .. '\n'
         end
      end
      
      for n, l in pairsByKeys(line) do 
         html = html .. doHxId(name .. '.' .. n, 2,name .. '_' .. n)
         if api_intro[name] then
            if #api_intro[name] == 0 then
               if api_intro[name][n] then
                  html = html .. api_intro[name][n] .. '\n'
               end
            end
         end
         if l ~= 'gmatch' then
            if l.Title then
               if l.Examples then
                  exampleString = ''
                  example = false
                  for z=1, #l.Examples do
                     if l.Examples[z] ~= nil then
                        if l.Examples[z]:find('<pre>') == nil then
                              exampleString = exampleString .. doPre(l.Examples[z])
                        else
                           exampleString = exampleString .. l.Examples[z]
                        end
                        example = true
                     end
 
                  end
                  if example then 
                     html = html .. doBlock(doPara('Example Usage of <b>' .. l.Usage .. '</b>'))
                     html = html .. exampleString:gsub('<pre>', '<pre class="noescape prettyprint lang-lua">')
                  end
                  
                  
               end
               if l.Usage then
                  html = html .. doHx('Usage', 3)
                  html = html .. doPara(doCode(l.Usage))
               end
               if l.Desc then
                  html = html .. doPara(doULP(doTable(doTarget(l.Desc:gsub('pre>', 'code>')))))
               end
               
      
               if l.Parameters then
                  
                  -- get required params
                  local req = false
                  local reqString = ''
                  for z=1, #l.Parameters do
                     for x, y in pairs(l.Parameters[z]) do
                        if y.Opt then 

                        else
                           req = true
                           reqString = reqString .. doLIreq(x,y.Desc)
                        end

                     end
 
                  end
                  if req then 
                     html = html .. doHx('Required Parameters', 3)
                     html = html .. doPara(doUL(reqString:gsub('<b>count<b>', '<b>count</b>')))
                  end
                  
                  -- get optional params
                  local opt = false
                  local optString = ''
                  for z=1, #l.Parameters do
                     for x, y in pairs(l.Parameters[z]) do
                        if y.Opt then 
                           opt = true
                           optString = optString .. doLIreq(x,y.Desc)
                        end

                     end
 
                  end
                  if opt then 
                     html = html .. doHx('Optional Parameters', 3)
                     html = html .. doPara(doUL(optString:gsub('<b>count<b>', '<b>count</b>')))
                  end
               end
               
               if l.Returns then
                  retString = ''
                  ret = false
                  for z=1, #l.Returns do
                     if l.Returns[z] ~= nil then
                        retString = retString .. doLI(l.Returns[z].Desc)
                        ret = true
                     end
 
                  end
                  if ret then 
                     html = html .. doHx('Returns', 3)
                     html = html .. doPara(doUL(retString))
                  end
                  
                  
               end
               if l.SeeAlso then
                  alsoString = ''
                  also = false
                  for z=1, #l.SeeAlso do
                     if l.SeeAlso[z] ~= nil then
                        alsoString = alsoString .. doLI(doLink(l.SeeAlso[z].Link, l.SeeAlso[z].Title))
                        also = true
                     end
 
                  end
                  if also then 
                     html = html .. doHx('For More Information', 3)
                     html = html .. doPara(doUL(alsoString))
                  end
                  
                  
               end
               
               
               
            else
               -- get the 2nd level
               for x, y in pairsByKeys(l) do  
                  html = html .. doHxId(name .. '.' .. n .. '.' .. x, 2, name .. '_' .. n .. '_' .. x)
                  if api_intro[name] then
                     if #api_intro[name] == 0 then
                        if api_intro[name][n] then
                           if #api_intro[name][n] == 0 then
                              if api_intro[name][n][x] then
                                 html = html .. api_intro[name][n][x] .. '\n'
                              end
                           end
                        end
                     end
                  end
                  if y.Examples then
                  exampleString = ''
                  example = false
                  for z=1, #y.Examples do
                     if y.Examples[z] ~= nil then
                           if y.Examples[z]:find('<pre>') == nil then
                              exampleString = exampleString .. doPre(y.Examples[z])
                        else
                           exampleString = exampleString .. y.Examples[z]
                        end
                        example = true
                     end
 
                  end
                  if example then 
                     html = html .. doBlock(doPara('Example Usage of <b>' .. y.Usage .. '</b>'))
                     html = html .. exampleString:gsub('<pre>', '<pre class="noescape prettyprint lang-lua">')
                  end
                  
                  
               end
               if y.Usage then
                  html = html .. doHx('Usage', 3)
                  html = html .. doPara(doCode(y.Usage))
               end
               if y.Desc then
                  html = html .. doPara(doULP(doTable(doTarget(y.Desc:gsub('pre>', 'code>')))))
               end
               
      
               if y.Parameters then
                  
                  -- get required params
                  local req = false
                  local reqString = ''
                  for z=1, #y.Parameters do
                     for x1, y1 in pairs(y.Parameters[z]) do
                        if y1.Opt then 

                        else
                           req = true
                           reqString = reqString .. doLIreq(x1,y1.Desc)
                        end

                     end
 
                  end
                  if req then 
                     html = html .. doHx('Required Parameters', 3)
                     html = html .. doPara(doUL(reqString:gsub('<b>count<b>', '<b>count</b>')))
                  end
                  
                  -- get optional params
                  local opt = false
                  local optString = ''
                  for z=1, #y.Parameters do
                     for x1, y1 in pairs(y.Parameters[z]) do
                        if y1.Opt then 
                           opt = true
                           optString = optString .. doLIreq(x1,y1.Desc)
                        end

                     end
 
                  end
                  if opt then 
                     html = html .. doHx('Optional Parameters', 3)
                     html = html .. doPara(doUL(optString:gsub('<b>count<b>', '<b>count</b>')))
                  end
               end
               
               if y.Returns then
                  retString = ''
                  ret = false
                  for z=1, #y.Returns do
                     if y.Returns[z] ~= nil then
                        retString = retString .. doLI(y.Returns[z].Desc)
                        ret = true
                     end
 
                  end
                  if ret then 
                     html = html .. doHx('Returns', 3)
                     html = html .. doPara(doUL(retString))
                  end
                  
                  
               end
               if y.SeeAlso then
                  alsoString = ''
                  also = false
                  for z=1, #y.SeeAlso do
                     if y.SeeAlso[z] ~= nil then
                        alsoString = alsoString .. doLI(doLink(y.SeeAlso[z].Link, y.SeeAlso[z].Title))
                        also = true
                     end
 
                  end
                  if also then 
                     html = html .. doHx('For More Information', 3)
                     html = html .. doPara(doUL(alsoString))
                  end
                  
                  
               end
                  
                  
               end
            end
         end
      end
   end
   
 
   local final = template1 .. html .. template2
   io.output(base .. version  .. '.html')

   
   
   local worked = io.write(final)
   io.flush()
   
   
   
end

function pairsByKeys (t, f)
   local a = {}
   for n in pairs(t) do table.insert(a, n) end
   table.sort(a, f)
   local i = 0      -- iterator variable
   local iter = function ()   -- iterator function
      i = i + 1
      if a[i] == nil then return nil
      else return a[i], t[a[i]]
      end
   end
   return iter
end

function pairsByKeysUnsorted(t, f)
   local a = {}
   for n in pairs(t) do table.insert(a, n) end
   local i = 0      -- iterator variable
   local iter = function ()   -- iterator function
      i = i + 1
      if a[i] == nil then return nil
      else return a[i], t[a[i]]
      end
   end
   return iter
end
function string.split(s,d)
   local t = {}
   local i = 0
   local f
   local match = '(.-)' .. d .. '()'
   if string.find(s, d) == nil then
      return {s}
   end
   for sub, j in string.gfind(s, match) do
      i = i + 1
      t[i] = sub
      f = j
   end
   if i~= 0 then
      t[i+1]=string.sub(s,f)
   end
   return t
end

function doHx(name, num)
   return '<h' .. num .. '>' .. name .. '</h' .. num .. '>\n'
end

function doHxId(name, num, id)
   return '<h' .. num .. ' id="' .. id .. '">' .. name .. '</h' .. num .. '>\n'
end
function doPara(val)
   return '<p>' .. val .. '</p>\n'
end

function doCode(val)
   return '<code>' .. val .. '</code>\n'
end
function doUL(val)
   return '<ul>' .. val .. '</ul>\n'
end
function doLIreq(name, val)
   return '<li><strong>' .. name .. ':</strong> ' .. val .. '</li>\n'
end
function doLI(val)
   return '<li>' .. val .. '</li>\n'
end
function doLink(link, title)
   return '<a href="' .. link .. '" target="_blank">' .. title .. '</a>'
end
function doBlock(val)
   return '<blockquote>' .. val .. '</blockquote>\n'
end
function doPre(val)
   return '<pre class="noescape prettyprint lang-lua">' .. val .. '</pre>\n'
end
function doTarget(val)
   return val:gsub('<a href', '<a target="_blank" href')
end
function doTable(val)
   return val:gsub('<table', '</p><table'):gsub('</table>', '</table><p>')
end
function doULP(val)
   return val:gsub('<ul', '</p><ul'):gsub('</ul>', '</ul><p>')
end


template1 = [[<!doctype html>
<html>
  <head>
    <meta charset="utf-8">
    <meta content="IE=edge,chrome=1" http-equiv="X-UA-Compatible">
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1">
    <title>IGUANA API Reference</title>

    <link href="stylesheets/screen.css" rel="stylesheet" type="text/css" media="screen" />
    <link href="stylesheets/print.css" rel="stylesheet" type="text/css" media="print" />
    <link href="stylesheets/ifwareapi.css" rel="stylesheet" type="text/css" media="screen" />
    <link href="http://fonts.googleapis.com/css?family=Open+Sans" rel="stylesheet" type="text/css">
    <script src="javascripts/lib/jquery.1.11.0.min.js"></script>
    <script src="javascripts/all.js" type="text/javascript"></script>
    <script src="javascripts/version.js" type="text/javascript"></script>
    <script src="javascripts/lib/run_prettify.js?lang=lua&skin=sons-of-obsidian"></script>
  </head>

  <body class="index">

    <a href="#" id="nav-button">
      <span>
        <img src="images/navbar.png"/>
      </span>
    </a>
    <div class="tocify-wrapper">
        <div class="logo menu">
            <img src="images/logo-interfaceware.png" width="180"/>
        </div>
        <div class="search">
          <input type="text" class="search" id="input-search" placeholder="Search">
        </div>
        <ul class="search-results"></ul>
      <div id="toc">
      </div>
        <ul class="toc-footer">
            <li>&copy; iNTERFACEWARE Inc.</li>
        </ul>
    </div>
    <div class="page-wrapper">
      <div id="header">
        <div class="logo">
           <img src="images/logo-interfaceware.png"/ class="logo-interfaceware">
           <img src="images/logo-API-reference.png"/ class="logo-API">
        </div>
        <div class="version-select">
          IGUANA Version:
            <select id="nav_select" onchange="window.open(this.options[this.selectedIndex].value,'_top')">
            </select>
        </div>
        <a href="http://help.interfaceware.com" target="_blank">Help Center &rarr;</a>
      </div>
      <div class="dark-box"></div>
      <div class="content">
        <h1 id="introduction">Introduction</h1>

        <p>Welcome to the Iguana API reference section! The following built-in modules are available for use within your Iguana Translator scripts.</p>
<p>Select the Iguana version above and then browse or search for specific modules to see usage information and code examples.  Enjoy.</p>]]
template2 = [[</div>
      <div class="dark-box">
      </div>
    </div>
<script>
  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
  })(window,document,'script','//www.google-analytics.com/analytics.js','ga');

  ga('create', 'UA-329140-1', 'auto');
  ga('send', 'pageview');

</script>
  </body>
</html>]]