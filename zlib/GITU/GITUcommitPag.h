#ifndef GITCOMMITPAGE_H
#define GITCOMMITPAGE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITUcommitPag.h
//
//  Description: Git log for instance with pagination.
//  Pulls 10 at a time by default, can specify other with pag=<num>
//  Author: Matthew Sobkowski
//  Date:   24/09/24 11:54 AM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;
class COLwebRequest;

void GITUcommitPag(const COLwebRequest& Request);


#endif // end of defensive include
