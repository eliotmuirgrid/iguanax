#ifndef __USRGENERATEADMINIFEMPTY_H__
#define __USRGENERATEADMINIFEMPTY_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRgenerateAdminIfEmpty.h
//
//  Description: Generates an admin user with random 16 char password if no usr file is found
//
//  Author: Matthew Sobkowski
//  Date:   02/11/23 3:59 PM
//  ---------------------------------------------------------------------------

class COLstring;

void USRgenerateAdminIfEmpty(const COLstring& UserName);

#endif // end of defensive include