/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*   @file  timm_osal_pipes.c
*   This file contains methods that provides the functionality
*   for creating/using Nucleus pipes.
*
*  @path \
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 07-Nov-2008 Maiya ShreeHarsha: Linux specific changes
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

/******************************************************************************
* Includes
******************************************************************************/

#include "timm_osal_types.h"
#include "timm_osal_error.h"
#include "timm_osal_memory.h"
/*#include "timm_osal_trace.h"*/

#include <unistd.h>
#include <stdio.h>
#include <unistd.h>


#define _COMPONENT_DEBUG 
#ifdef _COMPONENT_DEBUG
#define DPRINT printf
#endif


/**
* TIMM_OSAL_PIPE structure define the OSAL pipe
*/
typedef struct TIMM_OSAL_PIPE
{
	int pfd[2];
	TIMM_OSAL_U32  pipeSize;
	TIMM_OSAL_U32  messageSize;
	TIMM_OSAL_U8   isFixedMessage;
    int messageCount;
    int totalBytesInPipe;
} TIMM_OSAL_PIPE;


/******************************************************************************
* Function Prototypes
******************************************************************************/

/* ========================================================================== */
/**
* @fn TIMM_OSAL_CreatePipe function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreatePipe (TIMM_OSAL_PTR *pPipe,
                                          TIMM_OSAL_U32  pipeSize,
                                          TIMM_OSAL_U32  messageSize,
                                          TIMM_OSAL_U8   isFixedMessage)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
    TIMM_OSAL_PIPE *pHandle = TIMM_OSAL_NULL;

    TIMM_OSAL_PIPE *pHandleBackup = TIMM_OSAL_NULL;

    pHandle = (TIMM_OSAL_PIPE *)TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_PIPE), 0, 0, 0);
    TIMM_OSAL_Memset(pHandle, 0x0, sizeof(TIMM_OSAL_PIPE));

    if (TIMM_OSAL_NULL == pHandle) {
        bReturnStatus = TIMM_OSAL_ERR_ALLOC;
        goto EXIT;
    }

	if (SUCCESS != pipe(pHandle->pfd)) {
        /*TIMM_OSAL_Error ("Create_Pipe failed!!!");*/
		goto EXIT;
	}
    
/*AD - This ensures that file descriptors for stdin/out/err are not assigned to 
  component pipes incase those file descriptors are free Normally this if 
  condition will not be true and we'll go directly to the else part*/
    if(pHandle->pfd[0] == 0 || pHandle->pfd[0] == 1 || pHandle->pfd[0] == 2 ||
       pHandle->pfd[1] == 0 || pHandle->pfd[1] == 1 || pHandle->pfd[1] == 2)
    {
        pHandleBackup = (TIMM_OSAL_PIPE *)TIMM_OSAL_Malloc(sizeof(TIMM_OSAL_PIPE), 0, 0, 0);
        TIMM_OSAL_Memset(pHandleBackup, 0x0, sizeof(TIMM_OSAL_PIPE));
        if (TIMM_OSAL_NULL == pHandleBackup) 
        {
            bReturnStatus = TIMM_OSAL_ERR_ALLOC;
            goto EXIT;
        }
/*Allocating the new pipe*/
        if (SUCCESS != pipe(pHandleBackup->pfd)) 
        {
            goto EXIT;
	    }
/*Really crazy case if all 3 reserved file descriptors have been freed up!!
  Should never happen normally*/
	    if(pHandleBackup->pfd[0] == 2 || pHandleBackup->pfd[1] == 2)
	    {
            int pfdDummy[2];
            
	        if (SUCCESS != close(pHandleBackup->pfd[0])) 
            {
	           goto EXIT;
	        }
            if (SUCCESS != close(pHandleBackup->pfd[1])) 
            {
	            goto EXIT;
   	        }
   	        /*Allocating the reserved file descriptor to dummy*/
   	        if(SUCCESS != pipe(pfdDummy))
            {
                goto EXIT;
            }
            /*Now the backup pfd will not get a reserved value*/
            if (SUCCESS != pipe(pHandleBackup->pfd)) 
            {
                goto EXIT;
	        }
	        /*Closing the dummy pfd*/
	        if (SUCCESS != close(pfdDummy[0])) 
            {
	           goto EXIT;
	        }
            if (SUCCESS != close(pfdDummy[1])) 
            {
	            goto EXIT;
   	        }
   	        
        }
/*Closing the previous pipe*/
	    if (SUCCESS != close(pHandle->pfd[0])) 
        {
	        goto EXIT;
	    }
        if (SUCCESS != close(pHandle->pfd[1])) 
        {
	        goto EXIT;
   	    }
   	    TIMM_OSAL_Free(pHandle);
   	    
	    pHandleBackup->pipeSize = pipeSize;
        pHandleBackup->messageSize = messageSize;
	    pHandleBackup->isFixedMessage = isFixedMessage;
        pHandleBackup->messageCount = 0;
        pHandleBackup->totalBytesInPipe = 0; 
        
        *pPipe = (TIMM_OSAL_PTR ) pHandleBackup ;
    }
/*This is the normal case when a reserved file descriptor is not assigned to our pipe*/
    else
    {        
        pHandle->pipeSize = pipeSize;
        pHandle->messageSize = messageSize;
	    pHandle->isFixedMessage = isFixedMessage;
        pHandle->messageCount = 0;
        pHandle->totalBytesInPipe = 0; 

        *pPipe = (TIMM_OSAL_PTR ) pHandle ;
    }

	bReturnStatus = TIMM_OSAL_ERR_NONE;

EXIT:
    if ((TIMM_OSAL_ERR_NONE != bReturnStatus) && (TIMM_OSAL_NULL != pHandle)) {
       TIMM_OSAL_Free(pHandle);
    }
    
    if ((TIMM_OSAL_ERR_NONE != bReturnStatus) && (TIMM_OSAL_NULL != pHandleBackup)) {
       TIMM_OSAL_Free(pHandleBackup);
    }
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_DeletePipe function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeletePipe (TIMM_OSAL_PTR pPipe)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;

    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

	if(TIMM_OSAL_NULL == pHandle) {
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

	if (SUCCESS != close(pHandle->pfd[0])) {
    	/*TIMM_OSAL_Error ("Delete_Pipe Read fd failed!!!");*/
	    bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
	}
    if (SUCCESS != close(pHandle->pfd[1])) {
        /*TIMM_OSAL_Error ("Delete_Pipe Write fd failed!!!");*/
	    bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
   	}

    TIMM_OSAL_Free(pHandle);
EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_WriteToPipe function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_WriteToPipe (TIMM_OSAL_PTR pPipe,
                                           void *pMessage,
                                           TIMM_OSAL_U32 size,
                                           TIMM_OSAL_S32 timeout)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
    TIMM_OSAL_U32 lSizeWritten = -1;

    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

	if(size == 0) {
		/*TIMM_OSAL_Error("Nothing to write");*/
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
	
    lSizeWritten = write(pHandle->pfd[1], pMessage, size);

	if(lSizeWritten != size){
		/*TIMM_OSAL_Error("Writing to Pipe failed");*/
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    /*Update message count and size*/
    pHandle->messageCount++;
    pHandle->totalBytesInPipe += size;

	bReturnStatus = TIMM_OSAL_ERR_NONE;

EXIT:
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_WriteToFrontOfPipe function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_WriteToFrontOfPipe (TIMM_OSAL_PTR pPipe,
                                                  void *pMessage,
                                                  TIMM_OSAL_U32 size,
                                                  TIMM_OSAL_S32 timeout)
{

    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
    TIMM_OSAL_U32 lSizeWritten = -1;
    TIMM_OSAL_U32 lSizeRead = -1;
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;
    TIMM_OSAL_U8 * tempPtr;

       
    /*First write to this pipe*/
    if(size == 0) {
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}
	
    lSizeWritten = write(pHandle->pfd[1], pMessage, size);

	if(lSizeWritten != size){
		bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		goto EXIT;
	}

    /*Update number of messages*/
    pHandle->messageCount++;

    
    if(pHandle->messageCount > 1) {
        /*First allocate memory*/
        tempPtr = (TIMM_OSAL_U8 *)TIMM_OSAL_Malloc(pHandle->totalBytesInPipe, 0, 0, 0);
        
        if(tempPtr == NULL) {
            bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		    goto EXIT;
        }

        /*Read out of pipe*/
        lSizeRead = read(pHandle->pfd[0], tempPtr, pHandle->totalBytesInPipe);

        /*Write back to pipe*/
        lSizeWritten = write(pHandle->pfd[1], tempPtr, pHandle->totalBytesInPipe);

	    if(lSizeWritten != size){
		    bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
		    goto EXIT;
	    }

        /*Update Total bytes in pipe*/
        pHandle->totalBytesInPipe += size;
    }

     
EXIT:
    TIMM_OSAL_Free(tempPtr);
    
   	return bReturnStatus;

}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_ReadFromPipe function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_ReadFromPipe (TIMM_OSAL_PTR pPipe,
                                            void *pMessage,
                                            TIMM_OSAL_U32 size,
                                            TIMM_OSAL_U32 *actualSize,
                                            TIMM_OSAL_S32 timeout)
{
		TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_UNKNOWN;
		TIMM_OSAL_U32 lSizeRead = -1;
		TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

		if((size == 0) || (pHandle->messageCount == 0)) {			/*|| size > SSIZE_MAX)*/
			/*TIMM_OSAL_Error("Read size has error.");*/
			bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
			goto EXIT;
		}

		*actualSize =  lSizeRead = read(pHandle->pfd[0], pMessage, size);
		if(0 == lSizeRead){
			/*TIMM_OSAL_Error("EOF reached or no data in pipe");*/
			bReturnStatus = TIMM_OSAL_ERR_PARAMETER;
			goto EXIT;
		}

		bReturnStatus = TIMM_OSAL_ERR_NONE;
        
        /*Update message count and pipe size*/
        pHandle->messageCount--;
        pHandle->totalBytesInPipe -= size;

	EXIT:
		return bReturnStatus;

}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_ClearPipe function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_ClearPipe (TIMM_OSAL_PTR pPipe)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR;

#if 0
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    STATUS status = NU_SUCCESS;

    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

    status = NU_Reset_Pipe (&(pHandle->pipe));

    if (NU_SUCCESS != status) {
        /*TIMM_OSAL_Error ("NU_Reset_Pipe failed!!!");*/
        bReturnStatus = TIMM_OSAL_ERR_CREATE(TIMM_OSAL_ERR, TIMM_OSAL_COMP_PIPES, status);
    }
#endif
    return bReturnStatus;
}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_IsPipeReady function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_IsPipeReady (TIMM_OSAL_PTR pPipe)
{
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR;
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;

#if 0
    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;
    PI_PCB *pipe = (PI_PCB *)&(pHandle->pipe);

    if (0 != pipe->pi_messages) {
        return TIMM_OSAL_ERR_NONE;
    } else {
        return TIMM_OSAL_ERR_NOT_READY;
    }
#endif
    
    if(pHandle->messageCount <= 0) {
        bReturnStatus = TIMM_OSAL_ERR_NOT_READY;
    } else {
        bReturnStatus = TIMM_OSAL_ERR_NONE;
    }

	return bReturnStatus;

}



/* ========================================================================== */
/**
* @fn TIMM_OSAL_GetPipeReadyMessageCount function
*
*
*/
/* ========================================================================== */

TIMM_OSAL_ERRORTYPE TIMM_OSAL_GetPipeReadyMessageCount (TIMM_OSAL_PTR pPipe,
                                                        TIMM_OSAL_U32 *count)
{
		TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR;
        TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;
#if 0

    TIMM_OSAL_PIPE *pHandle = (TIMM_OSAL_PIPE *)pPipe;
    PI_PCB *pipe = (PI_PCB *)&(pHandle->pipe);

    *count = pipe->pi_messages;

#endif

    *count = pHandle->messageCount;
    return bReturnStatus;

}
