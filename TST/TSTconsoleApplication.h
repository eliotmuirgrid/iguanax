#ifndef __TST_CONSOLE_APPLICATION_H__  
#define __TST_CONSOLE_APPLICATION_H__  
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTconsoleApplication
//
// Description:
//
// A convenience class that makes writing console test applications fairly easy
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

struct TSTconsoleApplicationPrivate;
class CMDlineParser;
class TSTtestCollection;
class COLstring;
class TSTflags;

#include <COL/COLminimumInclude.h>

class TSTconsoleApplication
{
public:
   //takes the collection that will be used for tests, which should be 
   //initialized with the test information
   //
   // Once constructed, a description, and flags are set in the lineparser. parseArgs
   // should be called after construction
   //   
   TSTconsoleApplication(const COLstring& ApplicationName, //used for lineparser description
                         TSTtestCollection& Collection, // Does not take ownership
                         CMDlineParser& LineParser
                         );

   virtual ~TSTconsoleApplication();

   //the run behaviour depends on the parseargs result
   //
   // if --test is present, its argument is used as the test specification for the tests. 
   // if --test is not present, all tests in the collection are run
   // if --showtests is present, the collection tests are printed in --test form, and then function returns
   // if --showtree is present, the collection tests are printed in tree form, and the function returns
   // if --help is present, usage is shown and then function returns
   //
   // Returns true if the tests ran successfully, false otherwise
   bool run();

   bool runWithFlags(const TSTflags& Flags);

   void generateExampleTestSpec(COLostream& Stream);


private:
   TSTconsoleApplicationPrivate* pMember;
   TSTconsoleApplication(const TSTconsoleApplication& Orig); 
   TSTconsoleApplication& operator=(const TSTconsoleApplication& Orig); 

};

#endif // end of defensive include 
