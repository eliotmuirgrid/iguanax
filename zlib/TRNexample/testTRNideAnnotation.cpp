// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTRNideAnnotation
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Saturday 29 April 2023 - 09:00PM
// ---------------------------------------------------------------------------
#include "testTRNideAnnotation.h"

#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>
#include <TRN/TRNannotationJob.h>
#include <TRN/TRNannotation.h>
#include <TRN/TRNannotationBlock.h>
#include <TRN/TRNwriteScriptError.h>

#include <LHL7/LHL7api.h>
#include <LUA/LUAutils.h>

#include <COL/COLclosure.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// annotation/simple
static void testAnnotation(){
   COL_FUNCTION(testAnnotation);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "foo_guid";
   P["development"] = true;
   P["filename"]    = "main.lua";
   P["sample_data_index"] = 2;

   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(2, Result["BlockData"].size());
}

// annotation/argument/request
static void testArgumentRequest(){
   COL_FUNCTION(testArgumentRequest);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "luafunction_guid";
   P["development"] = true;
   P["filename"]    = "main.lua";
   P["full_argument_requests"][0]["id"] = "2FA037B75D41D18B921C8C7E5032906C-1-r";
   P["full_argument_requests"][0]["expansion_tree"].setMapType();
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COL_VAR(P.json(1));
   COLvar Result;
   TRNannotation(P, &State, &Result);
   COL_VAR(Result.json(1));
}

// annotation/argument/iteration
static void testArgumentIteration(){
   COL_FUNCTION(testArgumentIteration);
   TRNannotationJob State;
   COLvar P;
   P["component"]      = "function_jump";
   P["development"]    = true;
   P["filename"]       = "jump.lua";
   P["start_line"]     = 2;
   P["iteration_tree"]["i"] = 2;
   P["iteration_tree"]["c"].setArrayType();
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COL_VAR(P.json(1));
   COLvar Result;
   //TRNannotation(P, &State, &Result);
   TRNannotationBlock(P, &State, &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(2, Result["BlockData"]["scope"]["p"].asInt());
   UNIT_ASSERT_EQUALS(2, Result["BlockData"]["scope"]["l"].asInt());
   UNIT_ASSERT(Result["BlockData"]["scope"]["d"].asString().find("BBB") != npos);
}

// annotation/function/jump
static void testFunctionJump(){
   COL_FUNCTION(testFunctionJump);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "loop_jump";
   P["development"] = true;
   P["filename"]    = "jump.lua";
   P["function_block"]["start_line"] = 2;
   P["function_block"]["iteration"] = 2;
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   COL_VAR(Result.json(1));  
   UNIT_ASSERT_EQUALS(6,Result["BlockData"][0]["scope"]["e"].asInt());
   UNIT_ASSERT_EQUALS(2,Result["BlockData"][0]["scope"]["l"].asInt());
   UNIT_ASSERT_EQUALS(2,Result["BlockData"][0]["scope"]["p"].asInt());
   UNIT_ASSERT_EQUALS(2,Result["BlockData"][0]["scope"]["s"].asInt());
}

// annotation/function/nojump
static void testFunctionNoJump(){
   COL_FUNCTION(testFunctionNoJump);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "function_jump";
   P["development"] = true;
   P["filename"]    = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   COL_VAR(Result.json(1));
   COL_VAR(Result["BlockData"][0]["scope"].json(1));
   UNIT_ASSERT_EQUALS(6,Result["BlockData"][0]["scope"]["e"].asInt());
   UNIT_ASSERT_EQUALS(1,Result["BlockData"][0]["scope"]["l"].asInt());
   UNIT_ASSERT_EQUALS(1,Result["BlockData"][0]["scope"]["p"].asInt());
   UNIT_ASSERT_EQUALS(3,Result["BlockData"][0]["scope"]["s"].asInt());
}

// annotation/jump/back
static void testJumpBack(){
   COL_FUNCTION(testJumpBack);
   TRNannotationJob State;
   COLvar P;
   P["component"]      = "backjump";
   P["development"]    = true;
   P["filename"]       = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COLvar Result1;
   TRNannotation(P, &State, &Result1);
   P["start_line"] = 16;
   P["iteration_tree"]["i"] = 2;
   P["iteration_tree"]["c"].setArrayType();
   COLvar Result2;
   TRNannotationBlock(P, &State, &Result2);
   P["start_line"]     = 7;
   P.erase("iteration_tree");
   P["target_callee"] = "5C60A8AA5A12AD2606B658C9BBCD6C30";
   COL_VAR(P.json(1));
   COLvar Result;
   TRNannotationBlock(P, &State, &Result);
   COL_VAR(Result.json(1));   
   Result = Result["BlockData"];
   COL_VAR(Result["scope"]["c"][0]["p"]);
   UNIT_ASSERT_EQUALS(1, Result["scope"]["c"][0]["p"].asInt());
   UNIT_ASSERT_EQUALS(2, Result["scope"]["c"][0]["l"].asInt());
   UNIT_ASSERT(Result["scope"]["c"][0]["d"].asString().size() > 0);
}

// annotation/require
// This tests out having a require statement in the main function, which was causing a crash IX-2511
static void testRequire(){
   COL_FUNCTION(testRequire);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "require";
   P["development"] = true;
   P["filename"]    = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   COL_VAR(Result.json(1));
   UNIT_ASSERT_EQUALS(1, Result["BlockData"].size());
   UNIT_ASSERT_EQUALS(1, Result["BlockData"][0]["scope"]["l"].asInt()); // current iteration is 1
   UNIT_ASSERT_EQUALS(1, Result["BlockData"][0]["scope"]["p"].asInt()); // target iteration is 1
   UNIT_ASSERT_EQUALS(1, Result["BlockData"][0]["scope"]["s"].asInt()); // annotation start line is 1
   UNIT_ASSERT_EQUALS(4, Result["BlockData"][0]["scope"]["e"].asInt()); // annotation end line is 4
}

static COLstring OriginalFile = 
"function main()\n"
"\terror(\"intentional error\");\n"
"end\n";

static COLstring ModifiedFile = 
"function main()\n"
"\tif (true)\n"
"\terror(\"intentional error\");\n"
"end\n";

// annotation/error/syntax/crash
static void testErrorSyntaxCrash(){
   COL_FUNCTION(testErrorSyntaxCrash);
   COLstring FilePath = "config/comps/errorSyntaxCrash/dev/main.lua";
   FILcorrectPathSeparators(FilePath);
   FILwriteFile(FilePath, OriginalFile);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "errorSyntaxCrash";
   P["development"] = true;
   P["filename"]    = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   // Make sure we have set a script error
   UNIT_ASSERT(Result.exists("ScriptError"));
   UNIT_ASSERT_EQUALS("intentional error", Result["ScriptError"]["Description"].asString());
   // Alter the code to produce a syntax error prior to the script error
   FILwriteFile(FilePath, ModifiedFile);
   TRNannotationResetMaps(&State.m_AnnotationState); // need this to make sure we re-load the changed file.
   TRNideBoilerPlate(P, &State);
   bool CompileSuccess = TRNcompileAnnotationProgram(&State, &CompileError);
   UNIT_ASSERT(!CompileSuccess);
   COL_VAR(CompileError);
   UNIT_ASSERT_EQUALS("[string \"main.lua\"]:3: 'then' expected near 'error'", CompileError);
   TRNwriteScriptError(&Result, CompileError, State.m_AnnotationState, true); // Prior to IX-2725 this would segfault
   // Make sure we have set a script error in the Result so it will reach the front-end ui
   UNIT_ASSERT(Result.exists("ScriptError"));
   UNIT_ASSERT_EQUALS("'then' expected near 'error'", Result["ScriptError"]["Description"].asString());
}

static void updateVmdFileAddLine(){
   COLstring Path = "config/comps/errorEdiCrash/dev/test.vmd";
   FILcorrectPathSeparators(Path);
   COLstring VmdContent;
   FILreadFile(Path, &VmdContent);
   VmdContent.append("}");
   FILwriteFile(Path, VmdContent);
}

static void updateVmdFileRemoveLine(){
	COLstring Path = "config/comps/errorEdiCrash/dev/test.vmd";
   FILcorrectPathSeparators(Path);
   COLstring VmdContent;
   FILreadFile(Path, &VmdContent);
   VmdContent = VmdContent.substr(0, VmdContent.size() - 2);
   FILwriteFile(Path, VmdContent);
}

// This test starts with a valid vmd, we compile then modify the vmd to be invalid and recompile
static void testErrorEdiCrash(){
   COL_FUNCTION(testErrorEdiCrash);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "errorEdiCrash";
   P["development"] = true;
   P["filename"]    = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   LHL7api(State.L);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   COL_VAR(Result);
   COL_TRC("Remove the last character of the vmd to change the modified time and break the parse");
   updateVmdFileRemoveLine();
   COL_TRC("Do a regular annotation call without a recompile to trigger the cache reload");
   TRNannotation(P, &State, &Result);
   COL_TRC("Fix vmd for the next test");
   updateVmdFileAddLine();
   COL_VAR(Result);
   COL_TRC("Recompile now");
   TRNideBoilerPlate(P, &State);
   TRNcompileAnnotationProgram(&State, &CompileError);
}

// This test starts with a vmd with bad JSON format, and we try to compile twice
static void testErrorEdiCrash2(){
   COL_FUNCTION(testErrorEdiCrash2);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "errorEdiCrash2";
   P["development"] = true;
   P["filename"]    = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   LHL7api(State.L);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   COL_VAR(Result);
   COL_TRC("Recompile now");
   TRNideBoilerPlate(P, &State);
   TRNcompileAnnotationProgram(&State, &CompileError);
}

static void testRequireString(){
   COL_FUNCTION(testRequireString);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "globalStringCrash";
   P["development"] = true;
   P["filename"]    = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   COL_VAR(CompileError);
   UNIT_ASSERT_EQUALS(true, CompileError.find("unexpected symbol near '\"asd\"'") != npos);
   COLvar Result;
   COL_VAR(P.json(1));
   TRNannotation(P, &State, &Result);
   UNIT_ASSERT(true);
}

// Just make sure there are no compilation errors
static void testHl7NodeNameLookup(){
   COL_FUNCTION(testHl7NodeNameLookup);
   TRNannotationJob State;
   COLvar P;
   P["component"]   = "hl7NodeName";
   P["development"] = true;
   P["filename"]    = "main.lua";
   TRNideBoilerPlate(P, &State);
   COLstring CompileError;
   TRNcompileAnnotationProgram(&State, &CompileError);
   UNIT_ASSERT(CompileError.is_null());
   LHL7api(State.L);
   COLvar Result;
   TRNannotation(P, &State, &Result);
   UNIT_ASSERT(true);
}

void testTRNideAnnotation(UNITapp* pApp) {
   pApp->add("annotation/simple",             &testAnnotation);
   pApp->add("annotation/argument/request",   &testArgumentRequest);
   pApp->add("annotation/argument/iteration", &testArgumentIteration);
   pApp->add("annotation/function/jump",      &testFunctionJump);
   pApp->add("annotation/function/nojump",    &testFunctionNoJump);    
   pApp->add("annotation/jump/back",          &testJumpBack);    
   pApp->add("annotation/require",            &testRequire);   
   pApp->add("annotation/error/syntax/crash", &testErrorSyntaxCrash);
   pApp->add("annotation/error/edi/crash",    &testErrorEdiCrash);
   pApp->add("annotation/error/edi/crash2",    &testErrorEdiCrash2);
   pApp->add("annotation/single_string",      &testRequireString);
   pApp->add("annotation/hl7/name_lookup",    &testHl7NodeNameLookup);
}
