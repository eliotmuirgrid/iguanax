require "sr"
local SR_MSG= '<DcmFile>' ..
                '<FileMetaInfo Service="STANDARD_COMPREHENSIVE_SR" Command="C_STORE_RQ"></FileMetaInfo>' ..
                '<DataSet Service="STANDARD_COMPREHENSIVE_SR" Command="C_STORE_RQ" TransferSyntax="1.2.840.10008.1.2.1"></DataSet>' ..
              '</DcmFile>'

function transform(SR)
   local fmi = SR.DcmFile.FileMetaInfo
   local dat = SR.DcmFile.DataSet
   -- Step 1: update values of existing tags
   fmi:tag_find('00020002'):value('1.2.840.10008.5.1.4.1.1.88.3')
   fmi:tag_find('00020010'):value('1.2.840.10008.1.2.1'         )
   fmi:tag_find('00020012'):value('1.2.840.10008.5.1.4.1.1.88.3')
   fmi:tag_find('00020013'):value('S_7_2'                       )   
   dat:tag_find('00080016'):value('1.2.840.10008.5.1.4.1.1.88.3')
   -- Step 2: add new/manually created tag
   dat:tag_add ('00080070', 'LO', 'Manufacturer'                        , ''                   )
   dat:tag_add ('00081070', 'PN', "Operators' Name"                     , ""                   )
   dat:tag_add ('002000AA', 'IS', 'Non-standard attribute name'         , '1'                  )
   dat:tag_add ('0040A007', 'CS', 'Non-standard attribute name'         , 'DICOM'              )
   dat:tag_add ('0040A020', 'SQ', 'Non-standard attribute name'         , nil                  )
   dat:tag_add ('0040A125', 'CS', 'Non-standard attribute name'         , 'ORIGINAL\UNVERIFIED')
   dat:tag_add ('400800FF', 'CS', 'Non-standard attribute name'         , 'RECORDED'           )
   dat:tag_add ('40080100', 'DA', 'Interpretation Recorded Date RETIRED', ''                   )
   dat:tag_add ('40080101', 'TM', 'Interpretation Recorded Time RETIRED', ''                   )
   -- Step 3: Create/update tags at sub-tree slevel
   local T0040A020 = dat:tag_find('0040A020'):add_item()
   -- Step 4: Copy tag by tab into a new empty/clean tre
   local SRout = xml.parse{data=SR_MSG}
   SRout.DcmFile.FileMetaInfo:tag_copy(fmi,
      {'00020001','00020002','00020003','00020010','00020012','00020013'})
   SRout.DcmFile.DataSet:tag_copy(dat,
      {'00080005','00080016','00080018','00080020','00080021','00080023',
       '00080030','00080031','00080033','00080050','00080060','00080016',
       '00080070','00080080','00080090','00081010','00081030','00081040',
       '00081070','00081090','00081111','00100010','00100020','00100030',
       '00100040','00101020','00101030','00102160','00181020','0020000D',
       '0020000E','00200010','00200011','00200013','002000AA','00400241',
       '00400242','00400243','00400244','00400245','00400253','00400254',
       '0040A007','0040A020','0040A125','400800FF','40080100','40080101'})
   return SRout
end