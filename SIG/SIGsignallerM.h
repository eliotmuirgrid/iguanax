//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignallerM (copied from COLsignalM.h).
//
// Description: This header is used to set up the defines before including other 
// header files in order to build up the definitions for a SIGsignallerN
// 
// The important defines are SIG_SIGNALLER_HEADER_PAYLOAD, and SIG_SIGNALLER_PARAM_COUNT
// Looking at SIGsignaller0.h, there are sections that look like below, which use
// the macros in this file:
//
// #define SIG_SIGNALLER_PARAM_COUNT 0 //define the parameter count
// #define SIG_SIGNALLER_HEADER_PAYLOAD <SIG/SIGsignallerMheader.h> //define the payload that will be using the macros
// #include <SIG/SIGsignallerM.h> //employ the header with the payload
// #undef SIG_SIGNALLER_HEADER_PAYLOAD //undef everything
// #undef SIG_SIGNALLER_PARAM_COUNT 
//
// Author: Nasron Cheong
// Date: Feb 2005
//
//---------------------------------------------------------------------------
#ifndef __SIG_SIGNALLER_M__
#define __SIG_SIGNALLER_M__
   #include <COL/COLerror.h>
   #include <SIG/SIGcommaList.h>
   #include "SIGslotCollectionVoid.h"
   
   #include "SIGsignallerVoid.h"
   #include "SIGsignalReceiver.h"

   // This is a class used to describe slots/signals that do not have
   // a return value.
   class TVoid;
   
   // Defines a Type Name, eg TParam0
   #define SIG_SIGNALLER_PARAM_TYPE( _N ) TParam##_N

   // Defines an argument that follows a type eg Param0
   #define SIG_SIGNALLER_ARG( _N ) Param##_N

   // Defines the declaration when used in a function declaration
   // eg TParam0 Param0
   #define SIG_SIGNALLER_ARG_DECL( _N ) SIG_SIGNALLER_PARAM_TYPE( _N ) SIG_SIGNALLER_ARG( _N )

   // Defines the type for use in template declarations, the use of typename 
   // will allow the use of the type without knowing that it is a class/int/etc
   // eg. typename TParam0
   #define SIG_SIGNALLER_TEMPLATE_DECL( _N ) typename SIG_SIGNALLER_PARAM_TYPE( _N )
#endif

// Used to generate a name based on the input _CLASSNAME and the parameter count
// eg SIGsignaller0
#define SIG_SIGNALLER_TEMPLATE_NAME( _CLASSNAME) SIG_SIGNALLER_TEMPLATE_NAME_I( _CLASSNAME, SIG_SIGNALLER_PARAM_COUNT )

// In order to pass the SIG_SIGNALLER_PARAM_COUNT as an argument to another macro, these
// helper macros are here
#define SIG_SIGNALLER_TEMPLATE_NAME_I( _CLASSNAME, _N ) SIG_SIGNALLER_TEMPLATE_NAME_I2( _CLASSNAME, _N )
#define SIG_SIGNALLER_TEMPLATE_NAME_I2( _CLASSNAME, _N ) _CLASSNAME##_N

// Used to generate a list of types, used for function name declarations when a argument name is not 
// needed. eg
// TParam0, TParam1
#define SIG_SIGNALLER_PARAMS SIG_COMMA_LIST( SIG_SIGNALLER_PARAM_COUNT, SIG_SIGNALLER_PARAM_TYPE )

// Used to provide a type list with argument names
// e.g 
// TParam0 Param0, TParam1 Param1
#define SIG_SIGNALLER_PARAMS_NAMES SIG_COMMA_LIST( SIG_SIGNALLER_PARAM_COUNT, SIG_SIGNALLER_ARG_DECL )

// Defines a list of just argument names
// eg Param0, Param1
#define SIG_SIGNALLER_PARAMS_NAMES2 SIG_COMMA_LIST( SIG_SIGNALLER_PARAM_COUNT, SIG_SIGNALLER_ARG )

//The following defines two sets of macros, 
//which differ between a count of 0 and more than zero to avoid a trailing "," in the lists
#if SIG_SIGNALLER_PARAM_COUNT == 0

#   define SIG_SIGNALLER_TEMPLATE_PARAMS  class Result = TVoid
#   define SIG_SIGNALLER_TEMPLATE_PARAMS_IMPL  class Result
#   define SIG_SIGNALLER_PARAMS_IMPL Result

#   define SIG_SIGNALLER_PARAMS_VOID
#   define SIG_SIGNALLER_PARAMS_VOID_WITH_CLASS class _Class
#   define SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS_VOID _Class
#   define SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER SignalType* pOwner
#   define SIG_SIGNALLER_PARAMS_NAMES2_WITH_OWNER( _FIRSTPARAM ) _FIRSTPARAM

#else

#   define SIG_SIGNALLER_TEMPLATE_PARAMS SIG_COMMA_LIST( SIG_SIGNALLER_PARAM_COUNT, SIG_SIGNALLER_TEMPLATE_DECL ), class Result = TVoid
#   define SIG_SIGNALLER_TEMPLATE_PARAMS_IMPL  SIG_COMMA_LIST( SIG_SIGNALLER_PARAM_COUNT, SIG_SIGNALLER_TEMPLATE_DECL ), class Result
#   define SIG_SIGNALLER_PARAMS_IMPL SIG_SIGNALLER_PARAMS, Result

#   define SIG_SIGNALLER_PARAMS_VOID SIG_SIGNALLER_PARAMS
#   define SIG_SIGNALLER_PARAMS_VOID_WITH_CLASS class _Class, SIG_COMMA_LIST( SIG_SIGNALLER_PARAM_COUNT, SIG_SIGNALLER_TEMPLATE_DECL )
#   define SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS_VOID _Class, SIG_SIGNALLER_PARAMS
#   define SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER SignalType* pOwner, SIG_SIGNALLER_PARAMS_NAMES
#   define SIG_SIGNALLER_PARAMS_NAMES2_WITH_OWNER( _FIRSTPARAM ) _FIRSTPARAM, SIG_SIGNALLER_PARAMS_NAMES2

#endif
#   define SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS _Class, SIG_SIGNALLER_PARAMS_IMPL

#ifdef SIG_SIGNALLER_HEADER_PAYLOAD

// this is the actual code that requires all those macro defines.
// e.g. #define SIG_SIGNALLER_HEADER_PAYLOAD <SIG/SIGsignallerMheader.h> //define the payload that will be using the macros
// e.g  #include <SIG/SIGsignallerM.h> //employ the header with the payload
#include SIG_SIGNALLER_HEADER_PAYLOAD

#endif

#undef SIG_SIGNALLER_TEMPLATE_NAME
#undef SIG_SIGNALLER_TEMPLATE_NAME_I
#undef SIG_SIGNALLER_TEMPLATE_NAME_I2
#undef SIG_SIGNALLER_PARAMS
#undef SIG_SIGNALLER_PARAMS_NAMES
#undef SIG_SIGNALLER_PARAMS_NAMES2
#undef SIG_SIGNALLER_TEMPLATE_PARAMS
#undef SIG_SIGNALLER_TEMPLATE_PARAMS_IMPL
#undef SIG_SIGNALLER_PARAMS_IMPL
#undef SIG_SIGNALLER_PARAMS_VOID
#undef SIG_SIGNALLER_PARAMS_VOID_WITH_CLASS
#undef SIG_SIGNALLER_PARAMS_IMPL_WITH_CLASS_VOID
#undef SIG_SIGNALLER_PARAMS_NAMES_WITH_OWNER 
#undef SIG_SIGNALLER_PARAMS_NAMES2_WITH_OWNER
 
