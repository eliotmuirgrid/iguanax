-- Unit tests for the ifware_license module.

local oldcpath = package.cpath
package.cpath = oldcpath .. ';../ext_ifware_license/?.so;../ext_ifware_license/?.dll'  
require('ifware_license')
package.cpath = oldcpath

-- Functions.
local test_ifware_license = {}

test_ifware_license.testBasic = function()
   local lic
  
   --pre-existing Chameleon permanent license
   lic = ifware_license.getLicense{product = 'Chameleon', instance_id = 'HCPHINIBM',
      license_expiry = 'none'}
   assert(lic.Code == 'E7HGJJFJHJ;ALE@EF>K')
   assert(lic.LicenseExpiry == 'none') 
   
   --pre-existing Chameleon trial license
   lic = ifware_license.getLicense{product = 'Chameleon', instance_id = 'IPCIBLBDMA',
      expiry_type = 'months', license_expiry = 1, date_stamp = '19/01/2011 11:46:16'}   
   assert(lic.Code == 'U]\\^]]^\\MAJLBLEK>')
   assert(lic.LicenseExpiry == '2011-02-19 00:00:00')
   
   --tested Iguana license - no expiry
   lic = ifware_license.getLicense{product = 'Iguana', instance_id = 'NHU7P5L94KPS4HRE',
      num_channels = 5, expiry_type = 'months', license_expiry = 'none',
      maintenance_expiry = 'none', date_stamp = '21/03/2013 14:30:35'
   }
   assert(lic.Code == table.concat{
         'C9C39210F2730B7A79B1747AC2EE63F1CF0EDE546C5507AA34B67862B99F',
         '4459ECCBC87E4B5CE2FE28308FD9F2A7BAF3A0B83692F9F82ECDD111C263',
         'F62DA2330EB80399123B18D2512C89E4098AB0FB8612FE1EB586C0F9AD7B',
         'A3D0D24532CE4BE403D4C954336BBE32C24FFCDDBD41AA0577C82E4E4BD0',
         '376116062B241469CF32F2B60C17FDD6D0BA8C23AC4566B79DF8A26F9F3D',
         'FEBDE3D4722D2347DC70C9440061276764CA8C6B8E2A80E2E1622EB56E21',
         '44F23CF7D5549AEF59AA8D802ECEBE99890840539396AA86780D4223921C',
         '05519C42DD9EE78EDBFB15246F9EDAE1845F3F849A76DC8D76C032321FCB'})
   assert(lic.LicenseExpiry == 'none')
   assert(lic.MaintenanceExpiry == 'none')
   assert(lic.NumChannels == 5)
   assert(lic.LicenseTimeStamp == '2013-03-21 14:30:35')   
    
   --tested Iguana license - expiry in months
   lic = ifware_license.getLicense{product = 'Iguana', instance_id = 'NHU7P5L94KPS4HRE',
      num_channels = 5, expiry_type = 'months', license_expiry = 12,
      maintenance_expiry = 6, date_stamp = '21/03/2013 14:27:57'
   }
   assert(lic.Code == table.concat{
         '5735BAE8ACCACE8CD0E249BCAF34AB8ECF0EDE546C5507AA34B67862B99F',
         '4459ECCBC87E4B5CE2FE28308FD9F2A7BAF3A0B83692F9F82ECDD111C263',
         'F62DA2330EB80399123B18D2512C89E4098AB0FB8612FE1EB586C0F9AD7B',
         'A3D0D24532CE4BE403D4C954336BBE32C24FFCDDBD418D3ABFEAA2A77863',
         'A4FFEA626B19DE1A7F3131C5840F459C28AD461E72AA094D9DF8A26F9F3D',
         'FEBDE3D4722D2347DC70CF00E1D2163A4D2F70F24079ABA9D44A8E6DE480',
         '3C359ED636F22B7F2CB5F80ECFDE84B135E6752E54AC55291BCFF9749A69',
         '9570665D76B4EE0366C4F1C3024E097DB49CD53835F6A1DC7F32EC0768ED'})
   assert(lic.LicenseExpiry == '2014-03-28 14:27:57')
   assert(lic.MaintenanceExpiry == '2013-09-23 14:27:57')
   assert(lic.NumChannels == 5)
   assert(lic.LicenseTimeStamp == '2013-03-21 14:27:57')

   --tested Iguana license - expiry in months, unlimited channels
   lic = ifware_license.getLicense{product = 'Iguana', instance_id = 'NHU7P5L94KPS4HRE',
      num_channels = 'unlimited', expiry_type = 'months', license_expiry = 12,
      maintenance_expiry = 6, date_stamp = '11/04/2013 15:34:19'
   }   
   assert(lic.Code == table.concat{
         '636F150AA57BFAB44757782E4DAEAF1FCF0EDE546C5507AA34B67862B99F',
         '4459ECCBC87E4B5CE2FE28308FD9F2A7BAF3A0B83692F9F82ECDD111C263',
         'F62DA2330EB80399123B18D2512C89E4098AB0FB8BE516B38898C43FA06B',
         '1808B9580C244957233B10728E538DC683FA21BAF2B6EB7587B615DDB220',
         '1C81E8367D3D75D2D97753243F1ADC180F5D9181500FB2749DF8A26F9F3D',
         'FEBDE3D4722D2347DC70CF00E1D2163A4D2F70F24079ABA9D44A8E6DE480',
         '3C359ED636F22B7F2CB5F80ECFDE84B135E6752E54AC55291BCFF9749A69',
         '9570665D76B4EE0366C4F1C3024E097DB49CD53835F6A1DC7F32EC0768ED'})   
   assert(lic.LicenseExpiry == '2014-04-18 15:34:19')
   assert(lic.MaintenanceExpiry == '2013-10-14 15:34:19')
   assert(lic.NumChannels == 'unlimited')
   assert(lic.LicenseTimeStamp == '2013-04-11 15:34:19')

   --tested Iguana license - expiry in days
   lic = ifware_license.getLicense{product = 'Iguana', instance_id = 'NHU7P5L94KPS4HRE',
      num_channels = 3, expiry_type = 'days', license_expiry = 3,
      maintenance_expiry = 3, date_stamp = '21/03/2013 14:37:30'
   }
   assert(lic.Code == table.concat{
         '2595AD8C3CFDD5BE670879581AB36123CF0EDE546C5507AA34B67862B99F',
         '4459ECCBC87E4B5CE2FE28308FD9F2A7BAF3A0B83692F9F82ECDD111C263',
         'F62DA2330EB80399123B18D2512C89E4098AB0FB46C7F236576568234FFD',
         '8B94C976A6BEA0DA81ACFC56F21CAA04E9209CDF8CD28B9ED26C2DA32A7A',
         '89861C630B92C960C9440061276764CA8C6B8E2A80E2E1629DF8A26F9F3D',
         'FEBDE3D4722D2347DC7019F52846CBB0CE1125980EC73C8D016F2EB56E21',
         '44F23CF7D5549AEF59AA8D8033F0E3546A894A997B968F72EAF744B6921C',
         '05519C42DD9EE78EDBFB15246F9EDAE1845F3F849A76DC8D76C032321FCB'})
   assert(lic.LicenseExpiry == '2013-03-24 14:37:30')
   assert(lic.MaintenanceExpiry == '2013-03-24 14:37:30')
   assert(lic.NumChannels == 3)
   --Not checking when this license claims to start - we don't need to see the magic.

   --untested Iguana license - specifying expiry with date
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 11:12:13',
      maintenance_expiry = '17/08/2013 11:12:13'}
   assert(lic.LicenseExpiry == '2013-07-17 11:12:13')
   assert(lic.MaintenanceExpiry == '2013-08-17 11:12:13')

   --Specifying mismatching Iguana license time-of-day should fail.
   assert(not pcall(ifware_license.getLicense, {product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '16/07/2013 11:12:13',
      maintenance_expiry = '17/08/2013 11:12:13'}))
   assert(not pcall(ifware_license.getLicense, {product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '18/07/2013 11:12:13',
      maintenance_expiry = '17/08/2013 11:12:13'}))
   assert(not pcall(ifware_license.getLicense, {product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 10:10:10',
      maintenance_expiry = '17/08/2013 11:12:13'}))

   --untested Iguana license - specifying just a license expiry with date
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 11:12:13',
      maintenance_expiry = 'none'}
   assert(lic.LicenseExpiry == '2013-07-17 11:12:13')
   assert(lic.MaintenanceExpiry == 'none')   

  --untested Iguana license - specifying just a maintenance expiry with date
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = 'none',
      maintenance_expiry = '17/08/2013 11:12:13'}
   assert(lic.LicenseExpiry == 'none')
   assert(lic.MaintenanceExpiry == '2013-08-17 11:12:13')

   --untested Iguana license - "date" expiry_type but no expiry
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = 'none',
      maintenance_expiry = 'none'}
   assert(lic.LicenseExpiry == 'none')
   assert(lic.MaintenanceExpiry == 'none')   

   --untested Iguana license - specified maintenance date that's to be rounded up
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 11:12:13',
      maintenance_expiry = '18/08/2013 12:00:00',
      maintenance_rounding = 'up'}
   assert(lic.MaintenanceExpiry == '2013-09-17 11:12:13')
   
   --untested Iguana license - specified maintenance date that's to be rounded up
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 11:12:13',
      maintenance_expiry = '16/08/2013 12:00:00',
      maintenance_rounding = 'up'}
   assert(lic.MaintenanceExpiry == '2013-08-17 11:12:13')

   --untested Iguana license - specified maintenance date that shouldn't need rounding
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 11:12:13',
      maintenance_expiry = '18/10/2013 11:12:13',
      maintenance_rounding = 'up'}
   assert(lic.MaintenanceExpiry == '2013-10-18 11:12:13')
   
   --untested Iguana license - specified maintenance date that's to be rounded down
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 11:12:13',
      maintenance_expiry = '16/08/2013 12:00:00',
      maintenance_rounding = 'down'}
   assert(lic.MaintenanceExpiry == '2013-07-17 11:12:13')

   --untested Iguana license - specified maintenance date that's to be rounded down
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = '17/07/2013 11:12:13',
      maintenance_expiry = '18/08/2013 12:00:00',
      maintenance_rounding = 'down'}
   assert(lic.MaintenanceExpiry == '2013-08-17 11:12:13')

--[[ THE FOLLOWING TESTS CAUSE A CRASH IN 32-BIT NODLexample ON WINDOWS
  
   --untested Iguana license - no expiry, so maintenance date should be exact
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'NHU7P5L94KPS4HRE', num_channels = 3,
      expiry_type = 'date', license_expiry = 'none',
      maintenance_expiry = '18/08/2013 12:00:00',
      maintenance_rounding = 'down'}
   assert(lic.MaintenanceExpiry == '2013-08-18 12:00:00')


   --untested Iguana license - expiry in months, maintenance date rounded up
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'foo', num_channels = 3,
      expiry_type = 'months', license_expiry = 5,
      maintenance_expiry = '10/09/2013 11:12:13',
      maintenance_rounding = 'up',
      date_stamp = '26/06/2013 13:17:31'}
   assert(lic.MaintenanceExpiry == '2013-09-27 13:17:31')

   --untested Iguana license - expiry in months, maintenance date rounded down
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'foo', num_channels = 3,
      expiry_type = 'months', license_expiry = 5,
      maintenance_expiry = '10/09/2013 11:12:13',
      maintenance_rounding = 'down',
      date_stamp = '26/06/2013 13:17:31'}
   assert(lic.MaintenanceExpiry == '2013-08-27 13:17:31')

   --untested Iguana license - expiry in months, maintenance date doesn't need rounding
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'foo', num_channels = 3,
      expiry_type = 'months', license_expiry = 5,
      maintenance_expiry = '27/09/2013 13:17:31',
      maintenance_rounding = 'down',
      date_stamp = '26/06/2013 13:17:31'}
   assert(lic.MaintenanceExpiry == '2013-09-27 13:17:31')
  
   --untested Iguana license - expiry in months, maintenance date in the past
   lic = ifware_license.getLicense{product = 'Iguana',
      instance_id = 'foo', num_channels = 3,
      expiry_type = 'months', license_expiry = 5,
      maintenance_expiry = '27/05/2013 12:15:18',
      maintenance_rounding = 'down',
      date_stamp = '26/06/2013 13:17:31'}
   assert(lic.MaintenanceExpiry == '2013-05-26 13:17:31')
   assert(lic.LicenseExpiry == '2013-11-28 13:17:31')

]]
    
end

return test_ifware_license

