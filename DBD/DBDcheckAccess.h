#ifndef __DBDCHECKACCESS_H__
#define __DBDCHECKACCESS_H__

#include "COL/COLstring.h"
class TUNstateClient;

COLstring DBDaccessFile();
void DBDcheckAccess(bool* pHasAllAccess, TUNstateClient* pClient);

#endif // end of defensive include
