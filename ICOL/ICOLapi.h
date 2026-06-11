#ifndef ICOLAPI_H
#define ICOLAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ICOLapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 1:50 PM
//  ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void ICOLapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap);


#endif // end of defensive include
