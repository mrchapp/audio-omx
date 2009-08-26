/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */



/******************************************************************************
* Includes
******************************************************************************/
/* ----- system and platform files ----------------------------*/
#include <string.h>
#include <dlfcn.h>
/*-------program files ----------------------------------------*/
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Audio.h>
#include <OMX_Types.h>
#include <OMX_Index.h>

#ifdef RESOURCE_MANAGER_ENABLED

#include <ResourceManagerProxyAPI.h>

#endif

#include "usn.h"
#include <omx_base.h>
#include <timm_osal_interfaces.h>
#include <omx_base_utils.h>
#include "TIDspOmx.h"
#include "LCML_DspCodec.h"

#include "omx_audioenc_aacutils.h"
#include "omx_audioencoderutils.h"





static count_buffer=0;													/*purely for debug purpose*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_DataNotify(OMX_HANDLETYPE hComponent,
                                              OMX_U32 nPortIndex,
                                              OMX_BUFFERHEADERTYPE* pBufHeader);

static OMX_ERRORTYPE OMX_AUDIO_ENC_CommandNotify(OMX_HANDLETYPE hComponent,
                                                 OMX_COMMANDTYPE Cmd,
                                                 OMX_U32 nParam,
                                                 OMX_PTR pCmdData);

static OMX_ERRORTYPE OMX_AUDIO_ENC_ComponentRoleEnum(OMX_HANDLETYPE,OMX_U8*,
                                                     OMX_U32);

static OMX_ERRORTYPE OMX_AUDIO_ENC_GetExtensionIndex(OMX_HANDLETYPE, OMX_STRING,
                                                     OMX_INDEXTYPE*);

static OMX_ERRORTYPE OMX_AUDIO_ENC_GetParameter(OMX_HANDLETYPE, OMX_INDEXTYPE,
                                                OMX_PTR);

static OMX_ERRORTYPE OMX_AUDIO_ENC_SetParameter(OMX_HANDLETYPE, OMX_INDEXTYPE,
                                                OMX_PTR);

static OMX_ERRORTYPE OMX_AUDIO_ENC_GetConfig(OMX_HANDLETYPE, OMX_INDEXTYPE,
                                             OMX_PTR);

static OMX_ERRORTYPE OMX_AUDIO_ENC_SetConfig(OMX_HANDLETYPE, OMX_INDEXTYPE,
                                             OMX_PTR);

static OMX_ERRORTYPE OMX_AUDIO_ENC_ComponentDeInit(OMX_HANDLETYPE);

static OMX_ERRORTYPE OMX_AUDIO_ENC_LcmlCallback(TUsnCodecEvent event,
                                                void * args [10]);

#ifdef RESOURCE_MANAGER_ENABLED

static void OMX_AUDIO_ENC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData);

#endif

#ifdef DEBUG
	#define DPRINT(...)    fprintf(stderr,__VA_ARGS__)			/* Debug prints */
#else
	#define DPRINT(...)
#endif
/* ========================================================================== */
/*
 * @fn OMX_ComponentInit() - Updates the component function pointer to the handle.
 *                           Sets default parameters.
 *
 *  @param hComponent        :Component handle.
 *
 *  @return                  :OMX_ERRORTYPE
 *                                OMX_ErrorNone on success
 *                                !OMX_ErrorNone on any failure
 */
/* ========================================================================== */

OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE hComponent)
{
    DPRINT("\nAUDIO: AudioEnc ComponentInit\n");
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    OMX_COMPONENTTYPE *pHandle = NULL;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    OMX_U32 i= 0;

	printf("entering ComponentInit\n");
    /* Check the input parameters */
    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);
    pHandle = (OMX_COMPONENTTYPE *)hComponent;
    OMX_BASE_CHK_VERSION(pHandle, OMX_COMPONENTTYPE, tError);

    /*Alocation and Initialization of the memory for the component private area*/
    pHandle->pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)TIMM_OSAL_Malloc
                                     (sizeof(OMX_AUDIOENC_COMPONENT_PRIVATE));
    OMX_BASE_ASSERT(pHandle->pComponentPrivate != NULL,
                    OMX_ErrorInsufficientResources);

    TIMM_OSAL_Memset(pHandle->pComponentPrivate, 0x0,
                     sizeof(OMX_AUDIOENC_COMPONENT_PRIVATE));
    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                         pHandle->pComponentPrivate;
    pComponentPrivate->pHandle = pHandle;
     /* Fill in all the parameters of Derived component to Base with Actual
        Values before invoking the  Base Compoent Init */
    pComponentPrivate->tDerToBase.cComponentName = (OMX_STRING )
                     TIMM_OSAL_Malloc(sizeof(OMX_U8) * OMX_MAX_STRINGNAME_SIZE);
    OMX_BASE_ASSERT(pComponentPrivate->tDerToBase.cComponentName != NULL,
                    OMX_ErrorInsufficientResources);
    strcpy(pComponentPrivate->tDerToBase.cComponentName, "OMX.TI.AUDIO.ENCODE");

     /* Initialize port params i.e no.of ports and startport num */
    OMX_BASE_INIT_STRUCT_PTR(&pComponentPrivate->tDerToBase.tPortParams,
                                                 OMX_PORT_PARAM_TYPE);
    pComponentPrivate->tDerToBase.tPortParams.nPorts = NUM_OF_PORTS;
    pComponentPrivate->tDerToBase.tPortParams.nStartPortNumber =
                                   DEFAULT_START_PORT_NUM;


    /* Create memory for the tunnel preference */
    pComponentPrivate->tDerToBase.pTunPref = (OMX_TUNNELSETUPTYPE **)
                           TIMM_OSAL_Malloc(sizeof(OMX_TUNNELSETUPTYPE *)
                                    *pComponentPrivate->tDerToBase.
                                      tPortParams.nPorts );
    OMX_BASE_ASSERT(pComponentPrivate->tDerToBase.pTunPref != NULL,
                                     OMX_ErrorInsufficientResources);
    for(i=0; i<pComponentPrivate->tDerToBase.tPortParams.nPorts; i++)
    {
        pComponentPrivate->tDerToBase.pTunPref[i] = (OMX_TUNNELSETUPTYPE*)
                           TIMM_OSAL_Malloc(sizeof(OMX_TUNNELSETUPTYPE));
        OMX_BASE_ASSERT(pComponentPrivate->tDerToBase.pTunPref[i] != NULL,
                                     OMX_ErrorInsufficientResources);
    }

    pComponentPrivate->tDerToBase.pTunPref[AUDIOENC_INPUT_PORT]->nTunnelFlags
                                                                 = 0;
    pComponentPrivate->tDerToBase.pTunPref[AUDIOENC_INPUT_PORT]->eSupplier
                                    = OMX_BufferSupplyUnspecified;
    pComponentPrivate->tDerToBase.pTunPref[AUDIOENC_OUTPUT_PORT]->nTunnelFlags
                                              = OMX_PORTTUNNELFLAG_READONLY;
    pComponentPrivate->tDerToBase.pTunPref[AUDIOENC_OUTPUT_PORT]->eSupplier
                                            = OMX_BufferSupplyUnspecified;
    /* Set the ContextType whether passive/Active depending on requirement */
    pComponentPrivate->tDerToBase.ctxType = DEFAULT_CONTEXT_TYPE;


    /* Fill in the Component Specification Version */
    pComponentPrivate->tDerToBase.tComVersion.s.nVersionMajor =
                                         OMX_AUDIO_ENC_COMP_VERSION_MAJOR;
    pComponentPrivate->tDerToBase.tComVersion.s.nVersionMinor =
                                         OMX_AUDIO_ENC_COMP_VERSION_MINOR;
    pComponentPrivate->tDerToBase.tComVersion.s.nRevision =
                                         OMX_AUDIO_ENC_COMP_VERSION_REVISION;
    pComponentPrivate->tDerToBase.tComVersion.s.nStep =
                                         OMX_AUDIO_ENC_COMP_VERSION_STEP;
    pComponentPrivate->tDerToBase.unGroupID  = OMX_AUDIO_ENC_GROUPID;

    /* Incase of Active Context, set the Stacksize and GroupPriority */
    if(pComponentPrivate->tDerToBase.ctxType  == OMX_BASE_CONTEXT_ACTIVE)
    {
        pComponentPrivate->tDerToBase.unStackSize =
                                   OMX_AUDIO_ENC_TASK_STACKSIZE;
        pComponentPrivate->tDerToBase.unGroupPrioirty  =
                            OMX_AUDIO_ENC_TASK_PRIORITY;
    }
    /* Setting command and data notify functions */
    pComponentPrivate->tDerToBase.fpCommandNotify = OMX_AUDIO_ENC_CommandNotify;
    pComponentPrivate->tDerToBase.fpDataNotify = OMX_AUDIO_ENC_DataNotify;
    /* Initializing the Base Component with the given number of ports */
    DPRINT("\nAUDIO: Base Init\n");
    tError = OMX_BASE_ComponentInit(hComponent);
    if(tError != OMX_ErrorNone)
    {
        goto EXIT;
    }


    /* Override the Function pointers of type OMX_COMPONENTTYPE which Base
            cannot handle and needs to be taken care by the Derived Component */
    /*===============================================*/
    /* Fill the function pointers which are specific to the this component  */

    pHandle->GetParameter      = OMX_AUDIO_ENC_GetParameter;
    pHandle->SetParameter      = OMX_AUDIO_ENC_SetParameter;
    pHandle->SetConfig         = OMX_AUDIO_ENC_SetConfig;
    pHandle->GetConfig         = OMX_AUDIO_ENC_GetConfig;
    pHandle->ComponentDeInit   = OMX_AUDIO_ENC_ComponentDeInit;
    pHandle->GetExtensionIndex = OMX_AUDIO_ENC_GetExtensionIndex;
    pHandle->ComponentRoleEnum = OMX_AUDIO_ENC_ComponentRoleEnum;

    /*portdefs have to be initialized by the derived component but memory for
      it is allocated by the base component*/

    for(i=0; i<pComponentPrivate->tDerToBase.tPortParams.nPorts; i++)
    {
        pComponentPrivate->bportdefsSet[i] = OMX_FALSE;
    }
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nSize =
                                           sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nPortIndex = 0x0;
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->eDir = OMX_DirInput;
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->bEnabled = OMX_TRUE;
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->bPopulated = 0;
    /*Input port will always have pcm format for encoders*/
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->format.audio.eEncoding =
                                                      OMX_AUDIO_CodingPCM;
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->eDomain =
                                                      OMX_PortDomainAudio;
    /*These 3 can be set only on setting the role*/
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferCountActual = 0;
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferCountMin = 0;
    pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferSize = 0;


    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nSize =
                                           sizeof(OMX_PARAM_PORTDEFINITIONTYPE);
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nPortIndex = 0x1;
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->eDir = OMX_DirOutput;
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->bEnabled = OMX_TRUE;
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->bPopulated = 0;
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->eDomain =
                                                       OMX_PortDomainAudio;
    /*These 4 can be set only on setting the role*/
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->format.audio.eEncoding =
                                                       OMX_AUDIO_CodingUnused;
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferCountActual = 0;
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferCountMin = 0;
    pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferSize = 0;

    pComponentPrivate->tLcmlParams.bLcmlParamsFreed = OMX_FALSE;
    pComponentPrivate->bNoIdleOnStop = OMX_FALSE;
    pComponentPrivate->inEOSReceived = 0;
    pComponentPrivate->bReturnOutputBufWithoutProcessing = OMX_FALSE;

    /*Creating internal data pipes - using 4 internal data pipes*/

    /*For all i/p buffers*/
    bReturnStatus = TIMM_OSAL_CreatePipe(&(pComponentPrivate->
                              tPipePtrs.pInDataPipe),
                              MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                              sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
                    OMX_ErrorContentPipeCreationFailed);
    /*For all o/p buffers*/
    bReturnStatus = TIMM_OSAL_CreatePipe(&(pComponentPrivate->
                              tPipePtrs.pOutDataPipe),
                              MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                              sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
                    OMX_ErrorContentPipeCreationFailed);
    /*All i/p buffers pending with lcml*/
    bReturnStatus = TIMM_OSAL_CreatePipe
                    (&(pComponentPrivate->tPipePtrs.pInBufPendingWithLcmlPipe),
                    MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                    sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
                    OMX_ErrorContentPipeCreationFailed);
    /*All o/p buffers pending with lcml*/
    bReturnStatus = TIMM_OSAL_CreatePipe
                    (&(pComponentPrivate->tPipePtrs.pOutBufPendingWithLcmlPipe),
                    MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                    sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,
                    OMX_ErrorContentPipeCreationFailed);

#ifdef RESOURCE_MANAGER_ENABLED
    /*Resource Manager*/
    tError = RMProxy_NewInitalize();
    if (tError != OMX_ErrorNone)
    {
        /*Error returned from loading ResourceManagerProxy thread*/
        goto EXIT;
    }

#endif

    /* These 3 can be set only on setting the role. Only allocating memory for
       them here */
    pComponentPrivate->pcmParams = (OMX_AUDIO_PARAM_PCMMODETYPE*)
                          TIMM_OSAL_Malloc(sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
    OMX_BASE_ASSERT(pComponentPrivate->pcmParams != NULL,
                    OMX_ErrorInsufficientResources);
    OMX_BASE_INIT_STRUCT_PTR(pComponentPrivate->pcmParams,
                             OMX_AUDIO_PARAM_PCMMODETYPE);
    pComponentPrivate->bpcmParamsSet = OMX_FALSE;
    pComponentPrivate->sDeviceString = (OMX_STRING *)
                                     TIMM_OSAL_Malloc(sizeof(OMX_STRING) * 100);
    TIMM_OSAL_Memset(pComponentPrivate->sDeviceString, 0,
                    (sizeof(OMX_STRING) * 100));
    for(i = 0; i<pComponentPrivate->tDerToBase.tPortParams.nPorts; i++)
    {
        OMX_BASE_INIT_STRUCT_PTR(&(pComponentPrivate->tAudioPortFormat[i]),
                                 OMX_AUDIO_PARAM_PORTFORMATTYPE);
        /* Initializing the portindex right now itself */
        pComponentPrivate->tAudioPortFormat[i].nPortIndex = i;
        /* Setting nIndex to 0 for all ports. All ports will support only one
           format - pcm for input and role specific(aac/amr etc.) for output */
        pComponentPrivate->tAudioPortFormat[i].nIndex = 0;
        pComponentPrivate->bAudioPortFormatSet[i] = OMX_FALSE;
    }

    /* Initialize the Component Role Information structure*/
    OMX_BASE_INIT_STRUCT_PTR(&pComponentPrivate->componentRole,
                                OMX_PARAM_COMPONENTROLETYPE);
    /*Initial role is undefined - role should be set by client via SetParameter*/
/*NOTE: Initial role cant be set to undefined since conformance tests expect the
component to change states all the way up to executing without setting the role
Thus we setting the default role to AAC and initializing its parameters. If the
client wants, it can query the current role (via GetParameter) and then set a
different role (via SetParameter) if he wants to*/
    /*strcpy((char *)(pComponentPrivate->componentRole.cRole),"undefined_role");
    pComponentPrivate->eComponentRole = UNDEFINED_ROLE;*/
    strcpy((char *)(pComponentPrivate->componentRole.cRole),"audio_encode.dsp.aac");
    tError = OMX_AUDIO_ENC_AAC_InitParams(hComponent);
    if(tError != OMX_ErrorNone)
    {
        goto EXIT;
    }

    DPRINT("\nAUDIO: Init done\n");
/* Not initializing role specific params(i.e. aacParams/amrParams etc.) That can
   be done only on setting the role */
EXIT:
   return tError;
}



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_ComponentRoleEnum() - This function allows the IL core to
 *                                         query a component for all the roles
 *                                         it supports. It enumerates (one role
 *                                         at a time) the component roles that a
 *                                         component supports.
 *
 *  @ param hComponent      :Handle of the component.
 *
 *  @ param cRole           :Component role string to be filled by the function.
 *
 *  @ param nIndex          :Index of the role being queried.
 */
/* ===========================================================================*/
static OMX_ERRORTYPE OMX_AUDIO_ENC_ComponentRoleEnum(OMX_HANDLETYPE hComponent,
                                                     OMX_U8 *cRole,
                                                     OMX_U32 nIndex)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) && (cRole != NULL),
                     OMX_ErrorBadParameter);

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    switch(nIndex)
    {
        case 0:
            strcpy((char *)cRole, "audio_encode.dsp.aac");
        break;
        /*Add other roles here*/
        default:
            tError = OMX_ErrorNoMore;
	        goto EXIT;
    }

EXIT:
    return tError;
};



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_GetExtensionIndex() - Returns index for vendor specific
 *                                         settings specific to AAC encoding.
 *
 * @ see OMX_core.h
 *
 */
/* ===========================================================================*/
static OMX_ERRORTYPE OMX_AUDIO_ENC_GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                                     OMX_STRING cParameterName,
                                                     OMX_INDEXTYPE* pIndexType)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                     (cParameterName != NULL),
                      OMX_ErrorBadParameter);

    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)hComponent;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate =
                   (OMX_AUDIOENC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_ENCODE:
            /* OMX_AUDIO_ENC_AAC_GetExtensionIndex will also call common audio
            GetExtensionIndex internally */
            tError = OMX_AUDIO_ENC_AAC_GetExtensionIndex(hComponent,
                                                         cParameterName,
                                                         pIndexType);
        break;
        /*Add other roles here*/
        default:
            tError = OMX_AUDIO_ENC_COMMON_GetExtensionIndex(hComponent,
                                                            cParameterName,
                                                            pIndexType);
    }
EXIT:
    return tError;
}



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_GetParameter() - This function gets the configuration
 *                                    parameters for the component.
 *
 *  @ calltype                            :Non - Blocking call.
 *
 *  @ param hComponent                    :Handle of the component.
 *
 *  @ param nParamIndex                   :Index of the strucutre to be filled.
 *
 *  @ param pComponentParameterStructure  :Pointer to the IL client allocated
 *                                         structure that the component fills
 *
 *  @ PreRequisites                       :This method can be invoked when the
 *                                         component is in Loaded State or on a
 *                                         port that is disabled.
 */
/* ===========================================================================*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_GetParameter (OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nParamIndex,
                                           OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                     (pComponentParameterStructure != NULL),
                      OMX_ErrorBadParameter);
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) !=
                        NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*GetParameter can't be invoked when the comp is in Invalid state */
    OMX_BASE_ASSERT(pComponentPrivate->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation);

    switch(pComponentPrivate->eComponentRole)
    {
        /* OMX_AUDIO_ENC_AAC_GetParameter will also call common audio GetParameter
           internally */
        case AAC_ENCODE:
            tError = OMX_AUDIO_ENC_AAC_GetParameter(hComponent, nParamIndex,
                                        pComponentParameterStructure);
        break;
        /*Add more roles here*/

        /* UNDEFINED_ROLE calls common audio GetParameter - to take care of
           initial case when role has not been set*/
        case UNDEFINED_ROLE:
            tError = OMX_AUDIO_ENC_COMMON_GetParameter(hComponent, nParamIndex,
                                                  pComponentParameterStructure);
        break;
        default:
            tError = OMX_ErrorUnsupportedIndex;
    }
    EXIT:
        return tError;
}



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_SetParameter() - This function gets the configuration
 *                                    parameters for the component.
 *
 *  @ calltype                            :Non - Blocking call.
 *
 *  @ param hComponent                    :Handle of the component.
 *
 *  @ param nParamIndex                   :Index of the strucutre to be filled.
 *
 *  @ param pComponentParameterStructure  :Pointer to the IL client allocated
 *                                         structure that the component fills
 *
 *  @ PreRequisites                       :This method can be invoked when the
 *                                         component is in Loaded State or on a
 *                                         port that is disabled.
 */
/* ===========================================================================*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_SetParameter (OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nParamIndex,
                                           OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;

    /* Check for the Input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                     (pComponentParameterStructure != NULL),
                      OMX_ErrorBadParameter);
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) !=
                                                   NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*Check prerequisites for the method*/
    OMX_BASE_ASSERT((pComponentPrivate->tCurState == OMX_StateLoaded) ||
     (pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->bEnabled == OMX_FALSE) ||
     (pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->bEnabled == OMX_FALSE),
      OMX_ErrorIncorrectStateOperation);

    switch(pComponentPrivate->eComponentRole)
    {
        /* OMX_AUDIO_ENC_AAC_SetParameter will also call common audio SetParameter
           internally */
        case AAC_ENCODE:
            tError = OMX_AUDIO_ENC_AAC_SetParameter(hComponent, nParamIndex,
                                                  pComponentParameterStructure);
        break;
        /*Add more roles here*/

        /* UNDEFINED_ROLE calls common audio SetParameter - to take care of
           initial case when role has not been set*/
        case UNDEFINED_ROLE:
            tError = OMX_AUDIO_ENC_COMMON_SetParameter(hComponent, nParamIndex,
                                                  pComponentParameterStructure);
        break;
        default:
            tError = OMX_ErrorUnsupportedIndex;
    }
    EXIT:
        return tError;
}



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_GetConfig() - This function gets the current configurations
 *                                 for the component.
 *
 *  @ calltype                         :Non - Blocking call.
 *
 *  @ param hComponent                 :Handle of the component.
 *
 *  @ param nConfigIndex               :Index of the strucutre to be filled.
 *
 *  @ param pComponentConfigStructure  :Pointer to the IL Client allocated structure
 *                                      that the component uses for initialization.
 *
 *  @ PreRequisites                    :This can be invoked when the component
 *                                      is in any state except Invalid state.
 */
/* ===========================================================================*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_GetConfig (OMX_HANDLETYPE hComponent,
                                       OMX_INDEXTYPE nConfigIndex,
                                       OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate;

    /* Check the input params */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pComponentConfigStructure != NULL), OMX_ErrorBadParameter);


    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /* GetConfig: cannot be performed when the comp is in invalid state  */
    if(pComponentPrivate->tCurState == OMX_StateInvalid)
    {
        tError = OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }


    switch(pComponentPrivate->eComponentRole)
    {
        /* Role specific GetConfogs can be added here. Each role specific
           GetConfig must call OMX_AUDIO_ENC_COMMON_GetConfig internally */

        /* Presently all audio encoders support only one config which is handled
           by the common GetConfig */

        default:
            tError = OMX_AUDIO_ENC_COMMON_GetConfig(hComponent, nConfigIndex,
                                                    pComponentConfigStructure);
    }

    EXIT:
        return tError;
}



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_SetConfig() - This function sets the current configurations
 *                                 for the component.
 *
 *  @ calltype                         :Non - Blocking call.
 *
 *  @ param hComponent                 :Handle of the component.
 *
 *  @ param nConfigIndex               :Index of the strucutre to be filled.
 *
 *  @ param pComponentConfigStructure  :Pointer to the IL Client allocated structure
 *                                      that the component uses for initialization.
 *
 *  @ PreRequisites                    :This can be invoked when the component
 *                                      is in any state except Invalid state.
 */
/* ===========================================================================*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_SetConfig (OMX_HANDLETYPE hComponent,
                                       OMX_INDEXTYPE nConfigIndex,
                                       OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate;

    /* Check the input params */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                    (pComponentConfigStructure != NULL), OMX_ErrorBadParameter);

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /* SetConfig cannot be performed when the comp is in invliad state  */
    if(pComponentPrivate->tCurState == OMX_StateInvalid)
    {
        tError =  OMX_ErrorIncorrectStateOperation;
        goto EXIT;
    }

    switch(pComponentPrivate->eComponentRole)
    {
        /* OMX_AUDIO_ENC_AAC_SetConfig will also call common audio SetConfig
           internally */
        case AAC_ENCODE:
            tError = OMX_AUDIO_ENC_AAC_SetConfig(hComponent, nConfigIndex,
                                                 pComponentConfigStructure);
        break;
        /*Add more roles here*/

        default:
            tError = OMX_AUDIO_ENC_COMMON_SetConfig (hComponent, nConfigIndex,
                                                     pComponentConfigStructure);
    }
    EXIT:
        return tError;
}



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_CommandNotify() - This method is an hook from the Base to
 *                                     to notify commands to the Derived component
 *                                     which need to be processed by it. Once
 *                                     the command is processed, notify the Base
 *                                     Component via ReturnEventNotify call to
 *                                     communicate with the IL client or
 *                                     tunneled component.
 *
 *  @ param hComponent       :Handle of the component to be accessed.
 *
 *  @ param Cmd              :Command event parameter
 *
 *  @ param nParam           :Flag indicating command completion
 *
 *  @ param pCmdData         :Flag indicating command completion
 */
/* ===========================================================================*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_CommandNotify(OMX_HANDLETYPE hComponent,
                                                 OMX_COMMANDTYPE Cmd,
                                                 OMX_U32 nParam,
                                                 OMX_PTR pCmdData)
{
    OMX_ERRORTYPE rm_error, tError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = NULL;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    OMX_HANDLETYPE pLcmlHandle = NULL;
    LCML_CALLBACKTYPE cb;
    LCML_DSP *pLcmlDsp;
    OMX_U16 arr[100] = {0};  /*only 18 values filled by init params for aac but
                               setting to 100 bec of old code*/

    /*Check for valid component handle*/
    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *)hComponent;
    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)pHandle->
                                                        pComponentPrivate;
    /*Flag - to decide whether to give callback to base immediately or in the
      LCML callback function*/
    OMX_BOOL bGiveCallback = OMX_TRUE;

    OMX_U32 aParam[3] = {0};
    TIMM_OSAL_U32 elementsInPipe = 0;
    TIMM_OSAL_U32 elementsOutPipe = 0;

    /* These two directly from old code - might even be dont cares,
       sent to lcml in control codec calls */
    char *p  = "start";
    char *pArgs = "damedesuStr";
#ifdef RESOURCE_MANAGER_ENABLED
    /* Used in sending RM commands */
    OMX_COMPONENTINDEXTYPE eComponentNameForRM;
#endif
    OMX_U32 CPULoadForRM;

    /* Check for valid component private handle */
    OMX_BASE_REQUIRE(pComponentPrivate != NULL, OMX_ErrorBadParameter);

    pLcmlHandle = pComponentPrivate->tLcmlParams.pLcmlHandle;
    cb.LCML_Callback = (void *)OMX_AUDIO_ENC_LcmlCallback;

    switch(Cmd)
    {
        case OMX_CommandStateSet:

            switch(pComponentPrivate->eComponentRole)
            {
                case AAC_ENCODE:
                #ifdef RESOURCE_MANAGER_ENABLED
                    eComponentNameForRM = OMX_AAC_Encoder_COMPONENT;
                    CPULoadForRM = 45;
                #endif
                break;
                default:
                    tError = OMX_ErrorUnsupportedIndex;
                    goto EXIT;
                break;
            }

            /*Check that cur and new state are not equal*/
            if (pComponentPrivate->tCurState == pComponentPrivate->tNewState)
            {
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventError,
                                                       OMX_ErrorSameState,
                                                       0, NULL);
            }

            /* Loaded ------> Idle */

            else if((pComponentPrivate->tCurState == OMX_StateLoaded) &&
                    (pComponentPrivate->tNewState == OMX_StateIdle))
            {
                /* Getting the lcml handle */
                pLcmlHandle = (OMX_HANDLETYPE)
                              OMX_AUDIO_ENC_GetLCMLHandle(pComponentPrivate);
                if(pLcmlHandle == NULL)
                {
                    tError = OMX_ErrorInsufficientResources;
                    goto EXIT;
                }
                pLcmlDsp = (((LCML_DSP_INTERFACE*)pLcmlHandle)->dspCodec);
                pComponentPrivate->tLcmlParams.pLcmlHandle =
                                   (LCML_DSP_INTERFACE *)pLcmlHandle;
                /*Set some default codec specific values before moving to idle*/
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_ENCODE:
                        tError = OMX_AUDIO_ENC_AAC_FillLCMLInitParams(hComponent,
                                                                      pLcmlDsp,
                                                                      arr);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }
                    break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                    break;
                }
                tError = LCML_InitMMCodecEx(((LCML_DSP_INTERFACE *)
                                           pLcmlHandle)->pCodecinterfacehandle,
                                           p, &pLcmlHandle, (void *)p, &cb,
                                           (OMX_STRING)pComponentPrivate->
                                                       sDeviceString);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
#ifdef RESOURCE_MANAGER_ENABLED
                /* Need check the resource with RM */
                pComponentPrivate->rmproxyCallback.RMPROXY_Callback = (void *)
                                          OMX_AUDIO_ENC_ResourceManagerCallback;

                rm_error = RMProxy_NewSendCommand(pHandle,
                                                 RMProxy_RequestResource,
                                                 eComponentNameForRM,
                                                 CPULoadForRM, 3456,
                                                 &(pComponentPrivate->
                                                              rmproxyCallback));
                if(rm_error == OMX_ErrorNone)
                {
                    /* resource is available */
                    rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet,
                                eComponentNameForRM, OMX_StateIdle, 3456, NULL);
                }
                else if(rm_error == OMX_ErrorInsufficientResources)
                {
                    /*Resource is not available, send error to client. It is
                      now the client's responsibility to transition to the
                      WaitForResources state if it wants to. Base does not allow
                      the component to move to WaitForResources automatically*/
                    tError = rm_error;
                    pComponentPrivate->fpReturnEventNotify(pHandle,
                                                           OMX_EventError,
                                                           tError,
                                                           0, NULL);
                    goto EXIT;
                }
#endif
            }

            /* Executing ------> Idle */

            else if((pComponentPrivate->tCurState == OMX_StateExecuting) &&
                    (pComponentPrivate->tNewState == OMX_StateIdle))
            {
                DPRINT("\nAUDIO: Moving from executing to idle inside OMX_AUDIO_ENC_CommandNotify\n");
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                      pCodecinterfacehandle, MMCodecControlStop, (void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
                /*Return all paused pending (if any)*/
                tError = OMX_AUDIO_ENC_ReturnPendingBuffers(pComponentPrivate,
                                                            DATA_PIPE, OMX_ALL);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }

                /* Callback to base and setting of cur state to idle will be
                   done on receiving lcml callback. Buffers pending with lcml
                   will also be returned there */
                bGiveCallback = OMX_FALSE;
            }

            /* Pause ------> Idle */

            else if((pComponentPrivate->tCurState == OMX_StatePause) &&
                    (pComponentPrivate->tNewState == OMX_StateIdle))
            {

				DPRINT("\n%d AUDIO: About to move from Pause to Idle\n",__LINE__);
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                       pCodecinterfacehandle, MMCodecControlStop,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
                /*Return all paused pending*/
                tError = OMX_AUDIO_ENC_ReturnPendingBuffers(pComponentPrivate,
                                                            DATA_PIPE, OMX_ALL);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }

                /* Callback to base and setting of cur state to idle will be
                   done on receiving lcml callback. Buffers pending with lcml
                   (if any) will also be returned there */
                bGiveCallback = OMX_FALSE;
            }

            /* Idle ------> Executing */

            else if((pComponentPrivate->tCurState == OMX_StateIdle) &&
                    (pComponentPrivate->tNewState == OMX_StateExecuting))
            {
                DPRINT("\nAUDIO: About to move to executing\n");
                /*Some values need to be set before transitioning to executing*/
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_ENCODE:
                        tError =
                               OMX_AUDIO_ENC_AAC_SendParamsToCodecForTransitionToExecuting
                               (hComponent);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }
                    break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                    break;
                }

                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                            pCodecinterfacehandle,
                                            EMMCodecControlStart,(void *)pArgs);
                DPRINT("\nAUDIO: Codec start command given to LCML\n");
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
#ifdef RESOURCE_MANAGER_ENABLED
                rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet,
                                                  eComponentNameForRM,
                                                  OMX_StateExecuting,
                                                  3456, NULL);
#endif
            }

            /* Pause ------> Executing */

            else if((pComponentPrivate->tCurState == OMX_StatePause) &&
                    (pComponentPrivate->tNewState == OMX_StateExecuting))
            {
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                            pCodecinterfacehandle,
                                            EMMCodecControlStart,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
#ifdef RESOURCE_MANAGER_ENABLED
                rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet,
                                                  eComponentNameForRM,
                                                  OMX_StateExecuting,
                                                  3456, NULL);

#endif
                /*Giving callback here so that state can be set to executing
                  by the base before we start processing paused data buffers*/
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventCmdComplete,
                                                       OMX_CommandStateSet,
                                                       nParam,
                                                       NULL);
                bGiveCallback = OMX_FALSE;
                /*Send all paused pending buffers to lcml*/
                TIMM_OSAL_GetPipeReadyMessageCount(pComponentPrivate->tPipePtrs.
                                                pInDataPipe, &elementsInPipe);
                TIMM_OSAL_GetPipeReadyMessageCount(pComponentPrivate->tPipePtrs.
                                                pOutDataPipe, &elementsOutPipe);
                /* Sending buffers one by one because DataNotify expects one
                   buffer at a time */
                while(elementsInPipe || elementsOutPipe)
                {
                    if(elementsInPipe)
                    {
                        tError = OMX_AUDIO_ENC_DataNotify(pHandle,
                                                          PAUSED_TO_EXECUTING,
                                                          NULL);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }
                        elementsInPipe--;
                    }
                    if(elementsOutPipe)
                    {
                        tError = OMX_AUDIO_ENC_DataNotify(pHandle,
                                                          PAUSED_TO_EXECUTING,
                                                          NULL);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }
                        elementsOutPipe--;
                    }
                }
            }

            /* Idle ------> Pause */
            /* Executing ------> Pause */

            else if(((pComponentPrivate->tCurState == OMX_StateIdle) ||
                     (pComponentPrivate->tCurState == OMX_StateExecuting)) &&
                     (pComponentPrivate->tNewState == OMX_StatePause))
            {
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                            pCodecinterfacehandle,
                                            EMMCodecControlPause,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
                /* Returning EventNotify to base and rm call is done in
                   lcml callback */
                bGiveCallback = OMX_FALSE;
            }


            /* Loaded ------> WaitForResources */

            else if((pComponentPrivate->tCurState == OMX_StateLoaded) &&
                    (pComponentPrivate->tNewState == OMX_StateWaitForResources))
            {
				#ifdef RESOURCE_MANAGER_ENABLED
                rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet,
                                                  eComponentNameForRM,
                                                  OMX_StateWaitForResources,
                                                  3456, NULL);
            	#endif
            }

            /* WaitForResources ------> Loaded */

            else if((pComponentPrivate->tCurState == OMX_StateWaitForResources)
                    && (pComponentPrivate->tNewState == OMX_StateLoaded))
            {
                /*Nothing to be done. Just make callback to the base*/
            }

            /* Idle ------> Loaded */

            else if((pComponentPrivate->tCurState == OMX_StateIdle) &&
                    (pComponentPrivate->tNewState == OMX_StateLoaded))
            {
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                         pCodecinterfacehandle,
                                         EMMCodecControlDestroy, (void *)pArgs);
                if (tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
                /*Clean up lcml init params*/
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_ENCODE:
                        tError = OMX_AUDIO_ENC_AAC_FreeLCMLInitParams(pHandle);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }
                        pComponentPrivate->tLcmlParams.bLcmlParamsFreed = OMX_TRUE;
                    break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                    break;
                }
                if (pComponentPrivate->tLcmlParams.ptrLibLCML != NULL)
                {
                    /* Linux kernel call as dlclose not supported by OSAL */
                    dlclose(pComponentPrivate->tLcmlParams.ptrLibLCML);
                    pComponentPrivate->tLcmlParams.ptrLibLCML = NULL;
                }
#ifdef RESOURCE_MANAGER_ENABLED
                tError = RMProxy_NewSendCommand(pHandle, RMProxy_FreeResource,
                                            eComponentNameForRM, 0, 3456, NULL);
                if (tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
#endif
            }

            /* Any State ------> Invalid */

            else if(pComponentPrivate->tNewState == OMX_StateInvalid)
            {
                if (pComponentPrivate->tCurState != OMX_StateWaitForResources &&
                             pComponentPrivate->tCurState != OMX_StateInvalid &&
                             pComponentPrivate->tCurState != OMX_StateLoaded)
                {
                    tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)
                                                 pLcmlHandle)->
                                                 pCodecinterfacehandle,
                                                 EMMCodecControlDestroy,
                                                 (void *)pArgs);
#ifdef RESOURCE_MANAGER_ENABLED
                     tError = RMProxy_NewSendCommand(pHandle,
                                                    RMProxy_FreeResource,
                                                    eComponentNameForRM,
                                                    0, 3456, NULL);
#endif
                }
                /* Base will return the error event */
            }

            /* Incorrect State Transition */
            else
            {
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                              OMX_EventError,
                                              OMX_ErrorIncorrectStateTransition,
                                              0, NULL);
            }
            /*Give EventNotify callback*/
            if(bGiveCallback == OMX_TRUE)
            {
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventCmdComplete,
                                                       OMX_CommandStateSet,
                                                       nParam,
                                                       NULL);
            }
            bGiveCallback = OMX_TRUE; /*Just resetting the value*/
        break;
        case OMX_CommandPortEnable:
            if(((pComponentPrivate->tCurState == OMX_StateExecuting) &&
               ((nParam == OMX_DirOutput) || (nParam == OMX_ALL))))
            {
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                            pCodecinterfacehandle,
                                            EMMCodecControlStart,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            pComponentPrivate->fpReturnEventNotify(hComponent,
                                                   OMX_EventCmdComplete,
                                                   Cmd, nParam, NULL);
        break;
        case OMX_CommandPortDisable:
            DPRINT("\nAUDIO: PortDisable command received\n");
            /* Codec to be stopped if state is executing and o/p port is
               disabled. Codec is not stopped if only i/p port is disabled so
               that if the codec is currently processing any o/p buffers then
               these can be returned if the o/p port is enabled. */
            if((pComponentPrivate->tCurState == OMX_StateExecuting) &&
               ((nParam == OMX_DirOutput) || (nParam == OMX_ALL)))
            {
                /* To ensure that LCML callback doesn't cause transition
                   to idle on stopping */
                pComponentPrivate->bNoIdleOnStop = OMX_TRUE;
                DPRINT("\nAUDIO: Sending stop commmand in port disable\n");
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                             pCodecinterfacehandle,
                                             MMCodecControlStop,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            /*Returning paused pending buffers*/
            tError = OMX_AUDIO_ENC_ReturnPendingBuffers(pComponentPrivate,
                                                        DATA_PIPE, OMX_ALL);
            if(tError != OMX_ErrorNone)
            {
                goto EXIT;
            }
            DPRINT("\nAUDIO: Sending return event notify to base\n");
            pComponentPrivate->fpReturnEventNotify(hComponent,
                                                   OMX_EventCmdComplete,
                                                   Cmd, nParam, NULL);
        break;
        case OMX_CommandFlush:
          /* Send back all paused pending buffers */
            if(nParam == OMX_DirInput || nParam == OMX_ALL)
            {
                tError = OMX_AUDIO_ENC_ReturnPendingBuffers(pComponentPrivate,
                                                    DATA_PIPE, OMX_DirInput);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }

                aParam[0] = USN_STRMCMD_FLUSH;
                aParam[1] = 0x0;
                aParam[2] = 0x0;

                tError=LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                         pCodecinterfacehandle,
                                         EMMCodecControlStrmCtrl,(void*)aParam);
                if (tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            if(nParam == OMX_DirOutput || nParam == OMX_ALL)
            {
                tError = OMX_AUDIO_ENC_ReturnPendingBuffers(pComponentPrivate,
                                                    DATA_PIPE, OMX_DirOutput);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }

                aParam[0] = USN_STRMCMD_FLUSH;
                aParam[1] = 0x1;
                aParam[2] = 0x0;

                tError=LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                         pCodecinterfacehandle,
                                         EMMCodecControlStrmCtrl,(void*)aParam);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            /* In lcml callback send back pending with lcml buffers */
        break;
        case OMX_CommandMarkBuffer:
            /*Should never reach here - this is taken care of by base*/
            tError = OMX_ErrorUndefined;
            goto EXIT;
        break;
        default:
        break;
    }
    EXIT:
        return tError;
}



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_DataNotify() - This method is an hook from the Base to notify
 *                                  the Derived component of data either input
 *                                  or output buffers which need to be processed
 *                                  by it. Once the buffer is processed, notify
 *                                  the Base Component via ReturnDataNotify call
 *                                  to communicate with the IL client or
 *                                  tunneled component.
 *
 * @param hComponent     :Handle of the component to be accessed.
 *
 * @param nPortIndex     :Index of the port
 *
 * @param pBufHeader     :Pointer to an Bufferheader which need to be processed
 */
/* ===========================================================================*/
static OMX_ERRORTYPE OMX_AUDIO_ENC_DataNotify(OMX_HANDLETYPE hComponent,
                                              OMX_U32 nPortIndex,
                                              OMX_BUFFERHEADERTYPE* pBufHeader)
{
    DPRINT("\nAUDIO: 1st line of data notify for %lu\n",nPortIndex);
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    OMX_COMPONENTTYPE *pComp;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;
    /* void pointers so that they can be mapped to auxInfo struct of any codec */
    void *inAuxInfo = NULL;
    void *outAuxInfo = NULL;

    OMX_S32 inAuxInfoLen = 0;
    OMX_S32 outAuxInfoLen = 0;
    OMX_BUFFERHEADERTYPE* pInBufHeader = NULL;
    OMX_BUFFERHEADERTYPE* pOutBufHeader = NULL;
    TIMM_OSAL_U32 actualSize = 0;
    OMX_ERRORTYPE inStatus, outStatus;

/*Assuming that corresponding i/p and o/p buffers will be recd.*/

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);

    pComp = (OMX_COMPONENTTYPE *)hComponent;
    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)pComp->pComponentPrivate;
    pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->tLcmlParams.pLcmlHandle;

    if(nPortIndex != PAUSED_TO_EXECUTING)
    {
        OMX_BASE_REQUIRE((pBufHeader != NULL), OMX_ErrorBadParameter);
    /* Store the buffer to an Internal datapipe of an Input port,
       which will be processed later */
        if(nPortIndex == AUDIOENC_INPUT_PORT)
        {
            tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe
                                    ((pComponentPrivate->tPipePtrs.pInDataPipe),
                                    &pBufHeader, sizeof(pBufHeader),
                                    TIMM_OSAL_NO_SUSPEND);
            OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
        }
    /* Storing the buffer to an Internal datapipe of output port,
       which will be processed later */
        else if(nPortIndex == AUDIOENC_OUTPUT_PORT)
        {
            tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe
                                    ((pComponentPrivate->tPipePtrs.pOutDataPipe),
                                    &pBufHeader, sizeof(pBufHeader),
                                    TIMM_OSAL_NO_SUSPEND);
            OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
        }
    }

    /* Start buffer processing only when the comp is in Executing state or
       Paused to Executing command has been given */
    if(pComponentPrivate->tCurState == OMX_StateExecuting ||
       nPortIndex == PAUSED_TO_EXECUTING)
    {
        /* Check for whether the input and output pipes are ready  */
        inStatus  = (OMX_ERRORTYPE)TIMM_OSAL_IsPipeReady
                                   (pComponentPrivate->tPipePtrs.pInDataPipe);
        outStatus = (OMX_ERRORTYPE)TIMM_OSAL_IsPipeReady
                                   (pComponentPrivate->tPipePtrs.pOutDataPipe);

        if(inStatus == TIMM_OSAL_ERR_NONE)
        {
            /* Reading an input buffer from input data pipe to process  */
            TIMM_OSAL_ReadFromPipe((pComponentPrivate->tPipePtrs.pInDataPipe),
                                    &pInBufHeader, sizeof(pInBufHeader),
                                    &actualSize, TIMM_OSAL_SUSPEND);
            OMX_BASE_ASSERT(pInBufHeader != NULL,
                            OMX_ErrorInsufficientResources);

            if(pInBufHeader->nFilledLen > 0 ||
               pInBufHeader->nFlags == OMX_BUFFERFLAG_EOS)
            {
                if(pInBufHeader->nFlags == OMX_BUFFERFLAG_EOS)
                {
                /* This is to keep the info that we have received an EOS input
                   buffer which needs to informed to output buffer also. */
                    pComponentPrivate->inEOSReceived = 1;
                    DPRINT("\nAUDIO: EOS Received\n");
                pInBufHeader->nFlags = 0;
                }

                /* To set the inAuxInfo */
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_ENCODE:
                        pComponentPrivate->pAacPrivateParams->inBufParam
                          [pComponentPrivate->pAacPrivateParams->posInBufParam].
                            bLastBuffer = pComponentPrivate->inEOSReceived;

                        inAuxInfo = (AACENC_UAlgInBufParamStruct *)
                                 (&(pComponentPrivate->pAacPrivateParams->
                                    inBufParam[pComponentPrivate->
                                            pAacPrivateParams->posInBufParam]));
                        inAuxInfoLen = sizeof(AACENC_UAlgInBufParamStruct);

                        pComponentPrivate->pAacPrivateParams->posInBufParam++;
                        if(pComponentPrivate->pAacPrivateParams->posInBufParam >=
                                                        MAX_NUM_OF_BUFS)
                        {
                            pComponentPrivate->pAacPrivateParams->posInBufParam -=
                                                         MAX_NUM_OF_BUFS;
                        }
                    break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                    break;
                }

                /* Write to buffer pending with lcml pipe */


               /*purely for debug purpose*/

                tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe((pComponentPrivate->
                                      tPipePtrs.pInBufPendingWithLcmlPipe),
                                      &pInBufHeader, sizeof(pInBufHeader),
                                      TIMM_OSAL_NO_SUSPEND);
                OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);


            	/* Sending buffer to LCML */
				DPRINT("\nAUDIO: Sending i/p buf to lcml\n");
				tError = LCML_QueueBuffer(pLcmlHandle->pCodecinterfacehandle,
				                         EMMCodecInputBuffer,
				                         (OMX_U8 *)pInBufHeader->pBuffer,
				                         pInBufHeader->nAllocLen,
				                         pInBufHeader->nFilledLen,
				                         (OMX_U8 *)(inAuxInfo),
				                          inAuxInfoLen, NULL);
				if (tError != OMX_ErrorNone)
				{
				 	tError = OMX_ErrorHardware;
				 	goto EXIT;
				}

            }
            else
/*To handle empty buffers - To send back both i/p and correspondinig o/p buffers*/
            {
                pComponentPrivate->bReturnOutputBufWithoutProcessing = OMX_TRUE;
                pComponentPrivate->fpReturnDataNotify (hComponent,
                                                       OMX_DirInput,
                                                       pInBufHeader);
            }
        }

        if(outStatus == TIMM_OSAL_ERR_NONE)
        {
            /* At this point we have an input buffer,
               needs an output buffer to process */
            TIMM_OSAL_ReadFromPipe((pComponentPrivate->tPipePtrs.pOutDataPipe),
                                    &pOutBufHeader, sizeof(pOutBufHeader),
                                    &actualSize, TIMM_OSAL_SUSPEND);
            OMX_BASE_ASSERT(pOutBufHeader != NULL,
                            OMX_ErrorInsufficientResources);
            /*Return o/p buffer bec corresponding i/p buffer was empty*/
            if(pComponentPrivate->bReturnOutputBufWithoutProcessing == OMX_TRUE)
            {
                pComponentPrivate->fpReturnDataNotify (hComponent,
                                                       OMX_DirOutput,
                                                       pOutBufHeader);
                /* Resetting the flag */
                pComponentPrivate->bReturnOutputBufWithoutProcessing = OMX_FALSE;
            }
            else
            {
            /* Setting the outAuxInfo */
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_ENCODE:
                    /* Resetting the value  for each time*/
                        pComponentPrivate->pAacPrivateParams->outBufParam
                         [pComponentPrivate->pAacPrivateParams->posOutBufParam].
                         unNumFramesEncoded = 0;

                        outAuxInfo = (AACENC_UAlgOutBufParamStruct *)
                                 (&(pComponentPrivate->pAacPrivateParams->
                                    outBufParam[pComponentPrivate->
                                           pAacPrivateParams->posOutBufParam]));
                        outAuxInfoLen = sizeof(AACENC_UAlgOutBufParamStruct);

                        pComponentPrivate->pAacPrivateParams->posOutBufParam++;
                        if(pComponentPrivate->pAacPrivateParams->posOutBufParam >=
                                                        MAX_NUM_OF_BUFS)
                        {
                            pComponentPrivate->pAacPrivateParams->posOutBufParam -=
                                                         MAX_NUM_OF_BUFS;
                        }
                    break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                    break;
                }

            /* Sending output buffer to LCML */
                DPRINT("\nAUDIO: Sending o/p buf to lcml\n");
                tError = LCML_QueueBuffer(pLcmlHandle->pCodecinterfacehandle,
                                          EMMCodecOuputBuffer,
                                          (OMX_U8 *)pOutBufHeader->pBuffer,
                                          pOutBufHeader->nAllocLen,
                                          pOutBufHeader->nFilledLen,
                                          (OMX_U8 *)(outAuxInfo),
                                          outAuxInfoLen, NULL);
                if (tError != OMX_ErrorNone)
                {
                    tError = OMX_ErrorHardware;
                    goto EXIT;
                }
                /* Write to buffer pending with lcml pipe */
                tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe((pComponentPrivate->
                                    tPipePtrs.pOutBufPendingWithLcmlPipe),
                                    &pOutBufHeader, sizeof(pOutBufHeader),
                                    TIMM_OSAL_NO_SUSPEND);
                OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
            }
        }


        /* Handling an EOS flag */
#if 0
		if((pComponentPrivate->inEOSReceived)
           && (outStatus == TIMM_OSAL_ERR_NONE))
        {
            DPRINT("\nAUDIO: Transferring EOS to o/p bufheader\n");
            pComponentPrivate->inEOSReceived = 0;
            pOutBufHeader->nFlags = OMX_BUFFERFLAG_EOS;

            /* Notify EOS event flag to the Base Component via ReturnEventNotify
                               call , which communicates with the IL Client  */
            pComponentPrivate->fpReturnEventNotify(hComponent,
                                                OMX_EventBufferFlag,
                                                pOutBufHeader->nOutputPortIndex,
                                                pOutBufHeader->nFlags, NULL);
        }
#endif
        /* Handling a Mark Buffer  */
        if((outStatus == TIMM_OSAL_ERR_NONE) &&
           (pComponentPrivate->pMark != NULL))
        {
            pOutBufHeader->hMarkTargetComponent =
                           pComponentPrivate->pMark->hMarkTargetComponent;
            pOutBufHeader->pMarkData = pComponentPrivate->pMark->pMarkData;
            pComponentPrivate->pMark = NULL;
        }

        /* send an Mark Event incase if the target comp is same as this comp */
        if((inStatus == TIMM_OSAL_ERR_NONE) &&
           (pInBufHeader->hMarkTargetComponent == pComp))
        {
            /*  Notifyt this Mark Event to Base Comp via ReturnEventNotfiy
                           call to communicate with  the IL client */
            pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventMark, 0,
                                                   0, pInBufHeader->pMarkData);
        }

        /* Propogating the mark from an input buffer to the output buffer */
        if((inStatus == TIMM_OSAL_ERR_NONE) &&
           (pInBufHeader->hMarkTargetComponent != NULL))
        {
            if(outStatus == TIMM_OSAL_ERR_NONE)
            {
                pOutBufHeader->hMarkTargetComponent =
                               pInBufHeader->hMarkTargetComponent;
                pOutBufHeader->pMarkData = pInBufHeader->pMarkData;
            }
            pInBufHeader->pMarkData = NULL;
        }
    }
    EXIT:
        return tError;
}



/* ===========================================================================*/
/*
 *  OMX_AUDIO_ENC_Lcml_Callback() - This function will be called by LCML
 *                                  component thread.
 *
 * @param event                 :Event which gives to details about USN status
 *
 * @param void * args           :args [0] BufferType;
 *                              :args [1] ARM address fur buffer
 *                              :args [2] BufferSize;
 *                              :args [3] ARM address for param
 *                              :args [4] ParamSize;
 *                              :args [6] LCML Handle
 */
/* ===========================================================================*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_LcmlCallback(TUsnCodecEvent event,
                                                void * args [10])
{
    OMX_COMPONENTTYPE *pHandle=NULL;
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_U8 *pBuffer = args[1];
    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;
    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;
    LCML_DSP_INTERFACE *pLcmlHandle = NULL;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    TIMM_OSAL_U32 actualSize = 0;
    char *pArgs = "damedesuStr";

#ifdef RESOURCE_MANAGER_ENABLED
    OMX_ERRORTYPE rm_error = OMX_ErrorNone;
    OMX_COMPONENTINDEXTYPE eComponentNameForRM;
#endif
    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE*)
                        (((LCML_DSP_INTERFACE *)args[6])->pComponentPrivate);

    pHandle = pComponentPrivate->pHandle;

    switch(event)
    {
        case EMMCodecDspError:
        /* Checking for MMU_fault */
            if((args[4] == (void*)USN_ERR_UNKNOWN_MSG) &&
               (args[5] == (void*)NULL))
            {
               /*State changed to OMX_StateInvalid*/
                pComponentPrivate->tCurState = OMX_StateInvalid;
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventError,
                                                       OMX_ErrorInvalidState,
                                                       0x2, NULL);
            }

			DPRINT("\n%d AUDIO: args[4] =%d args[5] = %d \n",__LINE__,args[4],args[5]);
            if(((int)args[4] == USN_ERR_WARNING)
               && ((int)args[5] == IUALG_WARN_PLAYCOMPLETED))
            {
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventBufferFlag,
                                                       (OMX_U32)NULL,
                                                       OMX_BUFFERFLAG_EOS,NULL);
            }

            if((int)args[5] == IUALG_ERR_GENERAL)
            {
            /*Algorithm error. Cannot continue*/
                pLcmlHandle = (LCML_DSP_INTERFACE *)
                               pComponentPrivate->tLcmlParams.pLcmlHandle;

			/*Later replace this with the event notify to the application*/
				DPRINT("\n%d AUDIO: IUALG_ERR_GENERAL\n",__LINE__);
  #if 0
  				tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                             pCodecinterfacehandle,
                                             MMCodecControlStop,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                /*Error Occurred in Codec Stop*/
                    goto EXIT;
                }
                /* Giving command to move to idle as that is what omap3 code does
                  Shouldn't it move to Invalid? */
                tError = pHandle->SendCommand(pHandle,OMX_CommandStateSet,
                                              OMX_StateIdle,NULL);

				if(tError != OMX_ErrorNone)
                {
                    pComponentPrivate->fpReturnEventNotify(pHandle,
                                                           OMX_EventError,
                                                           tError,0,NULL);
                }
#endif
			}

            if( (int)args[5] == IUALG_ERR_DATA_CORRUPT )
            {
            /*Algorithm error. Corrupt data*/
                pLcmlHandle = (LCML_DSP_INTERFACE *)
                              pComponentPrivate->tLcmlParams.pLcmlHandle;

				DPRINT("\n%d AUDIO: IUALG_ERR_DATA_CORRUPT\n",__LINE__);
				tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                             pCodecinterfacehandle,
                                             MMCodecControlStop,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    /*Error Occurred in Codec Stop..*/
                    goto EXIT;
                }
                /* Giving command to move to idle as that is what omap3 code does
                  Shouldn't it move to Invalid? */
                tError = pHandle->SendCommand(pHandle,OMX_CommandStateSet,
                                              OMX_StateIdle,NULL);
                if(tError != OMX_ErrorNone)
                {
                    pComponentPrivate->fpReturnEventNotify(pHandle,
                                                           OMX_EventError,
                                                           tError,0,NULL);
                }
            }
        break;

        case EMMCodecInternalError:
        /* Cheking for MMU_fault */
            if((args[4] == (void*)USN_ERR_UNKNOWN_MSG)
               && (args[5] == (void*)NULL))
            {
                pComponentPrivate->tCurState = OMX_StateInvalid;
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventError,
                                                       OMX_ErrorInvalidState,
                                                       0x2, NULL);
            }
        break;

        case EMMCodecInitError:
        /* Cheking for MMU_fault */
            if((args[4] == (void*)USN_ERR_UNKNOWN_MSG)
               && (args[5] == (void*)NULL))
            {
                pComponentPrivate->tCurState = OMX_StateInvalid;
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventError,
                                                       OMX_ErrorInvalidState,
                                                       0x2, NULL);
            }
        break;

        case EMMCodecDspMessageRecieved:
            /* Nothing to be done */
        break;

        case EMMCodecBufferProcessed:
       		count_buffer++;															/*purely for debug purpose*/
       		DPRINT("\nAUDIO: No of times buffer processed = %d",count_buffer);		/*purely for debug purpose*/
            DPRINT("\nAUDIO: Buf processed callback\n");
            if((OMX_U32)args [0] == EMMCodecInputBuffer)
            {
                DPRINT("\nAUDIO: Buf processed callback for i/p\n");


                /*purely for debug purpose*/


               tError = (OMX_ERRORTYPE)TIMM_OSAL_ReadFromPipe
                                        ((pComponentPrivate->tPipePtrs.
                                          pInBufPendingWithLcmlPipe),
                                        &pInBufHeader, sizeof(pInBufHeader),
                                        &actualSize, TIMM_OSAL_NO_SUSPEND);

                DPRINT("\nAUDIO: Error code = %x",tError);		/*purely for debug purpose*/
                OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
   /* Buffers should come back in the order in which they were sent for processing
   else error */
                if(pInBufHeader->pBuffer != pBuffer)
                {
                    tError = OMX_ErrorUndefined;
                    goto EXIT;
                }

/*Assuming that lcml cannot send back buffers once codec has been paused*/
                pComponentPrivate->fpReturnDataNotify (pHandle,
                                                       OMX_DirInput,
                                                       pInBufHeader);

            }
            else if ((OMX_U32)args [0] == EMMCodecOuputBuffer)
            {
                DPRINT("\nAUDIO: Buf processed callback for o/p\n");
                tError = (OMX_ERRORTYPE)TIMM_OSAL_ReadFromPipe
                                        ((pComponentPrivate->tPipePtrs.
                                          pOutBufPendingWithLcmlPipe),
                                        &pOutBufHeader, sizeof(pOutBufHeader),
                                        &actualSize, TIMM_OSAL_NO_SUSPEND);
                OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
/* Buffers should come back in the order in which they were sent for processing
   else error */
                if(pOutBufHeader->pBuffer != pBuffer)
                {
                    tError = OMX_ErrorUndefined;
                    goto EXIT;
                }
                pOutBufHeader->nFilledLen = (OMX_U32)args[8];
             /*This is the auxInfo for output buffers*/
                pOutBufHeader->pOutputPortPrivate = args[3];

        if(pComponentPrivate->inEOSReceived)
        {
            DPRINT("\nAUDIO: Transferring EOS to o/p bufheader\n");
            pComponentPrivate->inEOSReceived = 0;
            pOutBufHeader->nFlags = OMX_BUFFERFLAG_EOS;

            /* Notify EOS event flag to the Base Component via ReturnEventNotify
                               call , which communicates with the IL Client  */
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                OMX_EventBufferFlag,
                                                pOutBufHeader->nOutputPortIndex,
                                                pOutBufHeader->nFlags, NULL);
        }

/*Assuming that lcml cannot send back buffers once codec has been paused*/
                pComponentPrivate->fpReturnDataNotify (pHandle,
                                                       OMX_DirOutput,
                                                       pOutBufHeader);
            }

        break;

        case EMMCodecProcessingStarted:
            /* Nothing to be done */
        break;

        case EMMCodecProcessingPaused:
            DPRINT("\nAUDIO: CodecProcessingPaused\n");
            switch(pComponentPrivate->eComponentRole)
            {
                case AAC_ENCODE:
#ifdef RESOURCE_MANAGER_ENABLED
                    eComponentNameForRM = OMX_AAC_Encoder_COMPONENT;
#endif
                break;
                default:
                    tError = OMX_ErrorUnsupportedIndex;
                    goto EXIT;
                break;
            }
#ifdef RESOURCE_MANAGER_ENABLED
            rm_error = RMProxy_NewSendCommand(pHandle, RMProxy_StateSet,
                                              eComponentNameForRM,
                                              OMX_StatePause, 3456, NULL);
#endif
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                   OMX_EventCmdComplete,
                                                   OMX_CommandStateSet,
                                                   OMX_StatePause,
                                                   NULL);
        break;

        case EMMCodecProcessingStoped:
            DPRINT("\nAUDIO: CodecProcessingStoped\n");
            switch(pComponentPrivate->eComponentRole)
            {
                case AAC_ENCODE:
#ifdef RESOURCE_MANAGER_ENABLED
                    eComponentNameForRM = OMX_AAC_Encoder_COMPONENT;
#endif
                break;
                default:
                    tError = OMX_ErrorUnsupportedIndex;
                    goto EXIT;
                break;
            }

            /*Returning any buffers pending with lcml*/
            tError = OMX_AUDIO_ENC_ReturnPendingBuffers(pComponentPrivate,
                                                        LCML_DATA_PIPE, OMX_ALL);
            if(tError != OMX_ErrorNone)
            {
                goto EXIT;
            }

            if (!pComponentPrivate->bNoIdleOnStop)
            {
				#ifdef RESOURCE_MANAGER_ENABLED
                rm_error = RMProxy_NewSendCommand(pHandle,
                                                  RMProxy_StateSet,
                                                  eComponentNameForRM,
                                                  OMX_StateIdle,
                                                  3456, NULL);
				#endif
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventCmdComplete,
                                                       OMX_CommandStateSet,
                                                       OMX_StateIdle,
                                                       NULL);
            }
            else
            {
                /* Resetting the value */
                pComponentPrivate->bNoIdleOnStop = OMX_FALSE;
            }

        break;

        case EMMCodecProcessingEof:
            /* Nothing to be done */
        break;

        case EMMCodecBufferNotProcessed:
            /* Nothing to be done */
        break;

        case EMMCodecAlgCtrlAck:
            /* Nothing to be done */
        break;

        case EMMCodecStrmCtrlAck:
            if (args[1] == (void *)USN_STRMCMD_FLUSH)
            {
                if ( args[2] == (void *)EMMCodecInputBuffer)
                {
                    if (args[0] == (void*)USN_ERR_NONE )
                    {
                        /*Return buffers pending with lcml, if any*/
                        tError = OMX_AUDIO_ENC_ReturnPendingBuffers
                                             (pComponentPrivate, LCML_DATA_PIPE,
                                              OMX_DirInput);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }

                        pComponentPrivate->fpReturnEventNotify(pHandle,
                                                           OMX_EventCmdComplete,
                                                           OMX_CommandFlush,
                                                           AUDIOENC_INPUT_PORT,
                                                           NULL);
                    }
                    else
                    {
                         goto EXIT;
                    }
                }
                else if (args[2] == (void *)EMMCodecOuputBuffer)
                {
                    if (args[0] == (void*)USN_ERR_NONE )
                    {
                        tError = OMX_AUDIO_ENC_ReturnPendingBuffers
                                             (pComponentPrivate, LCML_DATA_PIPE,
                                              OMX_DirOutput);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }

                        pComponentPrivate->fpReturnEventNotify(pHandle,
                                                           OMX_EventCmdComplete,
                                                           OMX_CommandFlush,
                                                           AUDIOENC_OUTPUT_PORT,
                                                           NULL);
                    }
                    else
                    {
                         goto EXIT;
                    }
                }
            }
        break;

        default:

        break;
    }
    EXIT:
        return tError;
}


#ifdef RESOURCE_MANAGER_ENABLED
/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_ResourceManagerCallback() - The ResourceManager callback
 *                                               function. Called when resources
 *                                               are preempted from a running
 *                                               component or when resources are
 *                                               acquired for a component in
 *                                               WaitForResources state.
 */
/* ===========================================================================*/
static void OMX_AUDIO_ENC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_COMMANDTYPE Cmd = OMX_CommandStateSet;
    OMX_STATETYPE state = OMX_StateIdle;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)cbData.hComponent;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (pHandle->pComponentPrivate);

    if (*(cbData.RM_Error) == OMX_RmProxyCallback_ResourcesPreempted)
    {
        if (pComponentPrivate->tCurState == OMX_StateExecuting ||
            pComponentPrivate->tCurState == OMX_StatePause)
        {
            tError = pHandle->SendCommand(pHandle,Cmd,state,NULL);
            if(tError != OMX_ErrorNone)
            {
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                      OMX_EventError,
                                                      tError,0,NULL);
            }
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                   OMX_EventError,
                                                   OMX_ErrorResourcesPreempted,
                                                   0, NULL);
        }
    }
    else if (*(cbData.RM_Error) == OMX_RmProxyCallback_ResourcesAcquired)
    {
        pComponentPrivate->fpReturnEventNotify(pHandle,
                                               OMX_EventResourcesAcquired, 0, 0,
                                               NULL);

        /* Have to make SendCommand because base component cannot automatically
           make the transition to Idle - It has to come via a SendCommand call */
        tError = pHandle->SendCommand(pHandle,Cmd,state,NULL);
        if(tError != OMX_ErrorNone)
        {
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                   OMX_EventError,
                                                   tError,0,NULL);
        }
    }
}


#endif
/* ===========================================================================*/
/*
 * OMX_AUDIO_ENC_ComponentDeinit() - This methold will de init the component.
 *
 * @param hComponent                :Handle for this instance of the component.
 */
/* ===========================================================================*/

static OMX_ERRORTYPE OMX_AUDIO_ENC_ComponentDeInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_BASE_REQUIRE(hComponent != NULL, OMX_ErrorBadParameter);
    OMX_COMPONENTTYPE *pComponent = (OMX_COMPONENTTYPE *)hComponent;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate =
                  (OMX_AUDIOENC_COMPONENT_PRIVATE *)(pComponent->pComponentPrivate);
    OMX_U32 i = 0;
#ifdef RESOURCE_MANAGER_ENABLED
    tError = RMProxy_Deinitalize();
#endif
    TIMM_OSAL_Free(pComponentPrivate->sDeviceString);
    TIMM_OSAL_Free(pComponentPrivate->pcmParams);

    /*Destroying the pipes*/
    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe(pComponentPrivate->
                                                 tPipePtrs.pInDataPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);
    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe(pComponentPrivate->
                                                 tPipePtrs.pOutDataPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);
    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe
                      (pComponentPrivate->tPipePtrs.pInBufPendingWithLcmlPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);
    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe
                      (pComponentPrivate->tPipePtrs.pOutBufPendingWithLcmlPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);


    /*Clean up role specific params*/
    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_ENCODE:
            /*Free LCML Init params if not already freed*/
            if(pComponentPrivate->tLcmlParams.bLcmlParamsFreed == OMX_FALSE)
            {
                tError = OMX_AUDIO_ENC_AAC_FreeLCMLInitParams(hComponent);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
            }
            pComponentPrivate->tLcmlParams.bLcmlParamsFreed = OMX_TRUE;

            TIMM_OSAL_Free(pComponentPrivate->pAacPrivateParams->aacParams);
            TIMM_OSAL_Free(pComponentPrivate->pAacPrivateParams);
        break;
        default:
        break;
    }

    if (pComponentPrivate->tLcmlParams.ptrLibLCML != NULL)
    {
        dlclose( pComponentPrivate->tLcmlParams.ptrLibLCML    );
        pComponentPrivate->tLcmlParams.ptrLibLCML = NULL;
    }

    /* Deinitalize  the base component  */
    tError = OMX_BASE_ComponentDeInit(hComponent);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);

    for(i = 0;i<pComponentPrivate->tDerToBase.tPortParams.nPorts;i++)
    {
        TIMM_OSAL_Free(pComponentPrivate->tDerToBase.pTunPref[i]);
    }

    TIMM_OSAL_Free(pComponentPrivate->tDerToBase.pTunPref);
    TIMM_OSAL_Free(pComponentPrivate->tDerToBase.cComponentName);

    TIMM_OSAL_Free(pComponentPrivate);
    pComponentPrivate = NULL;

    EXIT:
        return tError;
}


