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

#include <omx_base.h>
#include <timm_osal_interfaces.h>
#include <omx_base_utils.h>

#ifdef RESOURCE_MANAGER_ENABLED
	#include <ResourceManagerProxyAPI.h>

#endif

#include "TIDspOmx.h"
#include "LCML_DspCodec.h"

#include "omx_audioenc_aacutils.h"
#include "omx_audioencoderutils.h"



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_COMMON_GetExtensionIndex() -
 *
 * @ see OMX_AudioEncoderUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                                     OMX_STRING cParameterName,
                                                     OMX_INDEXTYPE* pIndexType)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;

    /*****NOTE*****/
    /* This should ideally be only "OMX.TI.index.config.audioencstreamIDinfo"
    but checking for aac specific name as well to support legacy aac test cases*/
    if(!(strcmp(cParameterName,"OMX.TI.index.config.aacencstreamIDinfo"))
       || !(strcmp(cParameterName,"OMX.TI.index.config.audioencstreamIDinfo")))
    {
        *pIndexType = OMX_IndexCustomAudioEncStreamIDConfig;
    }
    else
    {
        tError = OMX_ErrorUnsupportedIndex;
        goto EXIT;
    }

    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_COMMON_GetParameter() -
 *
 * @ see OMX_AudioEncoderUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetParameter (OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nParamIndex,
                                           OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;

    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortFormatLocal = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefsLocal = NULL;

/*Check i/p params are !NULL*/
/*Not checking bec already done in calling function*/

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

/*Check prerequisites for the method*/
/*Not checking bec already done in calling function*/

    switch(nParamIndex)
    {
         case OMX_IndexParamAudioInit:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_PORT_PARAM_TYPE, tError);

            *((OMX_PORT_PARAM_TYPE*)pComponentParameterStructure) =
                                    pComponentPrivate->tDerToBase.tPortParams;
        break;
        case OMX_IndexParamAudioPcm:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_AUDIO_PARAM_PCMMODETYPE, tError);
            /*pcm only at i/p port for encoder*/
            OMX_BASE_REQUIRE(((OMX_AUDIO_PARAM_PCMMODETYPE*)
                              (pComponentParameterStructure))->nPortIndex ==
                              AUDIOENC_INPUT_PORT, OMX_ErrorBadPortIndex);

            TIMM_OSAL_Memcpy(pComponentParameterStructure,
                             pComponentPrivate->pcmParams,
                             sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
        break;
        case OMX_IndexParamStandardComponentRole:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_PARAM_COMPONENTROLETYPE, tError);

            *((OMX_PARAM_COMPONENTROLETYPE*)pComponentParameterStructure) =
                                            pComponentPrivate->componentRole;
        break;
        case OMX_IndexParamPortDefinition:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_PARAM_PORTDEFINITIONTYPE, tError);
            pPortDefsLocal = (OMX_PARAM_PORTDEFINITIONTYPE *)
                             (pComponentParameterStructure);

            /* Check for valid port index */
            OMX_BASE_REQUIRE((pPortDefsLocal->nPortIndex <
                   (pComponentPrivate->tDerToBase.tPortParams.nStartPortNumber +
                    pComponentPrivate->tDerToBase.tPortParams.nPorts)),
                    OMX_ErrorBadPortIndex);

            TIMM_OSAL_Memcpy(pPortDefsLocal, pComponentPrivate->
                             portdefs[pPortDefsLocal->nPortIndex],
                             sizeof(OMX_PARAM_PORTDEFINITIONTYPE));

        break;
        case OMX_IndexParamAudioPortFormat:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_AUDIO_PARAM_PORTFORMATTYPE, tError);
            pAudioPortFormatLocal = (OMX_AUDIO_PARAM_PORTFORMATTYPE *)
                                    (pComponentParameterStructure);
             /* Check for valid port index */
            OMX_BASE_REQUIRE(((pAudioPortFormatLocal->nPortIndex) <
                   (pComponentPrivate->tDerToBase.tPortParams.nStartPortNumber +
                    pComponentPrivate->tDerToBase.tPortParams.nPorts)),
                    OMX_ErrorBadPortIndex);
            OMX_BASE_REQUIRE((pAudioPortFormatLocal->nIndex <=
                            pComponentPrivate->
                            tAudioPortFormat[pAudioPortFormatLocal->nPortIndex].
                            nIndex), OMX_ErrorNoMore);

            TIMM_OSAL_Memcpy(pAudioPortFormatLocal, &(pComponentPrivate->
                        tAudioPortFormat[pAudioPortFormatLocal->nPortIndex]),
                        sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
        break;

         /*Add more audio encode specific indices here*/
        default:
            tError = OMX_BASE_GetParameter(hComponent, nParamIndex,
                                           pComponentParameterStructure);
        break;
    }
    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_COMMON_SetParameter() -
 *
 * @ see OMX_AudioEncoderUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_SetParameter (OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nParamIndex,
                                           OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;

    OMX_AUDIO_PARAM_PORTFORMATTYPE *pAudioPortFormatLocal = NULL;
    OMX_PARAM_PORTDEFINITIONTYPE *pPortDefsLocal = NULL;

/*Check i/p params are !NULL*/
/*Not checking bec already done in calling function*/

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

/*Check prerequisites for the method*/
/*Not checking bec already done in calling function*/

    switch(nParamIndex)
    {
         case OMX_IndexParamAudioInit:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_PORT_PARAM_TYPE, tError);
            pComponentPrivate->tDerToBase.tPortParams =
                          *((OMX_PORT_PARAM_TYPE*)pComponentParameterStructure);
        break;
        case OMX_IndexParamAudioPcm:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_AUDIO_PARAM_PCMMODETYPE, tError);
            /* pcm only at i/p port for encoder */
            OMX_BASE_REQUIRE(((OMX_AUDIO_PARAM_PCMMODETYPE*)
                              (pComponentParameterStructure))->nPortIndex ==
                              AUDIOENC_INPUT_PORT, OMX_ErrorBadPortIndex);
            OMX_BASE_ASSERT((pComponentPrivate->tCurState == OMX_StateLoaded) ||
                            (pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->
                             bEnabled == OMX_FALSE),
                             OMX_ErrorIncorrectStateOperation);

            TIMM_OSAL_Memcpy(pComponentPrivate->pcmParams,
                             pComponentParameterStructure,
                             sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
            pComponentPrivate->bpcmParamsSet = OMX_TRUE;
        break;
        case OMX_IndexParamStandardComponentRole:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_PARAM_COMPONENTROLETYPE, tError);
            if(strcmp((char*)(pComponentPrivate->componentRole.cRole),
                      (char*)(((OMX_PARAM_COMPONENTROLETYPE*)
                                pComponentParameterStructure)->cRole)) == 0)
            {
                /*Current and new role are the same - no need to do anything*/
            }
            else
            {
                /*Free up the structures allocated for the current role*/
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_ENCODE:
                        TIMM_OSAL_Free(pComponentPrivate->pAacPrivateParams->aacParams);
                        TIMM_OSAL_Free(pComponentPrivate->pAacPrivateParams);
                    break;
                    case UNDEFINED_ROLE:
                    break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                    break;
                    /*Add other roles here*/
                }
                pComponentPrivate->componentRole =
                  *((OMX_PARAM_COMPONENTROLETYPE*)pComponentParameterStructure);
            /*Initialize Role specific params inside the if-else ladder*/
                if(strcmp((char *)(pComponentPrivate->componentRole.cRole),
                      "audio_encode.dsp.aac") == 0)
                {
                    tError = OMX_AUDIO_ENC_AAC_InitParams(hComponent);
                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                }
                /*Add other roles here*/
                else
                {
                    tError = OMX_ErrorUnsupportedIndex;
                    goto EXIT;
                }
            }
        break;
        case OMX_IndexParamPortDefinition:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_PARAM_PORTDEFINITIONTYPE, tError);
            pPortDefsLocal = (OMX_PARAM_PORTDEFINITIONTYPE *)
                             (pComponentParameterStructure);

            /* Check for valid port index */
            OMX_BASE_REQUIRE((pPortDefsLocal->nPortIndex <
                   (pComponentPrivate->tDerToBase.tPortParams.nStartPortNumber +
                    pComponentPrivate->tDerToBase.tPortParams.nPorts)),
                    OMX_ErrorBadPortIndex);
            OMX_BASE_ASSERT((pComponentPrivate->tCurState == OMX_StateLoaded) ||
                            (pComponentPrivate->
                             portdefs[pPortDefsLocal->nPortIndex]->bEnabled ==
                             OMX_FALSE), OMX_ErrorIncorrectStateOperation);

            TIMM_OSAL_Memcpy(pComponentPrivate->
                                           portdefs[pPortDefsLocal->nPortIndex],
                             pPortDefsLocal,
                             sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            pComponentPrivate->bportdefsSet[pPortDefsLocal->nPortIndex] =
                                                                       OMX_TRUE;
        break;
        case OMX_IndexParamAudioPortFormat:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_AUDIO_PARAM_PORTFORMATTYPE, tError);
            pAudioPortFormatLocal = (OMX_AUDIO_PARAM_PORTFORMATTYPE *)
                                    (pComponentParameterStructure);

             /* Check for valid port index */
            OMX_BASE_REQUIRE(((pAudioPortFormatLocal->nPortIndex) <
                            (pComponentPrivate->
                             tDerToBase.tPortParams.nStartPortNumber +
                             pComponentPrivate->tDerToBase.tPortParams.nPorts)),
                             OMX_ErrorBadPortIndex);
            OMX_BASE_ASSERT((pComponentPrivate->tCurState == OMX_StateLoaded) ||
                            (pComponentPrivate->
                             portdefs[pAudioPortFormatLocal->nPortIndex]->
                             bEnabled == OMX_FALSE),
                             OMX_ErrorIncorrectStateOperation);
            OMX_BASE_REQUIRE((pAudioPortFormatLocal->nIndex <=
                              pComponentPrivate->tAudioPortFormat
                                   [pAudioPortFormatLocal->nPortIndex].nIndex),
                              OMX_ErrorNoMore);


            TIMM_OSAL_Memcpy(&(pComponentPrivate->
                           tAudioPortFormat[pAudioPortFormatLocal->nPortIndex]),
                           pAudioPortFormatLocal,
                           sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
            pComponentPrivate->bAudioPortFormatSet
                               [pAudioPortFormatLocal->nPortIndex] = OMX_TRUE;
        break;

        /*Add more audio enodcode specific indices here*/
        default:
            tError = OMX_BASE_SetParameter(hComponent, nParamIndex,
                                           pComponentParameterStructure);
    }
    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_COMMON_GetConfig() -
 *
 * @ see OMX_AudioEncoderUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetConfig (OMX_HANDLETYPE hComponent,
                                              OMX_INDEXTYPE nConfigIndex,
                                              OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;

    TI_OMX_STREAM_INFO *streamInfo = NULL;

/* Check i/p params are !NULL */
/* Not checking bec already done in calling function */

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

/* Check prerequisites for the method */
/* Not checking bec already done in calling function */

    streamInfo = TIMM_OSAL_Malloc(sizeof(TI_OMX_STREAM_INFO));
    if(streamInfo == NULL)
    {
        tError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }

    switch(nConfigIndex)
    {
        case OMX_IndexCustomAudioEncStreamIDConfig:
            /*Checking i/p structure is correct*/
            if(sizeof(pComponentConfigStructure) != sizeof(TI_OMX_STREAM_INFO))
            {
                tError = OMX_ErrorBadParameter;
                goto EXIT;
            }
            streamInfo->streamId = pComponentPrivate->streamID;
            TIMM_OSAL_Memcpy(pComponentConfigStructure, streamInfo,
                             sizeof(TI_OMX_STREAM_INFO));

        break;
        default:
            /*unsupported config*/
            tError = OMX_ErrorUnsupportedIndex;
    }


EXIT:
    if(streamInfo)
    {
        TIMM_OSAL_Free(streamInfo);
        streamInfo = NULL;
    }
    return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_COMMON_SetConfig() -
 *
 * @ see OMX_AudioEncoderUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_SetConfig (OMX_HANDLETYPE hComponent,
                                              OMX_INDEXTYPE nConfigIndex,
                                              OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;

    tError = OMX_ErrorUnsupportedIndex;
    goto EXIT;

    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_GetLCMLHandle() -
 *
 * @ see OMX_AudioEncoderUtils.h
 */
/* ========================================================================= */
OMX_HANDLETYPE OMX_AUDIO_ENC_GetLCMLHandle
               (OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_HANDLETYPE pHandle = NULL;
    /* Using Linux kernel calls like dlopen, dlclose etc. here as
          they are not supported by OSAL */
    void *handle;
    char *error;
    OMX_ERRORTYPE (*fpGetHandle)(OMX_HANDLETYPE);
    handle = dlopen("libLCML.so", RTLD_LAZY);
    if (!handle)
    {
        fputs(dlerror(), stderr);
        goto EXIT;
    }

    fpGetHandle = dlsym (handle, "GetHandle");
    if ((error = dlerror()) != NULL)
    {
        fputs(error, stderr);
        goto EXIT;
    }

    tError = (*fpGetHandle)(&pHandle);
    if(tError != OMX_ErrorNone)
    {
        tError = OMX_ErrorUndefined;
        pHandle = NULL;
        dlclose(handle);        /* got error - Close LCML lib  */
        handle = NULL;
        goto EXIT;
    }

    ((LCML_DSP_INTERFACE*)pHandle)->pComponentPrivate= pComponentPrivate;
    pComponentPrivate->tLcmlParams.ptrLibLCML=handle;/*saving LCML lib pointer*/

EXIT:
    return pHandle;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_ReturnPendingBuffers() -
 *
 * @ see OMX_AudioEncoderUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_ReturnPendingBuffers
                             (OMX_AUDIOENC_COMPONENT_PRIVATE* pComponentPrivate,
                              OMX_AUDIOENC_PIPE_TYPE pipe, OMX_U32 dir)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_HANDLETYPE hComponent = NULL;
    TIMM_OSAL_U32 elementsInPipe = 0;
    TIMM_OSAL_U32 elementsOutPipe = 0;
    TIMM_OSAL_U32 actualSize = 0;
    OMX_BUFFERHEADERTYPE* pInBufHeader = NULL;
    OMX_BUFFERHEADERTYPE* pOutBufHeader = NULL;

    OMX_BASE_REQUIRE((pComponentPrivate != NULL),
                      OMX_ErrorBadParameter);
    hComponent = (void *)pComponentPrivate->pHandle;

    switch(pipe)
    {
        case DATA_PIPE:
            if(dir == OMX_DirInput || OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount(pComponentPrivate->tPipePtrs.
                                                  pInDataPipe, &elementsInPipe);
                while(elementsInPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->tPipePtrs.pInDataPipe),
                              &pInBufHeader, sizeof(pInBufHeader), &actualSize,
                              TIMM_OSAL_SUSPEND);

                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                    OMX_BASE_ASSERT(pInBufHeader != NULL,
                                    OMX_ErrorInsufficientResources);
                    elementsInPipe--;
                    pComponentPrivate->fpReturnDataNotify(hComponent,
                                       AUDIOENC_INPUT_PORT, pInBufHeader);
                }
            }
            if(dir == OMX_DirOutput || OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount(pComponentPrivate->tPipePtrs.
                                                pOutDataPipe, &elementsOutPipe);
                while(elementsOutPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->tPipePtrs.pOutDataPipe),
                              &pOutBufHeader, sizeof(pOutBufHeader), &actualSize,
                              TIMM_OSAL_SUSPEND);

                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                    OMX_BASE_ASSERT(pOutBufHeader != NULL,
                                    OMX_ErrorInsufficientResources);
                    elementsOutPipe--;
                    pComponentPrivate->fpReturnDataNotify(hComponent,
                                       AUDIOENC_OUTPUT_PORT, pOutBufHeader);
                }
            }
        break;
        case LCML_DATA_PIPE:
            if(dir == OMX_DirInput || OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount(pComponentPrivate->tPipePtrs.
                                                   pInBufPendingWithLcmlPipe,
                                                   &elementsInPipe);
                while(elementsInPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->
                               tPipePtrs.pInBufPendingWithLcmlPipe),
                               &pInBufHeader, sizeof(pInBufHeader),
                               &actualSize, TIMM_OSAL_SUSPEND);
                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                    OMX_BASE_ASSERT(pInBufHeader != NULL,
                                    OMX_ErrorInsufficientResources);
                    elementsInPipe--;
                    pComponentPrivate->fpReturnDataNotify(hComponent,
                                                          AUDIOENC_INPUT_PORT,
                                                          pInBufHeader);
                }
            }
            if(dir == OMX_DirOutput || OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount(pComponentPrivate->tPipePtrs.
                                                   pOutBufPendingWithLcmlPipe,
                                                   &elementsOutPipe);
                while(elementsOutPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->
                               tPipePtrs.pOutBufPendingWithLcmlPipe),
                               &pOutBufHeader, sizeof(pOutBufHeader),
                               &actualSize, TIMM_OSAL_SUSPEND);
                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                    OMX_BASE_ASSERT(pOutBufHeader != NULL,
                                    OMX_ErrorInsufficientResources);
                    elementsInPipe--;
                    pComponentPrivate->fpReturnDataNotify(hComponent,
                                                          AUDIOENC_OUTPUT_PORT,
                                                          pOutBufHeader);
                }
            }
        break;
    }
EXIT:
    return tError;
}

