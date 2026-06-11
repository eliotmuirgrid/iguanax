#ifndef __RGN_OBFUSCATE_H__
#define __RGN_OBFUSCATE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: RGNobfuscate
//
// Description:
//
// Hide licensing symbols from Iguana or Chameleon shared library users.
// Stop-gap measure until we determine how to NOT export all local symbols
// by default in shared libraries on POSIX. Easy on GCC 4.0+, but I don't
// know about other POSIX compilers.
// See Ticket #10261 for a full explanation.
//
// Author: Andrew Vajoczki
// Date:   01/06/2009
//
//---------------------------------------------------------------------------

// Define DISABLE_RGN_OBFUSCATION to debug RGN-related issues.
//#define DISABLE_RGN_OBFUSCATION

// If you alter these remember to update the LicenseApp See #10364
// for details - Eliot. 

#ifndef DISABLE_RGN_OBFUSCATION

#define RGNdemoKey                  l8Djf9X
#define RGNmachineId                d8fy3ksi
#define RGNexpiryDate               ho9fkNH
#define RGNisLicensed               w8djB0l
#define RGNlicenseKey               difOpe0o
#define RGNmacAddress               d98fh30i
#define RGNscrambleId               q908dFjw
#define RGNcheckLicense             cdchudfa
#define RGNextensionKey             ks0wehr
#define RGNunscrambleId             msidfh0w
#define RGNdoesNotExpire            tdfj4deq
#define RGNfullMachineId            id3hffjb
#define RGNlegacyMachineId          sibdfn0
#define RGNprintMACAddress          jtd1uf9i
#define RGNsetRegistryPath          dfonPID9
#define RGNregistrationCode         djk9fmV
#define RGNnoLicenseRequired        m3kfj0df
#define RGNregistrationLevel        wuJHDfo2
#define RGNsetRegistrationCode      rNUis6fgi
#define RGNexpiryDateAsDateTime     jq4f8CF9b
#define RGNsetUpRegistrationEntries oDIfjbBV
#define RGN_MACHINE_ID_INTERFACE_NAME uHdf893s5

#endif // DISABLE_RGN_OBFUSCATION

#endif // end of defensive include

