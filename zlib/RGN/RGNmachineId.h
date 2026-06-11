#ifndef __RGN_MACHINE_ID_H__
#define __RGN_MACHINE_ID_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: RGNmachineId
//
// Description:
//
// Functions for getting machine ID and setting registration codes
//
// Author: Eliot Muir
// Date:   Tue 08/17/1999
//
//---------------------------------------------------------------------------

#include "RGNobfuscate.h"
#include <COL/COLminimumInclude.h>

class COLstring;
class COLdateTime;

/**
 * Sets the Registry root path to use to look up
 * registry information.  Default is
 * software\\interfaceware\\chameleon.
 */
void RGNsetRegistryPath(const COLstring& RegistryPath);

/**
 * Tries to access the registry for the entries needed for the following
 * functions. If the entries do not exist, will try to create them
 * If creation fails, then an exception is thrown indicating that the
 * application should be started as a user with administrator access.
 *
 * On unix this does nothing.
 */
void RGNsetUpRegistrationEntries();

/**
 * Returns the registration code
 */
COLstring RGNregistrationCode();

/**
 * Returns the machine id
 */
COLstring RGNmachineId();

/**
 * Returns the expiry date
 */
COLstring RGNexpiryDate();
void RGNexpiryDateAsDateTime(COLdateTime& DateTime);

/**
 * Returns an integer value indicating if a the license does not exprie
 */
COLint32 RGNdoesNotExpire();

/**
 * Returns a demo key
 */
COLstring RGNdemoKey(const COLstring& MachineId,
                     const COLstring& ExpiryDate,
                     int DoesNotExpire);

/**
 * Returns an extension key
 *
 * Note: This is the key that we give to customers always, not
 * just in situations where their license is being extended.
 *  -Kevin
 */
COLstring RGNextensionKey(const COLstring& MachineId,
                          const COLstring& ExpiryDate,
                          int DoesNotExpire);

/**
 * Returns a license key
 */
COLstring RGNlicenseKey(const COLstring& MachineId,
                        const COLstring& ExpiryDate,
                        int DoesNotExpire);

enum RGNlicenseLevel
{
   RGN_NOT_LICENSED      = 0,
   RGN_LICENSE_DEMO      = 1,
   RGN_LICENSE_EXTENSION = 2,
   RGN_LICENSE_FULL      = 3,
   RGN_EXPIRED           = 4
};


// DO NOT CHANGE THIS NUMBER !!!! SINCE IT IS HARDCODED INTO DELPHI CODE - ELIOT
#define RGN_REGISTRATION_ERROR 5444

/**
 * sets the current registration level
 */
RGNlicenseLevel RGNregistrationLevel(const COLstring& MachineId,
                                     const COLstring& RegKey,
                                     const COLstring& ExpiryDate,
                                     int              DoesNotExpire);

/**
 * sets the registration code
 */
void RGNsetRegistrationCode(const char* Code,
                            const char* ExpiryDate,
                            int DoesNotExpire);

/**
 * Checks if the host machine has a valid Chameleon license.
 * If not, an exception is thrown.
 */
void RGNcheckLicense();

/**
 * Returns true if the host machine has a valid Chameleon license.
 * If not, it returns false.
 */
bool RGNisLicensed();

/**
 * Provides a scrambled version of the given ID.
 */
COLstring RGNscrambleId(const COLstring& Id);

/**
 * Unscrambles an ID that was scrambled using the
 * RGNscrambleID method.
 */
COLstring RGNunscrambleId(const COLstring& Id);

const COLint16 RGN_MACHINE_ID_MAC_ADDRESS_LENGTH = 12;

#ifndef _WIN32
   /**
    * Buffer Capacity must be at least RGN_MACHINE_ID_MAC_ADDRESS_LENGTH + 1.
    */
   void RGNprintMACAddress(char* pBuffer, int Capacity, COLuint8* pMACAddress);
#endif

#endif // end of defensive include
