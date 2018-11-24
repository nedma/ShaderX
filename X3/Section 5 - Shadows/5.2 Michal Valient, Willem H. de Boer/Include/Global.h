/** @file Global.h
 * @brief a must-have file in any code file.
 * @author Michal Valient
 * @version 1.0
 *
 * Globaly used definitions and macros are specified in this file.
 */
#pragma once

///D3_FASTCALL stands for __fastcall keyword.
#define D3_FASTCALL    __fastcall

///D3_INLINE stands for inline keyword.
#define D3_INLINE      inline

///Macro \c SETBIT(n) returns number with \c n-th bit set to 1.
#define SETBIT(n) (1<<(n))


/**
 * @brief D3_DV_XXXXXXXX constants are used in evaluating _LOG function. See detailed description of D3_DV_GRP0_LEV0 for more.
 *
 * <b>verbose mode levels - strategy of usage</b>
 * There are four groups (_GRPx_ in name):
 *       @li Group 0 - classes, that are unique or very important (and i.e. created every 
 *                     execution time) and maximum number of instances is less than 5
 *       @li Group 1 - Important CBaseClass derived classes with less than 10 instances
 *       @li Group 2 - Other CBaseClass classes
 *       @li Group 3 - CRootClass derived classes and global functions 
 * 
 * Each group has five levels of verbose level (_LEVx in name)
 *       @li Level 0 - Construction, destruction and informations in critical methods
 *                     (that are not executed more than 5 times during object lifetime)
 *                     also logs, that means warning messages
 *       @li Level 1 - Critical flow control decisions and informations in important methods
 *                     (that are not executed more than 20 times during object lifetime)
 *       @li Level 2 - Information in methods that are not executed more than 100 times
 *       @li Level 3 - Information in methods that are not executed more than 1000 times
 *       @li Level 4 - all other informations
 */
const int D3_DV_GRP0_LEV0   =   SETBIT(0);


/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP0_LEV1   =   SETBIT(1);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP0_LEV2   =   SETBIT(2);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP0_LEV3   =   SETBIT(3);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP0_LEV4   =   SETBIT(4);

/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP1_LEV0   =   SETBIT(5);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP1_LEV1   =   SETBIT(6);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP1_LEV2   =   SETBIT(7);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP1_LEV3   =   SETBIT(8);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP1_LEV4   =   SETBIT(9);

/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP2_LEV0   =   SETBIT(10);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP2_LEV1   =   SETBIT(11);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP2_LEV2   =   SETBIT(12);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP2_LEV3   =   SETBIT(13);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP2_LEV4   =   SETBIT(14);

/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP3_LEV0   =   SETBIT(15);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP3_LEV1   =   SETBIT(16);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP3_LEV2   =   SETBIT(17);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP3_LEV3   =   SETBIT(18);
/// See D3_DV_GRP0_LEV0 for detailed informations
const int D3_DV_GRP3_LEV4   =   SETBIT(19);

///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one disables logging
static const int D3_DVHELP_NONE     =   0;              //No output
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one enables all logging
static const int D3_DVHELP_ALL      =   0xFFFFFFFF;     //All levels
//groups
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one enables group 0.
static const int D3_DVHELP_GRP0     =   D3_DV_GRP0_LEV0 | D3_DV_GRP0_LEV1 | D3_DV_GRP0_LEV2 | D3_DV_GRP0_LEV3 | D3_DV_GRP0_LEV4;
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one enables group 1.
static const int D3_DVHELP_GRP1     =   D3_DV_GRP1_LEV0 | D3_DV_GRP1_LEV1 | D3_DV_GRP1_LEV2 | D3_DV_GRP1_LEV3 | D3_DV_GRP1_LEV4;
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one enables group 2.
static const int D3_DVHELP_GRP2     =   D3_DV_GRP2_LEV0 | D3_DV_GRP2_LEV1 | D3_DV_GRP2_LEV2 | D3_DV_GRP2_LEV3 | D3_DV_GRP2_LEV4;
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one enables group 3.
static const int D3_DVHELP_GRP3     =   D3_DV_GRP3_LEV0 | D3_DV_GRP3_LEV1 | D3_DV_GRP3_LEV2 | D3_DV_GRP3_LEV3 | D3_DV_GRP3_LEV4;
//Levels
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one level 0 for every group.
static const int D3_DVHELP_LEV0     =   D3_DV_GRP0_LEV0 | D3_DV_GRP1_LEV0 | D3_DV_GRP2_LEV0 | D3_DV_GRP3_LEV0;
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one level 1 for every group.
static const int D3_DVHELP_LEV1     =   D3_DV_GRP0_LEV1 | D3_DV_GRP1_LEV1 | D3_DV_GRP2_LEV1 | D3_DV_GRP3_LEV1;
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one level 2 for every group.
static const int D3_DVHELP_LEV2     =   D3_DV_GRP0_LEV2 | D3_DV_GRP1_LEV2 | D3_DV_GRP2_LEV2 | D3_DV_GRP3_LEV2;
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one level 3 for every group.
static const int D3_DVHELP_LEV3     =   D3_DV_GRP0_LEV3 | D3_DV_GRP1_LEV3 | D3_DV_GRP2_LEV3 | D3_DV_GRP3_LEV3;
///Helper for setting D3_DEBUG_VERBOSE_LEVEL. This one level 4 for every group.
static const int D3_DVHELP_LEV4     =   D3_DV_GRP0_LEV4 | D3_DV_GRP1_LEV4 | D3_DV_GRP2_LEV4 | D3_DV_GRP3_LEV4;

/** 
 * This constant describes enabled debug levels.
 * Use bitwise \c operations to define enabled levels using 
 * \c D3_DV_GRPx_LEVx and \c D3_DVHELP_XXXX constants.<br>
 * For example <tt>(D3_DVHELP_GRP0 | D3_DV_GRP3_LEV0)</tt> enables 
 * all group 0 logs and level 0 logs for group 3.<br>
 * For example <tt> ~( D3_DVHELP_LEV4 | D3_DVHELP_LEV3 | D3_DVHELP_LEV2 | D3_DVHELP_GRP3)</tt> 
 * disables levels 4,3,2 and all group 3 logs.
 * @hideinitializer
 */
static const int D3_DEBUG_VERBOSE_LEVEL = ~( D3_DVHELP_LEV4 | D3_DVHELP_LEV3 | D3_DVHELP_LEV2 | D3_DVHELP_GRP3);
//static const int D3_DEBUG_VERBOSE_LEVEL = ~D3_DVHELP_LEV4;

#ifdef _DEBUG
    #ifndef D3_DEBUG_MEMORY_MANAGER
        /// Instructs Memory Manager to use Debug Routines. Undefine with release version.
        #define D3_DEBUG_MEMORY_MANAGER
    #endif  //D3_DEBUG_MEMORY_MANAGER
    #ifndef D3_DEBUG_VERBOSE
        ///  Verbose level - if defined, _LOG method is not ignored
        #define D3_DEBUG_VERBOSE 
    #endif  //D3_DEBUG_VERBOSE
#else
    #ifdef D3_DEBUG_MEMORY_MANAGER
        #undef D3_DEBUG_MEMORY_MANAGER
    #endif  //D3_DEBUG_MEMORY_MANAGER
    #ifdef D3_DEBUG_VERBOSE
        #undef D3_DEBUG_VERBOSE
    #endif  //D3_DEBUG_VERBOSE
#endif _DEBUG

//############################################################################
//# BEGIN: Global pragmas.
//#---------------------------------------------------------------------------
#pragma warning(disable : 4100 4127 4201)
//#---------------------------------------------------------------------------
//# END: Global pragmas.
//############################################################################

#include "MGR-ErrorManager.h"
#include "MGR-MemoryManager.h"
#include "ENG-Types.h"
