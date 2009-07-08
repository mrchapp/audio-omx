/* =============================================================================
*             Texas Instruments OMAP (TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file AudioDecoder.c
*
* This file implements OMX Component for Audio Decoder that
* is fully compliant with the OMX Audio specification 1.0.
*
* @path  $
*
* @rev  1.0
*/
/* ----------------------------------------------------------------------------
*!
*! Revision History
*! ===================================
*! 15-Dec-2008 mf:  Initial Version. Change required per OMAPSWxxxxxxxxx
*! to provide _________________.
*!
*!
*!  -Dec-2008 mf:
*! This is newest file
* =========================================================================== */


/* ------compilation control switches -------------------------*/
/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/

#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
/*-------program files ----------------------------------------*/
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Audio.h>
#include <OMX_Types.h>
#include <OMX_Index.h>

#include <ResourceManagerProxyAPI.h>
#include "usn.h"
#include <omx_base.h>
#include <timm_osal_interfaces.h>
#include <omx_base_utils.h>
#include "TIDspOmx.h"
#include "LCML_DspCodec.h"

#include "omx_audiodecutils.h"


/****************************************************************
*  EXTERNAL REFERENCES NOTE : only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/
extern OMX_ERRORTYPE OMX_BASE_Cb_ReturnDataNotify(OMX_HANDLETYPE hComponent,
                                                 OMX_S32 nPortIndex,
                                                 OMX_BUFFERHEADERTYPE* pBuffer);

extern OMX_ERRORTYPE OMX_BASE_Cb_ReturnEventNotify(OMX_HANDLETYPE hComponent,
                                                   OMX_EVENTTYPE eEvent,
                                                   OMX_U32 EventData1,
                                                   OMX_U32 EventData2,
                                                   OMX_PTR pEventData);


/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/

/*--------function prototypes ---------------------------------*/

OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE hComponent);
static OMX_ERRORTYPE OMX_AUDIO_DEC_ComponentDeInit(OMX_HANDLETYPE);

static OMX_ERRORTYPE OMX_AUDIO_DEC_GetParameter(OMX_HANDLETYPE, OMX_INDEXTYPE, OMX_PTR);
static OMX_ERRORTYPE OMX_AUDIO_DEC_SetParameter(OMX_HANDLETYPE, OMX_INDEXTYPE, OMX_PTR);

static OMX_ERRORTYPE OMX_AUDIO_DEC_GetConfig(OMX_HANDLETYPE, OMX_INDEXTYPE, OMX_PTR);
static OMX_ERRORTYPE OMX_AUDIO_DEC_SetConfig(OMX_HANDLETYPE, OMX_INDEXTYPE, OMX_PTR);


OMX_ERRORTYPE OMX_AUDIO_DEC_DataNotify(OMX_HANDLETYPE hComponent,
                                       OMX_U32 nPortIndex,
                                       OMX_BUFFERHEADERTYPE* pBufHeader);

OMX_ERRORTYPE OMX_AUDIO_DEC_CommandNotify(OMX_HANDLETYPE hComponent,
                                          OMX_COMMANDTYPE Cmd,
                                          OMX_U32 nParam,
                                          OMX_PTR pCmdData);

static OMX_ERRORTYPE OMX_AUDIO_DEC_GetExtensionIndex(OMX_IN  OMX_HANDLETYPE hComponent,
                                                     OMX_IN  OMX_STRING cParameterName,
                                                     OMX_OUT OMX_INDEXTYPE* pIndexType);

OMX_ERRORTYPE OMX_AUDIO_DEC_LCML_Callback (TUsnCodecEvent event,void * args [10]);

void OMX_AUDIO_DEC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData);

static OMX_HANDLETYPE OMX_AUDIO_DEC_GetLCMLHandle(AUDIODEC_COMPONENT_PRIVATE* pComponentPrivate);

OMX_ERRORTYPE OMX_LCML_DeInit(OMX_HANDLETYPE pComponent);

static OMX_ERRORTYPE OMX_AUDIO_DEC_ComponentRoleEnum(OMX_HANDLETYPE hComponent,
                                                     OMX_U8 *cRole,
                                                     OMX_U32 nIndex);

/* ========================================================================== */
/**
 * @fn OMX_ComponentInit - Updates the component function pointer to the handle.
 *                          Sets default parameters.
 * @param hComponent - Component handle.
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */

OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE hComponent)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate= NULL;
    OMX_COMPONENTTYPE *pHandle = NULL;
    TIMM_OSAL_ERRORTYPE bReturnStatus = TIMM_OSAL_ERR_NONE;
    OMX_U16 i;
    char *pTemp_char = NULL;


    /*Check i/p Params */
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);
    pHandle = (OMX_COMPONENTTYPE *)hComponent;

    AUDIODEC_DPRINT("\nentered in the component init\n");

    /*check for the version*/
    OMX_BASE_CHK_VERSION(pHandle, OMX_COMPONENTTYPE, tError);

    /*memeory allocation for the component private*/
    pHandle->pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)TIMM_OSAL_Malloc
                                     (sizeof(AUDIODEC_COMPONENT_PRIVATE));
    /*check whether able to allocate the memory or not*/
    OMX_BASE_ASSERT(pHandle->pComponentPrivate != NULL,OMX_ErrorInsufficientResources);
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

    /*initialize the pHandle of the componentprivate structure*/
    pComponentPrivate->pHandle = pHandle;

    AUDIODEC_DPRINT("\ninitializing the tDerToBase parameters\n");
    /*fill the tDerToBase  before BASE_INIT call*/
    pComponentPrivate->tDerToBase.cComponentName = (OMX_STRING )
                       TIMM_OSAL_Malloc(sizeof(OMX_U8) *OMX_MAX_STRINGNAME_SIZE);

    OMX_BASE_ASSERT(pComponentPrivate->tDerToBase.cComponentName != NULL,
                                                 OMX_ErrorInsufficientResources);
    strcpy(pComponentPrivate->tDerToBase.cComponentName, "OMX.TI.AUDIO.DECODE");

   /* Set the ContextType whether passive/Active depending on requirement */
    pComponentPrivate->tDerToBase.ctxType = OMX_BASE_CONTEXT_PASSIVE;//OMX_BASE_CONTEXT_ACTIVE;

    /* Fill in the Component Specification Version */
    pComponentPrivate->tDerToBase.tComVersion.s.nVersionMajor =
                                         OMX_AUDIO_DEC_COMP_VERSION_MAJOR;
    pComponentPrivate->tDerToBase.tComVersion.s.nVersionMinor =
                                         OMX_AUDIO_DEC_COMP_VERSION_MINOR;
    pComponentPrivate->tDerToBase.tComVersion.s.nRevision =
                                         OMX_AUDIO_DEC_COMP_VERSION_REVISION;
    pComponentPrivate->tDerToBase.tComVersion.s.nStep =
                                         OMX_AUDIO_DEC_COMP_VERSION_STEP;

    /* Incase of Active Context, set the Stacksize and GroupPriority */
    if(pComponentPrivate->tDerToBase.ctxType  == OMX_BASE_CONTEXT_ACTIVE) {
      pComponentPrivate->tDerToBase.unStackSize =
                                   OMX_AUDIO_DEC_TASK_STACKSIZE;
      pComponentPrivate->tDerToBase.unGroupPrioirty  =
                            OMX_AUDIO_DEC_TASK_PRIORITY;
    }

    pComponentPrivate->tDerToBase.unGroupID  = OMX_AUDIO_DEC_GROUPID;


    /* Initialize port params i.e no.of ports and startport num */
    OMX_BASE_INIT_STRUCT_PTR(&pComponentPrivate->tDerToBase.tPortParams,
                                                 OMX_PORT_PARAM_TYPE);
    pComponentPrivate->tDerToBase.tPortParams.nPorts = NUM_OF_PORTS;
    pComponentPrivate->tDerToBase.tPortParams.nStartPortNumber =
                                   DEFAULT_START_PORT_NUM;

    /* Create memory for the tunnel preference and set the output port's tunnel  preferences */
    /*taken from jpegd(eCosto code) as it is*/

    pComponentPrivate->tDerToBase.pTunPref = (OMX_TUNNELSETUPTYPE **)
                           TIMM_OSAL_Malloc(sizeof(OMX_TUNNELSETUPTYPE *)
                           *pComponentPrivate->tDerToBase.tPortParams.nPorts );
    OMX_BASE_ASSERT(pComponentPrivate->tDerToBase.pTunPref != NULL,
                                     OMX_ErrorInsufficientResources);
    for(i=0; i<pComponentPrivate->tDerToBase.tPortParams.nPorts; i++){

        pComponentPrivate->tDerToBase.pTunPref[i] = (OMX_TUNNELSETUPTYPE*)
                           TIMM_OSAL_Malloc(sizeof(OMX_TUNNELSETUPTYPE));
        OMX_BASE_ASSERT(pComponentPrivate->tDerToBase.pTunPref[i] != NULL,
                                     OMX_ErrorInsufficientResources);
    }

    pComponentPrivate->tDerToBase.pTunPref[INPUT_PORT]->nTunnelFlags
                                              = OMX_PORTTUNNELFLAG_READONLY;
    pComponentPrivate->tDerToBase.pTunPref[INPUT_PORT]->eSupplier
                                           = OMX_BufferSupplyUnspecified;
    pComponentPrivate->tDerToBase.pTunPref[OUTPUT_PORT]->nTunnelFlags
                                              = OMX_PORTTUNNELFLAG_READONLY;
    pComponentPrivate->tDerToBase.pTunPref[OUTPUT_PORT]->eSupplier
                                           = OMX_BufferSupplyUnspecified;
    /*till here taken from jpegd as it is*/


    /*map the hooks from the derived to base*/
    AUDIODEC_DPRINT("\nmap the hooks from the derived to base\n");
    pComponentPrivate->tDerToBase.fpCommandNotify = OMX_AUDIO_DEC_CommandNotify;
    pComponentPrivate->tDerToBase.fpDataNotify = OMX_AUDIO_DEC_DataNotify;

    /*done with the Initialization of tDerToBase parametere*/

    AUDIODEC_DPRINT("\ncall to BASE_ComponentInit\n");
    tError = OMX_BASE_ComponentInit(hComponent);
    /*Initializing the Base Component with the given number of ports */
    /*initialization of base_internal parameters */
    /*BasePvt:portdefs is mapped to BaseInt:ports->tportDefParams*/
    /*memory allocation for portdefs ... done in base(_OMX_BASE_InitializePorts call)*/

    OMX_BASE_ASSERT(tError == OMX_ErrorNone, OMX_ErrorInsufficientResources);


    /* Override the function pointers in the Derived Component private structure */
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    /* Override the Function pointers of type OMX_COMPONENTTYPE which Base
            cannot handle and needs to be taken care by the Derived Component */
    /* Fill the function pointers which are specific to the this component  */
    AUDIODEC_DPRINT("\nOverride the Function pointers\n");
    pHandle->GetParameter      = OMX_AUDIO_DEC_GetParameter;
    pHandle->SetParameter      = OMX_AUDIO_DEC_SetParameter;
    pHandle->SetConfig         = OMX_AUDIO_DEC_SetConfig;
    pHandle->GetConfig         = OMX_AUDIO_DEC_GetConfig;
    pHandle->ComponentDeInit   = OMX_AUDIO_DEC_ComponentDeInit;
    pHandle->GetExtensionIndex = OMX_AUDIO_DEC_GetExtensionIndex;
    pHandle->ComponentRoleEnum = OMX_AUDIO_DEC_ComponentRoleEnum;

    /* Override the default values set to an InputPort with Actual/Default values */

    pComponentPrivate->portdefs[INPUT_PORT]->bEnabled       =OMX_TRUE;
    pComponentPrivate->portdefs[INPUT_PORT]->bPopulated     =OMX_FALSE;
    pComponentPrivate->portdefs[INPUT_PORT]->eDir           =OMX_DirInput;
    pComponentPrivate->portdefs[INPUT_PORT]->eDomain        =OMX_PortDomainAudio;

    /*need to be specified by the specific role*/
    pComponentPrivate->portdefs[INPUT_PORT]->nPortIndex             = INPUT_PORT;
    pComponentPrivate->portdefs[INPUT_PORT]->nBufferCountMin        = DEC_DEFAULT_INPUT_BUFFER_COUNT;
    pComponentPrivate->portdefs[INPUT_PORT]->nBufferCountActual     = DEC_DEFAULT_INPUT_BUFFER_COUNT;
    pComponentPrivate->portdefs[INPUT_PORT]->nBufferSize            = DEC_DEFAULT_INPUT_BUFFER_SIZE;

    /* Override the default values set to an outputPort with Actual/Default values */
    pComponentPrivate->portdefs[OUTPUT_PORT]->bEnabled      =OMX_TRUE;
    pComponentPrivate->portdefs[OUTPUT_PORT]->bPopulated    =OMX_FALSE;
    pComponentPrivate->portdefs[OUTPUT_PORT]->eDir          =OMX_DirOutput;
    pComponentPrivate->portdefs[OUTPUT_PORT]->eDomain       =OMX_PortDomainAudio;

    /*need to be specified by the specific role*/
    pComponentPrivate->portdefs[OUTPUT_PORT]->nPortIndex            = OUTPUT_PORT;
    pComponentPrivate->portdefs[OUTPUT_PORT]->nBufferCountMin       = DEC_DEFAULT_OUTPUT_BUFFER_COUNT;
    pComponentPrivate->portdefs[OUTPUT_PORT]->nBufferCountActual    = DEC_DEFAULT_OUTPUT_BUFFER_COUNT;
    pComponentPrivate->portdefs[OUTPUT_PORT]->nBufferSize           = DEC_DEFAULT_OUTPUT_BUFFER_SIZE;
    //pComponentPrivate->portdefs[OUTPUT_PORT]->bBuffersContiguous          = OMX_FALSE;
    //pComponentPrivate->portdefs[OUTPUT_PORT]->nBufferAlignment            = NULL;
    pComponentPrivate->portdefs[OUTPUT_PORT]->format.audio.eEncoding             = OMX_AUDIO_CodingPCM;
    pComponentPrivate->portdefs[OUTPUT_PORT]->format.audio.cMIMEType             = NULL;
    pComponentPrivate->portdefs[OUTPUT_PORT]->format.audio.pNativeRender         = NULL;
    pComponentPrivate->portdefs[OUTPUT_PORT]->format.audio.bFlagErrorConcealment = OMX_FALSE;


    /*current state and new state are set to loaded in the BASE_ComponentInit function*/
    /*the returndatanotify/eventnotify are mapped to base calls*/


    /*DONE with BASE COMPONENT PRIVATE Params Initialization*/


    /*SET THE Component private Variables*/

    AUDIODEC_DPRINT("\ncreating datapipes one for each i/p buffers and o/p buffers\n");
    /*creating the datapipes one each for i/p buffers and o/p buffers storage*/
    bReturnStatus = TIMM_OSAL_CreatePipe(&(pComponentPrivate->pInDataPipe),
                          MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                           sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorContentPipeCreationFailed);

    bReturnStatus = TIMM_OSAL_CreatePipe(&(pComponentPrivate->pOutDataPipe),
                          MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                           sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorContentPipeCreationFailed);
    /*set memory for pcmparams*/
    pComponentPrivate->pcmParams = (OMX_AUDIO_PARAM_PCMMODETYPE*)
                          TIMM_OSAL_Malloc(sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
    OMX_BASE_ASSERT(pComponentPrivate->pcmParams != NULL,OMX_ErrorInsufficientResources);
    OMX_BASE_INIT_STRUCT_PTR(pComponentPrivate->pcmParams,OMX_AUDIO_PARAM_PCMMODETYPE);

    /* PCM format defaults */
    AUDIODEC_DPRINT("\ninitializing the PCM default params\n");

    OMX_BASE_INIT_STRUCT_PTR(pComponentPrivate->pcmParams, OMX_AUDIO_PARAM_PCMMODETYPE);
    pComponentPrivate->pcmParams->eNumData= OMX_NumericalDataSigned;
    pComponentPrivate->pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
    pComponentPrivate->pcmParams->nPortIndex = OUTPUT_PORT;
    pComponentPrivate->pcmParams->nBitPerSample = 16;
    pComponentPrivate->pcmParams->nChannels = 2;
    pComponentPrivate->pcmParams->nSamplingRate = 44100;
    pComponentPrivate->pcmParams->bInterleaved = OMX_TRUE;


    AUDIODEC_DPRINT("\ninitializing the pportformat default params\n");

    /*allocate the memory for the pPortformat*/
    pComponentPrivate->pPortFormat = (OMX_AUDIO_PARAM_PORTFORMATTYPE **)TIMM_OSAL_Malloc(
                                          sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE*) *
                                          pComponentPrivate->tDerToBase.tPortParams.nPorts);
    OMX_BASE_ASSERT(pComponentPrivate->pPortFormat != NULL,OMX_ErrorInsufficientResources);
    /*initialize memory for individual elements*/
    /*input port*/
    pComponentPrivate->pPortFormat[INPUT_PORT]=(OMX_AUDIO_PARAM_PORTFORMATTYPE *)TIMM_OSAL_Malloc(sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE ));
    OMX_BASE_ASSERT(pComponentPrivate->pPortFormat[INPUT_PORT] != NULL,OMX_ErrorInsufficientResources);
    /*ouput port*/
    pComponentPrivate->pPortFormat[OUTPUT_PORT]=(OMX_AUDIO_PARAM_PORTFORMATTYPE *)TIMM_OSAL_Malloc(sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE ));
    OMX_BASE_ASSERT(pComponentPrivate->pPortFormat[OUTPUT_PORT] != NULL,OMX_ErrorInsufficientResources);
    /*initialize the version*/
    OMX_BASE_INIT_STRUCT_PTR(pComponentPrivate->pPortFormat[INPUT_PORT], OMX_AUDIO_PARAM_PORTFORMATTYPE);
    OMX_BASE_INIT_STRUCT_PTR(pComponentPrivate->pPortFormat[OUTPUT_PORT], OMX_AUDIO_PARAM_PORTFORMATTYPE);
    /*initialize the pPortformat*/
    pComponentPrivate->pPortFormat[INPUT_PORT]->nPortIndex = 0;/*input:0 output:1*/
    pComponentPrivate->pPortFormat[INPUT_PORT]->nIndex = 0;/*numberof formats supported*/

    pComponentPrivate->pPortFormat[OUTPUT_PORT]->nPortIndex = 1;/*input:0 output:1*/
    pComponentPrivate->pPortFormat[OUTPUT_PORT]->nIndex = 0;/*numberof formats supported*/


    AUDIODEC_DPRINT("\ninitializing the RM\n");
    #ifdef RESOURCE_MANAGER_ENABLED
    tError = RMProxy_NewInitalize();
    if (tError != OMX_ErrorNone) {
        goto EXIT;
    }
    /*set the RM_cal back*/
    pComponentPrivate->rmproxyCallback.RMPROXY_Callback = (void *)OMX_AUDIO_DEC_ResourceManagerCallback;

    #endif/*RESOURCE_MANAGER_ENABLED*/

    /*initialize the other private params*/
    AUDIODEC_DPRINT("\ninitializing the other private params\n");
    pComponentPrivate->bNoIdleOnStop=OMX_FALSE;
    pComponentPrivate->bIsEOFSent=OMX_FALSE;
    pComponentPrivate->bMarkDataIn=OMX_FALSE;
    pComponentPrivate->bBypassDSP=OMX_FALSE;
    /*pComponentPrivate->dasfmode=OMX_FALSE;*/

    /*initializing the memory for the sDeviceString*/
    pComponentPrivate->sDeviceString = (OMX_STRING *)
                                     TIMM_OSAL_Malloc(sizeof(OMX_STRING) * 100);
    TIMM_OSAL_Memset(pComponentPrivate->sDeviceString, 0,
                    (sizeof(OMX_STRING) * 100));
    OMX_BASE_ASSERT(pComponentPrivate->sDeviceString != NULL,OMX_ErrorInsufficientResources);

    /*allocate memory for the LCMLParams*/

    /*initialize the pParams*/

    pComponentPrivate->LCMLParams=(LCMLParams*)
                           TIMM_OSAL_Malloc(sizeof(LCMLParams));
    OMX_BASE_ASSERT(pComponentPrivate->LCMLParams != NULL,OMX_ErrorInsufficientResources);

    pComponentPrivate->LCMLParams->pParams = (USN_AudioCodecParams*)
                           TIMM_OSAL_Malloc(sizeof(USN_AudioCodecParams) + DSP_CACHE_ALIGNMENT);
    OMX_BASE_ASSERT(pComponentPrivate->LCMLParams->pParams != NULL,OMX_ErrorInsufficientResources);
    pTemp_char = (char*)pComponentPrivate->LCMLParams->pParams;
    pTemp_char += EXTRA_BYTES;
    pComponentPrivate->LCMLParams->pParams =
                                         (USN_AudioCodecParams*)pTemp_char;

    /*create LCML pipes for*/
    /*i/p buffers pending with lcml*/
    bReturnStatus = TIMM_OSAL_CreatePipe
                    (&(pComponentPrivate->LCMLParams->pInBufPendingWithLcmlPipe),
                    MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                    sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorContentPipeCreationFailed);
    /*o/p buffers pending with lcml*/
    bReturnStatus = TIMM_OSAL_CreatePipe
                    (&(pComponentPrivate->LCMLParams->pOutBufPendingWithLcmlPipe),
                    MAX_NUM_OF_BUFS * sizeof(OMX_BUFFERHEADERTYPE *),
                    sizeof(OMX_BUFFERHEADERTYPE *), 1);
    OMX_BASE_ASSERT(bReturnStatus == TIMM_OSAL_ERR_NONE,OMX_ErrorContentPipeCreationFailed);

	/* Initialize the Component Role Information structure  */
    OMX_BASE_INIT_STRUCT_PTR(&pComponentPrivate->componentRole,OMX_PARAM_COMPONENTROLETYPE);
    /*Initial role is undefined - role should be set by client via SetParameter*/
	/*NOTE: Initial role cant be set to undefined since conformance tests expect the
	component to change states all the way up to executing without setting the role
	Thus we setting the default role to AAC and initializing its parameters. If the
	client wants, it can query the current role (via GetParameter) and then set a
	different role (via SetParameter) if he wants to*/
    /*strcpy((char *)(pComponentPrivate->componentRole.cRole),"undefined_role");
    pComponentPrivate->eComponentRole = UNDEFINED_ROLE;*/
    strcpy((char *)(pComponentPrivate->componentRole.cRole),"audio_decode.dsp.aac");
	pComponentPrivate->eComponentRole = AAC_DECODE;
	tError = OMX_AAC_DEC_InitParams(hComponent);
    if(tError != OMX_ErrorNone){
        goto EXIT;
    }
    /*aacdecParams are initialized in rolespecific initparameters*/

EXIT:
    AUDIODEC_DPRINT("\nerror returned=tError=%x\n",tError);
    return tError;
}

/* ================================================================================= */
/**
* @fn OMX_AUDIO_DEC_GetExtensionIndex() Returns index for vendor specific settings.
* @see         OMX_Core.h
* @param hComponent - handle for this instance of the component
* @param cParameterName - the string that will get
                            translated by the component into a configuration index.
* @param pIndexType - a pointer to a OMX_INDEXTYPE to receive the index value.
* @return:  OMX_ERRORTYPE
*           OMX_ErrorNone on success
*           !OMX_ErrorNone on any failure
*
*/
/* ================================================================================ */

static OMX_ERRORTYPE OMX_AUDIO_DEC_GetExtensionIndex(   OMX_IN  OMX_HANDLETYPE hComponent,
                                                OMX_IN  OMX_STRING cParameterName,
                                                OMX_OUT OMX_INDEXTYPE* pIndexType)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                     (cParameterName!=NULL), OMX_ErrorBadParameter);
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*can't be invoked when the component is there in Invalid state*/
    OMX_BASE_ASSERT(pComponentPrivate->tCurState != OMX_StateInvalid,
                                  OMX_ErrorIncorrectStateOperation);

    /*First search in role specific function
      if not found then search in common & if not found in common also then search goes the BASE call*/
    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:GetExtensionIndex\n");
            tError= OMX_AAC_DEC_GetExtensionIndex(hComponent,cParameterName,pIndexType);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
            break;
        case MP3_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:GetExtensionIndex\n");
            tError= OMX_MP3_DEC_GetExtensionIndex(hComponent,cParameterName,pIndexType);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
            break;
            /*Add more roles here*/
            /*One role is UNDEFINED_ROLE which will call common audio - to take care of initial case when roles havnt been set*/
        case UNDEFINED_ROLE:
            tError = OMX_ErrorUnsupportedIndex;
            break;
        default:
            tError = OMX_ErrorBadParameter;
            break;
    }

    if(tError==OMX_ErrorUnsupportedIndex){
		goto EXIT;
    	/*no common config found so set tError n goto EXIT*/
       /* tError= OMX_AUDIO_DEC_COMMON_GetExtensionIndex(hComponent, nParamIndex, pComponentParameterStructure);
       */
    }

    EXIT:
        AUDIODEC_DPRINT("\nerror returned to application (in function:GetExtensionIndex)=%x\n",tError);
        return tError;

}

/* ========================================================================== */
/**
* @fn OMX_AUDIO_DEC_GetParameter - Gets Parameters from the Component.
*   This method will update parameters from the component to the app.
* @param hComponent - handle for this instance of the component
* @param nParamIndex - Component Index Port
* @param pParamStruct - Component Parameter Structure
* @return:  OMX_ERRORTYPE
*           OMX_ErrorNone on success
*           !OMX_ErrorNone on any failure
* @see         None
*/
/* ========================================================================== */

static OMX_ERRORTYPE OMX_AUDIO_DEC_GetParameter (   OMX_HANDLETYPE hComponent,
                                            OMX_INDEXTYPE nParamIndex,
                                            OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) && (pComponentParameterStructure!=NULL), OMX_ErrorBadParameter);

    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*GetParameter can't be invoked when the comp is in Invalid state */
    OMX_BASE_ASSERT(pComponentPrivate->tCurState != OMX_StateInvalid,
                                  OMX_ErrorIncorrectStateOperation);

    /*switch to role specific getparameter call*/
    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:GetParameter\n");
            tError=OMX_AAC_DEC_GetParameter(hComponent, nParamIndex, pComponentParameterStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
        break;
        case MP3_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:GetParameter\n");
            tError=OMX_MP3_DEC_GetParameter(hComponent, nParamIndex, pComponentParameterStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
        break;
        /*Add more roles here*/
        /*One role is UNDEFINED_ROLE which will call common audio - to take care of initial case when roles havnt been set*/
        case UNDEFINED_ROLE:
            tError = OMX_ErrorUnsupportedIndex;
            break;
        default:
            tError = OMX_ErrorBadParameter;
            break;
    }
    /*index is not found in role specific:GetParameter so search in common */
    if(tError==OMX_ErrorUnsupportedIndex){
        AUDIODEC_DPRINT("\nentering the common function:GetParameter\n");
        tError = OMX_AUDIO_DEC_COMMON_GetParameter(hComponent, nParamIndex, pComponentParameterStructure);
    }
    EXIT:
        AUDIODEC_DPRINT("\nerror returned to application (in function:GetParameter)=%x\n",tError);
        return tError;
}/*GetParam ends*/


/* ========================================================================== */
/**
 * @fn OMX_AUDIO_DEC_SetParameter - Sets the parameters sent by the Application to the component structure
 * @param hComponent - handle for this instance of the component
 * @param nParamIndex - Component Index Port
 * @param pParamStruct - Component Parameter Structure to set
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */


static OMX_ERRORTYPE OMX_AUDIO_DEC_SetParameter (   OMX_HANDLETYPE hComponent,
                                            OMX_INDEXTYPE nParamIndex,
                                            OMX_PTR pComponentParameterStructure){

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&(pComponentParameterStructure!=NULL), OMX_ErrorBadParameter);
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*Prerequisite: SetParameter can't be invoked incase the comp is not in Loaded State*/
    OMX_BASE_ASSERT(pComponentPrivate->tCurState == OMX_StateLoaded,
                                  OMX_ErrorIncorrectStateOperation);

    /*switch to role specific setparameter call*/
    switch(pComponentPrivate->eComponentRole)
    {

        case AAC_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:SetParameter\n");
            tError=OMX_AAC_DEC_SetParameter(hComponent, nParamIndex, pComponentParameterStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
        break;
        case MP3_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:SetParameter\n");
            tError=OMX_MP3_DEC_SetParameter(hComponent, nParamIndex, pComponentParameterStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
        break;
        /*Add more roles here*/
        /*One role is UNDEFINED_ROLE which will call common audio - to take care of initial case when roles havnt been set*/
        case UNDEFINED_ROLE:
            tError = OMX_ErrorUnsupportedIndex;
            break;
        default:
            tError = OMX_ErrorBadParameter;
            break;


    }//switch case ends
    /*not found in role specific*//*this check facilitates...if any other error occurs in role specific ones..
                                            need to send that particular error on exit*/
    if(tError==OMX_ErrorUnsupportedIndex){
        AUDIODEC_DPRINT("\nentering the common function:SetParameter\n");
        tError = OMX_AUDIO_DEC_COMMON_SetParameter(hComponent, nParamIndex, pComponentParameterStructure);

    }


EXIT:
    AUDIODEC_DPRINT("\nerror returned to application (in function:SetParameter)=%x\n",tError);
    return tError;

}/*Setparameter ends*/

/* ========================================================================== */
/**
 * @fn OMX_AUDIO_DEC_GetConfig - Gets Configuration data from the Component
 * @param hComponent - handle for this instance of the component
 * @param nIndex - Component  Index Port
 * @param pConfigData - Component Config Structure
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */
static OMX_ERRORTYPE OMX_AUDIO_DEC_GetConfig (OMX_HANDLETYPE hComponent,
                                              OMX_INDEXTYPE nConfigIndex,
                                              OMX_PTR pComponentConfigStructure)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                     (pComponentConfigStructure !=NULL), OMX_ErrorBadParameter);
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*Prerequisite: GetConfig can't be invoked incase the comp is in invalid State*/
    OMX_BASE_ASSERT(pComponentPrivate->tCurState != OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation);


    switch(pComponentPrivate->eComponentRole)
    {

        case AAC_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:GetConfig\n");
            tError=OMX_AAC_DEC_GetConfig(hComponent, nConfigIndex, pComponentConfigStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
            break;
        case MP3_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:GetConfig\n");
            tError=OMX_MP3_DEC_GetConfig(hComponent, nConfigIndex, pComponentConfigStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
            break;
            /*Add more roles here*/
            /*One role is UNDEFINED_ROLE which will call common audio - to take care of initial case when roles havnt been set*/
        case UNDEFINED_ROLE:
            tError = OMX_ErrorUnsupportedIndex;
            break;
        default:
            tError = OMX_ErrorBadParameter;
            break;

    }/*switch case ends*/

    if(tError==OMX_ErrorUnsupportedIndex){
        goto EXIT;
        /*no common config found so set tError n goto EXIT*/
        /*tError= _OMX_AUDIO_DEC_COMMON_GetConfig(hComponent, nParamIndex, pComponentParameterStructure);
         */
    }


 EXIT:
    AUDIODEC_DPRINT("\nerror returned to application (in function:GetConfig)=%x\n",tError);
    return tError;


}/*getconfig ends*/

/* ========================================================================== */
/**
 * @fn OMX_AUDIO_DEC_SetConfig - Sends command to set new configuration
 * @param hComponent - handle for this instance of the component
 * @param nIndex - Component Config Index Port
 * @param pConfigData - Component Config Structure
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */
static OMX_ERRORTYPE OMX_AUDIO_DEC_SetConfig (OMX_HANDLETYPE hComponent,
                                              OMX_INDEXTYPE nConfigIndex,
                                              OMX_PTR pComponentConfigStructure)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&(pComponentConfigStructure !=NULL), OMX_ErrorBadParameter);
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*Prerequisite: SetParameter can't be invoked incase the comp is in invalid State*/
    OMX_BASE_ASSERT(pComponentPrivate->tCurState!= OMX_StateInvalid,
                    OMX_ErrorIncorrectStateOperation);

    /*switch case starts*/

    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:SetConfig\n");
            tError= OMX_AAC_DEC_SetConfig(hComponent, nConfigIndex, pComponentConfigStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
            break;
        case MP3_DECODE:
            AUDIODEC_DPRINT("\nentering the role specific function:SetConfig\n");
            tError= OMX_MP3_DEC_SetConfig(hComponent, nConfigIndex, pComponentConfigStructure);
            if(tError==OMX_ErrorNone){
                /*index is found*/
                goto EXIT;
            }
            break;
            /*Add more roles here*/
        case UNDEFINED_ROLE:
            tError = OMX_ErrorUnsupportedIndex;
            break;
        default:
            tError = OMX_ErrorBadParameter;
            break;

    }/*switch case ends*/

    if(tError==OMX_ErrorUnsupportedIndex){
        goto EXIT;
        /*no common set config params*/
        /* AUDIODEC_DPRINT("\nentering the common function:SetConfig\n");
           tError=OMX_AUDIO_DEC_COMMON_SetConfig(hComponent, nConfigIndex, pComponentConfigStructure);
        */
    }

 EXIT:
    AUDIODEC_DPRINT("\nerror returned to application (in function:SetConfig)=%x\n",tError);
    return tError;

}/*setconfig ends*/

/* =========================================================================*/
/*
* @fn OMX_AUDIO_DEC_CommandNotify:  This method is an hook from the Base Component
*                                   to notify the Command which need to be
*                                   processed by the Derived Component and notify
*                                   to the Base Component via ReturnEventNotify
*                                   call to communicate with the IL client.
*  @ param [in]        hComponent : Handle of the component to be accessed.
*  @ param [in]        Cmd             : Command event parameter
*  @ param [inout]    nParam        :  Flag indicating command completion
*  @ param [inout]    pCmdData    :  Flag indicating command completion
*  @see         None
*/
/*===========================================================================*/

OMX_ERRORTYPE OMX_AUDIO_DEC_CommandNotify(OMX_HANDLETYPE hComponent,
                                          OMX_COMMANDTYPE Cmd,
                                          OMX_U32 nParam,
                                          OMX_PTR pCmdData)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate= NULL;
    OMX_U32 aParam[3];
    LCML_CALLBACKTYPE cb;
    LCML_DSP *pLcmlDsp=NULL;
    OMX_HANDLETYPE pLcmlHandle=NULL;
    OMX_U16 arr[10];
    /*variable to hold the newstate type*/
    OMX_STATETYPE commandedState;
    char *pArgs = "damedesuStr";

    /*check for i/p parameters*/
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);


#ifdef RESOURCE_MANAGER_ENABLED
    OMX_ERRORTYPE rm_error = OMX_ErrorNone;
    OMX_COMPONENTINDEXTYPE eComponentNameForRM;/*type is already defined in RM*/
    OMX_U32 CPULoadForRM;
    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_DECODE:
            AUDIODEC_DPRINT("\nRM for AAC_DECODE\n");
            eComponentNameForRM = OMX_AAC_Decoder_COMPONENT;
            CPULoadForRM = AACDEC_CPU_USAGE;
            break;
        case MP3_DECODE:
            AUDIODEC_DPRINT("\nRM for MP3_DECODE\n");
            eComponentNameForRM = OMX_MP3_Decoder_COMPONENT;
            CPULoadForRM = MP3DEC_CPU_USAGE;
            break;
        default:
            break;
    }
#endif /*Resource Manager*/

    /*Switch case starts!!!!*/
    switch (Cmd){
        case OMX_CommandPortDisable:
            AUDIODEC_DPRINT("\nentering the OMX_CommandPortDisable case\n");

            if((nParam==INPUT_PORT)||(nParam==OMX_ALL)){
                AUDIODEC_DPRINT("\nOMX_CommandPortDisable for INPUT_PORT\n");
                /*need to send the pending buffers(both in the paused state and LCML pending oness*/
                /*LCML pending are taken care at the LCML call back*/
                /*returning the paused pending buffers*/
                tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                             DATA_PIPE, OMX_DirInput);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }



                /*notify the evnet to the base*/
                if(nParam==INPUT_PORT){
                    AUDIODEC_DPRINT("\nevent notify to the base:i/p port:OMX_CommandPortDisable case\n");
                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventCmdComplete, OMX_CommandPortDisable,INPUT_PORT, NULL);
                }
            }
            if((nParam==OUTPUT_PORT)||(nParam==OMX_ALL)){

                if ((pComponentPrivate->tCurState != OMX_StateLoaded) &&
                    (pComponentPrivate->tCurState != OMX_StateWaitForResources)){
                    char *pArgs = "damedesuStr";
                    pComponentPrivate->bNoIdleOnStop = OMX_TRUE;
                    /*get the LCML handle*/
                    pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                    /*send command to LCML to stop executing*/
                    AUDIODEC_DPRINT("\ncommand to LCML to stop the codec in OMX_CommandPortDisable case\n");
                    tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                               MMCodecControlStop,(void *)pArgs);
                    if (tError != OMX_ErrorNone) {
                        goto EXIT;
                    }
                    /*need to send the pending buffers(both in the paused state and LCML pending oness*/
                    /*LCML pending are taken care at the LCML call back*/
                    /*returning the paused pending buffers*/

                    tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                                 DATA_PIPE, OMX_DirOutput);
                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                    /*eventnotify for output port is done at LCML callback*/

                }
                else{/*in loaded or waitfor resources*/
                    AUDIODEC_DPRINT("\nwhen loaded/wait for resources :event notify to the base:o/p port:OMX_CommandPortDisable case\n");
                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventCmdComplete, OMX_CommandPortDisable,nParam, NULL);
                    goto EXIT;
                }
            }
            if((nParam!=INPUT_PORT)||(nParam!=OUTPUT_PORT)||(nParam!=OMX_ALL)){
                AUDIODEC_DPRINT("\nerror from OMX_CommandPortDisable case:%x\n",tError);
                tError=OMX_ErrorBadParameter;
                goto EXIT;
            }
            break;


        case OMX_CommandPortEnable:
            /*base is taking care of allocating all the resources*/

            if(((pComponentPrivate->tCurState == OMX_StateExecuting) &&
                ((nParam == OUTPUT_PORT) || (nParam == OMX_ALL))))
            {
                /*get the LCML handle*/
                pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                /*give start command to LCML*/
                AUDIODEC_DPRINT("\ncommand to LCML to start codec: in OMX_CommandPortEnable case\n");
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                           EMMCodecControlStart,(void *)pArgs);
                if(tError != OMX_ErrorNone)
                {
                    AUDIODEC_DPRINT("\nerror from OMX_CommandPortEnable case:%x\n",tError);
                    goto EXIT;
                }
            }
            AUDIODEC_DPRINT("\nevent notify to the base: in OMX_CommandPortEnable case\n");
            pComponentPrivate->fpReturnEventNotify(hComponent,
                                                   OMX_EventCmdComplete,
                                                   Cmd, nParam, NULL);
            break;
        case OMX_CommandFlush:
            if((nParam==OMX_ALL)||(nParam==INPUT_PORT)){
                /*need to send the pending buffers(both in the paused state and LCML pending ones*/
                /*returning the LCMLpending buffers ....this is handled at the LCML call back*/
                /*returning the paused pending buffers*/
                AUDIODEC_DPRINT("\nflushing the i/p pending buffers in OMX_CommandFlush case\n");
                tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                             DATA_PIPE, OMX_DirInput);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }

                aParam[0] = USN_STRMCMD_FLUSH;
                aParam[1] = 0x0; /*saying SN that ""need to flush the i/p buffers*/
                aParam[2] = 0x0;
                /*get the LCML handle*/
                pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                AUDIODEC_DPRINT("\ncommand sent to LCML for strm cntrl: in OMX_CommandFlush case\n");
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                           EMMCodecControlStrmCtrl,
                                           (void*)aParam);
                if (tError != OMX_ErrorNone) {
                    AUDIODEC_DPRINT("\nerror from OMX_CommandFlush case:%x\n",tError);
                    goto EXIT;
                }
            }else if((nParam==OMX_ALL)||(nParam==OUTPUT_PORT)){

                /*need to send the pending buffers(both in the paused state and LCML pending oness*/
                /*returning the pending buffers at LCML....this is handled at the LCML call back*/
                /*returning the paused pending buffers*/
                AUDIODEC_DPRINT("\nflushing the o/p pending buffers in OMX_CommandFlush case\n");
                tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                             DATA_PIPE, OMX_DirOutput);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
                aParam[0] = USN_STRMCMD_FLUSH;
                aParam[1] = 0x1;/*saying SN that ""need to flush the o/p buffers*/
                aParam[2] = 0x0;
                /*get the LCML handle*/
                pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                AUDIODEC_DPRINT("\ncommand sent to LCML for codec strmcntrl at o/p port:OMX_CommandFlush case\n");
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                           EMMCodecControlStrmCtrl,
                                           (void*)aParam);
                if (tError != OMX_ErrorNone) {
                    AUDIODEC_DPRINT("\nerror from OMX_CommandFlush case:%x\n",tError);
                    goto EXIT;
                }
            }
            /*eventnotify done at LCMLcallback*/

            break;

        case  OMX_CommandStateSet:
            /*get the desired state*/
            AUDIODEC_DPRINT("\nentered in OMX_CommandStateSet case\n");
            commandedState= pComponentPrivate->tNewState;
            AUDIODEC_DPRINT("\ndesired state in OMX_CommandStateSet case:%x\n",commandedState);

            /*check for the current state(transition form invalid state is not permitted)*/
            if(pComponentPrivate->tCurState==OMX_StateInvalid){
                AUDIODEC_DPRINT("\nentered in OMX_CommandStateSet case:tCurstate=OMX_StateInvalid\n");
                pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventCmdComplete, Cmd, nParam, NULL);
                goto EXIT;
            }
            /*check for the same state (curstate==newstate) transition is done in (OMX_Base_HandleStateTransiton() in base_internal.c) */
            switch(commandedState) {
                case OMX_StateIdle:
                    AUDIODEC_DPRINT("\nentered in OMX_CommandStateSet->OMX_StateIdle \n");
                    /*Stateidle case starts*/
                    /*check for valid state transistion*/
                    if (pComponentPrivate->tCurState == OMX_StateLoaded ||
                        pComponentPrivate->tCurState == OMX_StateWaitForResources) {
                        AUDIODEC_DPRINT("\nentered in OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources \n");

#ifdef DASF_Mode_enable
                        if (pComponentPrivate->dasfmode == 1) {
                            pComponentPrivate->portDefs[OUTPUT_PORT]->bEnabled= FALSE;
                            pComponentPrivate->portDefs[OUTPUT_PORT]->bPopulated= FALSE;
                            if(pComponentPrivate->streamID == 0) {
                                tError = OMX_ErrorInsufficientResources;
                                goto EXIT;
                            }
                        }
#endif /*dasf mode*/

                        /*Get the LCML Handle*/
                        AUDIODEC_DPRINT("\n get the LCML Handle in OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources \n");
                        pLcmlHandle = (OMX_HANDLETYPE) OMX_AUDIO_DEC_GetLCMLHandle(pComponentPrivate);
                        /*if unable to get the proper LCML Handle the transition can't happen
                          so put the component in the invalid state*/
                        if (pLcmlHandle == NULL) {
                            AUDIODEC_DPRINT("\n error in getting the LCML handle from OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:%x\n",tError);
                            tError = OMX_ErrorInsufficientResources;
                            pComponentPrivate->tCurState = OMX_StateInvalid;
                            pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                            goto EXIT;
                        }

                        pLcmlDsp = (((LCML_DSP_INTERFACE*)pLcmlHandle)->dspCodec);
                        switch(pComponentPrivate->eComponentRole)
                        {
                            case AAC_DECODE:
                                AUDIODEC_DPRINT("\nfill the AAC specific LCML params: OMX_CommandStateSet->OMX_StateIdle \n");
                                tError = OMX_AAC_DEC_Fill_LCMLInitParams(pComponentPrivate->pHandle, pLcmlDsp, arr);
                                if(tError != OMX_ErrorNone) {
                                    AUDIODEC_DPRINT("\n error in filling the LCML params from OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:%x\n",tError);
                                    pComponentPrivate->tCurState = OMX_StateInvalid;
                                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                                    goto EXIT;
                                }
                                break;
                            case MP3_DECODE:
                                AUDIODEC_DPRINT("\nfill the MP3 specific LCML params: OMX_CommandStateSet->OMX_StateIdle \n");
                                tError = OMX_MP3_DEC_Fill_LCMLInitParams(pComponentPrivate->pHandle, pLcmlDsp, arr);
                                if(tError != OMX_ErrorNone) {
                                    AUDIODEC_DPRINT("\n error in filling the LCML params from OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:%x\n",tError);
                                    pComponentPrivate->tCurState = OMX_StateInvalid;
                                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                                    goto EXIT;
                                }
                                break;
                            default:
                                OMX_BASE_ASSERT(0, OMX_ErrorUnsupportedIndex);
                                break;
                        }/*switch case for componentrole ends*/

                        /*store the LCML handle in the componentprivate variable*/
                        AUDIODEC_DPRINT("\n get the LCML handle into componentprivate  from OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources\n");
                        pComponentPrivate->LCMLParams->pLcmlHandle = (LCML_DSP_INTERFACE *)pLcmlHandle;

                        /*initializing the LCML callbacks*/
                        AUDIODEC_DPRINT("\n map the LCML callback :OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources\n");
                        cb.LCML_Callback = (void *)OMX_AUDIO_DEC_LCML_Callback;

                        /*Initialize the LCML with the init params that have been obtained*/
                        AUDIODEC_DPRINT("\n Initialize the LCML initCodec: OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:\n");
                        tError = LCML_InitMMCodecEx(((LCML_DSP_INTERFACE *)pLcmlHandle)->pCodecinterfacehandle,
                                                    pArgs,&pLcmlHandle,(void *)pArgs,&cb, (OMX_STRING)pComponentPrivate->sDeviceString);
                        if (tError != OMX_ErrorNone) {
                            AUDIODEC_DPRINT("\n error from the LCML_initcodec(): OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:%x\n",tError);
                            goto EXIT;
                        }

#ifdef RESOURCE_MANAGER_ENABLED
                        /* Need check the resource with RM */
                        AUDIODEC_DPRINT("\n map the RM callback: OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:\n");
                        pComponentPrivate->rmproxyCallback.RMPROXY_Callback =(void *) OMX_AUDIO_DEC_ResourceManagerCallback;
                        if (pComponentPrivate->tCurState != OMX_StateWaitForResources){
                            rm_error = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                                              RMProxy_RequestResource,
                                                              eComponentNameForRM, CPULoadForRM,
                                                              3456, &(pComponentPrivate->rmproxyCallback));

                            if(rm_error == OMX_ErrorNone) {
                                /* resource is available */
                                AUDIODEC_DPRINT("\n send state info:idle to RM: OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:\n");
                                rm_error = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                                                  RMProxy_StateSet,
                                                                  eComponentNameForRM,
                                                                  OMX_StateIdle,
                                                                  3456,
                                                                  NULL);
                            }
                            else if(rm_error == OMX_ErrorInsufficientResources) {
                                /* resource is not available, need set state to OMX_StateWaitForResources */
                                pComponentPrivate->tCurState = OMX_StateWaitForResources;
                            }

                        }else{/*if it there in wait for resources it simply informs the rm that it is going to idle state and sets to idle state*/
                            AUDIODEC_DPRINT("\n send state info:idle to RM: OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:\n");
                            rm_error = RMProxy_NewSendCommand(pComponentPrivate->pHandle,
                                                              RMProxy_StateSet,
                                                              eComponentNameForRM,
                                                              OMX_StateIdle,
                                                              3456,
                                                              NULL);


                        }
#endif
                        /*no error:state transition successful:state set is done at base*/
                        AUDIODEC_DPRINT("\n event notify to base:successful transition: OMX_CommandStateSet->OMX_StateIdle<-loaded/waitforresources:\n");
                        pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventCmdComplete, Cmd, nParam, NULL);
                    }/*****************case OMX_StateIdle >>if (pComponentPrivate->tCurState == OMX_StateLoaded || pComponentPrivate->tCurState == OMX_StateWaitForResources)ends*******************************************/

                    else if (pComponentPrivate->tCurState == OMX_StateExecuting) {
                        /*from exec-> idle need to stop the codec*/
                        pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                        pComponentPrivate->bNoIdleOnStop=OMX_FALSE;
                        AUDIODEC_DPRINT("\n stop the codec: OMX_CommandStateSet->OMX_StateIdle<-OMX_StateExecuting:\n");
                        tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                                   MMCodecControlStop,(void *)pArgs);
                        if(tError != OMX_ErrorNone) {
                            AUDIODEC_DPRINT("\n error after the codec stop:OMX_CommandStateSet->OMX_StateIdle<-OMX_StateExecuting:%x\n",tError);
                            goto EXIT;
                        }
                        /*need to return all the buffers(i/p n o/p) to the suppliers via return notify calls*/
                        AUDIODEC_DPRINT("\n ready to read the pending buffers: OMX_CommandStateSet->OMX_StateIdle<-OMX_StateExecuting:\n");
                        tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                                     DATA_PIPE, OMX_DirInput);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }

                        tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                                     DATA_PIPE, OMX_DirOutput);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }

                        /*returning the  LCML paused pending buffers & notify event is done at the LCML callback*/
                    }/*case OMX_StateIdle >>else if (pComponentPrivate->tCurState == OMX_StateExecuting) ends*/

                    else if (pComponentPrivate->tCurState == OMX_StatePause) {
                        pComponentPrivate->bNoIdleOnStop=OMX_FALSE;
                        pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;

                        AUDIODEC_DPRINT("\n send command to LCML to stop: OMX_CommandStateSet->OMX_StateIdle<-OMX_StatePause:\n");
                        tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                                   MMCodecControlStop,(void *)pArgs);
                        if(tError != OMX_ErrorNone) {
                            AUDIODEC_DPRINT("\n error after stop codec:OMX_CommandStateSet->OMX_StateIdle<-OMX_StatePause:%x\n",tError);
                            goto EXIT;
                        }
                        /*need to return all the buffers to the suppliers via return notify calls*/
                        /*need to send the pending buffers(both in the paused state and LCML pending oness*/
                        /*returning the paused pending buffers & notify event is done at the LCML callback*/
                        tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                                     DATA_PIPE, OMX_DirInput);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }
                        tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(hComponent,
                                                                     DATA_PIPE, OMX_DirOutput);
                        if(tError != OMX_ErrorNone)
                        {
                            goto EXIT;
                        }

                    }/*case OMX_StateIdle >>else if (pComponentPrivate->tCurState == OMX_StatePause ends*/
                    else{

                        tError=OMX_ErrorIncorrectStateTransition;
                        AUDIODEC_DPRINT("\n error due to incorrect state transition:OMX_CommandStateSet->OMX_StateIdle:%x\n",tError);
                        goto EXIT;
                    }/*case OMX_StateIdle >>else ends*/
                    break;
                    /*stateidle case ends*/

                case OMX_StateExecuting:
                    /*StateExecuting case starts*/
                    if ( pComponentPrivate->tCurState == OMX_StateIdle) {
                        switch(pComponentPrivate->eComponentRole){
                            case AAC_DECODE:
                                AUDIODEC_DPRINT("\n initialize the SN : OMX_CommandStateSet->OMX_Executing<-idle:\n");
                                tError = OMX_AAC_DEC_SN_InitParams(hComponent);
                                if(tError != OMX_ErrorNone) {
                                    AUDIODEC_DPRINT("\n error after Sn initialization:OMX_CommandStateSet->OMX_StateExecuting:%x\n",tError);
                                    goto EXIT;
                                }
                                break;
                            case MP3_DECODE:
                                AUDIODEC_DPRINT("\n initialize the SN : OMX_CommandStateSet->OMX_Executing<-idle:\n");
                                tError = OMX_MP3_DEC_SN_InitParams(hComponent);
                                if(tError != OMX_ErrorNone) {
                                    AUDIODEC_DPRINT("\n error after Sn initialization:OMX_CommandStateSet->OMX_StateExecuting:%x\n",tError);
                                    goto EXIT;
                                }
                                break;
                            default:
                                /*OMX_BASE_ASSERT(0, OMX_ErrorRoleUndefined);*/
                                break;
                        }
                    }
                    else if (pComponentPrivate->tCurState == OMX_StatePause) {
                        pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                        AUDIODEC_DPRINT("\n start command to codec: OMX_CommandStateSet->OMX_Executing<-pause:\n");
                        tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                                   EMMCodecControlStart,(void *)pArgs);
                        if (tError != OMX_ErrorNone) {
                            AUDIODEC_DPRINT("\n error after start command:OMX_CommandStateSet->OMX_StateExecuting<-pause:%x\n",tError);
                            goto EXIT;
                        }
                        /*the paused pending buffers are processed at data notify call..after the state is set to executing*/

                    }/*end of paused state*/
                    else{
                        tError=OMX_ErrorIncorrectStateTransition;
                        AUDIODEC_DPRINT("\n error due to incorrect state transition:OMX_CommandStateSet->OMX_StateExecuting:%x\n",tError);
                        goto EXIT;
                    }
#ifdef RESOURCE_MANAGER_ENABLED
                    rm_error = RMProxy_NewSendCommand(hComponent, RMProxy_StateSet, eComponentNameForRM, OMX_StateExecuting, 3456, NULL);
#endif
                    AUDIODEC_DPRINT("\n event notify to base:successful transition: OMX_CommandStateSet->Executing:\n");
                    pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventCmdComplete, Cmd, nParam, NULL);

                    break;
                    /*StateExecuting case ends*/

                case OMX_StateLoaded:
                    /*StateLoaded case starts*/

                    if (pComponentPrivate->tCurState == OMX_StateWaitForResources ){
                        AUDIODEC_DPRINT("\n event notify to base:successful transition: OMX_CommandStateSet->OMX_Loaded:\n");
                        pComponentPrivate->fpReturnEventNotify(hComponent,OMX_EventCmdComplete, Cmd, nParam, NULL);
                        goto EXIT;
                    }
                    if (pComponentPrivate->tCurState != OMX_StateIdle) {
                        tError=OMX_ErrorIncorrectStateTransition;
                        AUDIODEC_DPRINT("\n error due to incorrect state transition:OMX_CommandStateSet->OMX_StateLoaded:%x\n",tError);
                        goto EXIT;
                    }
                    pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                    /*base is taking care of  freeing the resouces before this tarnsition occurs*/
                    AUDIODEC_DPRINT("\n destroy the codec: OMX_CommandStateSet->OMX_Loaded:\n");
                    tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                               EMMCodecControlDestroy,(void *)pArgs);
                    if(tError != OMX_ErrorNone)
                    {
                        AUDIODEC_DPRINT("\n error after destroy codec:OMX_CommandStateSet->OMX_StateLoaded:%x\n",tError);
                        goto EXIT;
                    }
                    AUDIODEC_DPRINT("\n event notify to base:successful transition: OMX_CommandStateSet->OMX_StateLoaded:\n");
                    pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventCmdComplete, Cmd, nParam, NULL);

#ifdef RESOURCE_MANAGER_ENABLED
                    rm_error = RMProxy_NewSendCommand(hComponent, RMProxy_StateSet, eComponentNameForRM, OMX_StateLoaded, 3456, NULL);
#endif
                    break;

                    /*StateLoaded case ends*/

                case OMX_StatePause:
                    /*StatePause case starts*/
                    if ((pComponentPrivate->tCurState != OMX_StateExecuting) &&(pComponentPrivate->tCurState != OMX_StateIdle)) {
                        tError=OMX_ErrorIncorrectStateTransition;
                        AUDIODEC_DPRINT("\n error due to incorrect state transition:OMX_CommandStateSet->OMX_StatePause:%x\n",tError);
                        goto EXIT;
                    }
                    pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                    AUDIODEC_DPRINT("\n pause command to codec: OMX_CommandStateSet->OMX_StatePause:\n");
                    tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                               EMMCodecControlPause,(void *)pArgs);

                    if (tError != OMX_ErrorNone) {
                        AUDIODEC_DPRINT("\n error after pause codec command to LCML:OMX_CommandStateSet->OMX_StatePause:%x\n",tError);
                        goto EXIT;
                    }
                    //is done at LCML callback:pBaseComPvt->fpReturnEventNotify(hComponent, OMX_EventCmdComplete, Cmd, nParam, NULL);

                    break;
                    /*StatePause case ends*/


                case OMX_StateWaitForResources:
                    /*StateWaitForResources case starts*/
                    if (pComponentPrivate->tCurState == OMX_StateLoaded) {
#ifdef RESOURCE_MANAGER_ENABLED
                        rm_error = RMProxy_NewSendCommand(hComponent, RMProxy_StateSet, eComponentNameForRM, OMX_StateWaitForResources, 3456,NULL);
#endif
                    }
                    else {
                        tError=OMX_ErrorIncorrectStateTransition;
                        AUDIODEC_DPRINT("\n error due to incorrect state transition:OMX_CommandStateSet->OMX_StateWaitforresources:%x\n",tError);
                        goto EXIT;
                    }
                    AUDIODEC_DPRINT("\n event notify to base:successful transition: OMX_CommandStateSet->OMX_Statewaitforresources:\n");
                    pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventCmdComplete, Cmd, nParam, NULL);
                    break;


                    /*StateWaitForResources case ends*/



                case OMX_StateInvalid:
                    /*StateInvalid case starts*/
                    if (pComponentPrivate->tCurState  != OMX_StateWaitForResources &&
                        pComponentPrivate->tCurState != OMX_StateInvalid &&
                        pComponentPrivate->tCurState  != OMX_StateLoaded) {
                        pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
                        AUDIODEC_DPRINT("\n destroy the codec: OMX_CommandStateSet->OMX_StateInvalid:\n");
                        tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                                   EMMCodecControlDestroy, (void *)pArgs);
                        if (tError != OMX_ErrorNone) {
                            AUDIODEC_DPRINT("\n error after the destroy codec:OMX_CommandStateSet->OMX_StateInvalid:%x\n",tError);
                            goto EXIT;
                        }
                    }
                    AUDIODEC_DPRINT("\n event notify to base:successful transition: OMX_CommandStateSet->OMX_StateInvalid:\n");
                    pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventCmdComplete, Cmd, nParam, NULL);
                    AUDIODEC_DPRINT("\n LCML_Deinit:successful transition: OMX_CommandStateSet->OMX_StateInvalid:\n");
                    tError =OMX_LCML_DeInit(hComponent);
                    if (tError != OMX_ErrorNone) {
                        AUDIODEC_DPRINT("\n error after LCML Deinit:OMX_CommandStateSet->OMX_StateInvalid:%x\n",tError);
                        goto EXIT;
                    }

                    break;
                    /*StateInvalid case ends*/
                default:
                    AUDIODEC_DPRINT("\nWarning:entered in default case:need to set the curstate properly\n");
                    break;
            }/*switch(commandedState) ends*/

            break;/*commandstateset ends*/
            /*case OMX_CommandMarkBuffer: is done at the base*/

        default:
            AUDIODEC_DPRINT("\nWarning:entered in default case:need to set the cmd properly\n");
            break;
    }/*switch case ends*/



 EXIT:
    AUDIODEC_DPRINT("\n error from command notify:%x\n",tError);
    return tError;

}/*command notify ends*/

/* ============================================================================*/
/*
*@fn OMX_AUDIO_DEC_DataNotify()- This method is an hook from the Base Component to
*                                notify the Data either input or output buffers which need
*                                processed by the Derived Component. once the buffer is
*                                processed notify to the Base Component via ReturnDataNotify
*                                 call to communicate with the IL client or tunneled component.
* @param [in]     hComponent  :  Handle of the component to be accessed.
* @param [in]     nPortIndex  :  Index of the port
* @param [in]     pBufHeader  :  Pointer to an Bufferheader which need to be processed
* @see         None
*/
/*============================================================================*/

OMX_ERRORTYPE OMX_AUDIO_DEC_DataNotify( OMX_HANDLETYPE hComponent,
                                        OMX_U32 nPortIndex,
                                        OMX_BUFFERHEADERTYPE* pBufHeader)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate= NULL;
    /*pointers to the auxilary info of the buffer that need to be sent to the LCML*/
    UAlgInBufParamStruct* pIpParam=NULL;
    UAlgOutBufParamStruct* pOpParam=NULL;
    /*for pipes processing*/
    OMX_BUFFERHEADERTYPE* pInBufHeader = NULL;
    OMX_BUFFERHEADERTYPE* pOutBufHeader = NULL;
    TIMM_OSAL_U32 actualSize = 0;
    TIMM_OSAL_U32 elementsInPipe = 0;
    TIMM_OSAL_U32 elementsOutPipe = 0;
    OMX_ERRORTYPE inStatus, outStatus;
    char* pTemp_char = NULL;

    /*check i/p parameters*/
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((pBufHeader != NULL), OMX_ErrorBadParameter);
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    LCML_DSP_INTERFACE *pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;


    /*Check for version(omx version==bufferhdr version) is done at the base*/
    /*check for the valid port index(i/p port or o/p port) is done at the base*/
    /*check for the valid state (executing/paused state) is done at the base*/

    /*keep the buffers that are coming in the pipe
      then do the processing if it is there in executing */
    if(nPortIndex == INPUT_PORT)
    {
        AUDIODEC_DPRINT("\n write the i/p buffer in i/p pipe\n");
        tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe
            ((pComponentPrivate->pInDataPipe),
             &pBufHeader,
             sizeof(pBufHeader), TIMM_OSAL_NO_SUSPEND);
        OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
    }
    if(nPortIndex == OUTPUT_PORT)
    {
        AUDIODEC_DPRINT("\n write the o/p buffer in o/p pipe\n");
        tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe
            ((pComponentPrivate->pOutDataPipe),
             &pBufHeader,
             sizeof(pBufHeader), TIMM_OSAL_NO_SUSPEND);
        OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
    }

    if(pComponentPrivate->tCurState == OMX_StateExecuting)
    {
        AUDIODEC_DPRINT("\n component in executing state so start processing the buffers\n");
        /* Check for whether the input and output pipes are ready  */
        inStatus  = (OMX_ERRORTYPE)TIMM_OSAL_IsPipeReady
            (pComponentPrivate->pInDataPipe);
        outStatus = (OMX_ERRORTYPE)TIMM_OSAL_IsPipeReady
            (pComponentPrivate->pOutDataPipe);

        if(inStatus == TIMM_OSAL_ERR_NONE)
        {
            TIMM_OSAL_GetPipeReadyMessageCount (pComponentPrivate->pInDataPipe,
                                                &elementsInPipe);

            while(elementsInPipe)
            {

                /* Reading an input buffer from input data pipe to process  */
                AUDIODEC_DPRINT("\n reading i/p buffer from the i/p queue\n");
                TIMM_OSAL_ReadFromPipe((pComponentPrivate->pInDataPipe),
                                       &pInBufHeader, sizeof(pInBufHeader),
                                       &actualSize, TIMM_OSAL_SUSPEND);
                OMX_BASE_ASSERT(pInBufHeader != NULL,
                                OMX_ErrorInsufficientResources);

                if ((pInBufHeader->nFilledLen <= 0)&&(!(pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS))){
                    /*if the buffer contains no data then need to bypass the DSP by just calling returndatanotify call*/
                    pComponentPrivate->bBypassDSP = 1;
                    AUDIODEC_DPRINT("\n return data notify for filledlen=0 i/p buffer\n");
                    pComponentPrivate->fpReturnDataNotify(hComponent,INPUT_PORT, pInBufHeader);

                }
                else{
                    /*allocate memory for inauxiinfo*/
                    pIpParam = (UAlgInBufParamStruct*)TIMM_OSAL_Malloc(sizeof(UAlgInBufParamStruct) +
                                                                   DSP_CACHE_ALIGNMENT);
                    OMX_BASE_ASSERT(pIpParam != NULL,OMX_ErrorInsufficientResources);

                    pTemp_char = (char*)pIpParam;
                    pTemp_char += EXTRA_BYTES;
                    pIpParam = (UAlgInBufParamStruct*)pTemp_char;

                    /*get the aux info from the buffer flags*/
                    pIpParam->bLastBuffer = 0;
                    pIpParam->bConcealBuffer = 0;/*not checked the conditon yet*/
                    if(pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS){
                        pIpParam->bLastBuffer = 1; /*this is sent to the codec as auxilary information*/
                    }
                    if (pInBufHeader->nFlags & OMX_BUFFERFLAG_DATACORRUPT){
                        pIpParam->bConcealBuffer = 1;
                    }
                    /*????????not sure whether it is required or not ... set the codec with the required params for the next one buffer after the EOS buffer*/
                    /*not sure whether this condition is necessary or not*/
                    /*
                      if(pComponentPrivate->codec_params.aacdecParams->SendAfterEOS == 1){
                      switch(pComponentPrivate->eComponentRole){
                      case AAC_DECODE:
                      AUDIODEC_DPRINT("\n SN Init\n");
                      tError=OMX_AAC_DEC_SN_InitParams(hComponent);
                      OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
                      break;
                      default:
                      break;
                      }
                      pComponentPrivate->codec_params.aacdecParams->SendAfterEOS = 0;
                      }
                    */
                    /*if received EOS then need to notify to the corresponding o/p buffer*/
                    if(pInBufHeader->nFlags & OMX_BUFFERFLAG_EOS) {
                        AUDIODEC_DPRINT("\n EOS at i/p buffer\n");
                        pComponentPrivate->bIsEOFSent = 1;/*need to send this to the next output buffer that comes for processing*/
                        pInBufHeader->nFlags = 0;
                        //RADHA pComponentPrivate->codec_params.aacdecParams->SendAfterEOS = 1;
                    }

                    /*Handle the Mark Data at the i/p port*/
                    if(pBufHeader->pMarkData){
                        AUDIODEC_DPRINT("\n mark data obtained at i/p buffer\n");
                        /*check whether this port is the targetcomponent for the mark data*/
                        if(pBufHeader->hMarkTargetComponent==hComponent){
                            AUDIODEC_DPRINT("\n return event notify for mark component\n");
                            pComponentPrivate->fpReturnEventNotify(hComponent, OMX_EventMark, 0,
                                                                   0, pInBufHeader->pMarkData);
                        }else{
                            pComponentPrivate->pMark->pMarkData = pBufHeader->pMarkData;
                            pComponentPrivate->pMark->hMarkTargetComponent = pBufHeader->hMarkTargetComponent;
                            /*need to pass it to the output buffer as well*/
                            pComponentPrivate->bMarkDataIn=1;/*flag indicating that need to copy the mark data to the nxt coming o/p buffer*/
                        }
                    }
                    /*keeping the buffers the we r going to send to the LCML in LCML pipe*/
                    AUDIODEC_DPRINT("\n keeping the LCMLsending i/p buffers in LCMLi/ppedndingqueue \n");
                    tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe((pComponentPrivate->
                                                                   LCMLParams->pInBufPendingWithLcmlPipe),
                                                                  &pInBufHeader, sizeof(pInBufHeader),
                                                                  TIMM_OSAL_NO_SUSPEND);
                    OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
                    /*send the buffer to LCML*/
                    AUDIODEC_DPRINT("\n send i/p buffer to LCML \n");
                    tError = LCML_QueueBuffer(pLcmlHandle->pCodecinterfacehandle,
                                              EMMCodecInputBuffer,
                                              pInBufHeader->pBuffer,
                                              pInBufHeader->nAllocLen,
                                              pInBufHeader->nFilledLen,
                                              (OMX_U8 *)pIpParam,
                                              sizeof(UAlgInBufParamStruct),
                                              NULL);
                    if (tError != OMX_ErrorNone) {
                        tError = OMX_ErrorHardware;
                        AUDIODEC_DPRINT("\n error in LCML queue i/p buffer:%x\n",tError);
                        /*need to remove the buffer from the LCMLi/ppendingpipe*/
                        tError = (OMX_ERRORTYPE)TIMM_OSAL_ReadFromPipe
                            ((pComponentPrivate->LCMLParams->pInBufPendingWithLcmlPipe),
                             &pInBufHeader, sizeof(pInBufHeader),
                             &actualSize, TIMM_OSAL_NO_SUSPEND);
                        OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
                        goto EXIT;
                    }



                }/*else: nfilledlen>0 ends*/
                elementsInPipe--;

            }/*while(elementsInpipe) ends*/

        }/*if(inStatus == TIMM_OSAL_ERR_NONE) ends*/
        if(outStatus == TIMM_OSAL_ERR_NONE)
        {
            TIMM_OSAL_GetPipeReadyMessageCount (pComponentPrivate->pOutDataPipe,
                                                &elementsOutPipe);

            while(elementsOutPipe)
            {
                AUDIODEC_DPRINT("\nreading the o/p buffer from the o/p queue\n");
                TIMM_OSAL_ReadFromPipe((pComponentPrivate->pOutDataPipe),
                                       &pOutBufHeader, sizeof(pOutBufHeader),
                                       &actualSize, TIMM_OSAL_SUSPEND);
                OMX_BASE_ASSERT(pOutBufHeader != NULL,
                                OMX_ErrorInsufficientResources);


                /*handle the mark data that has arrived at the i/p need to pass it to the o/p*/
                if(pComponentPrivate->bMarkDataIn==1){
                    AUDIODEC_DPRINT("\n passing the mark data to o/p buffer\n");
                    pOutBufHeader->pMarkData=pComponentPrivate->pMark->pMarkData  ;
                    pOutBufHeader->hMarkTargetComponent=pComponentPrivate->pMark->hMarkTargetComponent;
                    pComponentPrivate->bMarkDataIn=0;
                }
                /*handle the EOS*/
                if(pComponentPrivate->bIsEOFSent == 1){
                    AUDIODEC_DPRINT("\n EOS sent to the o/p buffer\n");
                    pOutBufHeader->nFlags |= OMX_BUFFERFLAG_EOS;
                    pComponentPrivate->bIsEOFSent =0;
                }
                if (pComponentPrivate->bBypassDSP == 0) {
                    pOpParam = (UAlgOutBufParamStruct*)TIMM_OSAL_Malloc(sizeof(UAlgOutBufParamStruct) +
                                                                        DSP_CACHE_ALIGNMENT);
                    OMX_BASE_ASSERT(pOpParam != NULL,OMX_ErrorInsufficientResources);

                    pTemp_char = (char*)pOpParam;
                    pTemp_char += EXTRA_BYTES;
                    pOpParam = (UAlgOutBufParamStruct*)pTemp_char;

                    /*frame count:pOpParam->ulFrameCount is always set to zero*/
                    pOpParam->ulFrameCount=0;
                    /*queueup  the buffers the we r going to send to the LCML in LCMLpipe*/
                    AUDIODEC_DPRINT("\n keeping the LCMLsent o/p buffers in LCMLo/ppedndingqueue \n");
                    tError = (OMX_ERRORTYPE)TIMM_OSAL_WriteToPipe((pComponentPrivate-> LCMLParams->pOutBufPendingWithLcmlPipe),
                                                                  &pOutBufHeader, sizeof(pOutBufHeader),
                                                                  TIMM_OSAL_NO_SUSPEND);

                    OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
                    AUDIODEC_DPRINT("\n send o/p buffer to LCML \n");
                    tError = LCML_QueueBuffer(pLcmlHandle->pCodecinterfacehandle,
                                              EMMCodecOuputBuffer,
                                              pOutBufHeader->pBuffer,
                                              pOutBufHeader->nAllocLen,
                                              0,
                                              (OMX_U8 *)pOpParam,
                                              sizeof(UAlgOutBufParamStruct),
                                              pOutBufHeader->pBuffer);
                    if (tError != OMX_ErrorNone ) {
                        tError = OMX_ErrorHardware;
                        AUDIODEC_DPRINT("\n error in LCML queue o/p buffer:%x\n",tError);
                        /*need to remove the buffer from the LCMLpending pipe*/
                        tError = (OMX_ERRORTYPE)TIMM_OSAL_ReadFromPipe
                            ((pComponentPrivate->LCMLParams->pOutBufPendingWithLcmlPipe),
                             &pOutBufHeader, sizeof(pOutBufHeader),
                             &actualSize, TIMM_OSAL_NO_SUSPEND);
                        OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
                        goto EXIT;
                    }

                }else {
                    AUDIODEC_DPRINT("\n data notify:bypassingdsp for the corresponding o/p buffer\n");
                    /*by pass DSP ==1;
                      need to sent the corresponding o/p buffer without sending it for processing*/
                    pComponentPrivate->fpReturnDataNotify(hComponent,OUTPUT_PORT, pOutBufHeader);

                }

                elementsOutPipe--;
            }/*while(elementsOutPipe)ends*/
        }/*if(outStatus == TIMM_OSAL_ERR_NONE) ends*/
    }/*state executing ends*/

 EXIT:
    return tError;

}/*data notify ends*/
/* ================================================================================= * */
/*
*@fn OMX_AUDIO_DEC_LCML_Callback():     function is callback which is called by LCML whenever
*                                       there is an event generated for the component.
* @param event  This is event that was generated.
* @param arg    This has other needed arguments supplied by LCML like handles
*               etc.
* @pre          None
* @post         None
* @return       OMX_ErrorNone = Successful processing.
*               OMX_ErrorInsufficientResources = Not enough memory.
*  @see         None
*/
/* ================================================================================ * */

OMX_ERRORTYPE OMX_AUDIO_DEC_LCML_Callback (TUsnCodecEvent event,void * args [10])
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_U8 *pBuffer = args[1];/*args[1] are sent by the LCML containing the bufaddr to which LCML is giving the event notifycall*/
    OMX_COMPONENTTYPE *pHandle = NULL;
    LCML_DSP_INTERFACE *pLcmlHandle;
    AUDIODEC_COMPONENT_PRIVATE* pComponentPrivate = NULL;
    OMX_U32 pValues[4];
    /*pipes related*/
    OMX_BUFFERHEADERTYPE *pInBufHeader = NULL;
    OMX_BUFFERHEADERTYPE *pOutBufHeader = NULL;
    TIMM_OSAL_U32 actualSize = 0;

    #ifdef RESOURCE_MANAGER_ENABLED
    OMX_ERRORTYPE rm_error = OMX_ErrorNone;
    #endif

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE*)((LCML_DSP_INTERFACE*)args[6])->pComponentPrivate;
    pHandle=pComponentPrivate->pHandle;

    switch(event) {
    case EMMCodecBufferProcessed:
        AUDIODEC_DPRINT("\nin EMMCodecBufferProcessed at LCML call back\n");
        if( args[0] == (void *)EMMCodecInputBuffer) {
        /* get the buffer from lcml pipe*/
            AUDIODEC_DPRINT("\n reading the i/p buffer from the LCMLi/ppedndingqueue \n");
            tError = (OMX_ERRORTYPE)TIMM_OSAL_ReadFromPipe
                                            ((pComponentPrivate->LCMLParams->pInBufPendingWithLcmlPipe),
                                            &pInBufHeader, sizeof(pInBufHeader),
                                            &actualSize, TIMM_OSAL_NO_SUSPEND);
             OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
            /*Buffers should come back in the order in which they were processed else error*/
             if(pInBufHeader->pBuffer != pBuffer)
             {
                tError = OMX_ErrorUndefined;
                AUDIODEC_DPRINT("\n error in LCMLi/ppendingqueue i/p buffer:%x\n",tError);
                goto EXIT;
            }
            /*notify the event to the base via returndatanotify*/
            AUDIODEC_DPRINT("\ndata notify to the base:i/p buffer at LCML call back\n");
            pComponentPrivate->fpReturnDataNotify(pComponentPrivate->pHandle,INPUT_PORT, pInBufHeader);
        }
        else if (args[0] == (void *)EMMCodecOuputBuffer) {
        /* get the buffer from lcml pipe*/
            AUDIODEC_DPRINT("\n reading the o/p buffer from the  LCMLo/ppedndingqueue \n");
            tError = (OMX_ERRORTYPE)TIMM_OSAL_ReadFromPipe
                                            ((pComponentPrivate->LCMLParams->pOutBufPendingWithLcmlPipe),
                                            &pOutBufHeader, sizeof(pOutBufHeader),
                                            &actualSize, TIMM_OSAL_NO_SUSPEND);
            OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
            /*Buffers should come back in the order in which they were processed else error*/
            if(pOutBufHeader->pBuffer != pBuffer)           {
                tError = OMX_ErrorUndefined;
                AUDIODEC_DPRINT("\n error in LCMLo/ppendingqueue o/p buffer:%x\n",tError);
                goto EXIT;
            }
            pOutBufHeader->nFilledLen = (int)args[8];
            /*if the EOS is received then notify it to the base via eventnotify call*/
            if (pOutBufHeader->nFlags & OMX_BUFFERFLAG_EOS){
                /* Note: Notify this completion to the Base comp via ReturnEventNotify call */
                AUDIODEC_DPRINT("\nevent notify to the base:EOS at o/p buffer at LCML call back\n");
                pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventBufferFlag,OUTPUT_PORT,pOutBufHeader->nFlags, NULL);
            }
            /*notify the event to the base via returndatanotify*/
            AUDIODEC_DPRINT("\ndata notify to the base:o/p buffer at LCML call back\n");
            pComponentPrivate->fpReturnDataNotify(pComponentPrivate->pHandle,OUTPUT_PORT, pOutBufHeader);
            /*IF time stamp information is needed need to add that here...*/
        }
        /**************************************EMMCodecBufferProcessed*******************************************************/
    break;
    case EMMCodecProcessingStoped:
        /* need to send all the LCML pending buffers*/
        AUDIODEC_DPRINT("\nin EMMCodecProcessingStoped at LCML call back\n");
        tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(pHandle,
                                                    LCML_DATA_PIPE, OMX_DirInput);
        if(tError != OMX_ErrorNone)
        {
            goto EXIT;
        }
        tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(pHandle,
                                                    LCML_DATA_PIPE, OMX_DirOutput);
        if(tError != OMX_ErrorNone)
        {
            goto EXIT;
        }
        if (pComponentPrivate->bNoIdleOnStop==OMX_FALSE) {
            #ifdef RESOURCE_MANAGER_ENABLED
                OMX_ERRORTYPE rm_error = OMX_ErrorNone;
                OMX_COMPONENTINDEXTYPE eComponentNameForRM;
                OMX_U32 CPULoadForRM;
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_DECODE:
                        eComponentNameForRM = OMX_AAC_Decoder_COMPONENT;
                        CPULoadForRM = AACDEC_CPU_USAGE;
                    break;
                    case MP3_DECODE:
                        eComponentNameForRM = OMX_MP3_Decoder_COMPONENT;
                        CPULoadForRM = MP3DEC_CPU_USAGE;
                    break;
                    default:
                    break;
                }

            rm_error = RMProxy_NewSendCommand(pComponentPrivate->pHandle, RMProxy_StateSet, eComponentNameForRM, OMX_StateIdle, 3456, NULL);
            #endif /*Resource Manager*/
            AUDIODEC_DPRINT("\nevent notify to the base:state transition to idle at LCML call back\n");
            pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventCmdComplete, OMX_CommandStateSet,pComponentPrivate->tCurState, NULL);
       }
        if (pComponentPrivate->bNoIdleOnStop==OMX_TRUE) {
            AUDIODEC_DPRINT("\nevent notify to the base:port disabled at LCML call back\n");
            pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventCmdComplete, OMX_CommandPortDisable,OUTPUT_PORT, NULL);
            pComponentPrivate->bNoIdleOnStop = OMX_FALSE;
       }
        if(pComponentPrivate->bNoIdleOnStop==OMX_CODECSTOP){
            AUDIODEC_DPRINT("\ncodec is stoped becuase of DSP error: at LCML call back\n");
        }
    /**************************************EMMCodecProcessingStoped*******************************************************/
    break;
    case EMMCodecAlgCtrlAck:
         break;
    case EMMCodecDspError:
        AUDIODEC_DPRINT("\nin EMMCodecDspError at LCML call back\n");

        if(((int)args[4] == USN_ERR_WARNING) && ((int)args[5] == IUALG_WARN_PLAYCOMPLETED)) {
            AUDIODEC_DPRINT("\nevent notify to the base:received EOS at LCML call back\n");
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventBufferFlag,
                                                       (OMX_U32)NULL,
                                                       OMX_BUFFERFLAG_EOS,NULL);
        }
        if((int)args[5] == IUALG_WARN_CONCEALED) {
        }
        if((int)args[5] == IUALG_ERR_GENERAL) {
        }
        if( (int)args[5] == IUALG_ERR_DATA_CORRUPT ) {
            char *pArgs = "damedesuStr";
            pHandle = pComponentPrivate->pHandle;
            pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
            pComponentPrivate->bNoIdleOnStop=OMX_CODECSTOP;
            AUDIODEC_DPRINT("\nsend command to codec to stop at LCML call back\n");
            tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                       MMCodecControlStop,(void *)pArgs);
            if(tError != OMX_ErrorNone) {
                pComponentPrivate->tCurState = OMX_StateInvalid;
                AUDIODEC_DPRINT("\nevent notify to the base:invalidstate at LCML call back\n");
                pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                goto EXIT;
            }
            /*send command to state set to idle*/
            tError = pHandle->SendCommand(pHandle,OMX_CommandStateSet,OMX_StateIdle,NULL);
            if(tError != OMX_ErrorNone)
            {
                pComponentPrivate->fpReturnEventNotify(pHandle,
                                                       OMX_EventError,
                                                       tError,0,NULL);
            }
            #ifdef RESOURCE_MANAGER_ENABLED
            OMX_ERRORTYPE rm_error = OMX_ErrorNone;
            OMX_COMPONENTINDEXTYPE eComponentNameForRM;
            OMX_U32 CPULoadForRM;
            switch(pComponentPrivate->eComponentRole)
            {
                case AAC_DECODE:
                    eComponentNameForRM = OMX_AAC_Decoder_COMPONENT;
                    CPULoadForRM = AACDEC_CPU_USAGE;
                break;
                case MP3_DECODE:
                    eComponentNameForRM = OMX_MP3_Decoder_COMPONENT;
                    CPULoadForRM = MP3DEC_CPU_USAGE;
                break;
                default:
                break;  //error
            }
            rm_error = RMProxy_NewSendCommand(pComponentPrivate->pHandle, RMProxy_StateSet, eComponentNameForRM, OMX_StateIdle, 3456, NULL);
            #endif /*Resource Manager*/

        }
        if ( ( (int)args[4] == USN_ERR_WARNING ) && ( (int)args[5] == AACDEC_SBR_CONTENT)) {
            if(pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile != OMX_AUDIO_AACObjectHE &&
               pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile != OMX_AUDIO_AACObjectHE_PS){

                pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile = OMX_AUDIO_AACObjectHE;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->nProfile = OMX_AUDIO_AACObjectHE;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->iEnablePS =  0;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->DownSampleSbr = 1;

                pValues[0] = IUALG_CMD_SETSTATUS;
                pValues[1] = (OMX_U32)pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam;
                pValues[2] = sizeof(MPEG4AACDEC_UALGParams);
                AUDIODEC_DPRINT("\nsend command to LCML for alg cntrl :LCML call back\n");

                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pComponentPrivate->LCMLParams->pLcmlHandle)->pCodecinterfacehandle,
                                           EMMCodecControlAlgCtrl,(void *)pValues);
                if(tError != OMX_ErrorNone) {
                    pComponentPrivate->tCurState = OMX_StateInvalid;
                    AUDIODEC_DPRINT("\nevent notify to the base:invalidstate at LCML call back\n");
                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                    goto EXIT;
                }
                AUDIODEC_DPRINT("\nevent notify to the base:port settings changed at LCML call back\n");
                pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle,
                                                       OMX_EventPortSettingsChanged,
                                                       OMX_DirInput,
                                                       OMX_AUDIO_AACObjectHE,
                                                       NULL);
            }
        }
        if ( ( (int)args[4] == USN_ERR_WARNING ) && ( (int)args[5] == AACDEC_PS_CONTENT )){
            if(pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile != OMX_AUDIO_AACObjectHE_PS){
                pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile = OMX_AUDIO_AACObjectHE_PS;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->nProfile = OMX_AUDIO_AACObjectHE_PS;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->iEnablePS =  1;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->DownSampleSbr = 1;

                pValues[0] = IUALG_CMD_SETSTATUS;
                pValues[1] = (OMX_U32)pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam;
                pValues[2] = sizeof(MPEG4AACDEC_UALGParams);
                AUDIODEC_DPRINT("\nalgcntrl command to LCML at LCML call back\n");
                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pComponentPrivate->LCMLParams->pLcmlHandle)->pCodecinterfacehandle,
                                           EMMCodecControlAlgCtrl,(void *)pValues);
                if(tError != OMX_ErrorNone) {
                    pComponentPrivate->tCurState = OMX_StateInvalid;
                    AUDIODEC_DPRINT("\nevent notify to the base:invalidstate at LCML call back\n");
                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                    goto EXIT;
                }

                AUDIODEC_DPRINT("\nevent notify to the base:port settings changed at LCML call back\n");
                pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle,
                                                       OMX_EventPortSettingsChanged,
                                                       OMX_DirInput,
                                                       OMX_AUDIO_AACObjectHE_PS,
                                                       NULL);
            }

        }
        if( (int)args[5] == IUALG_WARN_OVERFLOW ){
        }
        if( (int)args[5] == IUALG_WARN_UNDERFLOW ){
        }
        if( (int)args[4] == USN_ERR_PROCESS ){
            pHandle = pComponentPrivate->pHandle;
            char *pArgs = "damedesuStr";
            pLcmlHandle = (LCML_DSP_INTERFACE *)pComponentPrivate->LCMLParams->pLcmlHandle;
            pComponentPrivate->bNoIdleOnStop=OMX_CODECSTOP;
            AUDIODEC_DPRINT("\nstop codec command to LCML at LCML call back\n");
            tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                                       MMCodecControlStop,(void *)pArgs);
            if(tError != OMX_ErrorNone) {
                pComponentPrivate->tCurState = OMX_StateInvalid;
                AUDIODEC_DPRINT("\nevent notify to the base:invalidstate at LCML call back\n");
                pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                goto EXIT;
            }
            pComponentPrivate->tCurState = OMX_StateInvalid;
            AUDIODEC_DPRINT("\nevent notify to the base:invalidstate at LCML call back\n");
            pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle,OMX_EventError,OMX_ErrorInvalidState,0, NULL);
            goto EXIT;
        }
    /**************************************EMMCodecDspError*******************************************************/
    break;

    case EMMCodecStrmCtrlAck:
        AUDIODEC_DPRINT("\nEMMCodecStrmCtrlAck at LCML call back\n");

        if (args[1] == (void *)USN_STRMCMD_FLUSH) {
            pHandle = pComponentPrivate->pHandle;
            if ( args[2] == (void *)EMMCodecInputBuffer) {
                if (args[0] == (void *)USN_ERR_NONE ) {
                /*flush the lcml pending*/
                    tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(pHandle,
                                                                LCML_DATA_PIPE, OMX_DirInput);
                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                    pComponentPrivate->fpReturnEventNotify(pHandle,OMX_EventCmdComplete, OMX_CommandFlush,INPUT_PORT, NULL);
                }
                else {
                    goto EXIT;
                }
            }else if ( args[2] == (void *)EMMCodecOuputBuffer) {
                if (args[0] == (void *)USN_ERR_NONE ) {
                /* flush the lcml pending*/
                tError = _OMX_AUDIO_DEC_ReturnPendingBuffers(pHandle,
                                                            LCML_DATA_PIPE, OMX_DirOutput);
                if(tError != OMX_ErrorNone)
                {
                    goto EXIT;
                }
                AUDIODEC_DPRINT("\nevent notify to the base: at LCML call back\n");
                pComponentPrivate->fpReturnEventNotify(pHandle,OMX_EventCmdComplete, OMX_CommandFlush,OUTPUT_PORT, NULL);
                }
                else {
                    goto EXIT;
                }
            }
        }
    /**************************************EMMCodecStrmCtrlAck*******************************************************/
    break;
    case EMMCodecProcessingPaused :
         AUDIODEC_DPRINT("\nevent notify to the base :EMMCodecProcessingPaused at LCML call back\n");
         pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle,OMX_EventCmdComplete, OMX_CommandStateSet,
                                               pComponentPrivate->tCurState, NULL);


    /**************************************EMMCodecProcessingPaused*******************************************************/
    break;
    case EMMCodecInitError:
        /* Cheking for MMU_fault */
        if((args[4] == (void*)USN_ERR_UNKNOWN_MSG) && (args[5] == (void*)NULL)) {
            pComponentPrivate->tCurState = OMX_StateInvalid;
            pHandle = pComponentPrivate->pHandle;
            AUDIODEC_DPRINT("\neventnotify to the base :EMMCodecInitError at LCML call back\n");
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                   OMX_EventError,
                                                   OMX_ErrorInvalidState,
                                                   0x2,
                                                   NULL);
        }

    /**************************************EMMCodecInitError*******************************************************/
    break;

    case EMMCodecInternalError:
        /* Cheking for MMU_fault */
        if((args[4] == (void*)USN_ERR_UNKNOWN_MSG) && (args[5] == (void*)NULL)) {
            pComponentPrivate->tCurState = OMX_StateInvalid;
            pHandle = pComponentPrivate->pHandle;
            AUDIODEC_DPRINT("\neventnotify to the base :EMMCodecInternalError at LCML call back\n");
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                   OMX_EventError,
                                                   OMX_ErrorInvalidState,
                                                   0x2,
                                                   NULL);
        }

    /**************************************EMMCodecInternalError*******************************************************/
    break;
}/*switch case for the event ends*/

 EXIT:
    return tError;
}
/* ================================================================================= * */
/*
*@fn OMX_AUDIO_DEC_ResourceManagerCallback():   function is callback which is called by RM whenever
*                                               there is any data in response to the component's request.
* @param cbData  data in response to the component's request/query.
* @see         None
*/
/* ================================================================================ * */

void OMX_AUDIO_DEC_ResourceManagerCallback(RMPROXY_COMMANDDATATYPE cbData)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_COMMANDTYPE Cmd = OMX_CommandStateSet;
    OMX_STATETYPE state = OMX_StateIdle;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)cbData.hComponent;
    AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)
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
        tError = pHandle->SendCommand(pHandle,Cmd,state,NULL);
        if(tError != OMX_ErrorNone)
        {
            pComponentPrivate->fpReturnEventNotify(pHandle,
                                                   OMX_EventError,
                                                   tError,0,NULL);
        }
    }
}
/* ========================================================================== */
/**
 * @fn OMX_JPEG_DEC_ComponentDeInit - Deinitialize Component. This method will clean all
 *  resources in the component
 * @param hComponent - handle for this instance of the component
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 */
/* ========================================================================== */

static OMX_ERRORTYPE OMX_AUDIO_DEC_ComponentDeInit(OMX_HANDLETYPE pHandle)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_U32 nIndex =0;
    AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    OMX_BASE_REQUIRE((pHandle != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE *)pHandle)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate=(AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE *)pHandle)->pComponentPrivate);
    AUDIODEC_DPRINT("\nentered in the component Deinit \n");
    #ifdef RESOURCE_MANAGER_ENABLED
    OMX_ERRORTYPE rm_error = OMX_ErrorNone;
    OMX_COMPONENTINDEXTYPE eComponentNameForRM;
    OMX_U32 CPULoadForRM;
    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_DECODE:
            eComponentNameForRM = OMX_AAC_Decoder_COMPONENT;
            CPULoadForRM = AACDEC_CPU_USAGE;
        break;
        case MP3_DECODE:
            eComponentNameForRM = OMX_MP3_Decoder_COMPONENT;
            CPULoadForRM = MP3DEC_CPU_USAGE;
        break;
        default:
        break;
    }
    AUDIODEC_DPRINT("\ninform to the RM\n");
    tError = RMProxy_NewSendCommand(pHandle, RMProxy_FreeResource, eComponentNameForRM, 0, 3456, NULL);
    if (tError != OMX_ErrorNone) {
        goto EXIT;
    }
    /*deinitialize the RM*/
    tError = RMProxy_Deinitalize();
    if (tError != OMX_ErrorNone) {
        goto EXIT;

    }
    #endif
    AUDIODEC_DPRINT("\ncall to the role specific component Deinit\n");
    switch(pComponentPrivate->eComponentRole)
    {
        case AAC_DECODE:
            tError=OMX_AACDEC_DeInit(pHandle);
            if (tError != OMX_ErrorNone) {
                goto EXIT;
            }
            break;
        case MP3_DECODE:
            tError=OMX_MP3DEC_DeInit(pHandle);
            if (tError != OMX_ErrorNone) {
                goto EXIT;
            }
            break;
        default:
            break;   //error
    }
    AUDIODEC_DPRINT("\nfree the common params\n");

    AUDIODEC_DPRINT("\ncall to LCML Deinit\n");
	if((pComponentPrivate->tCurState!=OMX_StateLoaded)&&(pComponentPrivate->tCurState!=OMX_StateInvalid))
    tError=OMX_LCML_DeInit(pHandle);/*LCML specific params are deleted*/
    if(tError!=OMX_ErrorNone){
        AUDIODEC_DPRINT("\nerror in LCMLDeinit\n");
        goto EXIT;
    }

    /*free the pcm params*/
    TIMM_OSAL_Free(pComponentPrivate->pcmParams);

    /*delete the pipes*/
    AUDIODEC_DPRINT("\ndeleting the pipes\n");
    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe(pComponentPrivate->pInDataPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);

    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe(pComponentPrivate->pOutDataPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);
    AUDIODEC_DPRINT("\nfree the sDevice string\n");

    /*free the sdevice string*/
    TIMM_OSAL_Free(pComponentPrivate->sDeviceString);
    AUDIODEC_DPRINT("\nfree the pPortFormat\n");

    /*free the pPortFormat*/
    TIMM_OSAL_Free(pComponentPrivate->pPortFormat);
    AUDIODEC_DPRINT("\ncall to BASEDeinit\n");
    /* Deinitalize  the base component  */
    tError = OMX_BASE_ComponentDeInit(pHandle);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, tError);
    AUDIODEC_DPRINT("\nfree the tderToBase params\n");
    for(nIndex=0; nIndex<pComponentPrivate->tDerToBase.tPortParams.nPorts; nIndex++) {
       TIMM_OSAL_Free(pComponentPrivate->tDerToBase.pTunPref[nIndex]);
    }
    TIMM_OSAL_Free(pComponentPrivate->tDerToBase.pTunPref);
    TIMM_OSAL_Free(pComponentPrivate->tDerToBase.cComponentName);
    TIMM_OSAL_Free(pComponentPrivate);
    pComponentPrivate = NULL;


 EXIT:
    AUDIODEC_DPRINT("\nerror=%x in component Deinit\n",tError);
    return tError;
}



/* ================================================================================= * */
/**
 * OMX_AUDIO_DEC__GetLCMLHandle() function gets the LCML handle and interacts with LCML
 * by using this LCML Handle.
 *
 * @param *pBufHeader This is the buffer header that needs to be processed.
 *
 * @param *pComponentPrivate  This is component's private date structure.
 *
 * @pre          None
 *
 * @post         None
 *
 * @return      OMX_HANDLETYPE = Successful loading of LCML library.
 *               OMX_ErrorHardware = Hardware error has occured.
 *
 * @see         None
 */
/* ================================================================================ * */


static OMX_HANDLETYPE OMX_AUDIO_DEC_GetLCMLHandle(AUDIODEC_COMPONENT_PRIVATE* pComponentPrivate)
{

    OMX_HANDLETYPE pHandle = NULL;
    OMX_ERRORTYPE eError;
    void *handle;
    OMX_ERRORTYPE (*fpGetHandle)(OMX_HANDLETYPE);
    char *error;
    AUDIODEC_DPRINT("\nbefore the dlopen at LCMLGethandle \n");
    handle = dlopen("libLCML.so", RTLD_LAZY);
    if (!handle) {
        fputs(dlerror(), stderr);
        goto EXIT;
    }
    AUDIODEC_DPRINT("\nbefore the dlsym at LCMLGethandle \n");
    fpGetHandle = dlsym (handle, "GetHandle");
    if ((error = dlerror()) != NULL) {
        fputs(error, stderr);
        goto EXIT;
    }
    AUDIODEC_DPRINT("\nbefore the getHandle call at LCMLGethandle \n");
    eError = (*fpGetHandle)(&pHandle);
    if(eError != OMX_ErrorNone) {
        AUDIODEC_DPRINT("\nin error condition after gethandle at LCMLGethandle \n");
        eError = (*fpGetHandle)(&pHandle);
        eError = OMX_ErrorUndefined;
        dlclose(handle);
        pHandle = NULL;
        goto EXIT;
    }
    AUDIODEC_DPRINT("\ninitialize the pHandle->pComponentPrivate(LCML) at LCMLGethandle \n");
    ((LCML_DSP_INTERFACE*)pHandle)->pComponentPrivate = pComponentPrivate;

 EXIT:

    return pHandle;
}
/* ================================================================================= * */
/**
 * OMX_LCML_DeInit() function frees the LCMLcomponent resources.
 *
 * @param pComponent This is the component handle.
 *
 * @pre          None
 *
 * @post         None
 *
 *  @return      OMX_ErrorNone = Successful Inirialization of the component\n
 *               OMX_ErrorHardware = Hardware error has occured.
 *
 *  @see         None
 */
/* ================================================================================ * */

OMX_ERRORTYPE OMX_LCML_DeInit(OMX_HANDLETYPE pComponent)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle =NULL;
    char *pTemp;
    AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    /*Check i/p Params */
    OMX_BASE_REQUIRE((pComponent != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)pComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pHandle = (OMX_COMPONENTTYPE *)pComponent;
    pComponentPrivate=(AUDIODEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;

    #ifdef DASF_Mode_enable
        TIMM_OSAL_Free(pComponentPrivate->LCMLParams->strmAttr);
    #endif

    /*free up the LCMLParams->pParams*/
    pTemp = (char*)pComponentPrivate->LCMLParams->pParams;
    if (pTemp != NULL) {
        pTemp -= EXTRA_BYTES;
    }
    pComponentPrivate->LCMLParams->pParams = (USN_AudioCodecParams*)pTemp;
    TIMM_OSAL_Free(pComponentPrivate->LCMLParams->pParams);
    AUDIODEC_DPRINT("\nfreeup LCMLParams->Pparams done\n");

    /*free the LCMLHandle*/
    /*TIMM_OSAL_Free(pComponentPrivate->LCMLParams->pLcmlHandle);*/

    /*delete the LCMLPending Pipes */
    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe
                            (pComponentPrivate->LCMLParams->pInBufPendingWithLcmlPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);
    tError = (OMX_ERRORTYPE)TIMM_OSAL_DeletePipe
                            (pComponentPrivate->LCMLParams->pOutBufPendingWithLcmlPipe);
    OMX_BASE_ASSERT(OMX_ErrorNone == tError, OMX_ErrorBadParameter);

    AUDIODEC_DPRINT("\npipes r deleted\n");
    TIMM_OSAL_Free(pComponentPrivate->LCMLParams);
EXIT:
    return tError;
}
/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_DEC_ComponentRoleEnum() - This function allows the IL core to
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
static OMX_ERRORTYPE OMX_AUDIO_DEC_ComponentRoleEnum(OMX_HANDLETYPE hComponent,
                                                     OMX_U8 *cRole,
                                                     OMX_U32 nIndex)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;

    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) && (cRole != NULL),
                     OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    switch(nIndex)
    {
        case 0:
            strcpy((char *)cRole, "undefined_role");
            break;
        case 1:
            strcpy((char *)cRole, "audio_decode.dsp.aac");
            break;
        case 2:
            strcpy((char *)cRole, "audio_decode.dsp.mp3");
            break;
        default:
            tError = OMX_ErrorNoMore;
            goto EXIT;
    }

EXIT:
    return tError;
};








