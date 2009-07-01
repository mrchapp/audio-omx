/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*  @file  omx_base_process.c
*   This file contains methods to process the Base component.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_base\src
*
*  @rev  1.0
*/
/* ----------------------------------------------------------------------*/
/* =========================================================================
 *! Revision History
 *! ========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ========================================================================= */

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
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
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
static OMX_ERRORTYPE OMX_BASE_PROCESS_DataEvent(OMX_HANDLETYPE hComponent);

static OMX_ERRORTYPE OMX_BASE_PROCESS_CmdEvent(OMX_HANDLETYPE hComponent,
                                               OMX_BASE_CMDPARAMS *cmdEvent);


/**=========================================================================**/
/**
 * @fn OMX_BASE_ThreadFunctionEntry()
 *     This mehtod is activated if the component runs in Active Context,
 */
/**=========================================================================**/
/*void OMX_BASE_ThreadFunctionEntry(OMX_U32 nArgs, void* arg)*/
/*Mods for Linux*/
void *OMX_BASE_ThreadFunctionEntry (void* arg)
{
   OMX_COMPONENTTYPE *pComp ;
   OMX_HANDLETYPE hComponent;
   OMX_BASE_PRIVATETYPE* pBaseComPvt ;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   OMX_U32 retrievedEvents = 0;

   hComponent = arg;
   pComp = (OMX_COMPONENTTYPE *)arg;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   while(1)   {
      /*  wait for Any of the event/s to process  */
      TIMM_OSAL_EventRetrieve(pBaseComInt->pEventToProcessThread,
                               (CMDEVENT | DATAEVENT | ENDEVENT),
                                TIMM_OSAL_EVENT_OR_CONSUME,
                                &retrievedEvents,  TIMM_OSAL_SUSPEND);
      /* terminate the process when it acquires an ENDEVENT */
      if (retrievedEvents & ENDEVENT)  break;

      /* Process the Event that has retrieved */
      OMX_BASE_PROCESS_Events(hComponent, retrievedEvents);

   }
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_PROCESS_TriggerEvent():
 *     This function is used to set an Event to control the
 *     processing of an Active Component's thread
 *
 *  @ param[in] hComponent : Handle of the Component
 *  @ Param unEventToSet   : Event that need to be set to invoke the Thread
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_PROCESS_TriggerEvent(OMX_HANDLETYPE hComponent,
                                            OMX_U32 EventToSet)
{
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   OMX_COMPONENTTYPE *pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;

   OMX_BASE_Entering();

   /* Check the input parameters */
   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /*Activate an Event */
   TIMM_OSAL_EventSet(pBaseComInt->pEventToProcessThread,
                        EventToSet, TIMM_OSAL_EVENT_OR);
EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_PROCESS_Events():
 *     This function is used to process either of the Command or Data Events .
 *
 *  @ param hComponent       :  Handle of the Component
 *  @ Param unRetrivedEvent  :  Event which needs to be processed
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_PROCESS_Events(OMX_HANDLETYPE hComponent,
                                      OMX_U32 retrivedEvent)
{
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_U32 actualSize = 0;
   OMX_BASE_CMDPARAMS cmdParams;

   OMX_BASE_Entering();

   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)(((OMX_COMPONENTTYPE *)hComponent)->pComponentPrivate);
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* Process commands if acquired an CMDEVENT */
   if (retrivedEvent & CMDEVENT)  {
       while (TIMM_OSAL_IsPipeReady (pBaseComInt->cmdPipe) == TIMM_OSAL_ERR_NONE )  {
            TIMM_OSAL_ReadFromPipe(pBaseComInt->cmdPipe, &cmdParams,
                         sizeof(OMX_BASE_CMDPARAMS), &actualSize, TIMM_OSAL_SUSPEND);
             OMX_BASE_PROCESS_CmdEvent(hComponent, &cmdParams);
       }
   }
   /* Process Data(input and output buffers) if retrieved an DATAEVENT */
    if (retrivedEvent & DATAEVENT) {
         OMX_BASE_PROCESS_DataEvent(hComponent);
   }

EXIT:
   OMX_BASE_Exiting(0);
   return OMX_ErrorNone;
}

/**=========================================================================**/
/**
* @fn OMX_BASE_PROCESS_CmdEvent()
*     This function handles the command sent by SendCommand.
*
* @param [in]  hComponent :    Handle of the component to be accessed.
* @param [in]  cmdEvent   :    General info requested by SendCommand
*/
/**=========================================================================**/
static OMX_ERRORTYPE OMX_BASE_PROCESS_CmdEvent(OMX_HANDLETYPE hComponent,
                                               OMX_BASE_CMDPARAMS *cmdEvent)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt ;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   OMX_U32 nPortIndex;
   OMX_U32 retEvent = 0;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   switch(cmdEvent->tCmdType)
   {
       /* Request the Component to Switch the state */
       case OMX_CommandStateSet:
            tError = _OMX_BASE_HandleStateTransition(hComponent, cmdEvent);
            if(tError != OMX_ErrorNone)
                pBaseComInt->tCbInfo.EventHandler(hComponent,
                              pComp->pApplicationPrivate, OMX_EventError,
                              tError, 0, "Error While state transition");
       break;

       /* Request the Component to Flush one or more Component port/s */
       case OMX_CommandFlush:
            /* Notify Flush operation to the Derived Component */
            tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent,
                        cmdEvent->tCmdType, cmdEvent->unParam, cmdEvent->pCmdData);
            if(tError == OMX_ErrorNone) {
               TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                           TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);

            }
            if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                OMX_BASE_ASSERT(0, OMX_ErrorUndefined);

            /* Flush out the buffers on all ports of a component*/
            if(cmdEvent->unParam == OMX_ALL) {
                for(nPortIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                    nPortIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nPortIndex++) {
                    tError = _OMX_BASE_FlushBuffers(hComponent, nPortIndex);
                    if(tError != OMX_ErrorNone)
                      pBaseComInt->tCbInfo.EventHandler(hComponent,
                                     pComp->pApplicationPrivate, OMX_EventError,
                                     tError, 0, "Error While Flushing buffers ");
                }
            }
            /* Flush the Queue of buffers on a specific port of a component */
            else {
                tError = _OMX_BASE_FlushBuffers(hComponent, cmdEvent->unParam);
                if(tError != OMX_ErrorNone)
                    pBaseComInt->tCbInfo.EventHandler(hComponent,
                                   pComp->pApplicationPrivate, OMX_EventError,
                                   tError, 0, "Error While Flushing buffers ");
            }
       break;

       /* Request the Component to Disable a Port/s */
       case OMX_CommandPortDisable:
            /* Notify portDisable command to the Derived Component */
            tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent,
                          cmdEvent->tCmdType, cmdEvent->unParam, cmdEvent->pCmdData);
            if(tError == OMX_ErrorNone) {
              TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                           TIMM_OSAL_EVENT_OR_CONSUME,  &retEvent, TIMM_OSAL_SUSPEND);

            }
            if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                       OMX_BASE_ASSERT(0, OMX_ErrorUndefined);

            /* Disable all the ports of a component */
            if(cmdEvent->unParam == OMX_ALL) {
               /* Consider if the component is not in a loaded state, then the port
                * need to return all the buffers to the it is holding before completion */
                if(pBaseComPvt->tCurState != OMX_StateLoaded) {
                    for(nPortIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                        nPortIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nPortIndex++) {
                        /* Flush the buffers of a port since the port is going to be disabled  */
                        tError = _OMX_BASE_FlushBuffers(hComponent, nPortIndex);
                        pPort = pBaseComInt->ports[nPortIndex];
                        if(pPort->bIsPortflushed == OMX_TRUE) {
                            pBaseComInt->tCbInfo.EventHandler(hComponent,
                                        pComp->pApplicationPrivate, OMX_EventError,
                                        OMX_ErrorPortUnresponsiveDuringStop, nPortIndex, NULL);

                        }
                    }
                }
                for(nPortIndex= pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                    nPortIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nPortIndex++) {
                    /* Disabling a port */
                    tError = _OMX_BASE_DisablePort(hComponent, nPortIndex);
                    if(tError != OMX_ErrorNone)
                       pBaseComInt->tCbInfo.EventHandler(hComponent,
                                       pComp->pApplicationPrivate, OMX_EventError,
                                       tError, 0, "Error While Disabling a port ");
                }
            }
            /* Disable the specific port of a component */
            else {
                if(pBaseComPvt->tCurState != OMX_StateLoaded) {
                    tError = _OMX_BASE_FlushBuffers(hComponent, cmdEvent->unParam);
                    pPort = pBaseComInt->ports[cmdEvent->unParam];
                    if(pPort->bIsPortflushed == OMX_TRUE) {
                          pBaseComInt->tCbInfo.EventHandler(hComponent,
                                       pComp->pApplicationPrivate, OMX_EventError,
                                       OMX_ErrorPortUnresponsiveDuringStop,
                                       cmdEvent->unParam, NULL);
                    }
                }
                tError = _OMX_BASE_DisablePort(hComponent, cmdEvent->unParam);
                if(tError != OMX_ErrorNone)
                    pBaseComInt->tCbInfo.EventHandler(hComponent,
                                   pComp->pApplicationPrivate, OMX_EventError,
                                   tError, 0, "Error While Disabling a port ");
            }
       break;

       /* Request the component to enable a port/s */
       case OMX_CommandPortEnable:
            /* enabling all the ports of a component */
            if(cmdEvent->unParam == OMX_ALL) {
                for(nPortIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                    nPortIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nPortIndex++) {
                    tError = _OMX_BASE_EnablePort(hComponent, nPortIndex);
                    if(tError != OMX_ErrorNone)
                       pBaseComInt->tCbInfo.EventHandler(hComponent,
                                 pComp->pApplicationPrivate, OMX_EventError,
                                 tError, 0, "Error While Enabling a port ");
                }
            }
            /* Enabling the specific port of a component */
            else {
                tError = _OMX_BASE_EnablePort(hComponent, cmdEvent->unParam);
                if(tError != OMX_ErrorNone)
                    pBaseComInt->tCbInfo.EventHandler(hComponent,
                                    pComp->pApplicationPrivate, OMX_EventError,
                                     tError, 0, "Error While Enabling a port ");
            }

            /* Notify State transition to the Derived Component */
            tError = pBaseComPvt->tDerToBase.fpCommandNotify(hComponent, cmdEvent->tCmdType,
                                                         cmdEvent->unParam, cmdEvent->pCmdData);
            if(tError == OMX_ErrorNone) {
              TIMM_OSAL_EventRetrieve(pBaseComInt->hEvent, OMX_BASE_CMD_EVENTFLAG,
                                TIMM_OSAL_EVENT_OR_CONSUME, &retEvent, TIMM_OSAL_SUSPEND);
            }
            if (!(retEvent & OMX_BASE_CMD_EVENTFLAG))
                       OMX_BASE_ASSERT(0, OMX_ErrorUndefined);
       break;

        /* Mark a Buffer and specify which component will raise the event mark received */
       case OMX_CommandMarkBuffer:
            pBaseComPvt->pMark = (OMX_MARKTYPE *)(cmdEvent->pCmdData);
       break;

       default:
            OMX_BASE_Error("unknown command received ");
       break;
   }

   /* since command sent by OMX comp is processed, return an Eventhandler to IL client */
   if((tError == OMX_ErrorNone) && (cmdEvent->tCmdType != OMX_CommandMarkBuffer)) {
        tError = _OMX_BASE_HandleEventNotify3(hComponent, cmdEvent);
    }

EXIT:
  OMX_BASE_Exiting(tError);
  return tError;
}

/**=========================================================================**/
/**
* @fn OMX_BASE_PROCESS_DataEvent()
*     This method passes the data(input or output buffer) to the actual comp.
*
* @param [in] hComponent : Handle of the component to be accessed  to  access.
*/
/**=========================================================================**/
static OMX_ERRORTYPE OMX_BASE_PROCESS_DataEvent(OMX_HANDLETYPE hComponent)
{
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt; 
   OMX_U32 nPortIndex;
   OMX_BASE_PORTTYPE *pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   OMX_BUFFERHEADERTYPE *pBufHeader = NULL;
   OMX_U32 actualSize = 0;
   OMX_U32 elementsInPipe = 0;

   OMX_BASE_Entering();

   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)(((OMX_COMPONENTTYPE *)hComponent)->pComponentPrivate);
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   OMX_BASE_ASSERT(pBaseComPvt->tDerToBase.fpDataNotify != NULL, OMX_ErrorBadParameter);

   for(nPortIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
       nPortIndex < pBaseComPvt->tDerToBase.tPortParams.nPorts; nPortIndex++) {

       pPort = pBaseComInt->ports[nPortIndex];

	   TIMM_OSAL_GetPipeReadyMessageCount(pPort->dataPipe, &elementsInPipe);
       while(elementsInPipe) {
           TIMM_OSAL_ReadFromPipe((pPort->dataPipe),  &pBufHeader,
                                sizeof(pBufHeader), &actualSize, TIMM_OSAL_SUSPEND);
           OMX_BASE_ASSERT(pBufHeader != NULL, OMX_ErrorInsufficientResources);

		   elementsInPipe--;
		   
		   /* Notify the data(i/p or o/p buffers) to the Actual(Derived) Component */
           tError = pBaseComPvt->tDerToBase.fpDataNotify(hComponent, nPortIndex, pBufHeader);
           OMX_BASE_ASSERT(tError == OMX_ErrorNone, tError);
       }

    }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

