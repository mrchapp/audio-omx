/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*   @file  omx_base.c
*   This file contains methods that provides the functionality for
*   the OpenMAX1.1 Base component.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_base\src
*
*  @rev 1.0
*/
/* -------------------------------------------------------------------------- */
/** ===========================================================================
 *! Revision History
 *! ===========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ==========================================================================*/

/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/**----- system and platform files ----------------------------**/
#include <string.h>

/**-------program files ----------------------------------------**/
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
/**--------function prototypes ---------------------------------**/

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
/**--------function prototypes ------------------------------- **/


/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentInit()
 *
 *  @ param hComponent  :  Handle of the Component
 *  @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentInit(OMX_IN OMX_HANDLETYPE hComponent)
{
   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;

   OMX_BASE_Entering();

   /* Check the input parameters, This should be true else report an error */
   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE*)hComponent;

   /* Check the version of the component */
   OMX_BASE_CHK_VERSION(pComp, OMX_COMPONENTTYPE, tError);

   /* Fill in all the function pointers for the dervied comp's handle */
   pComp->SetCallbacks           =  OMX_BASE_SetCallbacks;
   pComp->GetComponentVersion    =  OMX_BASE_GetComponentVersion;
   pComp->GetState               =  OMX_BASE_GetState;
   pComp->GetParameter           =  OMX_BASE_GetParameter;
   pComp->SetParameter           =  OMX_BASE_SetParameter;
   pComp->SendCommand            =  OMX_BASE_SendCommand;
   pComp->AllocateBuffer         =  OMX_BASE_AllocateBuffer;
   pComp->UseBuffer              =  OMX_BASE_UseBuffer;
   pComp->FreeBuffer             =  OMX_BASE_FreeBuffer;
   pComp->EmptyThisBuffer        =  OMX_BASE_EmptyThisBuffer;
   pComp->FillThisBuffer         =  OMX_BASE_FillThisBuffer;
   pComp->ComponentTunnelRequest =  OMX_BASE_ComponentTunnelRequest;
   pComp->ComponentDeInit        =  OMX_BASE_ComponentDeInit;

   /* Note: Not Implemented these:
    *  Need to Overwrite these function pointers by derived comp */
   pComp->GetConfig              = OMX_BASE_GetConfig;
   pComp->SetConfig              = OMX_BASE_SetConfig;
   pComp->UseEGLImage            = OMX_BASE_UseEGLImage;
   pComp->GetExtensionIndex      = OMX_BASE_GetExtensionIndex;
   pComp->ComponentRoleEnum      = OMX_BASE_ComponentRoleEnum;

   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;

   /* Initialize memory for the Base component private area */
   pBaseComPvt->hOMXBaseInt = (OMX_BASE_INTERNALTYPE *)TIMM_OSAL_Malloc(
                                               sizeof(OMX_BASE_INTERNALTYPE));
   OMX_BASE_ASSERT(pBaseComPvt->hOMXBaseInt != NULL, OMX_ErrorInsufficientResources);

   TIMM_OSAL_Memset(pBaseComPvt->hOMXBaseInt, 0x0, sizeof(OMX_BASE_INTERNALTYPE));

   pBaseComInt = (OMX_BASE_INTERNALTYPE *)pBaseComPvt->hOMXBaseInt;

   /* Create a command pipe to queue up the command by SendCommand,for later execution */
   tStatus = TIMM_OSAL_CreatePipe(&(pBaseComInt->cmdPipe), OMX_BASE_CMDPIPE_ELEMENTS *
                           sizeof(OMX_BASE_CMDPARAMS), sizeof(OMX_BASE_CMDPARAMS), 1);
   OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

   /* Create an mutex */
   tStatus = TIMM_OSAL_MutexCreate(&(pBaseComPvt->pMutex));
   OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

   /* Create an Event */
   tStatus = TIMM_OSAL_EventCreate(&(pBaseComInt->hEvent));
   OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

   /* Check whether the component runs in PASSIVE/ACTIVE Context
    * Incase of Active Context: Create an Thread and EVENT for communication */
   if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE) {

       /* create an Event To communicate or process the Active Component's Thread */
       tStatus = TIMM_OSAL_EventCreate(&pBaseComInt->pEventToProcessThread);
       OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

       /* Request a Thread from Resource Manager  */
       tStatus = TIMM_OSAL_CreateTask(&pBaseComInt->pThreadId,
                                      OMX_BASE_ThreadFunctionEntry,
                                      0, (void *)hComponent,
                                      pBaseComPvt->tDerToBase.unStackSize,
                                      pBaseComPvt->tDerToBase.unGroupPrioirty,
                                      pBaseComPvt->tDerToBase.cComponentName);
       OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

       pBaseComInt->fpInvokeProcessFunction = OMX_BASE_PROCESS_TriggerEvent;

    } else {
       pBaseComInt->fpInvokeProcessFunction = OMX_BASE_PROCESS_Events;
    }

   /* Initialize the Given number of ports with default values */
   tError = _OMX_BASE_InitializePorts(hComponent);
   OMX_BASE_ASSERT(tError == OMX_ErrorNone, tError);

   /* Set the Current State and New state to OMX_StateLoaded */
   pBaseComPvt->tCurState        = OMX_StateLoaded;
   pBaseComPvt->tNewState        = OMX_StateLoaded;

   pBaseComPvt->fpReturnDataNotify    = OMX_BASE_CB_ReturnDataNotify;
   pBaseComPvt->fpReturnEventNotify   = OMX_BASE_CB_ReturnEventNotify;
   
 EXIT:
    if((tError == OMX_ErrorInsufficientResources)) {
       tError = pComp->ComponentDeInit(hComponent);
    }
    OMX_BASE_Exiting(tError);
    return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_SetCallbacks()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_SetCallbacks(OMX_IN OMX_HANDLETYPE hComponent,
                                    OMX_IN OMX_CALLBACKTYPE* pCallbacks,
                                    OMX_IN OMX_PTR pAppData)
{
   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pCallbacks != NULL) , OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE *)pBaseComPvt->hOMXBaseInt;

   /* Saving the Callback Structure from the IL Client to the component */
   pBaseComInt->tCbInfo = *pCallbacks;

   /* Save pointer to Application private data */
   pComp->pApplicationPrivate = pAppData;

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetComponentVersion()
 *
 * @ see  omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetComponentVersion(OMX_IN OMX_HANDLETYPE hComponent,
                                           OMX_OUT OMX_STRING pComponentName,
                                           OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
                                           OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
                                           OMX_OUT OMX_UUIDTYPE* pComponentUUID)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pComponentName != NULL) &&
                    (pComponentVersion != NULL) &&
                    (pSpecVersion != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;

   strncpy(pComponentName, pBaseComPvt->tDerToBase.cComponentName,
                                      OMX_MAX_STRINGNAME_SIZE);

   /* Assigning the Component and OpenMAX Specification version */
   *pComponentVersion = pBaseComPvt->tDerToBase.tComVersion;
   *pSpecVersion      = pComp->nVersion;

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetState()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetState(OMX_IN  OMX_HANDLETYPE hComponent,
                                OMX_OUT OMX_STATETYPE* pState)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pState != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;

   /* Set the Current State of the Component */
   *pState = pBaseComPvt->tCurState;

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetParameter():
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetParameter(OMX_IN OMX_HANDLETYPE hComponent,
                                    OMX_IN OMX_INDEXTYPE nParamIndex,
                                    OMX_INOUT OMX_PTR pParamStruct)
{

   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_PARAM_PORTDEFINITIONTYPE *pPortdef;
   OMX_PARAM_BUFFERSUPPLIERTYPE *pBufSupplier;
   OMX_BASE_PORTTYPE *pPort;
   OMX_BUFFERSUPPLIERTYPE tSupplier;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pParamStruct != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE* )hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /*GetParameter can't be invoked when the comp is in Invalid state */
   OMX_BASE_ASSERT(pBaseComPvt->tCurState != OMX_StateInvalid,
                                 OMX_ErrorIncorrectStateOperation);

   /* Note: All standard components shall support the following indexes */
   switch(nParamIndex)
   {
       /* domain specific init needs to be taken care in the Derived comp,
        * here all non-supported inits will be set to Zero */
       case OMX_IndexParamAudioInit:
       case OMX_IndexParamVideoInit:
       case OMX_IndexParamImageInit:
       case OMX_IndexParamOtherInit:
            /* Check for the Correct nSize and nVersion information */
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PORT_PARAM_TYPE, tError);
            ((OMX_PORT_PARAM_TYPE*)pParamStruct)->nPorts = 0;
            ((OMX_PORT_PARAM_TYPE*)pParamStruct)->nStartPortNumber = 0;
       break;

       /* IL client uses this to retrieve the general info from each port */
       case OMX_IndexParamPortDefinition:
            /* Check for the correct nSize and nVersion Information */
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_PORTDEFINITIONTYPE, tError);
            pPortdef = (OMX_PARAM_PORTDEFINITIONTYPE *)pParamStruct;

            /* Check for valid port index */
            OMX_BASE_REQUIRE(((pPortdef->nPortIndex) <
                              (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                              pBaseComPvt->tDerToBase.tPortParams.nPorts)),
                              OMX_ErrorBadPortIndex);

            /* Populate the OMX_PARAM_PORTDEFINITIONTYPE structure */
            *pPortdef = pBaseComInt->ports[pPortdef->nPortIndex]->tPortDefParams;
       break;

       /* IL Client uses to retrieve the priority Assigned to a group of components  */
       case OMX_IndexParamPriorityMgmt:
            /* Check for the correct nSize and nVersion Information */
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PRIORITYMGMTTYPE, tError);
            /* Populate the "OMX_PRIORITYMGMTTYPE" structure */
            ((OMX_PRIORITYMGMTTYPE *)pParamStruct)->nGroupID = pBaseComPvt->tDerToBase.unGroupID;
            ((OMX_PRIORITYMGMTTYPE *)pParamStruct)->nGroupPriority = pBaseComPvt->tDerToBase.unGroupPrioirty;
       break;

       /*Used to Communicate buffer supplier settings/preferences from each port  */
       case OMX_IndexParamCompBufferSupplier:
            /* Check for the correct nSize and nVersion Information */
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_BUFFERSUPPLIERTYPE, tError);
            pBufSupplier = (OMX_PARAM_BUFFERSUPPLIERTYPE *)pParamStruct;

            /* Check for vaild port index, need to be less than no of ports */
            OMX_BASE_REQUIRE((pBufSupplier->nPortIndex) <
                             (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                              pBaseComPvt->tDerToBase.tPortParams.nPorts), OMX_ErrorBadPortIndex);

            pPort = pBaseComInt->ports[pBufSupplier->nPortIndex];
            tSupplier = pBaseComPvt->tDerToBase.pTunPref[pBufSupplier->nPortIndex]->eSupplier;

            TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
            /* Get the buffer supplier preferences depending on direction of
             * port and whether the port is tunneled/non-tunneled */
            if(pPort->tPortDefParams.eDir == OMX_DirInput) {
               if(PORT_IS_BUFFER_SUPPLIER(pPort))
                  pBufSupplier->eBufferSupplier = OMX_BufferSupplyInput;
               else if(PORT_IS_TUNNEL(pPort))
                   pBufSupplier->eBufferSupplier = OMX_BufferSupplyOutput;
               else
                   pBufSupplier->eBufferSupplier = tSupplier;
            } else if(pPort->tPortDefParams.eDir == OMX_DirOutput) {
               if(PORT_IS_BUFFER_SUPPLIER(pPort))
                  pBufSupplier->eBufferSupplier = OMX_BufferSupplyOutput;
               else if(PORT_IS_TUNNEL(pPort))
                  pBufSupplier->eBufferSupplier = OMX_BufferSupplyInput;
               else
                   pBufSupplier->eBufferSupplier = tSupplier;
            }
            TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
       break;

       default:
            OMX_BASE_Error("Un-Supported Index type ");
            tError = OMX_ErrorUnsupportedIndex;
       break;
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_SetParameter()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_SetParameter(OMX_IN  OMX_HANDLETYPE hComponent,
                                    OMX_IN  OMX_INDEXTYPE nParamIndex,
                                    OMX_IN  OMX_PTR pParamStruct)
{

   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_PARAM_PORTDEFINITIONTYPE* pPortdef;
   OMX_BASE_PORTTYPE* pPort;
   OMX_PARAM_BUFFERSUPPLIERTYPE* pBufSupplier;
   OMX_PARAM_BUFFERSUPPLIERTYPE tTunnelBufSupplier;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pParamStruct != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE* )hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* SetParameter: this Method can be invoked only when the comp is in
    * Loaded State or on a port that is disabled */
   OMX_BASE_ASSERT((pBaseComPvt->tCurState == OMX_StateLoaded) ||
                   (nParamIndex == OMX_IndexParamPortDefinition) ||
                   (nParamIndex == OMX_IndexParamCompBufferSupplier),
                   OMX_ErrorIncorrectStateOperation);
   switch(nParamIndex)
   {
       /* Used to set the PortDef type params of an port  */
       case OMX_IndexParamPortDefinition:
            /* Check for correct nSize and nVersion fields */
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_PORTDEFINITIONTYPE, tError);
            pPortdef = (OMX_PARAM_PORTDEFINITIONTYPE *)pParamStruct;

            /* Check for valid port index, this need to be less than no of ports */
            OMX_BASE_REQUIRE((pPortdef->nPortIndex) <
                         (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                         pBaseComPvt->tDerToBase.tPortParams.nPorts),
                        OMX_ErrorBadPortIndex);

            pPort = (OMX_BASE_PORTTYPE*)pBaseComInt->ports[pPortdef->nPortIndex];

            /* Setparam call is successfull, only when the comp is in loaded or port is disabled */
            OMX_BASE_REQUIRE((pBaseComPvt->tCurState == OMX_StateLoaded) ||
                             (pPort->tPortDefParams.bEnabled == OMX_FALSE),
                             OMX_ErrorIncorrectStateOperation);
            /* set the parameter sent by client in to the pvt data */
            pPort->tPortDefParams = *((OMX_PARAM_PORTDEFINITIONTYPE *)pParamStruct);
       break;

       /* used to set the GroupPrioirty and GroupId of a  Component. ComponentGroup
        * Identifies  a set of co-dependent component associated with the same feature.
        * All these Components share  the same GroupId and GroupPriority  */
       case OMX_IndexParamPriorityMgmt:
            /* check for correct nSize and nVersion info */
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PRIORITYMGMTTYPE, tError);

            pBaseComPvt->tDerToBase.unGroupID = ((OMX_PRIORITYMGMTTYPE *)pParamStruct)->nGroupID;
            pBaseComPvt->tDerToBase.unGroupPrioirty = ((OMX_PRIORITYMGMTTYPE *)pParamStruct)->nGroupPriority;
       break;
       /* used to set the buffer supplier setting of an port */
       case OMX_IndexParamCompBufferSupplier:
            /* check for correct nSize and nVersion info */
            OMX_BASE_CHK_VERSION(pParamStruct, OMX_PARAM_BUFFERSUPPLIERTYPE, tError);
            pBufSupplier = (OMX_PARAM_BUFFERSUPPLIERTYPE *)pParamStruct;

            /* Check for valid port index, need to be less than the no of ports */
            OMX_BASE_REQUIRE((pBufSupplier->nPortIndex) <
                    (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                    pBaseComPvt->tDerToBase.tPortParams.nPorts), OMX_ErrorBadPortIndex);

            pPort = (OMX_BASE_PORTTYPE*)pBaseComInt->ports[pBufSupplier->nPortIndex];

            /* Set parameter call is sucessull, only when the comp is in loaded
             *  state or port is diabled */
            OMX_BASE_REQUIRE((pBaseComPvt->tCurState == OMX_StateLoaded) ||
                             (pPort->tPortDefParams.bEnabled == OMX_FALSE),
                             OMX_ErrorIncorrectStateOperation);

#ifndef OMX_CONF_TEST
             /*This piece of code is required for conformance testing  */
            /* Check whether the port is tunneled or not,
             * Incase if not tunneled, return OMX_ErrorNone */
            if(pPort->hTunnelComp == NULL) return OMX_ErrorNone;
#endif
            pPort->tBufferSupplier = pBufSupplier->eBufferSupplier;

            /* Setting the Buffer supplier parameters depending on the direction
             * of the port and supplier preferences */
            if((pBufSupplier->eBufferSupplier == OMX_BufferSupplyInput) &&
                (pPort->tPortDefParams.eDir == OMX_DirInput)) {

                pPort->unTunnelFlags |= PORT_IS_SUPPLIER;
                tTunnelBufSupplier = *pBufSupplier;
                tTunnelBufSupplier.nPortIndex = pPort->unTunnelPort;

                tError = OMX_SetParameter(pPort->hTunnelComp,
                               OMX_IndexParamCompBufferSupplier, &tTunnelBufSupplier);
            }

            /* Incase if the supplier pref is o/p, but i/p was already a buffer supplier,
             * then force the i/p port to be non-supplier and notify these changes to
             * the o/p port of te tunneled comp to be supplier*/
            else if((pBufSupplier->eBufferSupplier == OMX_BufferSupplyOutput) &&
                    (pPort->tPortDefParams.eDir == OMX_DirInput)) {

                 if (PORT_IS_BUFFER_SUPPLIER(pPort)) {
                    pPort->unTunnelFlags &= ~PORT_IS_SUPPLIER;
                    tTunnelBufSupplier = *pBufSupplier;
                    tTunnelBufSupplier.nPortIndex = pPort->unTunnelPort;

                    tError = OMX_SetParameter(pPort->hTunnelComp,
                                OMX_IndexParamCompBufferSupplier, &tTunnelBufSupplier);
                }
            }
            /* If an o/p port and supplier pref is also an o/p port, then configure
             * the o/p port to be non-supplier  */
            else if((pBufSupplier->eBufferSupplier == OMX_BufferSupplyOutput) &&
                  (pPort->tPortDefParams.eDir == OMX_DirOutput)) {
                 pPort->unTunnelFlags |= PORT_IS_SUPPLIER;
            }
            /* if supplier pref is i/p and an o/p port */
            else {
               if (PORT_IS_BUFFER_SUPPLIER(pPort))
                  pPort->unTunnelFlags &= ~PORT_IS_SUPPLIER;
            }
       break;

       default:
            OMX_BASE_Error("Un Supported Index type ");
            tError = OMX_ErrorUnsupportedIndex;
       break;
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_SendCommand()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_SendCommand(OMX_IN  OMX_HANDLETYPE hComponent,
                                   OMX_IN  OMX_COMMANDTYPE Cmd,
                                   OMX_IN  OMX_U32 nParam,
                                   OMX_IN  OMX_PTR pCmdData)
{

   OMX_COMPONENTTYPE* pComp;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_U32 nIndex = 0;
   OMX_U32 i = 0;
   OMX_BASE_CMDPARAMS tCmdParams;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;

   OMX_BASE_Entering();

   /* Check for the input parameters */
   OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter );

   if(Cmd == OMX_CommandMarkBuffer)
      OMX_BASE_REQUIRE(pCmdData != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE*)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* This method is not allowed when the comp is in Invalid State */
   OMX_BASE_ASSERT((pBaseComPvt->tCurState != OMX_StateInvalid),
                   OMX_ErrorInvalidState);

   switch(Cmd)
   {
       /* Uses this to change the component state */
       case OMX_CommandStateSet:
            TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
            /* Incase If the comp is moving from Loaded->Idle, Allocate Internal resources */
            if((pBaseComPvt->tCurState == OMX_StateLoaded) && (nParam == OMX_StateIdle)) {
                TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
                for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                    nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                              pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {

				  pPort = (OMX_BASE_PORTTYPE*)pBaseComInt->ports[nIndex];
                  pPort->pBufStateAllocated = (OMX_BUFHDR_STATUS_FLAG *)TIMM_OSAL_Malloc(
                                          pPort->tPortDefParams.nBufferCountActual *
                                          sizeof(OMX_BUFHDR_STATUS_FLAG));
                  OMX_BASE_ASSERT(pPort->pBufStateAllocated != NULL, OMX_ErrorInsufficientResources);

                  /* Initialize all the BufferAllocationStatusFlag to " OMX_BUFFER_FREE" */
                  for(i = 0; i < pPort->tPortDefParams.nBufferCountActual; i++)
                       pPort->pBufStateAllocated[i] = OMX_BUFFER_FREE;

                  /* Create memory for the bufferlist to have an reference to all the
                   * buffers handled by this port */
                  pPort->pBufferlist = (OMX_BUFFERHEADERTYPE **)TIMM_OSAL_Malloc(
                                           pPort->tPortDefParams.nBufferCountActual *
                                           sizeof(OMX_BUFFERHEADERTYPE *));
                  OMX_BASE_ASSERT(pPort->pBufferlist != NULL, OMX_ErrorInsufficientResources);
                  tError = _OMX_BASE_AllocateResources(hComponent, nIndex);
                  OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
               }
               TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
            }
            /* Save the NewState to be transitioned to */
            pBaseComPvt->tNewState = (OMX_STATETYPE)nParam;
            TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

       break;

       /* Uses this to enable the port/s of a component */
       case OMX_CommandPortEnable:
       /* Uses this to Disable the port/s of a component */
       case OMX_CommandPortDisable:
            /* nParam(PortId) needs to be less than the no of ports or equal to OMX_ALL */
            OMX_BASE_ASSERT((nParam < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                                       pBaseComPvt->tDerToBase.tPortParams.nPorts)) ||
                             (nParam == OMX_ALL), OMX_ErrorBadPortIndex);
            /*  For PortEnable command allocate the resoruces of an port
             *  and set the transientToEnable flag to true, whereas in case of portdiable
             *  command set the transienttoDisabled flag to TRUE */
            if(nParam == OMX_ALL) {
                for(nIndex = pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber;
                    nIndex < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                             pBaseComPvt->tDerToBase.tPortParams.nPorts); nIndex++) {
                     pPort = pBaseComInt->ports[nIndex];

                    if(Cmd == OMX_CommandPortDisable) {
                        TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex,TIMM_OSAL_SUSPEND);
                        pPort->bIsTransientToDisabled  = OMX_TRUE;
                        TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
                    } else if(Cmd == OMX_CommandPortEnable) {
                         TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex,TIMM_OSAL_SUSPEND);
                         pPort->bIsTransientToEnabled   = OMX_TRUE;
                         TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
                         tError = _OMX_BASE_AllocateResources(hComponent, nIndex);
                        OMX_BASE_ASSERT(tError == OMX_ErrorNone, tError);
                    }
                }
            } else {
                pPort = pBaseComInt->ports[nParam];
                if(Cmd == OMX_CommandPortDisable) {
                   TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex,TIMM_OSAL_SUSPEND);
                   pPort->bIsTransientToDisabled = OMX_TRUE;
                   TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
                } else if(Cmd == OMX_CommandPortEnable) {
                    TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex,TIMM_OSAL_SUSPEND);
                    pPort->bIsTransientToEnabled   = OMX_TRUE;
                    TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
                    tError = _OMX_BASE_AllocateResources(hComponent, nParam);
                    OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
                }
            }

       break;

       /* Used to Flush the data queue(s)  of a component */
       case OMX_CommandFlush:
       /* Used to Mark a Component/buffer for observation */
       case OMX_CommandMarkBuffer:
       /* Check for valid port index, which need to be less than no of ports */
           OMX_BASE_ASSERT((nParam < (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                                     pBaseComPvt->tDerToBase.tPortParams.nPorts)) ||
                           (nParam == OMX_ALL), OMX_ErrorBadPortIndex);
       break;

       default:
            OMX_BASE_Error(" Un-Supported CommandType received ");
             tError = OMX_ErrorUnsupportedIndex;
       break;
   }

   /* keep the info and process the command later */
   tCmdParams.unParam   = nParam;
   tCmdParams.tCmdType  = Cmd;
   tCmdParams.pCmdData  = pCmdData;

   tStatus = TIMM_OSAL_WriteToPipe(pBaseComInt->cmdPipe, &tCmdParams,
                                  sizeof(tCmdParams), TIMM_OSAL_SUSPEND);
   OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

   /* Invoke the process function with the CMDEVENT to process the commands */
   pBaseComInt->fpInvokeProcessFunction(hComponent, CMDEVENT);

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_AllocateBuffer()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_AllocateBuffer(OMX_IN OMX_HANDLETYPE hComponent,
                                      OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
                                      OMX_IN OMX_U32 nPortIndex,
                                      OMX_IN OMX_PTR pAppPrivate,
                                      OMX_IN OMX_U32 nSizeBytes)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_U32 nBufferCnt = 0;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   /* Check for the input parameters */
   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (ppBuffer != NULL) &&
                    (nSizeBytes > 0), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];
   nBufferCnt = pPort->unNumAssignedBufs;

   /* AllocateBuffer method is called only when the comp is moving from
    * Loaded to idle or on a port that is disabled */
   TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
   OMX_BASE_REQUIRE((pBaseComPvt->tCurState == OMX_StateLoaded &&
                     pBaseComPvt->tNewState == OMX_StateIdle) ||
                    (pPort->bIsTransientToEnabled == OMX_TRUE),
                    OMX_ErrorIncorrectStateOperation);
   TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

   /* Check if port is populated, which mean all buffers are Allocated*/
   if(PORT_IS_POPULATED(pPort)) {
      OMX_BASE_Error("Cannot Allocate Buffers since port is Already populated \n");
      return OMX_ErrorBadParameter;
   }

   /* This method is not applicable for tunneled ports  */
   if(PORT_IS_TUNNEL(pPort)) {
      OMX_BASE_Error(" Cannot Allocate Buffers for tunneled ports \n");
      return OMX_ErrorBadPortIndex;
   }

   /* Allocates New Buffer and New Bufferheader if the bufferState is BUFFER_FREE */
   if(pPort->pBufStateAllocated[nBufferCnt] == OMX_BUFFER_FREE) {
        /* Allocate a New Buffer Header */
        pPort->pBufferlist[nBufferCnt] = (OMX_BUFFERHEADERTYPE *)TIMM_OSAL_Malloc(
                                                      sizeof(OMX_BUFFERHEADERTYPE));
        OMX_BASE_ASSERT(pPort->pBufferlist[nBufferCnt] != NULL,
                            OMX_ErrorInsufficientResources);

        OMX_BASE_INIT_STRUCT_PTR(pPort->pBufferlist[nBufferCnt], OMX_BUFFERHEADERTYPE);

        /* Allocate a new Buffer */
        pPort->pBufferlist[nBufferCnt]->pBuffer = (OMX_U8*)TIMM_OSAL_Malloc(nSizeBytes);
        if(pPort->pBufferlist[nBufferCnt]->pBuffer == NULL) {
             TIMM_OSAL_Free(pPort->pBufferlist[nBufferCnt]);
             pPort->pBufferlist[nBufferCnt] = NULL;
             return OMX_ErrorInsufficientResources;
        }

        /* Initialize the BufferHeader with Actula/default Values */
        _OMX_BASE_InitializeBuffer(hComponent, pPort->pBufferlist[nBufferCnt],
                                    nPortIndex, pAppPrivate, nSizeBytes);

        *ppBuffer = pPort->pBufferlist[nBufferCnt];

        /* New BufferHeader and buffer is allocated, keeping this info
         * for future reference, which is required while freeing the buffers */
       pPort->pBufStateAllocated[nBufferCnt] |= OMX_BUFFER_ALLOCATED;
       pPort->pBufStateAllocated[nBufferCnt] |= OMX_HEADER_ALLOCATED;

       /* Since the buffer is allocated with out any problem increment
        * the count for number of buffers assigned */
       pPort->unNumAssignedBufs++;

       /* Handling EventHandler notification for 1. Enabling a port
        *   2. State transition from Loaded to idle state  */
       tError = _OMX_BASE_HandleEventNotify1(hComponent, nPortIndex);
    }

EXIT:
   if(tError == OMX_ErrorIncorrectStateOperation) {
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
   }
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_UseBuffer()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_UseBuffer(OMX_IN OMX_HANDLETYPE hComponent,
                                 OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                 OMX_IN OMX_U32 nPortIndex,
                                 OMX_IN OMX_PTR pAppPrivate,
                                 OMX_IN OMX_U32 nSizeBytes,
                                 OMX_IN OMX_U8* pBuffer)
{

   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_U32 nBufferCnt = 0;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (ppBufferHdr != NULL) &&
                    (nSizeBytes > 0), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];
   nBufferCnt = pPort->unNumAssignedBufs;

   /* This Operation is performed only under the following conditions:
    * 1. Comp is in loaded state and in a transition to Idle
    * 2. on Disabled port when the comp is in Executing, Pase, Idle states */
   TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
   OMX_BASE_REQUIRE((pBaseComPvt->tCurState== OMX_StateLoaded &&
                     pBaseComPvt->tNewState == OMX_StateIdle) ||
                     (pPort->bIsTransientToEnabled == OMX_TRUE),
                     OMX_ErrorIncorrectStateOperation);
   TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

   /* Check: if port is Populated which means all the buffers are allocated */
   if(PORT_IS_POPULATED(pPort)) {
        OMX_BASE_Error("Cannot Allocate Buffers since port is Already populated \n");
     return OMX_ErrorBadParameter;
   }

   /* Allocates New Buffer and New Bufferheader if the bufferState is BUFFER_FREE */
   if(pPort->pBufStateAllocated[nBufferCnt] == OMX_BUFFER_FREE) {

       /* Allocate New Buffer Header */
       pPort->pBufferlist[nBufferCnt] = (OMX_BUFFERHEADERTYPE*)TIMM_OSAL_Malloc(
                                                    sizeof(OMX_BUFFERHEADERTYPE));
       OMX_BASE_ASSERT(pPort->pBufferlist[nBufferCnt] != NULL,
                             OMX_ErrorInsufficientResources);

       OMX_BASE_INIT_STRUCT_PTR(pPort->pBufferlist[nBufferCnt], OMX_BUFFERHEADERTYPE);

       /* Assign the Buffer given by the IL Client */
       pPort->pBufferlist[nBufferCnt]->pBuffer = pBuffer;

       /* Initialize the BufferHeader with Actula/default Values */
       _OMX_BASE_InitializeBuffer(hComponent, pPort->pBufferlist[nBufferCnt],
                                    nPortIndex, pAppPrivate, nSizeBytes);

       *ppBufferHdr = pPort->pBufferlist[nBufferCnt];

       /* BufferHeader is allocated and buffer Assigned, keeping this info
        * for future reference, which is required while freeing the buffers */
       pPort->pBufStateAllocated[nBufferCnt] |= OMX_BUFFER_ASSIGNED;
       pPort->pBufStateAllocated[nBufferCnt] |= OMX_HEADER_ALLOCATED;
       pPort->unNumAssignedBufs++;

       /* Handle EventHandler notification for 1. Enabling a port
        *  2. State transition from Loaded to Idle */
       tError = _OMX_BASE_HandleEventNotify1(hComponent, nPortIndex);
   }
EXIT:
   if(tError == OMX_ErrorIncorrectStateOperation) {
        TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
   }
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_FreeBuffer()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_FreeBuffer(OMX_IN  OMX_HANDLETYPE hComponent,
                                  OMX_IN  OMX_U32 nPortIndex,
                                  OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   OMX_U32 i = 0;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pBuffer != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPortIndex];

   OMX_BASE_CHK_VERSION(pBuffer, OMX_BUFFERHEADERTYPE, tError);

   /* Release a buffer and bufHdr depending on the Allocation */
   for(i=0; i < pPort->tPortDefParams.nBufferCountActual; i++) {
       if(pPort->pBufferlist[i] == pBuffer) {

        /* Freeing-up the buffer if it is allocated */
        if(pPort->pBufStateAllocated[i] & OMX_BUFFER_ALLOCATED) {
            TIMM_OSAL_Free(pPort->pBufferlist[i]->pBuffer);
            pPort->pBufferlist[i]->pBuffer = NULL;
        }

        /* Release a BufferHeader */
        if(pPort->pBufStateAllocated[i] & OMX_HEADER_ALLOCATED) {
           TIMM_OSAL_Free(pPort->pBufferlist[i]);
           pPort->pBufferlist[i] = NULL;
        }

        /* Note: This Operation needs to be performed only under the following condition
         * 1. Comp is in Idle state and sent a request for transition to Loaded
         * 2. on Disabled port when the comp is in Executing, Pause, Idle states
         * if Call is not performed as described, results an
         * OMX_ErrorPortUnpopulated event error */
        TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
        if((pBaseComPvt->tCurState == OMX_StateIdle && pBaseComPvt->tNewState != OMX_StateLoaded) &&
           (pPort->tPortDefParams.bEnabled == OMX_TRUE)) {
               pBaseComInt->tCbInfo.EventHandler(hComponent, pComp->pApplicationPrivate,
                                          OMX_EventError, OMX_ErrorPortUnpopulated,
                                          nPortIndex, NULL);
        }
        TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

        /* Since a Buffer and BufferHeader are freed up depending on allocation
         * set the BufferStateAllocated flag to be BUFER_FREE */
        pPort->pBufStateAllocated[i] = OMX_BUFFER_FREE;
        pPort->unNumAssignedBufs--;

        /* Handle EventHandler Notifications for 1. Disabling a port
         *   2. State transition from Idle to loaded */
        tError = _OMX_BASE_HandleEventNotify2(hComponent, nPortIndex);
        break;
       }
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_EmptyThisBuffer()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_EmptyThisBuffer(OMX_IN OMX_HANDLETYPE hComponent,
                                       OMX_IN OMX_BUFFERHEADERTYPE* pBufHeader)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pBufHeader != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* Check the version of the buffer header */
   OMX_BASE_CHK_VERSION(pBufHeader, OMX_BUFFERHEADERTYPE, tError);

   /* check for valid inputport index in the pBufHeader */
   OMX_BASE_ASSERT((pBufHeader->nInputPortIndex)  <
                  (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                   pBaseComPvt->tDerToBase.tPortParams.nPorts),
                   OMX_ErrorBadPortIndex);

   pPort = pBaseComInt->ports[pBufHeader->nInputPortIndex];

   /* cannot be called on a disabled port */
   OMX_BASE_REQUIRE((pPort->tPortDefParams.bEnabled == OMX_TRUE) ||
                    (pPort->bIsTransientToDisabled  == OMX_TRUE),
                    OMX_ErrorIncorrectStateOperation);

   /* This operation is performed only for an input port */
   OMX_BASE_REQUIRE(pPort->tPortDefParams.eDir == OMX_DirInput,
                    OMX_ErrorBadPortIndex);

   /* Component needs to be in either of Executing/Pause/Idle states */
   OMX_BASE_REQUIRE((pBaseComPvt->tCurState == OMX_StateExecuting) ||
                    (pBaseComPvt->tCurState == OMX_StatePause),
                           OMX_ErrorIncorrectStateOperation);

   OMX_BASE_REQUIRE(pBufHeader->pBuffer != NULL, OMX_ErrorBadParameter);

   /* queue up the buffer into the data pipe */
   TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
   if(pPort->bIsPortflushed == OMX_FALSE) {
        TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
        tStatus = TIMM_OSAL_WriteToPipe((pPort->dataPipe), &pBufHeader,
                                   sizeof(pBufHeader), TIMM_OSAL_SUSPEND);
        OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

        /* Invoke the process function to process the data in the data pipe */
       pBaseComInt->fpInvokeProcessFunction(hComponent, DATAEVENT);

   }
   /* Incase if the port is an buffe supplier, but flushing the buffers
    *  then don't process the buffers */
   else if(PORT_IS_BUFFER_SUPPLIER(pPort)) {
        TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
        tStatus = TIMM_OSAL_WriteToPipe((pPort->dataPipe), &pBufHeader,
                                   sizeof(pBufHeader), TIMM_OSAL_SUSPEND);
        OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

   } else {
      TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
      OMX_BASE_ASSERT(0, OMX_ErrorIncorrectStateOperation);
   }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_FillThisBuffer()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_FillThisBuffer(OMX_IN  OMX_HANDLETYPE hComponent,
                                      OMX_IN  OMX_BUFFERHEADERTYPE* pBufHeader)
{

   OMX_COMPONENTTYPE *pComp;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_BASE_PORTTYPE *pPort;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pBufHeader != NULL), OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* check for the bufferheader version */
   OMX_BASE_CHK_VERSION(pBufHeader, OMX_BUFFERHEADERTYPE, tError);

   /* check for valid outputport index in the pBufHeader */
   OMX_BASE_ASSERT((pBufHeader->nOutputPortIndex)  <
                   (pBaseComPvt->tDerToBase.tPortParams.nStartPortNumber +
                   pBaseComPvt->tDerToBase.tPortParams.nPorts),
                   OMX_ErrorBadPortIndex);
   pPort = pBaseComInt->ports[pBufHeader->nOutputPortIndex];

   /* This operation is performed only for an output port */
   OMX_BASE_REQUIRE(pPort->tPortDefParams.eDir == OMX_DirOutput,
                                    OMX_ErrorBadPortIndex);

   /* cannot be called on a disabled port  */
   OMX_BASE_REQUIRE((pPort->tPortDefParams.bEnabled == OMX_TRUE) ||
                    (pPort->bIsTransientToDisabled == OMX_TRUE),
                    OMX_ErrorIncorrectStateOperation);

   /* Component can be either of Executing/Pause/Idle states */
   OMX_BASE_REQUIRE((pBaseComPvt->tCurState == OMX_StateExecuting) ||
                    (pBaseComPvt->tCurState == OMX_StatePause),
                    OMX_ErrorIncorrectStateOperation);

   OMX_BASE_REQUIRE(pBufHeader->pBuffer != NULL, OMX_ErrorBadParameter);

   /* queue up the buffer into the data pipe */
   TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
   if(pPort->bIsPortflushed == OMX_FALSE) {
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
       tStatus = TIMM_OSAL_WriteToPipe((pPort->dataPipe), &pBufHeader,
                                   sizeof(pBufHeader), TIMM_OSAL_SUSPEND);
       OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

       /* Invoke the process function to process the data in the data pipe */
       pBaseComInt->fpInvokeProcessFunction(hComponent, DATAEVENT);
    }
    /* Incase if the port is an buffer supplier, but flushing the buffers
     *  then don't process the buffers */
    else if(PORT_IS_BUFFER_SUPPLIER(pPort)) {
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
       tStatus = TIMM_OSAL_WriteToPipe((pPort->dataPipe), &pBufHeader,
                                   sizeof(pBufHeader), TIMM_OSAL_SUSPEND);
       OMX_BASE_ASSERT(tStatus == TIMM_OSAL_ERR_NONE, OMX_ErrorInsufficientResources);

    } else {
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);
       OMX_BASE_ASSERT(0, OMX_ErrorIncorrectStateOperation);
    }

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentTunnelRequest()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentTunnelRequest(OMX_IN  OMX_HANDLETYPE hComponent,
                                        OMX_IN  OMX_U32 nPort,
                                        OMX_IN  OMX_HANDLETYPE hTunneledComp,
                                        OMX_IN  OMX_U32 nTunneledPort,
                                        OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup)
{
   OMX_COMPONENTTYPE* pComp ;
   OMX_BASE_PRIVATETYPE* pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_PARAM_BUFFERSUPPLIERTYPE tBufSupplier;
   OMX_BASE_PORTTYPE *pPort ;
   OMX_ERRORTYPE tError = OMX_ErrorNone;

   OMX_BASE_Entering();

   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE*)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE*)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   pPort = pBaseComInt->ports[nPort];

   /* Cancel TunnelRequest incase if handle of the tunneled comp is NULL */
   if(pTunnelSetup == NULL || hTunneledComp == NULL) {
        pPort->hTunnelComp        = NULL;
        pPort->unTunnelPort       = PORT_NOT_TUNNELED;
        pPort->unTunnelFlags      = PORT_NOT_TUNNELED;
        pPort->tBufferSupplier    = OMX_BufferSupplyUnspecified;
        return OMX_ErrorNone;
   }

   pPort->hTunnelComp        = hTunneledComp;
   pPort->unTunnelPort       = nTunneledPort;

   /* Verify the Compatablity between two tunneled ports */
   tError = _OMX_BASE_VerifyTunnelConnection(pPort, hTunneledComp);
   OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorPortsNotCompatible);

   /* If this method is invoked on a Component that provides the
    * output port, indicates the port supplier preference */
   if(pPort->tPortDefParams.eDir == OMX_DirOutput) {
      pPort->unTunnelFlags |= PORT_IS_SUPPLIER;
      pPort->unTunnelFlags |= PORT_IS_TUNNELED;

        TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
        if(pBaseComPvt->tDerToBase.pTunPref[nPort]->eSupplier == OMX_BufferSupplyInput)
          pPort->tBufferSupplier = OMX_BufferSupplyInput;
        else
          pPort->tBufferSupplier = OMX_BufferSupplyOutput;

       pTunnelSetup->nTunnelFlags = pBaseComPvt->tDerToBase.pTunPref[nPort]->nTunnelFlags;
       pTunnelSetup->eSupplier = pPort->tBufferSupplier;
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

    }
   /* If this method is invoked on a component that provides the input port
    * 1. check for the data compatablity  2. Review the buffer supplier pref of
    *  o/p port and use set parametrs to inform o/p port which port supplies buffers*/
    else if(pPort->tPortDefParams.eDir  == OMX_DirInput) {

       /* If specified, obey output port preferences, otherwise choose output  */
       TIMM_OSAL_MutexObtain(pBaseComPvt->pMutex, TIMM_OSAL_SUSPEND);
       pPort->tBufferSupplier = pTunnelSetup->eSupplier;
       if(OMX_BufferSupplyUnspecified == pPort->tBufferSupplier) {
          pPort->tBufferSupplier = pTunnelSetup->eSupplier = OMX_BufferSupplyOutput;
       }
       /* Set the tunnel flag of input is supplier in case if the supplier is not an Outputport*/
       if(pPort->tBufferSupplier != OMX_BufferSupplyOutput)
          pPort->unTunnelFlags |= PORT_IS_SUPPLIER;

       if(pTunnelSetup->nTunnelFlags & OMX_PORTTUNNELFLAG_READONLY ) {
           pBaseComPvt->tDerToBase.pTunPref[nPort]->nTunnelFlags = pTunnelSetup->nTunnelFlags;
       }

       pPort->unTunnelFlags |= PORT_IS_TUNNELED;
       TIMM_OSAL_MutexRelease(pBaseComPvt->pMutex);

       /* Inform the output port, who the supplier is */
       OMX_BASE_INIT_STRUCT_PTR(&tBufSupplier, OMX_PARAM_BUFFERSUPPLIERTYPE);
       tBufSupplier.nPortIndex = nTunneledPort;
       tBufSupplier.eBufferSupplier = pPort->tBufferSupplier;

       tError = OMX_SetParameter(hTunneledComp,
                      OMX_IndexParamCompBufferSupplier, &tBufSupplier);
       if(tError != OMX_ErrorNone) {
            pPort->unTunnelFlags = PORT_NOT_TUNNELED;
            OMX_BASE_ASSERT(0, OMX_ErrorPortsNotCompatible);
       }
    }

EXIT:
   if(tError == OMX_ErrorPortsNotCompatible) {
	   pPort->hTunnelComp  =  NULL;
	   pPort->unTunnelPort =  PORT_NOT_TUNNELED;
   }
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentDeInit()     :
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentDeInit(OMX_IN OMX_HANDLETYPE hComponent)
{
   OMX_COMPONENTTYPE *pComp;
   OMX_BASE_PRIVATETYPE *pBaseComPvt;
   OMX_BASE_INTERNALTYPE* pBaseComInt;
   OMX_ERRORTYPE tError = OMX_ErrorNone;
   TIMM_OSAL_ERRORTYPE tStatus = TIMM_OSAL_ERR_NONE;

   OMX_BASE_Entering();

   /* Check for the input parameters */
   OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

   pComp = (OMX_COMPONENTTYPE *)hComponent;
   pBaseComPvt = (OMX_BASE_PRIVATETYPE *)pComp->pComponentPrivate;
   pBaseComInt = (OMX_BASE_INTERNALTYPE*)pBaseComPvt->hOMXBaseInt;

   /* De-initalize the Resources Allocated for Ports  */
   tError = _OMX_BASE_DeinitalizePorts(hComponent);
   OMX_BASE_ASSERT(OMX_ErrorNone == tError , tError);

   /* Destroy the Thread if the comp is running in a ACTIVE Context */
   if(pBaseComPvt->tDerToBase.ctxType == OMX_BASE_CONTEXT_ACTIVE){

	   pBaseComInt->fpInvokeProcessFunction(hComponent, ENDEVENT);

       tStatus = TIMM_OSAL_DeleteTask(pBaseComInt->pThreadId);
       OMX_BASE_ASSERT(OMX_ErrorNone == tStatus, OMX_ErrorInsufficientResources);

	   TIMM_OSAL_EventDelete(pBaseComInt->pEventToProcessThread);
   }

   /* Freeup the mutex/events allocated for sync while initalizing a component */
   tStatus = TIMM_OSAL_MutexDelete(pBaseComPvt->pMutex);
   OMX_BASE_ASSERT(OMX_ErrorNone == tStatus, OMX_ErrorInsufficientResources);

   TIMM_OSAL_EventDelete(pBaseComInt->hEvent);

   /* Destroy the Command Pipe used to Send Cmds to Component */
   tStatus = TIMM_OSAL_DeletePipe(pBaseComInt->cmdPipe);
   OMX_BASE_ASSERT(OMX_ErrorNone == tStatus , OMX_ErrorInsufficientResources);

   /* Free-up the Base Component Internal Memory Area */
   TIMM_OSAL_Free(pBaseComInt);
   pBaseComInt = NULL;

EXIT:
   OMX_BASE_Exiting(tError);
   return tError;
}

/**=========================================================================**/
/**          UN I M P L E M E N T E D       A P I'S
/**=========================================================================**/

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetConfig()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetConfig(OMX_IN  OMX_HANDLETYPE hComponent,
                                 OMX_IN  OMX_INDEXTYPE nIndex,
                                 OMX_INOUT OMX_PTR pComponentConfigStructure)
{
   OMX_ERRORTYPE tError = OMX_ErrorNoMore;
   OMX_BASE_ASSERT(0, OMX_ErrorNotImplemented);

EXIT:
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_SetConfig()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_SetConfig(OMX_IN  OMX_HANDLETYPE hComponent,
                                 OMX_IN  OMX_INDEXTYPE nIndex,
                                 OMX_IN  OMX_PTR pComponentConfigStructure)
{
   OMX_ERRORTYPE tError = OMX_ErrorNoMore;
   OMX_BASE_ASSERT(0, OMX_ErrorNotImplemented);

EXIT:
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_UseEGLImage()
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_UseEGLImage(OMX_IN OMX_HANDLETYPE hComponent,
                                   OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
                                   OMX_IN OMX_U32 nPortIndex,
                                   OMX_IN OMX_PTR pAppPrivate,
                                   OMX_IN void* eglImage)
{
   OMX_ERRORTYPE tError = OMX_ErrorNoMore;
   OMX_BASE_ASSERT(0, OMX_ErrorNotImplemented);

EXIT:
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetExtensionIndex()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetExtensionIndex(OMX_IN OMX_HANDLETYPE hComponent ,
                                         OMX_IN OMX_STRING  cParameterName,
                                         OMX_OUT OMX_INDEXTYPE *pIndexType)
{
   OMX_ERRORTYPE tError = OMX_ErrorNoMore;
   OMX_BASE_ASSERT(0, OMX_ErrorNotImplemented);

EXIT:
   return tError;
}

/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentRoleEnum()
 *
 * @ see omx_base.h
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentRoleEnum(OMX_IN OMX_HANDLETYPE hComponent,
                                         OMX_OUT OMX_U8 *cRole,
                                         OMX_IN OMX_U32 nIndex)
{
   OMX_ERRORTYPE tError = OMX_ErrorNoMore;
   OMX_BASE_ASSERT(0, OMX_ErrorNotImplemented);

EXIT:
   return tError;
}
