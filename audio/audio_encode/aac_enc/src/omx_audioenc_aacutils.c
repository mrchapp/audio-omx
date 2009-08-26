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
#include "usn.h"
#include "aacencsocket_ti.h"
#include "encode_common_ti.h"


#include "omx_audioenc_aacutils.h"
#include "omx_audioencoderutils.h"





/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_InitParams() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_InitParams(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;
    OMX_AUDIO_PARAM_AACPROFILETYPE *aac_op = NULL;

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*Initializing params for Aac encode*/
    pComponentPrivate->eComponentRole = AAC_ENCODE;
    aac_op = (OMX_AUDIO_PARAM_AACPROFILETYPE *)
              TIMM_OSAL_Malloc(sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
    if(NULL == aac_op)
    {
        tError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    OMX_BASE_INIT_STRUCT_PTR(aac_op, OMX_AUDIO_PARAM_AACPROFILETYPE);

    pComponentPrivate->pAacPrivateParams = (OMX_AUDIOENC_AAC_PRIVATE_PARAMS*)
                      TIMM_OSAL_Malloc(sizeof(OMX_AUDIOENC_AAC_PRIVATE_PARAMS));
    pComponentPrivate->pAacPrivateParams->aacParams = NULL;

    /*Default values for initialization mostly taken as it is from omap3 code*/

    aac_op->nChannels = 2;
    aac_op->nSampleRate = 44100;
    aac_op->eAACProfile = OMX_AUDIO_AACObjectLC;
    aac_op->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP2ADTS;     /* For khronos only : should  be MP4ADTS*/
    aac_op->nBitRate = 128000;
    aac_op->eChannelMode = OMX_AUDIO_ChannelModeStereo;
    aac_op->nPortIndex = 1;
    aac_op->nFrameLength = 0;
    aac_op->nAudioBandWidth = 0;
    pComponentPrivate->pAacPrivateParams->aacParams = aac_op;
    pComponentPrivate->pAacPrivateParams->File_Format = 2;
    pComponentPrivate->pAacPrivateParams->bitRateMode = 0;
    pComponentPrivate->pAacPrivateParams->framesPerOutBuf = 3;
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams = NULL;
    pComponentPrivate->pAacPrivateParams->pParams = NULL;
    TIMM_OSAL_Memset(pComponentPrivate->pAacPrivateParams->outBufParam, 0,
                    (MAX_NUM_OF_BUFS * sizeof(AACENC_UAlgOutBufParamStruct)));
    pComponentPrivate->pAacPrivateParams->posOutBufParam = 0;
    TIMM_OSAL_Memset(pComponentPrivate->pAacPrivateParams->inBufParam, 0,
                    (MAX_NUM_OF_BUFS * sizeof(AACENC_UAlgInBufParamStruct)));
    pComponentPrivate->pAacPrivateParams->posInBufParam = 0;

    strcpy((char*)pComponentPrivate->sDeviceString,"/rtmdn:i2:o1/codec\0");

    /*These values can be set by client before setting the role so checking
      If not already set only then setting to default values*/

    if(pComponentPrivate->bpcmParamsSet == OMX_FALSE)
    {
        pComponentPrivate->pcmParams->nBitPerSample = 16; /*Will be remapped for SN. 16:2,  24:3*/
        pComponentPrivate->pcmParams->nPortIndex = 0;
        pComponentPrivate->pcmParams->nChannels    = 2;   /*Will be remapped for SN. 0:mono, 1:stereo*/
        pComponentPrivate->pcmParams->eNumData = OMX_NumericalDataSigned;
        pComponentPrivate->pcmParams->nSamplingRate = 44100;
        pComponentPrivate->pcmParams->ePCMMode = OMX_AUDIO_PCMModeLinear;
        pComponentPrivate->pcmParams->bInterleaved = OMX_TRUE;

        pComponentPrivate->bpcmParamsSet = OMX_TRUE;
    }
    if(pComponentPrivate->bAudioPortFormatSet[AUDIOENC_INPUT_PORT] == OMX_FALSE)
    {
        pComponentPrivate->tAudioPortFormat[AUDIOENC_INPUT_PORT].eEncoding =
                                                            OMX_AUDIO_CodingPCM;

        pComponentPrivate->bAudioPortFormatSet[AUDIOENC_INPUT_PORT] = OMX_TRUE;
    }
    if(pComponentPrivate->bAudioPortFormatSet[AUDIOENC_OUTPUT_PORT] == OMX_FALSE)
    {
        pComponentPrivate->tAudioPortFormat[AUDIOENC_OUTPUT_PORT].eEncoding
                                                          = OMX_AUDIO_CodingAAC;

        pComponentPrivate->bAudioPortFormatSet[AUDIOENC_OUTPUT_PORT] = OMX_TRUE;
    }
    if(pComponentPrivate->bportdefsSet[AUDIOENC_INPUT_PORT] == OMX_FALSE)
    {
        pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferCountActual =
                                                       NUM_AACENC_INPUT_BUFFERS;
        pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferCountMin =
                                                       NUM_AACENC_INPUT_BUFFERS;
        pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferSize =
                                                       INPUT_AACENC_BUFFER_SIZE;

        pComponentPrivate->bportdefsSet[AUDIOENC_INPUT_PORT] = OMX_TRUE;
    }
    if(pComponentPrivate->bportdefsSet[AUDIOENC_OUTPUT_PORT] == OMX_FALSE)
    {
        pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferCountActual =
                                                      NUM_AACENC_OUTPUT_BUFFERS;
        pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferCountMin =
                                                      NUM_AACENC_OUTPUT_BUFFERS;
        pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferSize =
                                                      OUTPUT_AACENC_BUFFER_SIZE;
        pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->
                                   format.audio.eEncoding = OMX_AUDIO_CodingAAC;

        pComponentPrivate->bportdefsSet[AUDIOENC_OUTPUT_PORT] = OMX_TRUE;
    }
    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_GetExtensionIndex() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                                  OMX_STRING cParameterName,
                                                  OMX_INDEXTYPE* pIndexType)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;

    if (!(strcmp(cParameterName,"OMX.TI.index.config.aacencHeaderInfo")))
    {
        *pIndexType = OMX_IndexCustomAacEncHeaderInfoConfig;
    }
    else if(!(strcmp(cParameterName,"OMX.TI.index.config.aac.datapath")))
    {
        *pIndexType = OMX_IndexCustomAacEncDataPath;
    }
    else if (!(strcmp(cParameterName,"OMX.TI.index.config.aacencframesPerOutBuf")))
    {
        *pIndexType = OMX_IndexCustomAacEncFramesPerOutBuf;
    }
    else
    {
        tError = OMX_AUDIO_ENC_COMMON_GetExtensionIndex(hComponent,
                                                        cParameterName,
                                                        pIndexType);
    }
    return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_GetParameter() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_GetParameter (OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nParamIndex,
                                           OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;

/* Check i/p params are !NULL */
/* Not checking bec already done in prev. function */

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

/* Check prerequisites for the method */
/* Not checking bec already done in prev. function */

    switch(nParamIndex)
    {
        case OMX_IndexParamAudioAac:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_AUDIO_PARAM_AACPROFILETYPE, tError);
            /*aac only on o/p port as it is encoder*/
            OMX_BASE_REQUIRE(((OMX_AUDIO_PARAM_AACPROFILETYPE*)
                              (pComponentParameterStructure))->nPortIndex ==
                              AUDIOENC_OUTPUT_PORT, OMX_ErrorBadPortIndex);

            TIMM_OSAL_Memcpy(pComponentParameterStructure,
                             pComponentPrivate->pAacPrivateParams->aacParams,
                             sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
         break;
         /*Add more role specific indices here*/
        default:
            tError = OMX_AUDIO_ENC_COMMON_GetParameter(hComponent, nParamIndex,
                                                  pComponentParameterStructure);
    }
    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_SetParameter() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_SetParameter (OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nParamIndex,
                                           OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE  *pComponentPrivate;

/* Check i/p params are !NULL */
/* Not checking bec already done in prev. function */

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

/* Check prerequisites for the method */
/* Not checking bec already done in prev. function */

    switch(nParamIndex)
    {
        case OMX_IndexParamAudioAac:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure,
                                 OMX_AUDIO_PARAM_AACPROFILETYPE, tError);
            /*aac only on o/p port as it is encoder*/
            OMX_BASE_REQUIRE(((OMX_AUDIO_PARAM_AACPROFILETYPE*)
                              (pComponentParameterStructure))->nPortIndex ==
                              AUDIOENC_OUTPUT_PORT, OMX_ErrorBadPortIndex);
            OMX_BASE_ASSERT((pComponentPrivate->tCurState == OMX_StateLoaded) ||
                            (pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->
                             bEnabled == OMX_FALSE),
                             OMX_ErrorIncorrectStateOperation);

            TIMM_OSAL_Memcpy(pComponentPrivate->pAacPrivateParams->aacParams,
                             pComponentParameterStructure,
                             sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
        break;
        /*Add more role specific indices here*/
        default:
            tError = OMX_AUDIO_ENC_COMMON_SetParameter(hComponent, nParamIndex,
                                                 pComponentParameterStructure);
    }
    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_SetConfig() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_SetConfig (OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nConfigIndex,
                                           OMX_PTR pComponentConfigStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate;

    TI_OMX_DATAPATH dataPath;
    TI_OMX_DSP_DEFINITION* pDspDefinition = NULL;
    OMX_U16* ptrFramesPerOutBuf =NULL ;

    /* Check i/p params are !NULL */
    /* Not checking bec already done in prev. function */

    pComponentPrivate = (OMX_AUDIOENC_COMPONENT_PRIVATE *)
                        (((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /* Check state is not invalid */
    /* Not checking bec already done in prev. function */

    switch(nConfigIndex)
    {
        case OMX_IndexCustomAacEncHeaderInfoConfig:
            pDspDefinition = (TI_OMX_DSP_DEFINITION *)pComponentConfigStructure;
            if (pDspDefinition == NULL)
            {
                tError = OMX_ErrorBadParameter;
                goto EXIT;
            }

            /*dasf mode Not Supported
            pComponentPrivate->dasfmode = pDspDefinition->dasfMode;*/
            pComponentPrivate->pAacPrivateParams->bitRateMode =
                               pDspDefinition->aacencHeaderInfo->bitratemode;
            pComponentPrivate->streamID = pDspDefinition->streamId;
        break;

        case OMX_IndexCustomAacEncFramesPerOutBuf:
            ptrFramesPerOutBuf = (OMX_U16*)pComponentConfigStructure;
            if (ptrFramesPerOutBuf == NULL)
            {
                tError = OMX_ErrorBadParameter;
                goto EXIT;
            }
            pComponentPrivate->pAacPrivateParams->framesPerOutBuf =
                                                            *ptrFramesPerOutBuf;
        break;

        case OMX_IndexCustomAacEncDataPath:
            dataPath = *((OMX_S16 *)pComponentConfigStructure);
            switch(dataPath)
            {
                case DATAPATH_APPLICATION:
                    OMX_MMMIXER_DATAPATH(pComponentPrivate->sDeviceString,
                                         RENDERTYPE_ENCODER,
                                         pComponentPrivate->streamID);
                break;

                case DATAPATH_APPLICATION_RTMIXER:
                    strcpy((char*)pComponentPrivate->sDeviceString,
                           (char*)RTM_STRING_ENCODER);
                break;
                default:
                    /*Will have the default init value*/
                break;

            }
        break;
        default:
            tError = OMX_AUDIO_ENC_COMMON_SetConfig(hComponent, nConfigIndex,
                                                    pComponentConfigStructure);
    }
    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_FillLCMLInitParams() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */

OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_FillLCMLInitParams(OMX_HANDLETYPE hComponent,
                                         LCML_DSP *plcml_Init, OMX_U16 arr[])
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) hComponent;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate =
                 (OMX_AUDIOENC_COMPONENT_PRIVATE *)(pHandle->pComponentPrivate);
    char* pTemp_char1 = NULL;
    char* pTemp_char2 = NULL;

    /* Fill Input Buffers Info for LCML */
    plcml_Init->In_BufInfo.nBuffers =
           pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferCountActual;
    plcml_Init->In_BufInfo.nSize =
           pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferSize;
    plcml_Init->In_BufInfo.DataTrMethod = DMM_METHOD;

    /* Fill Output Buffers Info for LCML */
    plcml_Init->Out_BufInfo.nBuffers =
          pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferCountActual;
    plcml_Init->Out_BufInfo.nSize =
          pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferSize;
    plcml_Init->Out_BufInfo.DataTrMethod = DMM_METHOD;

    /*Copy the node information*/
    plcml_Init->NodeInfo.nNumOfDLLs = 3;

    plcml_Init->NodeInfo.AllUUIDs[0].uuid =
                                     (struct DSP_UUID*)&MPEG4AACENC_SN_UUID;
    strcpy((char*)plcml_Init->NodeInfo.AllUUIDs[0].DllName,
           "/lib/dsp/mpeg4aacenc_sn.dll64P");
    plcml_Init->NodeInfo.AllUUIDs[0].eDllType = DLL_NODEOBJECT;

    plcml_Init->NodeInfo.AllUUIDs[1].uuid =
                                     (struct DSP_UUID*)&MPEG4AACENC_SN_UUID;
    strcpy((char*)plcml_Init->NodeInfo.AllUUIDs[1].DllName,
           "/lib/dsp/mpeg4aacenc_sn.dll64P");
    plcml_Init->NodeInfo.AllUUIDs[1].eDllType = DLL_DEPENDENT;


    plcml_Init->NodeInfo.AllUUIDs[2].uuid = &ENCODE_COMMON_TI_UUID;
    strcpy((char*)plcml_Init->NodeInfo.AllUUIDs[2].DllName,
           "/lib/dsp/usn.dll64P");
     plcml_Init->NodeInfo.AllUUIDs[2].eDllType = DLL_DEPENDENT;

    plcml_Init->DeviceInfo.TypeofDevice = 0;

    plcml_Init->SegID = OMX_AACENC_DEFAULT_SEGMENT;
    plcml_Init->Timeout = -1; /*OMX_AACENC_SN_TIMEOUT;*/
    plcml_Init->Alignment = 0;
    plcml_Init->Priority = OMX_AACENC_SN_PRIORITY;
    plcml_Init->ProfileID = -1;


    if(pComponentPrivate->pAacPrivateParams->aacParams->eAACStreamFormat ==
                                                   OMX_AUDIO_AACStreamFormatRAW)
    {
        pComponentPrivate->pAacPrivateParams->File_Format = 0;
    }
    else if(pComponentPrivate->pAacPrivateParams->aacParams->eAACStreamFormat ==
                                                  OMX_AUDIO_AACStreamFormatADIF)
    {
        pComponentPrivate->pAacPrivateParams->File_Format = 1;
    }
    else if((pComponentPrivate->pAacPrivateParams->aacParams->eAACStreamFormat ==
             OMX_AUDIO_AACStreamFormatMP4ADTS) ||
            (pComponentPrivate->pAacPrivateParams->aacParams->eAACStreamFormat ==
             OMX_AUDIO_AACStreamFormatMP2ADTS))
    {
        pComponentPrivate->pAacPrivateParams->File_Format = 2;
    }


/* For non-dasf mode only. These values might be different for dasf mode */

    arr[0] = 2;                       /*Number of Streams*/
    arr[1] = 0;                       /*ID of the Input Stream*/
    arr[2] = 0;                       /*Type of Input Stream*/
    if(pComponentPrivate->portdefs[AUDIOENC_INPUT_PORT]->nBufferCountActual)
    {
            /*Number of buffers for Input Stream*/
        arr[3] = (OMX_U16)pComponentPrivate->
                              portdefs[AUDIOENC_INPUT_PORT]->nBufferCountActual;
    }
    else
    {
        arr[3] = 1;
    }
    arr[4] = 1;                       /*ID of the Output Stream*/
    arr[5] = 0;                       /*Type of Output Stream*/
    if(pComponentPrivate->portdefs[AUDIOENC_OUTPUT_PORT]->nBufferCountActual)
    {
        /*Number of buffers for Output Stream*/
        arr[6] = (OMX_U16)pComponentPrivate->
                             portdefs[AUDIOENC_OUTPUT_PORT]->nBufferCountActual;
    }
    else
    {
        arr[6] = 1;
    }

    arr[7] = 1;                       /*PNS Enable*/
    arr[8] = 1;                       /*TNS Enable*/
        /*Number of Channels*/
    arr[9] = (OMX_U16)((OMX_U16)
             (pComponentPrivate->pAacPrivateParams->aacParams->nChannels) - 1);
        /*Sampling rate - Lower bits*/
    arr[10] = (OMX_U16)
              (pComponentPrivate->pAacPrivateParams->aacParams->nSampleRate);
        /*Sampling rate - Higher bits */
    arr[11] = (OMX_U16)((pComponentPrivate->pAacPrivateParams->aacParams->
                             nSampleRate >>16) & 0xFFFF);
        /*Bit rate 2bytes*/
    arr[12] = (OMX_U16)(pComponentPrivate->pAacPrivateParams->aacParams->
                            nBitRate & 0xFFFF);
        /*Bit rate 2bytes*/
    arr[13] = (OMX_U16)(pComponentPrivate->pAacPrivateParams->aacParams->
                            nBitRate >> 16);
        /*Object Type */
    arr[14] = (OMX_U16)
              (pComponentPrivate->pAacPrivateParams->aacParams->eAACProfile);
        /*bitrateMode*/
    arr[15] = (OMX_U16)(pComponentPrivate->pAacPrivateParams->bitRateMode);
        /*FileFormat*/
    arr[16] = (OMX_U16)(pComponentPrivate->pAacPrivateParams->File_Format);
        /*FramesPerOutBuf*/
    arr[17] =  (OMX_U16)(pComponentPrivate->pAacPrivateParams->framesPerOutBuf);
    arr[18] = END_OF_CR_PHASE_ARGS;

    plcml_Init->pCrPhArgs = arr;

/*These 2 are allocated here but filled when transitioning to executing
  dont know much about these, taken as it is from legacy code*/
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams =
                       (MPEG4AACENC_UALGParams*)TIMM_OSAL_Malloc
                       (sizeof(MPEG4AACENC_UALGParams) + 256);
    if(NULL == pComponentPrivate->pAacPrivateParams->ptAlgDynParams)
    {
        tError =  OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    pTemp_char1 = (char*)pComponentPrivate->pAacPrivateParams->ptAlgDynParams;
    pTemp_char1 += 128;
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams =
                                     (MPEG4AACENC_UALGParams*)pTemp_char1;

    pComponentPrivate->pAacPrivateParams->pParams = (AACENC_AudioCodecParams*)
                       TIMM_OSAL_Malloc(sizeof(AACENC_AudioCodecParams) + 256);
    if(NULL == pComponentPrivate->pAacPrivateParams->pParams)
    {
        tError =  OMX_ErrorInsufficientResources;
        goto EXIT;
    }
    pTemp_char2 = (char*)pComponentPrivate->pAacPrivateParams->pParams;
    pTemp_char2 += 128;
    pComponentPrivate->pAacPrivateParams->pParams =
                                         (AACENC_AudioCodecParams*)pTemp_char2;

    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_SendParamsToCodecForTransitionToExecuting() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */

OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_SendParamsToCodecForTransitionToExecuting
                                                     (OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_U32 pValues1[4]={0};
    OMX_AUDIOENC_COMPONENT_PRIVATE* pComponentPrivate =
                                (OMX_AUDIOENC_COMPONENT_PRIVATE*)
                                (((OMX_COMPONENTTYPE*)hComponent)->
                                pComponentPrivate);
    LCML_DSP_INTERFACE* pLcmlHandle = (LCML_DSP_INTERFACE*)
                                   (pComponentPrivate->tLcmlParams.pLcmlHandle);

/* Will need to set some other values here if dasf mode is supported */

    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->
                       audenc_dynamicparams.size = sizeof(MPEG4AACENC_UALGParams);
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->
                       audenc_dynamicparams.bitRate = (OMX_U32)
                       (pComponentPrivate->pAacPrivateParams->aacParams->nBitRate);
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->
                       audenc_dynamicparams.sampleRate = (OMX_U16)
                       (pComponentPrivate->pAacPrivateParams->aacParams->nSampleRate);
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->
                       audenc_dynamicparams.numChannels =
                       (pComponentPrivate->pAacPrivateParams->aacParams->nChannels ==
                                                         2)?1:0;   /* Reduced */
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->useTns = 1;
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->usePns = 1;
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->outObjectType = (OMX_U16)
                       (pComponentPrivate->pAacPrivateParams->aacParams->eAACProfile);
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams->outFileFormat =
                       pComponentPrivate->pAacPrivateParams->File_Format;

    pValues1[0] = IUALG_CMD_SETSTATUS;
    pValues1[1] = (OMX_U32)pComponentPrivate->pAacPrivateParams->ptAlgDynParams;
    pValues1[2] = sizeof(MPEG4AACENC_UALGParams);

    tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->
                                 pCodecinterfacehandle,
                                 EMMCodecControlAlgCtrl,(void *)pValues1);
    if(tError != OMX_ErrorNone)
    {
        goto EXIT;
    }
    EXIT:
        return tError;
}



/* ========================================================================= */
/*
 * @fn OMX_AUDIO_ENC_AAC_FreeLCMLInitParams() -
 *
 * @ see OMX_AudioEnc_AacUtils.h
 */
/* ========================================================================= */
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_FreeLCMLInitParams(OMX_HANDLETYPE pComponent)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *) pComponent;
    OMX_AUDIOENC_COMPONENT_PRIVATE *pComponentPrivate =
                    (OMX_AUDIOENC_COMPONENT_PRIVATE *)(pHandle->pComponentPrivate);
    char* pTemp = NULL;

    pTemp = (char*)pComponentPrivate->pAacPrivateParams->ptAlgDynParams;
    if (pTemp != NULL)
    {
        pTemp -= 128;
    }
    pComponentPrivate->pAacPrivateParams->ptAlgDynParams =
                                     (MPEG4AACENC_UALGParams*)pTemp;
    TIMM_OSAL_Free(pComponentPrivate->pAacPrivateParams->ptAlgDynParams);

    pTemp = (char*)pComponentPrivate->pAacPrivateParams->pParams;
    if (pTemp != NULL)
    {
        pTemp -= 128;
    }
    pComponentPrivate->pAacPrivateParams->pParams = (AACENC_AudioCodecParams*)pTemp;
    TIMM_OSAL_Free(pComponentPrivate->pAacPrivateParams->pParams);

    /*EXIT:*/
    return tError;
}



