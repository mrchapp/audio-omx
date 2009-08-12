/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*  @file  omx_base_callbacks.c
*         This file contains the methods that provide the functionality
*         of the callbacks from Derived to Base component.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_base\src
*
*  @rev 1.0
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *! 17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ========================================================================= */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/** ----- system and platform files ----------------------------**/
#include <string.h>
/**-------program files ----------------------------------------**/
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <omx_base.h>
#include <omx_base_internal.h>
#include <timm_osal_interfaces.h>
#include <omx_base_utils.h>

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/**--------data declarations -----------------------------------**/
/**--------function prototypes ---------------------------------**/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/**--------data declarations -----------------------------------**/
/**--------function prototypes --------------------------------**/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/**--------macros -------------------------------------------------**/
/**--------data declarations ------------------------------------**/
/**--------function prototypes ---------------------------------**/

/**=========================================================================**/
/**
 * @fn OMX_BASE_Cb_ReturnDataNotify()
 *     This method is called by Derived Component to the Base Comp to
 *     reutn data notification. This method now checks for the port
 *     if not tunneled which communicates with the IL Client
 *     and returns buffers back to the IL client by calling EBD/FBD,
 *     if port is tunneled calls EmptyThisBuffer/FillThisBuffer on the
 *     corresponding tunneled port.
 *
 *  @ param [in] hComponent  : Handle of the Component that executes the call
 *  @ param [in] nPortIndex  : Index of the port
 *  @ param [in] pBuffer     : pointer to an OMX_BUFFERHEADERTYPE structure
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_CB_ReturnDataNotify(OMX_HANDLETYPE hComponent,
                                           OMX_S32 nPortIndex,
                                           OMX_BUFFERHEADERTYPE* pBuffer)
{
   OMX_COMPONENTTYPE* pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pBuffer != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];

   TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
   /* If port is Not-Tunneled, return buffers back to the IL Client by calling
    * FillBufferDone/EmptyBufferDone depending on the directin of port */
   if (!PORT_IS_TUNNEL(pPort)){
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
       if(pPort->tPortDefParams.eDir == OMX_DirInput)
          pBaseComInt->tCbInfo.EmptyBufferDone(hComponent,
                                 pComp->pApplicationPrivate, pBuffer);
       else if(pPort->tPortDefParams.eDir == OMX_DirOutput)
          pBaseComInt->tCbInfo.FillBufferDone(hComponent,
                                 pComp->pApplicationPrivate, pBuffer);
       TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
    }
   /* If port is tunneled,OpenMax components directly pass data buffers among
    * themselves without returning them to the IL Client, thus it calls
    * FillThisBuffer/EmptyThisBuffer on to the corresponding tunneled port */
   else if (PORT_IS_TUNNEL(pPort) && !PORT_IS_BUFFER_SUPPLIER(pPort)) {
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

       if (pPort->tPortDefParams.eDir == OMX_DirInput) {
            pBuffer->nOutputPortIndex = pPort->unTunnelPort;
            pBuffer->nInputPortIndex  =  pPort->tPortDefParams.nPortIndex;
            tError = ((OMX_COMPONENTTYPE*)(pPort->hTunnelComp))->FillThisBuffer(
                                                     pPort->hTunnelComp, pBuffer);
        } else {
            pBuffer->nInputPortIndex = pPort->unTunnelPort;
            pBuffer->nOutputPortIndex = pPort->tPortDefParams.nPortIndex;
            tError = ((OMX_COMPONENTTYPE*)(pPort->hTunnelComp))->EmptyThisBuffer(
                                                       pPort->hTunnelComp, pBuffer);
        }
       TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
   }
    /* Incase the port is Tunneled and buffer supplier, it calls FTB/ETB on the tunneled port ,
     * but if the tunneled comp is not in executing state which results in
     * incorrect state operation then the buffer need to be returned to pipe which
     * will be processed once after the tunneled comp goes to exeucting state */
   else if(PORT_IS_TUNNELED_N_BUFFER_SUPPLIER(pPort) && (pPort->bIsPortflushed==OMX_FALSE)) {
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
       if(pPort->tPortDefParams.eDir == OMX_DirInput) {
           pBuffer->nOutputPortIndex = pPort->unTunnelPort;
           pBuffer->nInputPortIndex  =  pPort->tPortDefParams.nPortIndex;
           tError = ((OMX_COMPONENTTYPE*)(pPort->hTunnelComp))->FillThisBuffer(
                                                     pPort->hTunnelComp, pBuffer);
           if(tError != OMX_ErrorNone)
                TIMM_OSAL_WriteToPipe((pPort->dataPipe), &pBuffer,
                                           sizeof(pBuffer), TIMM_OSAL_SUSPEND);
       }
       else {
           pBuffer->nInputPortIndex = pPort->unTunnelPort;
           pBuffer->nOutputPortIndex = pPort->tPortDefParams.nPortIndex;
           tError = ((OMX_COMPONENTTYPE*)(pPort->hTunnelComp))->EmptyThisBuffer(
                                                     pPort->hTunnelComp, pBuffer);
           if(tError != OMX_ErrorNone)
               TIMM_OSAL_WriteToPipe((pPort->dataPipe), &pBuffer,
                                   sizeof(pBuffer), TIMM_OSAL_SUSPEND);
       }
       TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
   }
   TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
* @fn OMX_BASE_Cb_ReturnEventNotify():
*     This method is invoked by the Derived comp to the Base Component to
*     notify the Asynchornous events,
*
* @ param [in] hComponent  :  Handle of the Component to access
* @ param [in] eEvent      :  Event that the component wants to notify the app
* @ param [in] EventData1  :  This can be OMX_ERRORTYPE for an error event and
*                             will be an OMX_COMMANDTYPEfor a command complete
*                             and OMX_INDEXTYPE for OMX_PortSettingsChanged event.
* @ param [in} EventData2  :  This can hold extra information related to the
*                             Event.can be OMX_STATETYPE for a state change
*                             command or port index for a port settings
*                             changed event.
*  @ param [in]pEventData  :  pointer to an additional event-specific data
*/
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_CB_ReturnEventNotify(OMX_HANDLETYPE hComponent,
                                            OMX_EVENTTYPE eEvent,
                                            OMX_U32 EventData1,
                                            OMX_U32 EventData2,
                                            OMX_PTR pEventData)
{
   OMX_COMPONENTTYPE* pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt ;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   switch(eEvent)
   {
       case OMX_EventCmdComplete:
            TIMM_OSAL_EventSet((pBaseComInt->hEvent), OMX_BASE_CMD_EVENTFLAG,
                                   TIMM_OSAL_EVENT_OR);
       break;

       case OMX_EventMark:
       case OMX_EventBufferFlag:
       case OMX_EventError:
       case OMX_EventPortSettingsChanged:
       case OMX_EventResourcesAcquired:
       case OMX_EventComponentResumed:
       case OMX_EventDynamicResourcesAvailable:
       case OMX_EventPortFormatDetected:
            pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
                             eEvent, EventData1, EventData2, pEventData);
       break;

       default:
            OMX_BASE_Error(" Unknown Event Type ");
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}




