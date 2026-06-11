xml.DOCUMENT  = 'document'
sr = { ATTR='Attribute', TAG='Tag', NAME='Name', LEN='Length', VR='VR', ITEM='Item',
       TOOLKIT_DIR='.\\dicom\\', DATA_DIR='.\\dicom\\' }

-- updates the Length attributes
-- return the XML document as a string
function node.sr_render(x)
   local pos  = 0
   local num  = 0
   local kind = x:nodeType()
   if kind==xml.DOCUMENT then
      for i=1,#x do node.sr_render(x[i]) end
      return tostring(x)
   elseif kind==xml.TEXT then return string.len(x:nodeValue())
   elseif kind==xml.ELEMENT then
      for i=1,#x do
         local cur = x[i]
         num = num + node.sr_render(cur)
         if cur:nodeName()==sr.LEN and cur:nodeType()==xml.ATTRIBUTE then pos=i end
      end
      if x:nodeName()==sr.ATTR then
         if pos==0 then x:append(xml.ATTRIBUTE,sr.LEN):setNodeValue(num)
         else           x[pos]:setNodeValue(num) end
      end
      return 1
   else return 0 end
end

-- returns the value container of "src" node and if "val" is not null stores that value 
function node.value(src,val)
   local dest = nil;
   if     src:nodeType()==xml.ATTRIBUTE then dest=src
   elseif src:nodeType()==xml.TEXT      then dest=src
   elseif src:nodeType()==xml.ELEMENT   then
      if #src==0 then dest=src
      else
         for i=1,#src do
            if src[i]:nodeType()==xml.TEXT then
               dest=src[i]
               break
            end
         end
         if dest==nil then dest=src:append(xml.TEXT,val) end
      end
   end
   if val~=nil and dest~=nil then dest:setNodeValue(val) end
   return dest
end

-- copies entire subtree rooted at "src" into a new child in "dest" node
-- returns newly creted child
function node.add(dest,src,pos)
   if src==nil then return nil end
   local cur  = nil
   local kind = src:nodeType()
   local name = src:nodeName()
   if kind==xml.TEXT then name=src:nodeValue() end
   if pos==nil then cur = dest:append(kind,name)
   else             cur = dest:insert(pos,kind,name) end
   if kind==xml.ELEMENT then
      for i=1,#src do cur:add(src[i]) end
   elseif kind==xml.ATTRIBUTE then
      cur:value(src:nodeValue())
   elseif kind~=xml.TEXT then
      assert(false, 'Unexpected node type=' .. kind)
   end
   return cur;
end

-- locate an xml element that is a direct child of "src" is "<Attribute Tag="tag
-- returns node
function node.tag_find(src,tag)
   if src:nodeType()==xml.ELEMENT then
      for i=1,#src do
         local cur = src[i]
         if cur:nodeName()==sr.ATTR then
            for j=1,#cur do
               local sub = cur[j]
               if sub:nodeType() ==xml.ATTRIBUTE and
                  sub:nodeName() ==sr.TAG and
                  sub:nodeValue()==tag then return cur end
            end
         end
      end
   end
   return nil
end

-- copy list of top level tags from "src" to "dest"
function node.tag_copy(dest,src,tags)
   for i=1,#tags do dest:add(src:tag_find(tags[i])) end
end

-- manually add a tag
function node.tag_add(dest,tag,vr,name,defval)
   if vr==nil and name==nil and defval==nil then
      return dest:tag_add(tag.Tag, tag.VR, tag.Name, tag:value():nodeValue())
   end
   local cur = dest:tag_find(tag)
   if cur==nil then
      cur = dest:append(xml.ELEMENT,sr.ATTR)
      cur:append(xml.ATTRIBUTE,sr.TAG ):setNodeValue(tag )
      cur:append(xml.ATTRIBUTE,sr.VR  ):setNodeValue(vr  )
      cur:append(xml.ATTRIBUTE,sr.NAME):setNodeValue(name)
      if defval~=nil then cur:value(defval) end
   end
   return cur
end

-- add an item node, returns added item
function node.add_item(dest)
   return dest:append(xml.ELEMENT,sr.ITEM)
end
function sr.move_file(orig, target)
   os.remove(sr.DATA_DIR..target)
   os.rename(sr.DATA_DIR..orig  , sr.DATA_DIR..target)
end

function sr.DCMtoXML(orig, target)
   os.remove(sr.DATA_DIR..target      )
   os.remove(sr.DATA_DIR..orig..'.xml')
   local File = io.popen(sr.TOOLKIT_DIR..'DcmXmlConvert.exe -d -f '..sr.DATA_DIR..orig)
   local Err  = File:read('*a')
   File:close()
   if Err=='' then os.rename(sr.DATA_DIR..orig..'.xml', sr.DATA_DIR..target) end
   os.remove(sr.DATA_DIR..orig..'.xml')
   return Err
end

function sr.XMLtoDCM(orig, target)
   os.remove(sr.DATA_DIR..target      )
   os.remove(sr.DATA_DIR..orig..'.dcm')
   local File = io.popen(sr.TOOLKIT_DIR..'DcmXmlConvert.exe -x -f '..sr.DATA_DIR..orig)
   local Err  = File:read('*a')
   File:close()
   if Err=='' then os.rename(sr.DATA_DIR..orig..'.dcm', sr.DATA_DIR..target) end
   os.remove(sr.DATA_DIR..orig..'.dcm')
   return Err
end

function sr.loadXML(name)
   local File = io.open(sr.DATA_DIR..name,'rb')
   local Raw  = File:read('*a')
   File:close()
   return xml.parse{data=Raw}
end

function sr.saveXML(name, data)
   local Raw  = data:sr_render()
   local File = io.open(sr.DATA_DIR..name,"w+")
   File:write(Raw)
   File:close()
end   