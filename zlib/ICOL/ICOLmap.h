#ifndef ICOLMAP_H
#define ICOLMAP_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ICOLmap.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 12:21 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

struct ICOLinfo {
   ICOLinfo(const int Width, const int Priority, const COLstring& Title)
       : m_Width(Width), m_Priority(Priority), m_Title(Title) {}
   ICOLinfo() : m_Width(0), m_Priority(0), m_Title("") {}
   int       m_Width;
   int       m_Priority;
   COLstring m_Title;
};

void ICOLinit();
void ICOLfree();
void ICOLadd(const COLstring& Name, int Width, int Priority, const COLstring& Title);
void ICOLremove(const COLstring& Name);
void ICOLmapSort(const COLmap<COLstring, ICOLinfo>& Map, COLvar* pOut);
void ICOLmapToVar(COLvar* pOut);

#endif  // end of defensive include
