#ifndef __SIG_COMMA_LIST_H__
#define __SIG_COMMA_LIST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc. All Rights Reserved
// 
// Module: SIGcommaList 
//
// SIG_COMMA_LIST can be used to make macros that generate lists of arguments like
// "typename Param1, typename Param2, typename Param3"
//
// For the above example the way to use it would be something like this
//
// #define COL_FAST_SIGNAL_COUNT 3
// 
// #define COL_FAST_TYPE(N)  typename Param##N
// 
// #define COL_FAST_SIGNAL_TYPELIST  SIG_COMMA_LIST(COL_FAST_SIGNAL_COUNT, COL_FAST_TYPE)
//
// If we change the constant COL_FAST_SIGNAL_COUNT to 4 we can get four items
// in the list and so on.
//
// So why is this useful?
//
// The problem is the C++ language doesn't allow us to define templates with
// variable numbers of parameters.  So this means we have to find work arounds to
// this language limitation.  This is one kind of workaround that relies heavily
// on the C Preprocessor.
//
// Other solutions are possible.  One solution that the book Modern C++ Design
// talks about is the use of 'typelists' which involves some other trickery manipulating
// the way the compiler implements templates.  At some point we may try this technique
// although the fear is that our range of C++ compilers may not be up to the job, whereas
// this solution definitely does work.
//
// Author: Eliot Muir (ruthlessly plagurizing Ryan Dewsbury and Nasron Cheong)
//
// Date:   Thu Jan 20 15:04:10 EST 2005
//
//---------------------------------------------------------------------------

// Notice the use of the SIG_COMMA_LIST_I macro?  This at first may seem unnecessary
// but is actually a very important part of implementation of SIG_COMMA_LIST.
//
// The issue is that the C Preprocessor is not a recursive parser.  This means that for
// each step of the macro expansion the macros must be explicitly passed as arguments into
// the macro being expanded.  This is why this macro is needed.  If you are interested
// further in this topic try and play with a copy of this code and see the fun you can
// get into... :-) Eliot

#define SIG_COMMA_LIST(COUNT_OF_LIST, FUNCTION ) SIG_COMMA_LIST_I( COUNT_OF_LIST, FUNCTION )
#define SIG_COMMA_LIST_I(COUNT_OF_LIST, FUNCTION ) SIG_COMMA_LIST_ ## COUNT_OF_LIST( FUNCTION )
#define SIG_COMMA_LIST_0(FUNCTION)
#define SIG_COMMA_LIST_1(FUNCTION) FUNCTION(1)
#define SIG_COMMA_LIST_2(FUNCTION) SIG_COMMA_LIST_1(FUNCTION), FUNCTION(2)
#define SIG_COMMA_LIST_3(FUNCTION) SIG_COMMA_LIST_2(FUNCTION), FUNCTION(3)
#define SIG_COMMA_LIST_4(FUNCTION) SIG_COMMA_LIST_3(FUNCTION), FUNCTION(4)
#define SIG_COMMA_LIST_5(FUNCTION) SIG_COMMA_LIST_4(FUNCTION), FUNCTION(5)
#define SIG_COMMA_LIST_6(FUNCTION) SIG_COMMA_LIST_5(FUNCTION), FUNCTION(6)
#define SIG_COMMA_LIST_7(FUNCTION) SIG_COMMA_LIST_6(FUNCTION), FUNCTION(7)
#define SIG_COMMA_LIST_8(FUNCTION) SIG_COMMA_LIST_7(FUNCTION), FUNCTION(8)
#define SIG_COMMA_LIST_9(FUNCTION) SIG_COMMA_LIST_8(FUNCTION), FUNCTION(9)

#define SIG_DELIMITER_LIST(COUNT_OF_LIST, FUNCTION, DELIMITER) SIG_DELIMITER_LIST_I( COUNT_OF_LIST, FUNCTION, DELIMITER )
#define SIG_DELIMITER_LIST_I(COUNT_OF_LIST, FUNCTION, DELIMITER ) SIG_DELIMITER_LIST_ ## COUNT_OF_LIST( FUNCTION, DELIMITER )
#define SIG_DELIMITER_LIST_0(FUNCTION, DELIMITER)
#define SIG_DELIMITER_LIST_1(FUNCTION, DELIMITER) FUNCTION(1)
#define SIG_DELIMITER_LIST_2(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_1(FUNCTION, DELIMITER) DELIMITER FUNCTION(2)
#define SIG_DELIMITER_LIST_3(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_2(FUNCTION, DELIMITER) DELIMITER FUNCTION(3)
#define SIG_DELIMITER_LIST_4(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_3(FUNCTION, DELIMITER) DELIMITER FUNCTION(4)
#define SIG_DELIMITER_LIST_5(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_4(FUNCTION, DELIMITER) DELIMITER FUNCTION(5)
#define SIG_DELIMITER_LIST_6(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_5(FUNCTION, DELIMITER) DELIMITER FUNCTION(6)
#define SIG_DELIMITER_LIST_7(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_6(FUNCTION, DELIMITER) DELIMITER FUNCTION(7)
#define SIG_DELIMITER_LIST_8(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_7(FUNCTION, DELIMITER) DELIMITER FUNCTION(8)
#define SIG_DELIMITER_LIST_9(FUNCTION, DELIMITER) SIG_DELIMITER_LIST_8(FUNCTION, DELIMITER) DELIMITER FUNCTION(9)

#endif // end of defensive include
