/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*  @file omx_base_utils.h
*  The omx_base_utils header file contains the utils of the base component.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_base\src
*
*  @rev  1.0
*/
/* ------------------------------------------------------------------------*/
/* =========================================================================
 *! Revision History
 *! =========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ========================================================================= */

#ifndef _OMX_BASE_UTILS_H_
#define _OMX_BASE_UTILS_H_

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/* User code goes here */
/* ------compilation control switches --------------------------------------*/
/****************************************************************
* INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
/*-------program files ----------------------------------------*/
#include "timm_osal_trace.h"
#include "timm_osal_types.h"

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/**--------------data declarations -------------------------------**/
/**---------------function prototypes ---------------------------**/
/**--------------macros ---------------------------------------------**/

/******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 *****************************************************************************/

#define OMX_BASE_CMD_EVENTFLAG 0x00001000

/* OMX_BASE_DEBUG_TRACE_LEVEL- For PCO Traces */
#define OMX_BASE_DEBUG_TRACE_LEVEL TIMM_OSAL_DEBUG_TRACE_LEVEL

/**--------  data declaration  ------------**/
/**-------------  function prototyp -------**/
/**-------------  macros       ------------**/

#define TIMM_OSAL_Malloc(_size) \
  TIMM_OSAL_Malloc(_size, TIMM_OSAL_TRUE, 0, TIMMOSAL_MEM_SEGMENT_EXT)


#define TIMM_OSAL_Free(_ptr) \
    if(_ptr) { TIMM_OSAL_Free(_ptr); \
         _ptr = NULL; }

/**=========================================================================**/
/**
 * @def OMX_BASE_Trace  - Trace macros
 */
/**=========================================================================**/
#define OMX_BASE_Error(ARGS,...)   \
      TIMM_OSAL_TraceFunction("\nERROR:" DL1 DL2 ARGS "\n" DR1 DR2 ,##__VA_ARGS__)

#if(OMX_BASE_DEBUG_TRACE_LEVEL >= 4)
#define OMX_BASE_Trace(ARGS,...) \
    TIMM_OSAL_TraceFunction("\nTRACE:"DL1 DL2 ARGS "\n" DR1 DR2 ,##__VA_ARGS__)
#else
#define OMX_BASE_Trace(ARGS,...)
#endif

/**=========================================================================**/
/**
 * @def OMX_BASE_Entering -- "Entering <function> " statements
 *
 * @def OMX_BASE_Exiting   --  "Exiting <function > " statements
 */
/**=========================================================================**/
#if (OMX_BASE_DEBUG_TRACE_LEVEL >= 5)
#define OMX_BASE_Entering  TIMM_OSAL_Entering
#define OMX_BASE_Exiting   TIMM_OSAL_Exiting
#else
#define OMX_BASE_Entering()
#define OMX_BASE_Exiting(ARG)
#endif

/**=========================================================================**/
/**
 * @def OMX_BASE_ASSERT - Macro to check Parameters. Exit with passed status
 *                      argument if the condition assert fails. Note that
 *                      use of this requires a locally declared variable
 *                      "tError" of OMX_ErrorType and a label named
 *                      "EXIT" typically at the end of the function
 */
/**=========================================================================**/
#define OMX_BASE_ASSERT  OMX_BASE_PARAMCHECK
#define OMX_BASE_REQUIRE OMX_BASE_PARAMCHECK
#define OMX_BASE_ENSURE  OMX_BASE_PARAMCHECK

#define OMX_BASE_PARAMCHECK(C,V)  if (!(C)) { tError = V;\
 TIMM_OSAL_TraceFunction("\n##Error::in %s::line %d \n", __FUNCTION__, __LINE__); \
       goto EXIT; }

/**=========================================================================**/
/**
 * @def OMX_BASE_CHK_VERSION - Checking for version compliance.
 *      If the nSize of the OMX structure is not set, raises bad parameter
 *      error. In case of version mismatch, raises a version mismatch error.
 */
/**=========================================================================**/
#define OMX_BASE_CHK_VERSION(_s_, _name_, _e_)    \
    if(((_name_ *)_s_)->nSize != sizeof(_name_)) _e_ = OMX_ErrorBadParameter; \
    if((((_name_ *)_s_)->nVersion.s.nVersionMajor != 0x1)||   \
       (((_name_ *)_s_)->nVersion.s.nVersionMinor != 0x1)||   \
       (((_name_ *)_s_)->nVersion.s.nRevision != 0x0)||    \
       (((_name_ *)_s_)->nVersion.s.nStep != 0x0)) _e_ = OMX_ErrorVersionMismatch;\
    if(_e_ != OMX_ErrorNone) goto EXIT;

/**=========================================================================**/
/**
 * @def OMX_OSAL_MALLOC_STRUCT - Allocates a memory to a given structure
 */
/**=========================================================================**/
#define OMX_BASE_MALLOC_STRUCT(_pStruct_, _sName_)   \
  _pStruct_ = (_sName_*)TIMM_OSAL_Malloc(sizeof(_sName_)); \
  if(_pStruct_ == NULL) {       \
    OMX_BASE_Error("Memory Allocation Failed");                \
    tError = OMX_ErrorInsufficientResources;   \
   goto EXIT;         \
  }

/**=========================================================================**/
/**
 * @def OMX_CONF_INIT_STRUCT_PTR - Initializes the data structure using a ptr to
 *  structure This initialization sets up the nSize and nVersin fields of the structure
 *=========================================================================**/
#define OMX_BASE_INIT_STRUCT_PTR(_s_, _name_)       \
 memset((_s_), 0x0, sizeof(_name_)); \
    (_s_)->nSize = sizeof(_name_);              \
    (_s_)->nVersion.s.nVersionMajor = 0x1;      \
    (_s_)->nVersion.s.nVersionMinor = 0x1;      \
    (_s_)->nVersion.s.nRevision  = 0x0;       \
    (_s_)->nVersion.s.nStep   = 0x0;

/*******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/**-------------------------------------- data declarations -------------------**/
/**---------------------------------------  function prototypes ---------------**/
/**------------------------------------          macros           -------------**/

#ifdef __cplusplus
 }
#endif /* __cplusplus */

#endif /* _OMX_BASE_UTILS_H_ */



