//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApi
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev
// Date:   8th Sept 2022
//---------------------------------------------------------------------------

#include "testApi.h"

#include <UNIT/UNITapp.h>

#include <HELP/HELPutils.h>
#include <HELP/HELPmodel.h>
#include <HELP/HELPserialize.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILdirEnumerator.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testInitObject(HELPobject& Object){
   COL_FUNCTION(testInitObject);

   HELPparameter Parameter1, Parameter2;
   Parameter1.Name = "Parameter name.";
   Parameter1.Description = "Parameter description.";
   Parameter2.Name = "Parameter 2 name.";
   Parameter2.Description = "Parameter 2 description.";
   Parameter2.Required = true;

   Object.Description = "Test description.";
   Object.SummaryLine = "Test summary line.";
   Object.ParameterTable = false;
   Object.Parameters.push_back(Parameter1);
   Object.Parameters.push_back(Parameter2);
}

static void testLoad(){
   COL_FUNCTION(testLoadConfig);
   COLstring Content, FileName, FileNameErr;
   FILpathSimplify("globalHelpDir/FOOget.help", &FileName, &FileNameErr);
   FILreadFile(FileName, &Content);
   COL_VAR(Content);
   HELPobject Model;
   HELPtoObject(&Model, Content);
   COL_VAR(Model);
}

static void testSave(){
   COL_FUNCTION(testSave);
   COLvar HelpVar;
   HELPobject Object;
   testInitObject(Object);
   COL_VAR(Object);
   HELPtoVar(&HelpVar, Object);
   COL_VAR(HelpVar.json(1));
}

void testApi(UNITapp* pApp){
	pApp->add("/model/save", &testSave);
}