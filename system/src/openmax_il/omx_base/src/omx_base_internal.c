/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*  @file  omx_base_internal.c
*  This file contains the menthods for the functionality of Base comp.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_base\src
*
*  @rev  1.0
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *!19-Aug-2008   Rabbani Patan rabbani@ti.com: Active context support
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ========================================================================= */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
/*#include <nucleus.h>*/
#include <string.h>

/*-------program files ----------------------------------------*/
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <omx_base.h>
#include <omx_base_internal.h>
#include <timm_osal_interfaces.h>
#include "omx_base_utils.h"

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/**--------data declarations -----------------------------------**/
/**--------function prototypes --------------------------------**/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/**--------data declarations -----------------------------------**/
/**--------function prototypes --------------------------------**/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/**--------macros ------------------------------------------------**/
/**--------data declarations -----------------------------------**/
/**--------function prototypes ---------------------------------**/

/**=========================================================================**/
/*  P R I V A T E   F U N C T I O N S                                       */
/**=========================================================================**/

/**=========================================================================**/
/**
 * @fn _OMX_BASE_InitializePorts()
 *
 * @ see omx_base_internal.h
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_InitializePorts(OMX_HANDLETYPE hComponent)
{
   OMX_COMPONENTTYPE* pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_U32 nIndex = 0;

   OMX_BASE_Entering();

   /* Check the input parameters */
   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* Initialize memory for the given no of ports */
   pBaseComInt->ports = (OMX_BASE_PORTTYPE**)TIMM_OSAL_Malloc(sizeof(OMX_BASE_PORTTYPE*) *
                                               pBaseComPvt->tDerToBase.tPortParams.nPorts);
   OMX_BASE_ASSERT(pBaseComInt->ports != NULL, OMX_ErrorInsufficientResources);

   pBaseComPvt->portdefs = (OMX_PARAM_PORTDEFINITIONTYPE **)TIMM_OSAL_Malloc(
                                          sizeof(OMX_PARAM_PORTDEFINITIONTYPE*) *
                                          pBaseComPvt->tDerToBase.tPortParams.nPorts);
   OMX_BASE_ASSERT(pBaseComPvt->portdefs != NULL, OMX_ErrorInsufficientResources);

   for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
       nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                  pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {

       pBaseComInt->ports[nIndex] = (OMX_BASE_PORTTYPE*)TIMM_OSAL_Malloc(
                                                  sizeof(OMX_BASE_PORTTYPE));
       OMX_BASE_ASSERT(pBaseComInt->ports[nIndex] != NULL, OMX_ErrorInsufficientResources);
       /* Clear the Base port type structure */
       TIMM_OSAL_Memset(pBaseComInt->ports[nIndex], 0x0, sizeof(OMX_BASE_PORTTYPE));

       /* Initialize an port with OMX specification version info */
       OMX_BASE_INIT_STRUCT_PTR(&(pBaseComInt->ports[nIndex]->tPortDefParams),
                                            OMX_PARAM_PORTDEFINITIONTYPE);
       /* Assigning the PortDef params of an BASE_PORTTYPE to an Private area,
        * which can be accessed by the dervied comp's  */
       pBaseComPvt->portdefs[nIndex] = &(pBaseComInt->ports[nIndex]->tPortDefParams);
       /* Incase of Active context, Requrie a semaphore to lock the
        * execution untill all the buffers are allocated/released  */
       if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
           tStatus = TIMM_OSAL_SemaphoreCreate(&(pBaseComInt->ports[nIndex]->pBufAllocSem), 0);
           OMX_BASE_ASSERT(TIMM_OSAL_ERR_NONE == tStatus, OMX_ErrorInsufficientResources);
       }
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_DeinitalizePorts()
 *
 * @ see omx_base_internal.h
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_DeinitalizePorts(OMX_HANDLETYPE hComponent)
{
   OMX_COMPONENTTYPE * pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt ;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_U32 nIndex = 0;

   OMX_BASE_Entering();

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* Freeup resources Allocated for each port */
   for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
       nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                  pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {

       /* Incase if the comp is in Active context, delete the semaphore
        * used to lokc the exeuction of the buffer allocation */
       if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
           tStatus = TIMM_OSAL_SemaphoreDelete(pBaseComInt->ports[nIndex]->pBufAllocSem);
           OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);
        }

       if(pBaseComInt->ports[nIndex] != NULL) {
          TIMM_OSAL_Free(pBaseComInt->ports[nIndex]);
       }
   }

   if(pBaseComPvt->portdefs != NULL) {
      TIMM_OSAL_Free(pBaseComPvt->portdefs);
   }

   if(pBaseComInt->ports != NULL) {
     TIMM_OSAL_Free(pBaseComInt->ports);
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}


/**=========================================================================**/
/**
 * @fn _OMX_BASE_EnablePort()
 *
 * @ see omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_EnablePort(OMX_HANDLETYPE hComponent, OMX_U32 nParam)
{
   OMX_COMPONENTTYPE* pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();
   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nParam];
   pPort->tPortDefParams.bEnabled     = OMX_TRUE;

   /* If port is enabled when the comp is not in loaded/wait for resources,
    * then port shall allocate its buffers via the same call */
   if((pBaseComPvt->tCurState != OMX_StateLoaded) &&
      (pBaseComPvt->tNewState != OMX_StateWaitForResources)) {

        if(!PORT_IS_BUFFER_SUPPLIER(pPort)) {
           /* If port is not a buffer suplier and comp is running in Active context,
            * wait on a semaphore till the allocation of buffers happen */
           if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
              TIMM_OSAL_SemaphoreObtain(pPort->pBufAllocSem, TIMM_OSAL_SUSPEND);
           }
        }
       /* Port is Tunneled and buffer suplier, Allocate the tunnel bufs over here */
       else {
          tError = _OMX_BASE_AllocateTunnelBuffer(hComponent, nParam,
                                     pPort->tPortDefParams.nBufferSize);
        }
   } else {
       TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
       pPort->bIsTransientToEnabled = OMX_FALSE;
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_DisablePort()
 *
 * @ see omx_base_internal.h
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_DisablePort(OMX_HANDLETYPE hComponent, OMX_U32 nParam)
{
   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);
   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nParam];
   pPort->tPortDefParams.bEnabled = OMX_FALSE;

   /* If the comp is in loaded state, then there wont be any buffers to freeup,
    * set the transientToDisbled flag to be FALSE*/
   if(pBaseComPvt->tCurState == OMX_StateLoaded) {
       pPort->bIsTransientToDisabled = OMX_FALSE;
   }
   else {
       if(!PORT_IS_BUFFER_SUPPLIER(pPort)) {
           /* in case of Active context,wait on a semaphore
            * till the buffer get released */
           if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
              TIMM_OSAL_SemaphoreObtain(pPort->pBufAllocSem, TIMM_OSAL_SUSPEND);
            }

           /* Incase if all the buffers of an port are freed and port is Unpopulated,
            * generate an EventCmdComplete event indicating OMX_CommandPortDisable */
           if((pPort->unNumAssignedBufs == 0) &&
              (pPort->tPortDefParams.bPopulated == OMX_FALSE)) {

               TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
               pPort->bIsTransientToDisabled = OMX_FALSE;
               TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

			   /* Freeup the resources allocated for an port */
               //_OMX_BASE_DeAllocateResources(hComponent, nParam);

               pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
                                          OMX_EventCmdComplete, OMX_CommandPortDisable,
                                          nParam, "disabling a port ");
              
            }

       }
       /* Here the port is Tunneled and buffer supplier,since port is
        * getting diabled, wait for Non-Supplier to return all the buffers */
        else {
           tError = _OMX_BASE_FreeTunnelBuffer(hComponent, nParam);
        }
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleEventNotify1()
 *
 * @see omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleEventNotify1(OMX_HANDLETYPE hComponent,
                                           OMX_U32 nPortIndex)
{
   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE* pPort;
   OMX_U32 nIndex = 0;
   OMX_U32 numBuffers = 0;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_BASE_Entering();

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];

   /* Set populated field to TRUE, if all the buffers are allocated */
   if(pPort->tPortDefParams.nBufferCountActual == pPort->unNumAssignedBufs) {

       pPort->tPortDefParams.bPopulated = OMX_TRUE;

      /* If the comp is runing remotely, transition frm loaded to idle
       * will not happen until the semaphore is released */
        if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
            tStatus = TIMM_OSAL_SemaphoreRelease(pPort->pBufAllocSem);
            OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);
        }
        /* Means the comp is in Passive context */
        else {
           /* Findout the number of buffers assigned (for all ports) */
           for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
              nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                        pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {
               numBuffers += pBaseComInt->ports[nIndex]->unNumAssignedBufs;
           }

           if(pPort->bIsTransientToEnabled) {
               TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
               pPort->bIsTransientToEnabled = OMX_FALSE;
               TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
               pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
                                       OMX_EventCmdComplete, OMX_CommandPortEnable,
                                               nPortIndex, "Enabling an Port");

            }
           /* if all the ports are populated, return Loaded to Idle state transition
            * complete notification to the client */
           if((pBaseComPvt->tCurState == OMX_StateLoaded) &&
              (pBaseComPvt->tNewState == OMX_StateIdle)) {
                 if(numBuffers == pBaseComInt->nNumBuffers) {
                    TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
                    pBaseComPvt->tCurState = pBaseComPvt->tNewState;
                    pBaseComPvt->tNewState = OMX_StateMax;
                    TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

                    pBaseComInt->tCbInfo.EventHandler(hComponent,
                                pComp->pApplicationPrivate, OMX_EventCmdComplete,
                                OMX_CommandStateSet, OMX_StateIdle, "Loaded->Idle");
                 }
           }
        }
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleEventNotify2()
 *
 * @ see omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleEventNotify2(OMX_HANDLETYPE hComponent,
                                           OMX_U32 nPortIndex)
{
   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE* pPort;
   OMX_U32 nIndex = 0;
   OMX_U32 i = 0;
   OMX_U32 numBuffers = 0;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;

   OMX_BASE_Entering();

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];
   /* set the bPopulated field to False if all the buffers of an port are freed */
   if(pPort->unNumAssignedBufs == 0) {
       pPort->tPortDefParams.bPopulated = OMX_FALSE;

       /* Freeup the resources as the port is unpopulated */
       _OMX_BASE_DeAllocateResources(hComponent, nPortIndex);

       if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
          tStatus = TIMM_OSAL_SemaphoreRelease(pPort->pBufAllocSem);
          OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);
       }
       else {
           /* Find out the total number of buffers assigned for a component(incl all ports) */
           for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
               nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                         pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {
               numBuffers += pBaseComInt->ports[nIndex]->unNumAssignedBufs;
           }

           if(pPort->bIsTransientToDisabled) {

              TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
              pPort->bIsTransientToDisabled = OMX_FALSE;
              TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

              pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
                                    OMX_EventCmdComplete, OMX_CommandPortDisable,
                                       nPortIndex, " disabling an port ");

           }

           /* All the ports are unpopulated , return Idle to loaded state
            * transition complete notification to the Client */
           if((pBaseComPvt->tCurState == OMX_StateIdle) &&
              (pBaseComPvt->tNewState == OMX_StateLoaded)) {
                if(numBuffers == 0) {
                   TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
                   pBaseComPvt->tCurState = pBaseComPvt->tNewState;
                   pBaseComPvt->tNewState = OMX_StateMax;
                   TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
                   for(i=0; i<pBaseComPvt->tDerToBase.tPortParams.nPorts; i++){

					   pPort = pBaseComInt->ports[i];
					   TIMM_OSAL_Free(pPort->pBufStateAllocated);
					   TIMM_OSAL_Free(pPort->pBufferlist);
                   }

                   pBaseComInt->nNumBuffers = 0;
                   pBaseComInt->tCbInfo.EventHandler(hComponent,
                                     pComp->pApplicationPrivate, OMX_EventCmdComplete,
                                     OMX_CommandStateSet, OMX_StateLoaded, NULL);
                }
           }
       }
   }

EXIT:
  OMX_BASE_Exiting(tError);
  return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleEventNotify3()
 *
 * @ see omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleEventNotify3(OMX_HANDLETYPE hComponent,
                                        OMX_BASE_CMDPARAMS *cmdEvent)
{

   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_U32 j=0;
   OMX_U32 ndisports = 0;
   OMX_U32 nIndex = 0;
   OMX_U32 numBuffers = 0;
   OMX_BUFFERHEADERTYPE *pBuffer = NULL;
   OMX_U32 actualSize = 0;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();
   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   switch(cmdEvent->tCmdType)
   {
       case OMX_CommandStateSet:

            if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
                if(pBaseComPvt->tCurState == OMX_StateIdle && 
                   pBaseComPvt->tNewState == OMX_StateLoaded)
                {
                    for(j=0; j<pBaseComPvt->tDerToBase.tPortParams.nPorts; j++)
                    {
					   pPort = pBaseComInt->ports[j];
					   TIMM_OSAL_Free(pPort->pBufStateAllocated);
					   TIMM_OSAL_Free(pPort->pBufferlist);
                    }
                }
			 	pBaseComPvt->tCurState = pBaseComPvt->tNewState;
				pBaseComPvt->tNewState = OMX_StateMax;
                pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
                                          OMX_EventCmdComplete, cmdEvent->tCmdType,
                                          cmdEvent->unParam, NULL);
             } else {

				TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
				/* Loaded to Idle and Idle to Loaded state transition needs
				 * allocation/deallocation buffers for all enabled ports */
				if((pBaseComPvt->tCurState == OMX_StateLoaded && pBaseComPvt->tNewState == OMX_StateIdle) ||
				   (pBaseComPvt->tNewState == OMX_StateLoaded && pBaseComPvt->tCurState == OMX_StateIdle)) {
					TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
					for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
						nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
								pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {
						if(!(pBaseComInt->ports[nIndex]->tPortDefParams.bEnabled)) ndisports++;
						if((pBaseComPvt->tCurState == OMX_StateLoaded &&
							pBaseComPvt->tNewState == OMX_StateIdle))
							numBuffers += pBaseComInt->ports[nIndex]->unNumAssignedBufs;
					}

				  /* Incase All the ports are disabled or all buffers are allocated
				   * for all enabled ports,then the loaded to idle
				   * state transition can be completed over here */
				   if((ndisports == pBaseComPvt->tDerToBase.tPortParams.nPorts) ||
					  (pBaseComInt->nNumBuffers == numBuffers)) {
                      if(pBaseComPvt->tCurState == OMX_StateIdle && 
                         pBaseComPvt->tNewState == OMX_StateLoaded)
                      {
                          for(j=0; j<pBaseComPvt->tDerToBase.tPortParams.nPorts; j++)
                          {
					          pPort = pBaseComInt->ports[j];
					          TIMM_OSAL_Free(pPort->pBufStateAllocated);
					          TIMM_OSAL_Free(pPort->pBufferlist);
                          }
                      }
					  pBaseComPvt->tCurState = pBaseComPvt->tNewState;
					  pBaseComPvt->tNewState = OMX_StateMax;

					  pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
											  OMX_EventCmdComplete, cmdEvent->tCmdType,
											  cmdEvent->unParam, NULL);
				   }
				}else {
				   pBaseComPvt->tCurState = pBaseComPvt->tNewState;
				   pBaseComPvt->tNewState = OMX_StateMax;
				   TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

				   pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
												OMX_EventCmdComplete, cmdEvent->tCmdType,
											   cmdEvent->unParam, NULL);
				}

      		}
			 if((pBaseComPvt->tCurState == OMX_StateIdle) && (pBaseComPvt->tNewState == OMX_StateExecuting)) {
                   for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                       nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
					   	        pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {

                       pPort = pBaseComInt->ports[nIndex];

                       if(PORT_IS_TUNNELED_N_BUFFER_SUPPLIER(pPort)) {
                           for(j=0; j < pPort->unTunnelBufs; j++) {

                             tStatus = TIMM_OSAL_ReadFromPipe(pPort->dataPipe, &pBuffer,
                                       sizeof(pBuffer), &actualSize, TIMM_OSAL_SUSPEND);
                             OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

                             tError = pBaseComPvt->tDerToBase.fpDataNotify(hComponent, nIndex, pBuffer);
                             OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);

                           }
                      }
               		}
			}
       break;

       case OMX_CommandPortEnable:
       case OMX_CommandPortDisable:
       case OMX_CommandFlush:
	   	    if(cmdEvent->unParam == OMX_ALL) {
               for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                   nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                            pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {

				   if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
                      pBaseComInt->tCbInfo.EventHandler(hComponent,
                                      pComp->pApplicationPrivate,OMX_EventCmdComplete,
                                      cmdEvent->tCmdType, nIndex , NULL);
				   	} else {
                        if((pBaseComPvt->tCurState == OMX_StateLoaded)||
							(cmdEvent->tCmdType == OMX_CommandFlush)) {
							pBaseComInt->tCbInfo.EventHandler(hComponent,
                                    pComp->pApplicationPrivate,OMX_EventCmdComplete,
                                    cmdEvent->tCmdType, nIndex , NULL);
                        }
				   	}
               }

	   	    } else {
	   	       if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {
                    pBaseComInt->tCbInfo.EventHandler(hComponent,
                                    pComp->pApplicationPrivate, OMX_EventCmdComplete,
                                    cmdEvent->tCmdType, cmdEvent->unParam , NULL);
	   	       } else {

                    if((pBaseComPvt->tCurState == OMX_StateLoaded) ||
                       (cmdEvent->tCmdType == OMX_CommandFlush)) {
                        pBaseComInt->tCbInfo.EventHandler(hComponent,
                                    pComp->pApplicationPrivate, OMX_EventCmdComplete,
                                    cmdEvent->tCmdType, cmdEvent->unParam , NULL);
	   	            }
	   	       	}
	   	    }
       break;

       default:
            OMX_BASE_Error(" Un-supported command type ");
       break;
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleStateTransition()
 *
 * @ see omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleStateTransition(OMX_HANDLETYPE hComponent,
                                              OMX_BASE_CMDPARAMS *cmdEvent)
{

   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE* pPort;
   OMX_U32 nIndex , j;
   OMX_U32 retEvent = 0;
   OMX_BUFFERHEADERTYPE *pBuffer = NULL;
   OMX_U32 elementsInPipe = 0;
   OMX_U32 actualSize = 0;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;

   OMX_BASE_Entering();

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* transition from or to the OMX_StateInvalid state is not recommended  */
   if((pBaseComPvt->tCurState == OMX_StateInvalid) ||
      (pBaseComPvt->tNewState == OMX_StateInvalid)) {
       OMX_BASE_Trace (" Invalid state transition ");
       /* Notify State transition to the Derived Component */
       tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent,
                     cmdEvent->tCmdType, cmdEvent->unParam, cmdEvent->pCmdData);
       if(tError == OMX_ErrorNone) {
          TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                       TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);

       }

       if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
       OMX_BASE_ASSERT(0, OMX_ErrorUndefined);
       pBaseComPvt->tCurState = pBaseComPvt->tNewState;
      return OMX_ErrorInvalidState;
    }

   OMX_BASE_ASSERT((pBaseComPvt->tCurState != pBaseComPvt->tNewState), OMX_ErrorSameState);

   /* Switching the Comp from the Current State to the New State */
   switch(pBaseComPvt->tNewState)
   {
       /* Valid State Transitions from AnyState to OMX_StateLoaded are :
        * 1.OMX_StateIdle             ----> OMX_StateLoaded
        * 2.OMX_StateWaitForResources ----> OMX_StateLoaded */
       case OMX_StateLoaded:
            /* Transition from OMX_StateIdle ---> OMX_StateLoaded */
            if(pBaseComPvt->tCurState == OMX_StateIdle) {
                /* Notify State transition to the Derived Component */
                tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent, cmdEvent->tCmdType,
                                                         cmdEvent->unParam, cmdEvent->pCmdData);
                                                         
                if(tError == OMX_ErrorNone) {
                   TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                                       TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);

                }
                if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                       OMX_BASE_ASSERT(0, OMX_ErrorUndefined);

                for (nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                     nIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nIndex++) {
                     pPort = pBaseComInt->ports[nIndex];

                  /* freeup Tuunnel buffers if the port is buffer supplier */
                  if(PORT_IS_TUNNELED_N_BUFFER_SUPPLIER(pPort) &&
                      (pPort->tPortDefParams.bEnabled == OMX_TRUE)) {
                         tError = _OMX_BASE_FreeTunnelBuffer(hComponent,nIndex);
                         OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);

                  }
                  else {
                       /* If comp runs in Active Context,
                        * wait until all the buffers are de-allocated  */
                       if((pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) &&
					   	  (pPort->tPortDefParams.bEnabled == OMX_TRUE))
                          TIMM_OSAL_SemaphoreObtain(pPort->pBufAllocSem, TIMM_OSAL_SUSPEND);
                  }
               }
            }
            else if(pBaseComPvt->tCurState != OMX_StateWaitForResources)
                OMX_BASE_ASSERT(OMX_FALSE, OMX_ErrorIncorrectStateTransition);
       break;

       /* Valid State Transitions from anyState to OMX_StateIdle are
        * 1.OMX_StateLoaded                  ----> OMX_StateIdle
        * 2.OMX_StatePause/OMX_StateExecuting----> OMX_StateIdle
        * 3.OMX_StateWaitForResources        ----> OMX_StateIdle */
       case OMX_StateIdle:
            /* Transition from OMX_StateLoaded -> OMX_StateIdle */
            if(pBaseComPvt->tCurState == OMX_StateLoaded) {
                for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                    nIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nIndex++) {
                    pPort = pBaseComInt->ports[nIndex];

                    pBaseComInt->nNumBuffers += pPort->tPortDefParams.nBufferCountActual;

                  /* If the port is Tunneeld and Buffer supplier,
                   * allocate the buffers  of this port */
                  if(PORT_IS_TUNNELED_N_BUFFER_SUPPLIER(pPort)) {
                      tError = _OMX_BASE_AllocateTunnelBuffer(hComponent,
                                            nIndex, pPort->tPortDefParams.nBufferSize);
                      OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
                  }
                  else {
                     /* If port is not a supplier and the comp runs remotely,
                      * then wait until the buffers are allocated */
                     if((pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) &&
					 	(pPort->tPortDefParams.bEnabled == OMX_TRUE))
                         TIMM_OSAL_SemaphoreObtain(pPort->pBufAllocSem, TIMM_OSAL_SUSPEND);
                  }
                }

               /* Notify This State transition to the Derived Component */
                tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent, cmdEvent->tCmdType,
                                                         cmdEvent->unParam, cmdEvent->pCmdData);
                if(tError == OMX_ErrorNone) {
                   TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                                       TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);
                }
                if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                       OMX_BASE_ASSERT(0, OMX_ErrorUndefined);

            }
            /* Transition from OMX_StateExecuting/OMX_Pause -> OMX_StateIdle */
            else if((pBaseComPvt->tCurState == OMX_StateExecuting) ||
                    (pBaseComPvt->tCurState == OMX_StatePause )) {
                /* Notify State transition to the Derived Component */
                tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent, cmdEvent->tCmdType,
                                                         cmdEvent->unParam, cmdEvent->pCmdData);
                if(tError == OMX_ErrorNone) {
                   TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                                       TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);
                }
                if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                       OMX_BASE_ASSERT(0, OMX_ErrorUndefined);

                for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                    nIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nIndex++) {
                    pPort = pBaseComInt->ports[nIndex];
                   /* Since the Comp is transition to Idle from Exeucting,flush all the
                    * buffers to the IL Client incase of non-tunneling or return buffers
                    * to the corresponding component incase of tunneling */
                    tError = _OMX_BASE_FlushBuffers(hComponent, nIndex);
                    OMX_BASE_ASSERT(tError == OMX_ErrorNone, tError);
                }
            }
            else if(pBaseComPvt->tCurState != OMX_StateWaitForResources)
                OMX_BASE_ASSERT(0, OMX_ErrorIncorrectStateTransition);

       break;

       /* Valid State transition from Any state to OMX_StateExecuting are
        * 1.OMX_StateIdle----->   OMX_StatExecuting
        * 2.OMX_Pause    ----->   OMX_StateExecuting */
       case OMX_StateExecuting:

            /* Transition from OMX_StateIdle--> OMX_StateExecuting */
            if(pBaseComPvt->tCurState == OMX_StateIdle) {
               /* Nothing to do over here */
            }
            /* Transition from OMX_Pause to OMX_Executing */
            else if(pBaseComPvt->tCurState == OMX_StatePause) {

                for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                     nIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nIndex++) {
                     pPort = pBaseComInt->ports[nIndex];
                     TIMM_OSAL_GetPipeReadyMessageCount(pPort->dataPipe,&elementsInPipe);
                     while(elementsInPipe) {
                         TIMM_OSAL_ReadFromPipe(pPort->dataPipe, &pBuffer,
                                               sizeof(pBuffer), &actualSize, TIMM_OSAL_SUSPEND);
                         OMX_BASE_ASSERT(pBuffer != NULL, OMX_ErrorInsufficientResources);

                         tError = pBaseComPvt->tDerToBase.fpDataNotify(hComponent, nIndex, pBuffer);
                         OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);

                         elementsInPipe--;
                     }
                 }
            }
            else
               OMX_BASE_ASSERT(OMX_FALSE, OMX_ErrorIncorrectStateTransition);

            /* Notify the state transition to the Derived Component */
            tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent, cmdEvent->tCmdType,
                                                      cmdEvent->unParam, cmdEvent->pCmdData);
            if(tError == OMX_ErrorNone) {
                TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                             TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);
            }
            if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                       OMX_BASE_ASSERT(0, OMX_ErrorUndefined);
       break;

       /* Valid Transitions from any state  to OMX_StatePause are
        * 1.OMX_StateIdle     ---->  OMX_StatePause
        * 2.OMX_StateExeucting---->  OMX_StatePause */
       case OMX_StatePause:
            if((pBaseComPvt->tCurState != OMX_StateExecuting) &&
                (pBaseComPvt->tCurState != OMX_StateIdle))
              OMX_BASE_ASSERT(OMX_FALSE, OMX_ErrorIncorrectStateTransition);

            /* Notify the state transition to the Derived Component */
            tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent, cmdEvent->tCmdType,
                                                         cmdEvent->unParam, cmdEvent->pCmdData);
            if(tError == OMX_ErrorNone) {
               TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                               TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);
            }
            
            if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                   OMX_BASE_ASSERT(0, OMX_ErrorUndefined);
       break;

       /* Valid only for  1. OMX_StateLoaded ---> OMX_StateWaitForResources */
       case OMX_StateWaitForResources:
            if(pBaseComPvt->tCurState != OMX_StateLoaded)
               OMX_BASE_ASSERT(OMX_FALSE, OMX_ErrorIncorrectStateTransition);

            /* Notify the state transition to the Derived Component */
            tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent, cmdEvent->tCmdType,
                                                         cmdEvent->unParam, cmdEvent->pCmdData);
            if(tError == OMX_ErrorNone) {
               TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                                       TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);
            }
            if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                   OMX_BASE_ASSERT(0, OMX_ErrorUndefined);
       break;

      default:
          OMX_BASE_Error(" Un supported command type ");
       break;
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_InitializeBuffer()
 *
 * @  see omc_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_InitializeBuffer(OMX_HANDLETYPE hComponent,
                                         OMX_BUFFERHEADERTYPE* pBufHeader,
                                         OMX_U32 nPortIndex,
                                         OMX_PTR pAppPrivate,
                                         OMX_U32 nSizeBytes)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE* pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];

   /* Initiailzes a Buffer with given parameters/Defaults */
   pBufHeader->nFilledLen                  = 0;
   pBufHeader->nOffset                     = 0;
   pBufHeader->nTickCount                  = 0;
   pBufHeader->nTimeStamp                  = 0;
   pBufHeader->nFlags                      = 0;
   pBufHeader->pInputPortPrivate           = NULL;
   pBufHeader->pOutputPortPrivate          = NULL;
   pBufHeader->hMarkTargetComponent        = NULL;
   pBufHeader->pMarkData                   = NULL;
   pBufHeader->nAllocLen                   = nSizeBytes;
   pBufHeader->pAppPrivate                 = pAppPrivate;
   pBufHeader->pPlatformPrivate            = pPort;

   /*Assign the portindex depending on the direction of port */
   if (pPort->tPortDefParams.eDir == OMX_DirInput) {
        pBufHeader->nInputPortIndex = pPort->tPortDefParams.nPortIndex;
        pBufHeader->nOutputPortIndex = OMX_NOPORT;
    } else {
        pBufHeader->nOutputPortIndex = pPort->tPortDefParams.nPortIndex;
        pBufHeader->nInputPortIndex = OMX_NOPORT;
    }

   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_AllocateTunnelBuffer()
 *
 *  @  see omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_AllocateTunnelBuffer(OMX_HANDLETYPE hComponent,
                                             OMX_U32 nPortIndex,
                                             OMX_U32 nSizeBytes)
{
   OMX_COMPONENTTYPE* pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_U8* pBuffer = NULL;
   OMX_BASE_PORTTYPE *pPort;
   OMX_U32 nTimeout   = 0;
   OMX_U32 nIndex = 0;

   OMX_BASE_Entering();

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];

   for(nIndex=0; nIndex < pPort->tPortDefParams.nBufferCountActual; nIndex++) {
      if(pPort->pBufStateAllocated[nIndex] == OMX_BUFFER_FREE) {
          /* supplier port allocates a buffer and calls
           * UseBuffer on the non-supplier port */
          pBuffer = (OMX_U8*)TIMM_OSAL_Malloc(nSizeBytes);
          OMX_BASE_ASSERT(pBuffer != NULL, OMX_ErrorInsufficientResources);

          /* Retry more than once, if the N-S port is not in loaded->idle state*/
          while(nTimeout < OMX_BASE_TUNNEL_TIMEOUT) {
            tError = OMX_UseBuffer(pPort->hTunnelComp, &pPort->pBufferlist[nIndex],
                                   pPort->unTunnelPort, pComp->pApplicationPrivate,
                                   nSizeBytes, pBuffer);

            if(tError != OMX_ErrorNone){
               if((tError == OMX_ErrorIncorrectStateOperation) &&
                    (nTimeout < OMX_BASE_TUNNEL_TIMEOUT)) {
                    /*Amith Hack*/
                    /*NU_Sleep(OMX_BASE_TUNNEL_SLEEPTIME);*/
                    nTimeout++;
                    continue;
               } else {
                    TIMM_OSAL_Free(pBuffer);
                    pBuffer = NULL;
                    return tError;
               }
            } else
            break;
          }

          /* Incase if the N-S port is not in loaded to idle state and
           * Timeout occured, then release an above buffer allocated  */
          if((tError == OMX_ErrorIncorrectStateOperation) &&
             (nTimeout == OMX_BASE_TUNNEL_TIMEOUT)) {
              TIMM_OSAL_Free(pBuffer);
              return OMX_ErrorIncorrectStateOperation;
          }

          pPort->pBufStateAllocated[nIndex] = OMX_BUFFER_ALLOCATED;
          pPort->unNumAssignedBufs++;

          /* Since the bufHeader is assigned successfully write to datapipe to process  */
          tStatus = TIMM_OSAL_WriteToPipe((pPort->dataPipe), &(pPort->pBufferlist[nIndex]),
                                     sizeof(pPort->pBufferlist), TIMM_OSAL_SUSPEND);
          OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

          /* set port populated field to TRUE if all the buffers are allocated */
          if(pPort->tPortDefParams.nBufferCountActual == pPort->unNumAssignedBufs) {
             pPort->tPortDefParams.bPopulated = OMX_TRUE;
             if(pPort->bIsTransientToEnabled == OMX_TRUE) {
                pPort->bIsTransientToEnabled = OMX_FALSE;
                   pBaseComInt->tCbInfo.EventHandler(hComponent,
                                     pComp->pApplicationPrivate, OMX_EventCmdComplete,
                                     OMX_CommandPortEnable, nPortIndex, "Enabling an S-Port ");

            }
         }
      }
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_FreeTunnelBuffer()
 *
 *  @ see omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_FreeTunnelBuffer(OMX_HANDLETYPE hComponent,
                                         OMX_U32 nPortIndex)
{
   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   OMX_BASE_PORTTYPE *pPort;
   OMX_U32 nTimeout = 0;
   OMX_U32 nIndex;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);
   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];

   for(nIndex=0; nIndex<pPort->tPortDefParams.nBufferCountActual; nIndex++) {
        /* Release a buffer on the Supplier port and call
         * FreeBuffer on the Non-supplier port to release a buffer header */
       if (pPort->pBufStateAllocated[nIndex] & OMX_BUFFER_ALLOCATED) {
           TIMM_OSAL_Free(pPort->pBufferlist[nIndex]->pBuffer);
       }

       /*Retry more than once, if the N-S port is not in Idle->Loaded State*/
       while (nTimeout < OMX_BASE_TUNNEL_TIMEOUT) {
           tError = OMX_FreeBuffer(pPort->hTunnelComp, pPort->unTunnelPort,
                                   pPort->pBufferlist[nIndex]);
           if (tError != OMX_ErrorNone ) {
               if ((tError == OMX_ErrorIncorrectStateOperation) &&
                   (nTimeout < OMX_BASE_TUNNEL_TIMEOUT)){
                   /*Amith Hack*/
                    /*NU_Sleep(OMX_BASE_TUNNEL_SLEEPTIME);*/
                   nTimeout++;
                   continue;
               }
               return tError;
           }else {
               break;
           }
       }

       pPort->pBufStateAllocated[nIndex] = OMX_BUFFER_FREE;
       pPort->unNumAssignedBufs--;

       /* If all buffers are freed, then the S-port can be disabled */
       if (pPort->unNumAssignedBufs == 0) {
            pPort->tPortDefParams.bPopulated = OMX_FALSE;

            /* Freeup the Resources ,As the port is getting disabled */
            _OMX_BASE_DeAllocateResources(hComponent,nPortIndex);

            if(pPort->bIsTransientToDisabled == OMX_TRUE) {
               pPort->bIsTransientToDisabled = OMX_FALSE;
               pBaseComInt->tCbInfo.EventHandler(hComponent,
                                pComp->pApplicationPrivate, OMX_EventCmdComplete,
                                OMX_CommandPortDisable,  nPortIndex, NULL);

            }
       }
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/*
* @fn _OMX_BASE_FlushProcessingBuffers ()
*
* @ see omx_base_internal.h file
*/
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_FlushBuffers(OMX_HANDLETYPE hComponent,
                                     OMX_U32 nPortIndex)
{
   OMX_COMPONENTTYPE* pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_BUFFERHEADERTYPE* pBuffer;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   OMX_U32 actualSize=0;
   OMX_U32 nTimeout = 0;
   OMX_U32 elementsInPipe = 0;

   OMX_BASE_Entering();
   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];

   pPort->bIsPortflushed=OMX_TRUE;

   TIMM_OSAL_GetPipeReadyMessageCount(pPort->dataPipe, &elementsInPipe);
   while(elementsInPipe) {
       if(!PORT_IS_BUFFER_SUPPLIER(pPort)) {
            TIMM_OSAL_ReadFromPipe((pPort->dataPipe), &pBuffer,
                                    sizeof(pBuffer), &actualSize, TIMM_OSAL_SUSPEND);
            elementsInPipe--;
            /* Incase if the port is Tunnneled but not a buffer supplier,
             * return these buffers to the supplier port via ETB/FTB */
            if(PORT_IS_TUNNEL(pPort)){
               if (pPort->tPortDefParams.eDir == OMX_DirInput) {
                    pBuffer->nOutputPortIndex = pPort->unTunnelPort;
                    ((OMX_COMPONENTTYPE*)(pPort->hTunnelComp))->FillThisBuffer(
                                                        pPort->hTunnelComp, pBuffer);
                 } else {
                     pBuffer->nInputPortIndex = pPort->unTunnelPort;
                   ((OMX_COMPONENTTYPE*)(pPort->hTunnelComp))->EmptyThisBuffer(
                                                      pPort->hTunnelComp, pBuffer);
                 }
            }
            /* In case of Non-Tunneling,  return all  the buffers
             * the IL Client using EmptyBufferDone and FillBufferDone */
            else {
                if(pPort->tPortDefParams.eDir == OMX_DirInput)
                    pBaseComInt->tCbInfo.EmptyBufferDone(hComponent,
                                          pComp->pApplicationPrivate, pBuffer);

                else if(pPort->tPortDefParams.eDir == OMX_DirOutput)
                    pBaseComInt->tCbInfo.FillBufferDone(hComponent,
                                         pComp->pApplicationPrivate, pBuffer);
            }
        }
        else
           /* reaches here if port is tunneled and supplier,
            * taken care out side of the While loop */
            break;
    }

    pPort->bIsPortflushed = OMX_FALSE;
     /* wait for the non-supplier port to return all the buffers */
    if(PORT_IS_TUNNELED_N_BUFFER_SUPPLIER(pPort)) {
      while(nTimeout < OMX_BASE_TUNNEL_TIMEOUT) {
           if(elementsInPipe == pPort->unNumAssignedBufs) {
               if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE)
                TIMM_OSAL_SemaphoreRelease(pPort->pBufAllocSem);
               break;
               } else {
               /*Amith Hack*/
               /*NU_Sleep(OMX_BASE_TUNNEL_SLEEPTIME);*/
                pPort->bIsPortflushed = OMX_TRUE;
                nTimeout++;
                continue;
               }
          }
    }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
* @fn _OMX_BASE_AllocateResources()
*
* @ see omx_base_internal.h file
*/
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_AllocateResources(OMX_HANDLETYPE hComponent,
                                          OMX_U32 nIndex)
{
   OMX_BASE_PRIVATETYPE* pBaseComPvt ;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_BASE_PORTTYPE *pPort;
   OMX_U32 i = 0;

   OMX_BASE_Entering();

   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)(((OMX_COMPONENTTYPE *)hComponent)->pComponentPrivate);
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = (OMX_BASE_PORTTYPE*)pBaseComInt->ports[nIndex];

   pPort->unNumAssignedBufs = 0;

   /* create a datapipe to queue up the buffers belonging to this port  */
   tStatus = TIMM_OSAL_CreatePipe(&(pPort->dataPipe),
                   pPort->tPortDefParams.nBufferCountActual * sizeof(OMX_BUFFERHEADERTYPE *),
                      sizeof(OMX_BUFFERHEADERTYPE *), 1);
   OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}


/**=========================================================================**/
/**
* @fn _OMX_BASE_DeAllocateResources()
*
* @ see omx_base_internal.h file
*/
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_DeAllocateResources(OMX_HANDLETYPE hComponent,
                                            OMX_U32 nIndex)
{

   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_BASE_PORTTYPE *pPort;

   OMX_BASE_Entering();

   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)(((OMX_COMPONENTTYPE *)hComponent)->pComponentPrivate);
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nIndex];

   /* deleting the data pipe */
   if(pPort->dataPipe != NULL) {
       tStatus = TIMM_OSAL_DeletePipe(pPort->dataPipe);
       OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);
    }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn _OMX_BASE_VerifyTunnelConnection()
 *
 * @ seee omx_base_internal.h file
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_VerifyTunnelConnection(OMX_BASE_PORTTYPE *pPort,
                                               OMX_HANDLETYPE hTunneledComp)
{
   OMX_PARAM_PORTDEFINITIONTYPE tTunPortDef;
   OMX_ERRORTYPE tError = OMX_ErrorUndefined;

   OMX_BASE_Entering();

   OMX_BASE_INIT_STRUCT_PTR(&tTunPortDef, OMX_PARAM_PORTDEFINITIONTYPE);
   tTunPortDef.nPortIndex = pPort->unTunnelPort;

   /*Get portdef params of tunnel component */
   tError = OMX_GetParameter(hTunneledComp, OMX_IndexParamPortDefinition, &tTunPortDef);
   OMX_BASE_ASSERT(tError == OMX_ErrorNone, tError);

   pPort->unTunnelBufs  = tTunPortDef.nBufferCountActual;

   /* Check data compatablity between the ports */
   switch(pPort->tPortDefParams.eDomain) {

	  case OMX_PortDomainOther:
	       if(pPort->tPortDefParams.format.other.eFormat !=
              tTunPortDef.format.other.eFormat) {
                tError = OMX_ErrorPortsNotCompatible;
		   }
	  break;

	  case OMX_PortDomainAudio:
	       if(pPort->tPortDefParams.format.audio.eEncoding !=
              tTunPortDef.format.audio.eEncoding) {
			    tError = OMX_ErrorPortsNotCompatible;
		   }
	  break;

      /* Image and Video Components compatablity
       * 1. Incase if the tunnel component and tunneled component are of
       *    different domains, check for color format of respective domains.
       * 2. Incas if both tunnel component and tunneled component are of
       *    same domains, compression formats should match. if compression
       *    format is unused, check for color format */
	  case OMX_PortDomainVideo:
	       if(tTunPortDef.eDomain == OMX_PortDomainImage) {
			  if(pPort->tPortDefParams.format.video.eColorFormat !=
			     tTunPortDef.format.image.eColorFormat) {
					tError = OMX_ErrorPortsNotCompatible;
			  }

		   } else if(tTunPortDef.eDomain == OMX_PortDomainVideo) {
			   if(pPort->tPortDefParams.format.video.eCompressionFormat !=
                  tTunPortDef.format.video.eCompressionFormat) {
					  tError = OMX_ErrorPortsNotCompatible;
			   } else { /* compression format is matching */
                  if(tTunPortDef.format.video.eCompressionFormat ==
                     OMX_VIDEO_CodingUnused) {
					 if(pPort->tPortDefParams.format.video.eColorFormat !=
					    tTunPortDef.format.video.eColorFormat) {
							tError = OMX_ErrorPortsNotCompatible;
					}
				  }
			   }
		   }
      break;

 	  case OMX_PortDomainImage:
	       if(tTunPortDef.eDomain == OMX_PortDomainVideo) {
			  if(pPort->tPortDefParams.format.image.eColorFormat !=
			     tTunPortDef.format.video.eColorFormat) {
					 tError = OMX_ErrorPortsNotCompatible;
			  }

		   } else if(tTunPortDef.eDomain == OMX_PortDomainImage) {
			   if(pPort->tPortDefParams.format.image.eCompressionFormat !=
                  tTunPortDef.format.image.eCompressionFormat) {
					  tError = OMX_ErrorPortsNotCompatible;
			   } else { /* compression format is matching */
                  if(tTunPortDef.format.image.eCompressionFormat ==
                     OMX_IMAGE_CodingUnused) {
					 if(pPort->tPortDefParams.format.image.eColorFormat !=
					    tTunPortDef.format.image.eColorFormat) {
							tError = OMX_ErrorPortsNotCompatible;
					}
				  }
			   }
		   }
      break;

	  default:
           tError = OMX_ErrorPortsNotCompatible;
	  break;

   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}



