/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*   @file  timm_osal_memory.c
*   This file contains methods that provides the functionality
*   for allocating/deallocating memory.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *!23-Oct-2008 Maiya ShreeHarsha: Linux specific changes
 *!0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/

#include <string.h>
#include <malloc.h>

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#endif


#include "timm_osal_types.h"
#include "timm_osal_trace.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"



static TIMM_OSAL_BOOL bOSALInitialized = TIMM_OSAL_FALSE;

static TIMM_OSAL_U32 gMallocCounter = 0;

/******************************************************************************
* Function Prototypes
******************************************************************************/

/* ========================================================================== */
/**
* @fn TIMM_OSAL_createMemoryPool function
*
* @see
*/
/* ========================================================================== */
TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreateMemoryPool (void)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_DeleteMemoryPool function
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeleteMemoryPool (void)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    return bReturnStatus;

}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_Malloc function
*
* @see
*/
/* ========================================================================== */
TIMM_OSAL_PTR TIMM_OSAL_Malloc(TIMM_OSAL_U32 size,
                              TIMM_OSAL_BOOL bBlockContiguous,
                              TIMM_OSAL_U32 unBlockAlignment,
                              TIMMOSAL_MEM_SEGMENTID tMemSegId )
{

    TIMM_OSAL_PTR pData = TIMM_OSAL_NULL;

    if(0 != unBlockAlignment) {
		/*TIMM_OSAL_Error("Memory Allocation:Not done for specified nBufferAlignment");*/

    } else  {
  		pData = malloc((size_t)size);	/*size_t is long long*/
		if (TIMM_OSAL_NULL == pData) {
			/*TIMM_OSAL_Error("Malloc failed!!!");*/
		} else {
		   /* Memory Allocation was successfull */
		   gMallocCounter++;
		}
	}

    return pData;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Free function ....
*
* @see
*/
/* ========================================================================== */

void TIMM_OSAL_Free (TIMM_OSAL_PTR pData)
{
    if (TIMM_OSAL_NULL == pData) {
        /*TIMM_OSAL_Warning("TIMM_OSAL_Free called on NULL pointer");*/
        goto EXIT;
    }

    free(pData);
    pData = NULL;
    gMallocCounter--;
EXIT:
    return;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memset function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memset (TIMM_OSAL_PTR pBuffer, TIMM_OSAL_U8 uValue, TIMM_OSAL_U32 uSize)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;

    void * pBufferSet = memset((void*)pBuffer,(int)uValue,(size_t)uSize);
	if(pBufferSet != pBuffer){
		/*TIMM_OSAL_Error("Memset failed!!!");*/
		goto EXIT;
	}

	bReturnStatus = TIMM_OSAL_ERR_NONE;

EXIT:
    return bReturnStatus;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memcmp function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_S32 TIMM_OSAL_Memcmp (TIMM_OSAL_PTR pBuffer1, TIMM_OSAL_PTR pBuffer2, TIMM_OSAL_U32 uSize)
{
    TIMM_OSAL_S32 result = memcmp(pBuffer1, pBuffer2, uSize);

    if (result > 0)  {
       return 1;
    }
    else if (result < 0)  {
        return -1;
    }

    return 0;
}

/* ========================================================================== */
/**
* @fn TIMM_OSAL_Memcpy function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memcpy (TIMM_OSAL_PTR pBufDst, TIMM_OSAL_PTR pBufSrc, TIMM_OSAL_U32 uSize)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;

    void * lpDest = memcpy(pBufDst, pBufSrc, uSize);
    if(lpDest != pBufDst){
		/*TIMM_OSAL_Error("Memcpy failed!!!");*/
		goto EXIT;
	}
	bReturnStatus = TIMM_OSAL_ERR_NONE;
EXIT:
    return bReturnStatus;
}
/* ========================================================================== */
/**
* @fn TIMM_OSAL_GetMemCounter function ....
*
* @see
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_GetMemCounter(void) {

    return gMallocCounter;
}
