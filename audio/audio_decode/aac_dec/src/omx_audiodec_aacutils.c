
/* =============================================================================
*             Texas Instruments OMAP (TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file AACDecUtils.c
* This file contains methods that provides the functionality for the AAC Decoder component.
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

/*-------program files ----------------------------------------*/
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <OMX_Audio.h>
#include <OMX_Types.h>
#include <OMX_Index.h>

#include <omx_base.h>
#include <omx_base_internal.h> 
#include <timm_osal_interfaces.h>
#include <omx_base_utils.h>
#include <ResourceManagerProxyAPI.h>
#include "TIDspOmx.h"
#include "LCML_DspCodec.h"
#include <usn.h>
#include "aacdecsocket_ti.h"
#include <decode_common_ti.h>

//#include "omx_audiodec_aacutils.h"
#include "omx_audiodecutils.h"
/* ================================================================================= */
/**
* @fn OMX_AAC_DEC_GetExtensionIndex() description for GetExtensionIndex  
GetExtensionIndex().  
Returns index for vendor specific settings.
* @see         OMX_Core.h
* @param hComponent - handle for this instance of the component
* @param cParameterName - the string that will get 
                            translated by the component into a configuration index.
* @param pIndexType - a pointer to a OMX_INDEXTYPE to receive the index value.
* @return:  OMX_ERRORTYPE
*           OMX_ErrorNone on success
*           !OMX_ErrorNone on any failure
* @see         None
*/
/* ================================================================================ */
OMX_ERRORTYPE OMX_AAC_DEC_GetExtensionIndex(OMX_IN  OMX_HANDLETYPE hComponent,
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
    
    if (!(strcmp(cParameterName,"OMX.TI.index.config.aacdecHeaderInfo"))) {
        
        *pIndexType = OMX_IndexCustomAacDecHeaderInfoConfig;        
    }
    else if(!(strcmp(cParameterName,"OMX.TI.index.config.aacdecstreamIDinfo"))){
        
        *pIndexType = OMX_IndexCustomAacDecStreamIDConfig;
    }
    else if(!(strcmp(cParameterName,"OMX.TI.index.config.aacdec.datapath"))) {
        
        *pIndexType = OMX_IndexCustomAacDecDataPath;
    }
    else {
        tError = OMX_ErrorBadParameter;
    }
    EXIT:
        
        return tError;
}
/* ========================================================================== */
/**
* @fn OMX_AAC_DEC_GetParameter - Gets Parameters Specific to AAC from the Component.
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


OMX_ERRORTYPE OMX_AAC_DEC_GetParameter (OMX_IN OMX_HANDLETYPE hComponent,
                                                OMX_IN OMX_INDEXTYPE nParamIndex,
                                                OMX_OUT OMX_PTR ComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;

    /*to access the private params we need to declare this.*/
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    /*switch case starts!!!*/

    switch(nParamIndex){
        
        case OMX_IndexParamAudioAac:

            if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(ComponentParameterStructure))->nPortIndex ==
               pComponentPrivate->codec_params.aacdecParams->aacParams->nPortIndex) {
                TIMM_OSAL_Memcpy(ComponentParameterStructure, pComponentPrivate->codec_params.aacdecParams->aacParams, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
            }else {
                tError = OMX_ErrorBadPortIndex;
            }
            break;
        default:
            tError = OMX_ErrorUnsupportedIndex;

    }/*end of swith case*/
    
    
 EXIT:
    return tError;


}

/* ========================================================================== */
/**
 * @fn OMX_AAC_DEC_SetParameter  - Sets the parameters Specific to AAC  sent by the Application and
 *  sets it to the component 
 * @param hComponent - handle for this instance of the component
 * @param nParamIndex - Component Index Port
 * @param pParamStruct - Component Parameter Structure to set
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */
OMX_ERRORTYPE OMX_AAC_DEC_SetParameter ( OMX_HANDLETYPE hComponent,
                                         OMX_INDEXTYPE nParamIndex,
                                         OMX_PTR ComponentParameterStructure)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    /*to access the private params we need to declare this.*/
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);


    /*switch case starts!!!*/

    switch(nParamIndex){
        case OMX_IndexParamAudioAac:

            if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(ComponentParameterStructure))->nPortIndex ==
               pComponentPrivate->codec_params.aacdecParams->aacParams->nPortIndex) {
                TIMM_OSAL_Memcpy(pComponentPrivate->codec_params.aacdecParams->aacParams, ComponentParameterStructure,sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
                /*need to update the aacdecParms*/
                if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectMain){
                    pComponentPrivate->codec_params.aacdecParams->nProfile = 0;
                } 
                else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectLC){
                    pComponentPrivate->codec_params.aacdecParams->nProfile = 1;
                } 
                else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectSSR){
                    pComponentPrivate->codec_params.aacdecParams->nProfile = 2;
                } 
                else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectLTP){
                    pComponentPrivate->codec_params.aacdecParams->nProfile = 3;
                } 
                else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectHE){
                    pComponentPrivate->codec_params.aacdecParams->nProfile = 1;
                    pComponentPrivate->codec_params.aacdecParams->SBR = 1;
                } 
                else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectHE_PS){
                    pComponentPrivate->codec_params.aacdecParams->nProfile = 1;
                    pComponentPrivate->codec_params.aacdecParams->parameteric_stereo = PARAMETRIC_STEREO_AACDEC;
                }
            }else {
                tError = OMX_ErrorBadPortIndex;
            }
            break;
        default:
            
            tError = OMX_ErrorUnsupportedIndex;
            break;

    }/*end of swith case*/


 EXIT:
    return tError;

}/*Setparamenter ends*/

/* ========================================================================== */
/**
 * @fn OMX_AAC_DEC_GetConfig - Gets AAC specific Configuration data from the Component
 * @param hComponent - handle for this instance of the component
 * @param nIndex - Component  Index Port
 * @param pConfigData - Component Config Structure
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */
OMX_ERRORTYPE OMX_AAC_DEC_GetConfig (OMX_HANDLETYPE hComponent,
                                     OMX_INDEXTYPE nConfigIndex,
                                     OMX_PTR pComponentConfigStructure)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
#ifdef DASF_Mode_enable
    TI_OMX_STREAM_INFO *streamInfo = NULL;
#endif
    /* Check the input parameters */
    OMX_BASE_REQUIRE((hComponent != NULL) &&
                     (pComponentConfigStructure !=NULL), OMX_ErrorBadParameter);
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);

    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
#ifdef DASF_Mode_enable
    streamInfo = TIMM_OSAL_Malloc(sizeof(TI_OMX_STREAM_INFO));
    if(streamInfo == NULL)
    {
        tError = OMX_ErrorInsufficientResources;
        goto EXIT;
    }   
    if(nConfigIndex == OMX_IndexCustomAacDecStreamIDConfig){
        /*component info check*/
        if(sizeof(pComponentConfigStructure) != sizeof(TI_OMX_STREAM_INFO))
        {
            tError = OMX_ErrorBadParameter;
            goto EXIT;
        }
        streamInfo->streamId = pComponentPrivate->streamID;
        TIMM_OSAL_Memcpy(pComponentConfigStructure, streamInfo,
                         sizeof(TI_OMX_STREAM_INFO));
    }
    if(streamInfo){
        TIMM_OSAL_Free(streamInfo);
        streamInfo = NULL;
        goto EXIT;
    }
#endif
    
    if (nConfigIndex ==  OMX_IndexParamAudioAac) {
            
        if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(pComponentConfigStructure))->nPortIndex ==
           pComponentPrivate->codec_params.aacdecParams->aacParams->nPortIndex) {
            TIMM_OSAL_Memcpy(pComponentConfigStructure, pComponentPrivate->codec_params.aacdecParams->aacParams, 
                             sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
        } 
        else if(((OMX_AUDIO_PARAM_AACPROFILETYPE *)(pComponentConfigStructure))->nPortIndex ==
                pComponentPrivate->pcmParams->nPortIndex) {
            TIMM_OSAL_Memcpy(pComponentConfigStructure, pComponentPrivate->pcmParams, 
                             sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
        }
        else {
                
            tError = OMX_ErrorBadPortIndex;
        }
    }
    else{
        tError = OMX_ErrorUnsupportedIndex;
    }

 EXIT:
    return tError;


}/*getconfig ends*/
/* ========================================================================== */
/**
 * @fn OMX_AUDIO_DEC_SetConfig - Sends command to set new configuration that is specific to AAC
 * @param hComponent - handle for this instance of the component
 * @param nIndex - Component Config Index Port
 * @param pConfigData - Component Config Structure
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */
OMX_ERRORTYPE OMX_AAC_DEC_SetConfig (OMX_HANDLETYPE hComponent,
                                     OMX_INDEXTYPE nConfigIndex,
                                     OMX_PTR pComponentConfigStructure)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    /*to access the private params we need to declare this.*/
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    
    TI_OMX_DSP_DEFINITION* pDspDefinition = NULL;
    OMX_S16* deviceString = NULL;
    TI_OMX_DATAPATH dataPath;
    OMX_AUDIO_PARAM_AACPROFILETYPE *aac_params = NULL;
    OMX_U32 pValues[4];

    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);


    /*switch case starts!!!*/

    switch(nConfigIndex){
        
        case OMX_IndexParamAudioAac:
            AUDIODEC_DPRINT("\n entered in OMX_IndexParamAudioAac case\n");    
            aac_params = (OMX_AUDIO_PARAM_AACPROFILETYPE*)pComponentConfigStructure;
            if(aac_params->eAACProfile == OMX_AUDIO_AACObjectHE_PS){
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->nProfile = OMX_AUDIO_AACObjectHE_PS;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->iEnablePS =  1;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->DownSampleSbr = 1;

                pValues[0] = IUALG_CMD_SETSTATUS;
                pValues[1] = (OMX_U32)pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam;
                pValues[2] = sizeof(MPEG4AACDEC_UALGParams);

                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pComponentPrivate->LCMLParams->pLcmlHandle)->pCodecinterfacehandle,
                                           EMMCodecControlAlgCtrl,(void *)pValues);
                if(tError != OMX_ErrorNone) {
                    pComponentPrivate->tCurState = OMX_StateInvalid;
                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                    AUDIODEC_DPRINT("\n going to EXIT b'coz of error\n");
                    goto EXIT;
                }
            }
            if(aac_params->eAACProfile == OMX_AUDIO_AACObjectHE){
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->nProfile = OMX_AUDIO_AACObjectHE;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->iEnablePS =  0;
                pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->DownSampleSbr = 1;

                pValues[0] = IUALG_CMD_SETSTATUS;
                pValues[1] = (OMX_U32)pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam;
                pValues[2] = sizeof(MPEG4AACDEC_UALGParams);

                tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pComponentPrivate->LCMLParams->pLcmlHandle)->pCodecinterfacehandle,
                                           EMMCodecControlAlgCtrl,(void *)pValues);
                if(tError != OMX_ErrorNone) {
                    pComponentPrivate->tCurState = OMX_StateInvalid;
                    pComponentPrivate->fpReturnEventNotify(pComponentPrivate->pHandle, OMX_EventError,OMX_ErrorInvalidState,0, NULL);
                    AUDIODEC_DPRINT("\n going to EXIT b'coz of error\n");
                    goto EXIT;
                }
            }
            /*copy the info into aacdecParams->aacParams*/
            TIMM_OSAL_Memcpy(pComponentPrivate->codec_params.aacdecParams->aacParams,pComponentConfigStructure, sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));
            /*need to update the aacdecParams*/
            if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectMain){
                pComponentPrivate->codec_params.aacdecParams->nProfile = 0;
            } 
            else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectLC){
                pComponentPrivate->codec_params.aacdecParams->nProfile = 1;
            } 
            else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectSSR){
                pComponentPrivate->codec_params.aacdecParams->nProfile = 2;
            } 
            else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectLTP){
                pComponentPrivate->codec_params.aacdecParams->nProfile = 3;
            } 
            else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectHE){
                pComponentPrivate->codec_params.aacdecParams->nProfile = 1;
                pComponentPrivate->codec_params.aacdecParams->SBR = 1;
            } 
            else if (pComponentPrivate->codec_params.aacdecParams->aacParams->eAACProfile == OMX_AUDIO_AACObjectHE_PS){
                pComponentPrivate->codec_params.aacdecParams->nProfile = 1;
                pComponentPrivate->codec_params.aacdecParams->parameteric_stereo = PARAMETRIC_STEREO_AACDEC;
            }
            break;

        case OMX_IndexCustomAacDecHeaderInfoConfig:
            AUDIODEC_DPRINT("\nentered in OMX_IndexCustomAacDecHeaderInfoConfig case\n");    
            pDspDefinition = (TI_OMX_DSP_DEFINITION *)pComponentConfigStructure;
            if (pDspDefinition == NULL) {
                tError = OMX_ErrorBadParameter;
                AUDIODEC_DPRINT("\n going to EXIT b'coz of error\n");
                goto EXIT;
            }
            pComponentPrivate->codec_params.aacdecParams->Framemode = pDspDefinition->framemode;
            /*pComponentPrivate->dasfmode = pDspDefinition->dasfMode;                    
              pComponentPrivate->streamID = pDspDefinition->streamId;*/
            break;                
        case  OMX_IndexCustomAacDecDataPath:
            AUDIODEC_DPRINT("\nentered in OMX_IndexCustomAacDecDataPath case\n");
            deviceString = (OMX_S16*)pComponentConfigStructure;
            if (deviceString == NULL) {
                tError = OMX_ErrorBadParameter;
                AUDIODEC_DPRINT("\n going to EXIT b'coz of error\n");
                goto EXIT;
            }
				
            dataPath = (TI_OMX_DATAPATH)*deviceString;

            switch(dataPath) {
                case DATAPATH_APPLICATION:
                    /*OMX_MMMIXER_DATAPATH(pComponentPrivate->sDeviceString, RENDERTYPE_DECODER, pComponentPrivate->streamID);*/
                    break;
                case DATAPATH_APPLICATION_RTMIXER:
                    strcpy((char*)pComponentPrivate->sDeviceString,(char*)RTM_STRING);
                    break;
                case DATAPATH_ACDN:
                    strcpy((char*)pComponentPrivate->sDeviceString,(char*)ACDN_STRING);
                    break;
                default:
                    break;
            }
            break;

        default:
            tError = OMX_ErrorUnsupportedIndex;
            break;
    }/*switch case ends*/       
 EXIT:
    AUDIODEC_DPRINT("\nerror returned to application (in function:role Specific SetConfig)=%x\n",tError);
    return tError;




}/*Setconfig ends*/

/* ===========================================================  */
/**
 *  OMX_AAC_DEC_InitParams()  Fills the parameters needed
 * to initialize the AAC_DECODER 
 *
 *  @param pComponent            OMX Handle
 *
 *  @return None
 */

/*================================================================== */
OMX_ERRORTYPE OMX_AAC_DEC_InitParams(OMX_HANDLETYPE hComponent)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    char *pTemp_char = NULL;

    /*to access the private params */
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    OMX_AUDIO_PARAM_PORTFORMATTYPE *pPortFormat = NULL;
    OMX_AUDIO_PARAM_AACPROFILETYPE *aacdec_ip = NULL;
    
    AUDIODEC_DPRINT("\nInitialize AAC Params\n");
    
    /* Check for valid component private handle */
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
    
    /*allocate memory for the aacdecParams*/
    pComponentPrivate->codec_params.aacdecParams = NULL;
    pComponentPrivate->codec_params.aacdecParams=(AACDECParams*)TIMM_OSAL_Malloc(sizeof(AACDECParams));
    TIMM_OSAL_Memset (pComponentPrivate->codec_params.aacdecParams,0x0, sizeof (AACDECParams));
    OMX_BASE_ASSERT(pComponentPrivate->codec_params.aacdecParams != NULL,OMX_ErrorInsufficientResources);

    /*allocate memory for aacdecParams->AACDEC_UALGParam will be updated at SN_initparams fn*/
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam=(MPEG4AACDEC_UALGParams*)TIMM_OSAL_Malloc(sizeof (MPEG4AACDEC_UALGParams)+ DSP_CACHE_ALIGNMENT);
    TIMM_OSAL_Memset(pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam,0,(sizeof (MPEG4AACDEC_UALGParams) ));
    OMX_BASE_ASSERT(pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam != NULL,OMX_ErrorInsufficientResources);
    pTemp_char = (char*)pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam;
    pTemp_char += EXTRA_BYTES;
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam = (MPEG4AACDEC_UALGParams*)pTemp_char;
    

    /*allocate memory n initialize the parameters of aacdecParams->aacParams*/
    aacdec_ip=(OMX_AUDIO_PARAM_AACPROFILETYPE *)
        TIMM_OSAL_Malloc(sizeof(OMX_AUDIO_PARAM_AACPROFILETYPE));   
    TIMM_OSAL_Memset (aacdec_ip,0x0, sizeof (OMX_AUDIO_PARAM_AACPROFILETYPE));
    OMX_BASE_ASSERT(aacdec_ip != NULL,OMX_ErrorInsufficientResources);
    pComponentPrivate->codec_params.aacdecParams->aacParams = aacdec_ip;
    
    /* AAC format defaults */
    OMX_BASE_INIT_STRUCT_PTR(aacdec_ip, OMX_AUDIO_PARAM_AACPROFILETYPE);
    aacdec_ip->nPortIndex = INPUT_PORT;
    aacdec_ip->nSampleRate = 44100;
    aacdec_ip->nChannels = 2;
    aacdec_ip->eChannelMode = OMX_AUDIO_ChannelModeStereo;
    aacdec_ip->eAACProfile = OMX_AUDIO_AACObjectLC;
    aacdec_ip->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMP2ADTS;     
    
    

    pComponentPrivate->codec_params.aacdecParams->parameteric_stereo = 0;
    pComponentPrivate->codec_params.aacdecParams->SBR = 0;
    pComponentPrivate->codec_params.aacdecParams->nOpBit = 0;
    pComponentPrivate->codec_params.aacdecParams->nProfile = OMX_AUDIO_AACObjectLC;/*default profile can be changed using set paramet*/
    pComponentPrivate->codec_params.aacdecParams->Framemode=0;
    pComponentPrivate->codec_params.aacdecParams->SendAfterEOS = 0;
    

    /*pComponentPrivate->dasfmode = 0;*/    
    /*pComponentPrivate->streamID=0;*/    

    /*sDevicestring initialization*/
    strcpy((char*)pComponentPrivate->sDeviceString,"/eteedn:i0:o0/codec\0");
        
    /* initialize enum role name */
    pComponentPrivate->eComponentRole=AAC_DECODE;
    
    /* Set input port defaults */
    
    pComponentPrivate->portdefs[INPUT_PORT]->nPortIndex                         = INPUT_PORT;
    pComponentPrivate->portdefs[INPUT_PORT]->eDir                               = OMX_DirInput;
    pComponentPrivate->portdefs[INPUT_PORT]->nBufferCountActual                 = AACD_NUM_INPUT_BUFFERS;
    pComponentPrivate->portdefs[INPUT_PORT]->nBufferCountMin                    = AACD_NUM_INPUT_BUFFERS;
    pComponentPrivate->portdefs[INPUT_PORT]->nBufferSize                        = AACD_INPUT_BUFFER_SIZE;
    pComponentPrivate->portdefs[INPUT_PORT]->bEnabled                           = OMX_TRUE;
    pComponentPrivate->portdefs[INPUT_PORT]->bPopulated                         = OMX_FALSE;
    pComponentPrivate->portdefs[INPUT_PORT]->eDomain                            = OMX_PortDomainAudio;
    pComponentPrivate->portdefs[INPUT_PORT]->format.audio.eEncoding             = OMX_AUDIO_CodingAAC;
    pComponentPrivate->portdefs[INPUT_PORT]->format.audio.cMIMEType             = NULL;
    pComponentPrivate->portdefs[INPUT_PORT]->format.audio.pNativeRender         = NULL;
    pComponentPrivate->portdefs[INPUT_PORT]->format.audio.bFlagErrorConcealment = OMX_FALSE;

    /* Set output port defaults */
    /*done at the ComponentInit*/
        
    /* Set input port format defaults */
    pPortFormat = pComponentPrivate->pPortFormat[INPUT_PORT];
    pPortFormat->nPortIndex         = INPUT_PORT;
    pPortFormat->nIndex             = 0 ;//OMX_IndexParamAudioAac;
    pPortFormat->eEncoding          = OMX_AUDIO_CodingAAC;

    /* Set output port format defaults */
    pPortFormat = pComponentPrivate->pPortFormat[OUTPUT_PORT];
    pPortFormat->nPortIndex         = OUTPUT_PORT;
    pPortFormat->nIndex             = 0;//OMX_IndexParamAudioPcm;
    pPortFormat->eEncoding          = OMX_AUDIO_CodingPCM;

       
 EXIT:
        
    return tError;

}

/* ================================================================================= * */
/**
 * OMX_AAC_DEC_Fill_LCMLInitParams() fills the LCML initialization structure.
 *
 * @param pHandle This is component handle allocated by the OMX core.
 *
 * @param plcml_Init This structure is filled and sent to LCML.
 *
 * @pre          None
 *
 * @post         None
 *
 *  @return      OMX_ErrorNone = Successful Inirialization of the LCML struct.
 *               OMX_ErrorInsufficientResources = Not enough memory
 *
 *  @see         None
 */
/* ================================================================================ * */

OMX_ERRORTYPE OMX_AAC_DEC_Fill_LCMLInitParams(OMX_HANDLETYPE pComponent,
                                  LCML_DSP *plcml_Init, OMX_U16 arr[]){
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    OMX_U32 nIpBuf,nIpBufSize,nOpBuf,nOpBufSize;
    OMX_COMPONENTTYPE *pHandle = (OMX_COMPONENTTYPE *)pComponent;
    AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE*)pHandle->pComponentPrivate;

    
    nIpBuf = pComponentPrivate->portdefs[INPUT_PORT]->nBufferCountActual;
    nOpBuf = pComponentPrivate->portdefs[OUTPUT_PORT]->nBufferCountActual;
    nIpBufSize = pComponentPrivate->portdefs[INPUT_PORT]->nBufferSize;
    nOpBufSize = pComponentPrivate->portdefs[OUTPUT_PORT]->nBufferSize;
    
    
    plcml_Init->In_BufInfo.nBuffers = nIpBuf;
    plcml_Init->In_BufInfo.nSize = nIpBufSize;
    plcml_Init->In_BufInfo.DataTrMethod = DMM_METHOD;
    plcml_Init->Out_BufInfo.nBuffers = nOpBuf;
    plcml_Init->Out_BufInfo.nSize = nOpBufSize;
    plcml_Init->Out_BufInfo.DataTrMethod = DMM_METHOD;
    plcml_Init->NodeInfo.nNumOfDLLs = 3;

    
    TIMM_OSAL_Memset(plcml_Init->NodeInfo.AllUUIDs[0].DllName,0, sizeof(plcml_Init->NodeInfo.AllUUIDs[0].DllName));
    TIMM_OSAL_Memset(plcml_Init->NodeInfo.AllUUIDs[1].DllName,0, sizeof(plcml_Init->NodeInfo.AllUUIDs[1].DllName));
    TIMM_OSAL_Memset(plcml_Init->NodeInfo.AllUUIDs[2].DllName,0, sizeof(plcml_Init->NodeInfo.AllUUIDs[1].DllName));
    TIMM_OSAL_Memset(plcml_Init->NodeInfo.AllUUIDs[0].DllName,0, sizeof(plcml_Init->DeviceInfo.AllUUIDs[1].DllName));
    
    
    plcml_Init->NodeInfo.AllUUIDs[0].uuid = (struct DSP_UUID*)&MPEG4AACDEC_SN_UUID;
    strcpy ((char*)plcml_Init->NodeInfo.AllUUIDs[0].DllName, AACDEC_DLL_NAME);
    plcml_Init->NodeInfo.AllUUIDs[0].eDllType = DLL_NODEOBJECT;

    plcml_Init->NodeInfo.AllUUIDs[1].uuid = (struct DSP_UUID*)&MPEG4AACDEC_SN_UUID;
    strcpy ((char*)plcml_Init->NodeInfo.AllUUIDs[1].DllName, AACDEC_DLL_NAME);
    plcml_Init->NodeInfo.AllUUIDs[1].eDllType = DLL_DEPENDENT;

    plcml_Init->NodeInfo.AllUUIDs[2].uuid = &USN_TI_UUID;
    strcpy ((char*)plcml_Init->NodeInfo.AllUUIDs[2].DllName, AACDEC_USN_DLL_NAME);
    plcml_Init->NodeInfo.AllUUIDs[2].eDllType = DLL_DEPENDENT;

    plcml_Init->SegID = OMX_AACDEC_DEFAULT_SEGMENT;
    plcml_Init->Timeout = OMX_AACDEC_SN_TIMEOUT;
    plcml_Init->Alignment = 0;
    plcml_Init->Priority = OMX_AACDEC_SN_PRIORITY;
    plcml_Init->ProfileID = -1; /*Previously 0 is the vaule for OMAP2430*/

    plcml_Init->DeviceInfo.TypeofDevice = 0; /*Initialisation for F2F mode*/
    plcml_Init->DeviceInfo.TypeofRender = 0;
    
    
        arr[0] = STREAM_COUNT_AACDEC;                        /*Number of Streams*/
        arr[1] = INPUT_PORT;                                 /*ID of the Input Stream*/
        arr[2] = 0;                                          /*Type of Input Stream */
        arr[3] = 4;                                          /*Number of buffers for Input Stream*/
        arr[4] = OUTPUT_PORT;                                 /*ID of the Output Stream*/
        arr[5] = 0;                                          /*Type of Output Stream */
        arr[6] = 4;                                          /*Number of buffers for Output Stream*/
        arr[7] = 0;                                          /*Decoder Output PCM width is 24-bit or 16-bit*/
        if(pComponentPrivate->codec_params.aacdecParams->nOpBit == 1){
            arr[7] = 1;
        }
        arr[8] = pComponentPrivate->codec_params.aacdecParams->Framemode;         /*Frame mode enable */
        arr[9] = END_OF_CR_PHASE_ARGS;

    plcml_Init->pCrPhArgs = arr;    

    
    #ifdef DASF_Mode_enable
    if(pComponentPrivate->dasfmode == 1) {

        pComponentPrivate->LCMLParams->strmAttr = NULL;
        LCML_STRMATTR *strmAttr;
        strmAttr=(LCML_STRMATTR *)TIMM_OSAL_Malloc(sizeof(LCML_STRMATTR));
        TIMM_OSAL_Memset (strmAttr,0x0, sizeof (LCML_STRMATTR));
        OMX_BASE_ASSERT(strmAttr != NULL,OMX_ErrorInsufficientResources);
        pComponentPrivate->LCMLParams->strmAttr = strmAttr;
        strmAttr->uSegid = 0;
        strmAttr->uAlignment = 0;
        strmAttr->uTimeout = -1;
        strmAttr->uBufsize = pComponentPrivate->portdefs[OUTPUT_PORT]->nBufferSize;/*Changed for DASF AAC*/
        strmAttr->uNumBufs = 2;
        strmAttr->lMode = STRMMODE_PROCCOPY;
        plcml_Init->DeviceInfo.TypeofDevice = 1; /*for dasf mode*/
        plcml_Init->DeviceInfo.TypeofRender = 0;
        plcml_Init->DeviceInfo.AllUUIDs[0].uuid = &DCTN_TI_UUID;
        plcml_Init->DeviceInfo.DspStream = strmAttr;

        arr[0] = STREAM_COUNT_AACDEC;                         /*Number of Streams*/
        arr[1] = INPUT_PORT;                           /*ID of the Input Stream*/
        arr[2] = 0;                                           /*Type of Input Stream */
        arr[3] = 4;                                           /*Number of buffers for Input Stream*/
        arr[4] = OUTPUT_PORT;                          /*ID of the Output Stream*/
        arr[5] = 2;                                           /*Type of Output Stream */
        arr[6] = 2;                                           /*Number of buffers for Output Stream*/
        arr[7] = 0;                                           /*Decoder Output PCM width is 24-bit or 16-bit*/
        if(pComponentPrivate->codec_params.aacdecParams->nOpBit == 1) {
            arr[7] = 1;
        }
        arr[8] = pComponentPrivate->codec_params.aacdecParams->Framemode;          /*Frame mode enable */
        arr[9] = END_OF_CR_PAHSE_ARGS;

    }
    EXIT:   
    
    #endif /*DASF_Mode_enable*/

    return tError;
    
}
/* ===========================================================  */
/**
 * OMX_AAC_DEC_SN_InitParams()  Fills the parameters needed
 * to initialize the AAC_DEC_SN before any Buffer Processing
 *
 *  @param pComponent            OMX Handle
 *
 *  @return None
 */

/*================================================================== */

OMX_ERRORTYPE OMX_AAC_DEC_SN_InitParams(OMX_HANDLETYPE hComponent)
{
        
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    /*to access the private params */
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    OMX_HANDLETYPE pLcmlHandle;
    OMX_U32 pValues[4];
    char *pArgs = "damedesuStr";
    
    /*Check i/p Params */   
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);    
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
    
    pComponentPrivate = (AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
    pLcmlHandle = pComponentPrivate->LCMLParams->pLcmlHandle;
    
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->size = sizeof(MPEG4AACDEC_UALGParams);
    
#ifdef DASF_Mode_enable    
    if(pComponentPrivate->dasfmode == 1) {
        pComponentPrivate->LCMLParams->pParams->unAudioFormat = STEREO_NONINTERLEAVED_STREAM_AACDEC;
        if(pComponentPrivate->codec_params.aacdecParams->aacParams->nChannels == OMX_AUDIO_ChannelModeMono) {
            pComponentPrivate->LCMLParams->pParams->unAudioFormat = MONO_STREAM_AACDEC;
        }
        pComponentPrivate->LCMLParams->pParams->ulSamplingFreq = pComponentPrivate->codec_params.aacdecParams->aacParams->nSampleRate;
        pComponentPrivate->LCMLParams->pParams->unUUID = pComponentPrivate->streamID;
        pValues[0] = USN_STRMCMD_SETCODECPARAMS;
        pValues[1] = (OMX_U32)pComponentPrivate->LCMLParams->pParams;
        pValues[2] = sizeof(USN_AudioCodecParams);
        tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,EMMCodecControlStrmCtrl,(void *)pValues);
        if(tError != OMX_ErrorNone) {
            goto EXIT;
        }
    }
#endif/*dasf mode*/
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->size = sizeof(MPEG4AACDEC_UALGParams);
    /*set the codec according to the values set by the client*/
    if(pComponentPrivate->codec_params.aacdecParams->parameteric_stereo == PARAMETRIC_STEREO_AACDEC){
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->iEnablePS        = 1;/*Added for eAAC*/
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->DownSampleSbr    = 1;
    }else{
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->iEnablePS        = 0;
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->DownSampleSbr    = 0;
        if(pComponentPrivate->codec_params.aacdecParams->SBR ){
            pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->DownSampleSbr    = 1;
        }
    }
    if(pComponentPrivate->pcmParams->bInterleaved){
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->lOutputFormat    = EAUDIO_INTERLEAVED;
    }else{
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->lOutputFormat    = EAUDIO_BLOCK;
    }
#ifdef DASF_Mode_enable
    if(pComponentPrivate->dasfmode == 1){
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->lOutputFormat    = EAUDIO_BLOCK;
    }
#endif
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->nProfile         = pComponentPrivate->codec_params.aacdecParams->nProfile;
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->lSamplingRateIdx = _OMX_AACDEC_GetSampleRateIndexL(pComponentPrivate->codec_params.aacdecParams->aacParams->nSampleRate);
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->bRawFormat       = 0;
    if(pComponentPrivate->codec_params.aacdecParams->aacParams->eAACStreamFormat == OMX_AUDIO_AACStreamFormatRAW){
        pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam->bRawFormat       = 1;
    }
    pValues[0] = IUALG_CMD_SETSTATUS;
    pValues[1] = (OMX_U32)pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam;
    pValues[2] = sizeof(MPEG4AACDEC_UALGParams);
    tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                               EMMCodecControlAlgCtrl,(void *)pValues);
    if(tError != OMX_ErrorNone) {                                          
        goto EXIT;
    }
    tError = LCML_ControlCodec(((LCML_DSP_INTERFACE*)pLcmlHandle)->pCodecinterfacehandle,
                               EMMCodecControlStart,(void *)pArgs);
    if(tError != OMX_ErrorNone) {
        goto EXIT;
    }
 EXIT:
    return tError;


}/*end of AAC_DEC_SN_InitParams*/


/* ================================================================================= * */
/**
 * OMX_AACDEC_DeInit() function frees the component resources.
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

OMX_ERRORTYPE OMX_AACDEC_DeInit(OMX_HANDLETYPE hComponent)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    char *pTemp;
	AUDIODEC_DPRINT("\nentered in AAC_Deinit\n");
    OMX_COMPONENTTYPE *pHandle = NULL;
    AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate = NULL;
    /*check i/p params*/
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
	AUDIODEC_DPRINT("\nafter param check\n");
    pHandle=(OMX_COMPONENTTYPE *)hComponent;
    pComponentPrivate=(AUDIODEC_COMPONENT_PRIVATE *)pHandle->pComponentPrivate;
    AUDIODEC_DPRINT("\nafter componentprivate initialization\n");
    
    /*free the aacdecParams->AACDEC_UALGParam*/     
    pTemp = (char*)pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam;
    if (pTemp != NULL) { 
		AUDIODEC_DPRINT("\n in codec_params.aacdecParams free\n");
        pTemp -= EXTRA_BYTES;
    }   
    pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam = (MPEG4AACDEC_UALGParams*)pTemp;
    
    TIMM_OSAL_Free(pComponentPrivate->codec_params.aacdecParams->AACDEC_UALGParam);
    
    AUDIODEC_DPRINT("\naacdecParams are freedup\n");
    
    /*free the aacdecParams->aacParams*/
    TIMM_OSAL_Free(pComponentPrivate->codec_params.aacdecParams->aacParams);
        
    /*free the aacdecParams*/
    TIMM_OSAL_Free(pComponentPrivate->codec_params.aacdecParams);
    AUDIODEC_DPRINT("\naacdecParams are freedup\n");

    EXIT:
    return tError;
}

/*=======================================================================*/
/*! @fn _OMX_AACDEC_GetSampleRateIndexL

 * @brief Gets the sample rate index

 * @param  aRate : Actual Sampling Freq

 * @Return  Index

 */
/*=======================================================================*/
int _OMX_AACDEC_GetSampleRateIndexL( const int aRate)
{
    int index = 0;
    
    switch( aRate ){
    case 96000:
        index = 0;
        break;
    case 88200:
        index = 1;
        break;
    case 64000:
        index = 2;
        break;
    case 48000:
        index = 3;
        break;
    case 44100:
        index = 4;
        break;
    case 32000:
        index = 5;
        break;
    case 24000:
        index = 6;
        break;
    case 22050:
        index = 7;
        break;
    case 16000:
        index = 8;
        break;
    case 12000:
        index = 9;
        break;
    case 11025:
        index = 10;
        break;
    case 8000:
        index = 11;
        break;
    default:
        index=-1;
        break;
    }
    
    return index;
}


