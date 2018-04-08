/*
 * Extended Operating System Loader (XOSL) - Open Watcom Migration
 * Copyright (c) 2010 by Mario Looijkens:
 * - Comment-out unused variables to get rid of Warning! W014: no reference 
 *   to symbol.
 *
 * This code is distributed under GNU General Public License (GPL)
 *
 * The full text of the license can be found in the GPL.TXT file,
 * or at http://www.gnu.org
 *
 * - The WPP compiler emits a few symbols that cannot be suppressed by changing
 *   compiler settings. The XOSL project is not using default (Open Watcom) 
 *   libraries. Because of this Open Watcom specific symbols emitted by the WPP 
 *   compiler will cause undefined references linker errors.
 *   We'll need to add appropriate symbols using the dummy functions
 *   defined in this file to be able to link without errors.
 * - When using the * operator on variables of type long the Watcom Compiler will
 *   generate calls to modules i4m and i4d which are part of the clib library.
 *   Because we are not using default Open Watcom libraries to link this project
 *   we need to extract modules i4m and i4d from the clib library and indicate
 *   to the linker to add these object files to library misc.lib
 *
 */

//ML - extracted from:
//open_watcom_1.8.0-src.zip\OW18src\bld\plusplus\h\wcpp.h

// There are __wcpp_* names that are internal to the runtime library.
// These names should be changed to something like __wint_* so that
// they are not confused with external compiler entry points.
//
#define CPPLIB( name ) __wcpp_4_##name##_       // name of run-time function
#define CPPDATA( name ) __wcpp_4_data_##name##_ // name of data
#define CPPDATA_LIB( name ) ___wcpp_4_data_##name##_ // name of data

//ML - extracted from:
//open_watcom_1.8.0-src.zip\OW18src\bld\plusplus\cpplib\runtime\cpp\undefed.cpp

extern "C"
void CPPLIB( undefed_cdtor )(   // ISSUE ERROR FOR UNDEFINED CTOR, DTOR
    void )
{
    //CPPLIB(fatal_runtime_error)( RTMSG_UNDEFED_CDTOR, 1 );
}


//ML - extracted from:
//open_watcom_1.8.0-src.zip\OW18src\bld\plusplus\cpplib\runtime\cpp\undefmbd.cpp

extern "C" {

#ifdef _M_IX86
    int CPPDATA_LIB( undefined_member_data );
#else
    int CPPDATA( undefined_member_data );
#endif

};


//ML - extracted from:
//open_watcom_1.8.0-src.zip\OW18src\bld\plusplus\cpplib\runtime\cpp\from dtorarst.cpp

extern "C"
void* CPPLIB(dtor_array_store)(){
  return 0;
}


//ML - extracted from:
//open_watcom_1.8.0-src.zip\OW18src\bld\plusplus\cpplib\runtime\cpp\pure_err.cpp

extern "C"
//_WPRTLINK    //ML
void CPPLIB(pure_error)(        // TRAP NON-OVERRIDDEN PURE VIRTUAL CALLS
    void )
{
    //if( !_RWD_PureErrorFlag ) {
    //    _RWD_PureErrorFlag = 1;
    //    CPPLIB(fatal_runtime_error)( RTMSG_PURE_ERR, 1 );
    //}
}
