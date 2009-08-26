/* =============================================================================
*             Texas Instruments OMAP (TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file AudioDecUtils.c
* This file contains methods that provides the common functionality for the Audio Decoder component.
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
#include "TIDspOmx.h"
#include "LCML_DspCodec.h"

#include "omx_audiodecutils.h"

/* ========================================================================== */
/**
* @fn OMX_AUDIO_DEC_COMMON_GetParameter - Gets Common Parameters from the Component.
*                                           This method will update parameters from the component to the app.
* @param hComponent - handle for this instance of the component
* @param nParamIndex - Component Index Port
* @param pParamStruct - Component Parameter Structure
* @return:  OMX_ERRORTYPE
*           OMX_ErrorNone on success
*           !OMX_ErrorNone on any failure
* @see         None
*/
/* ========================================================================== */

OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_GetParameter (OMX_HANDLETYPE hComponent,
                                              OMX_INDEXTYPE nParamIndex,
                                              OMX_PTR pComponentParameterStructure)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    /*check the i/p params*/    
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
        
    pComponentPrivate=(AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);

    switch(nParamIndex)
    {
        /* Client uses this to get the num of Audio ports */
        case OMX_IndexParamAudioInit:
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_PORT_PARAM_TYPE, tError);
            TIMM_OSAL_Memcpy((OMX_PORT_PARAM_TYPE*)pComponentParameterStructure,
                                &(pComponentPrivate->tDerToBase.tPortParams),
                                 sizeof(OMX_PORT_PARAM_TYPE));
            
        break;

        case OMX_IndexParamAudioPcm:
            
            /*get the PCM port params*/
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_AUDIO_PARAM_PCMMODETYPE, tError);
            
            if(((OMX_AUDIO_PARAM_PCMMODETYPE*)(pComponentParameterStructure))->nPortIndex ==
                  pComponentPrivate->pcmParams->nPortIndex)
                  
            TIMM_OSAL_Memcpy(pComponentParameterStructure,
                             pComponentPrivate->pcmParams,
                             sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));            
        break;

        

        case OMX_IndexParamStandardComponentRole:
            /*get the role info*/
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_PARAM_COMPONENTROLETYPE, tError);
            
            TIMM_OSAL_Memcpy(pComponentParameterStructure,
                             &(pComponentPrivate->componentRole),
                             sizeof(OMX_PARAM_COMPONENTROLETYPE));
            
        break;



        case OMX_IndexParamPortDefinition:
            
            /* to get the buffer count/size and port diable/enable properties and domain of the port configurations*/
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_PARAM_PORTDEFINITIONTYPE, tError);
            
            if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pComponentParameterStructure))->nPortIndex ==
           pComponentPrivate->portdefs[INPUT_PORT]->nPortIndex)
            {               
                TIMM_OSAL_Memcpy(pComponentParameterStructure, pComponentPrivate->portdefs[INPUT_PORT], sizeof(OMX_PARAM_PORTDEFINITIONTYPE));          
            }
            else if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pComponentParameterStructure))->nPortIndex ==
                  pComponentPrivate->portdefs[OUTPUT_PORT]->nPortIndex)
            {                
                TIMM_OSAL_Memcpy(pComponentParameterStructure, pComponentPrivate->portdefs[OUTPUT_PORT], sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            }
            else
            {
                tError = OMX_ErrorBadPortIndex;
            }
        break;
        
 
         /* client uses this to get the format type of an port  */
        case OMX_IndexParamAudioPortFormat:
           
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_AUDIO_PARAM_PORTFORMATTYPE, tError);

            if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nPortIndex ==
                pComponentPrivate->portdefs[INPUT_PORT]->nPortIndex) {
                    if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nIndex >
                        pComponentPrivate->pPortFormat[INPUT_PORT]->nIndex) {
                        tError = OMX_ErrorNoMore;
                    }
                    else {
                        TIMM_OSAL_Memcpy(pComponentParameterStructure, pComponentPrivate->pPortFormat[INPUT_PORT], sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                    }
            }
            else if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nPortIndex ==
                    pComponentPrivate->portdefs[OUTPUT_PORT]->nPortIndex){
                        if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nIndex >
                           pComponentPrivate->pPortFormat[OUTPUT_PORT]->nIndex) {
                           tError = OMX_ErrorNoMore;
                        }
                        else {
                            TIMM_OSAL_Memcpy(pComponentParameterStructure, pComponentPrivate->pPortFormat[OUTPUT_PORT], sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                        }
            }else{
                tError = OMX_ErrorBadPortIndex;
            }
        break;


        /*Add more audio decoder specific indices here*/
        default:
            return OMX_BASE_GetParameter(hComponent, nParamIndex, pComponentParameterStructure);
    }
    EXIT:
        return tError;
}//AUDIO_DEC_COMMON_GetParam ends

/* ========================================================================== */
/**
 * @fn OMX_AUDIO_DEC_COMMON_SetParameter - Sets the common parameters sent by the Application and
 *                                              to the component
 * @param hComponent - handle for this instance of the component
 * @param nParamIndex - Component Index Port
 * @param pParamStruct - Component Parameter Structure to set
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None
 */
/* ========================================================================== */

OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_SetParameter (OMX_HANDLETYPE hComponent,
                                                 OMX_INDEXTYPE nParamIndex,
                                                 OMX_PTR pComponentParameterStructure)
{

    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE  *pComponentPrivate;
    
    /*check the i/p params*/    
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
            
    pComponentPrivate=(AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
    
    switch(nParamIndex)
    {
        /* Client uses this to modify the num of Audio ports */
        case OMX_IndexParamAudioInit:
            AUDIODEC_DPRINT("\nOMX_IndexParamAudioInit\n");
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_PORT_PARAM_TYPE, tError);
            pComponentPrivate->tDerToBase.tPortParams.nPorts=((OMX_PORT_PARAM_TYPE*)pComponentParameterStructure)->nPorts;
            pComponentPrivate->tDerToBase.tPortParams.nStartPortNumber=((OMX_PORT_PARAM_TYPE*)pComponentParameterStructure)->nStartPortNumber;
            break;

        case OMX_IndexParamAudioPcm:
            AUDIODEC_DPRINT("\nOMX_IndexParamAudioPcm\n");
            /*set the pcm related params*/
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_AUDIO_PARAM_PCMMODETYPE, tError);
            if((((OMX_AUDIO_PARAM_PCMMODETYPE*)pComponentParameterStructure)->nPortIndex) ==
               pComponentPrivate->pcmParams->nPortIndex){
                AUDIODEC_DPRINT("\nCopy PCM params\n");
                  
                TIMM_OSAL_Memcpy(pComponentPrivate->pcmParams,
                                 pComponentParameterStructure,
                                 sizeof(OMX_AUDIO_PARAM_PCMMODETYPE));
                /*                
                if (((OMX_AUDIO_PARAM_PCMMODETYPE *)(pComponentParameterStructure))->nBitPerSample == 24) {
                    pComponentPrivate->codec_params.aacdecParams->nOpBit = 1;
                }
                else { 
                    pComponentPrivate->codec_params.aacdecParams->nOpBit = 0;
                }
                */
            }
            else
            tError=OMX_ErrorBadParameter;

            break;
        

        case OMX_IndexParamStandardComponentRole:
            AUDIODEC_DPRINT("\nOMX_IndexParamStandardComponentRole\n");
            /*set the role info*/
            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_PARAM_COMPONENTROLETYPE, tError);

            /*pComponentPrivate->componentRole = *((OMX_PARAM_COMPONENTROLETYPE*)pComponentParameterStructure);*/
            //Initialize Role specific params inside the if-else ladder
            if(strcmp((char*)(pComponentPrivate->componentRole.cRole),
                      (char*)(((OMX_PARAM_COMPONENTROLETYPE*)
                               pComponentParameterStructure)->cRole)) == 0){
                AUDIODEC_DPRINT("\nCurrent and new role are the same - %s\n",
                                pComponentPrivate->componentRole.cRole);
                /*Current and new role are the same - no need to do anything*/
            }
            else
            {
                /*Free up the structures allocated for the current role*/
#if 0
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_DECODE:
                        AUDIODEC_DPRINT("\ncall InitParams for AAC_DECODE\n");
                        tError = OMX_AAC_DEC_InitParams(hComponent);
                        if(tError != OMX_ErrorNone){
                            goto EXIT;
                        }
                        break;
                     case MP3_DECODE:
                        AUDIODEC_DPRINT("\ncall InitParams for MP3_DECODE\n");
                        tError = OMX_MP3_DEC_InitParams(hComponent);
                        if(tError != OMX_ErrorNone){
                            goto EXIT;
                        }
                        break;
                    case UNDEFINED_ROLE:
                        break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                        break;
                        /*Add other roles here*/
                }
#endif
                pComponentPrivate->componentRole = *((OMX_PARAM_COMPONENTROLETYPE*)pComponentParameterStructure);
                /*Initialize Role specific params inside the if-else ladder*/
                if(strcmp((char *)(pComponentPrivate->componentRole.cRole),
                          "audio_decode.dsp.aac") == 0){
                    pComponentPrivate->eComponentRole = AAC_DECODE;
                    tError = OMX_AAC_DEC_InitParams(hComponent);
                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                }
                else if(strcmp((char *)(pComponentPrivate->componentRole.cRole),
                          "audio_decode.dsp.mp3") == 0){
                    pComponentPrivate->eComponentRole = MP3_DECODE;
                    tError = OMX_MP3_DEC_InitParams(hComponent);
                    if(tError != OMX_ErrorNone)
                    {
                        goto EXIT;
                    }
                }
                else
                {
                    pComponentPrivate->eComponentRole = UNDEFINED_ROLE;
                }
            }/*!=samerole ends */
            break;      

            /* to set the buffer count/size and port diable/enable properties and domain of the port configurations*/
        case OMX_IndexParamPortDefinition:
            AUDIODEC_DPRINT("\nOMX_IndexParamPortDefinition\n");

            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_PARAM_PORTDEFINITIONTYPE, tError);

            if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pComponentParameterStructure))->nPortIndex ==
               pComponentPrivate->portdefs[INPUT_PORT]->nPortIndex)
            {
                /*check whether the port has been disabled or not*/
                OMX_BASE_REQUIRE(((pComponentPrivate->portdefs[INPUT_PORT]->bEnabled==OMX_FALSE)||
                                  (pComponentPrivate->tCurState==OMX_StateLoaded)),
                                 OMX_ErrorIncorrectStateOperation);
                switch(pComponentPrivate->eComponentRole)
                {
                    case AAC_DECODE:
                        AUDIODEC_DPRINT("\nOMX_IndexParamPortDefinition for AAC_DECODE\n");
                        OMX_BASE_REQUIRE(((((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->format.audio.eEncoding == OMX_AUDIO_CodingAAC)&&
                                          (((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->eDir == OMX_DirInput)),OMX_ErrorBadParameter);
                        break;
                    case MP3_DECODE:
                        AUDIODEC_DPRINT("\nOMX_IndexParamPortDefinition for MP3_DECODE\n");
                        OMX_BASE_REQUIRE(((((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->format.audio.eEncoding == OMX_AUDIO_CodingMP3)&&
                                          (((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->eDir == OMX_DirInput)),OMX_ErrorBadParameter);
                        break;
                    case UNDEFINED_ROLE:
                        AUDIODEC_DPRINT("\nOMX_IndexParamPortDefinition for UNDEFINED_ROLE\n");
                        break;
                    default:
                        tError = OMX_ErrorUnsupportedIndex;
                        goto EXIT;
                        break;
                        /*Add other roles here*/
                }
                /*OMX_BASE_REQUIRE(((((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->format.audio.eEncoding == OMX_AUDIO_CodingAAC)&&(((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->eDir == OMX_DirInput)),OMX_ErrorBadParameter);*/
                OMX_BASE_REQUIRE(((OMX_PARAM_PORTDEFINITIONTYPE *)(pComponentParameterStructure))->nPortIndex == pComponentPrivate->portdefs[INPUT_PORT]->nPortIndex,OMX_ErrorBadParameter);
                TIMM_OSAL_Memcpy(pComponentPrivate->portdefs[INPUT_PORT], pComponentParameterStructure, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            
            } else if(((OMX_PARAM_PORTDEFINITIONTYPE *)(pComponentParameterStructure))->nPortIndex ==
                      pComponentPrivate->portdefs[OUTPUT_PORT]->nPortIndex)
            {
                /*check whether the port has been disabled or not*/
                OMX_BASE_REQUIRE(((pComponentPrivate->portdefs[INPUT_PORT]->bEnabled==OMX_FALSE)||(pComponentPrivate->tCurState==OMX_StateLoaded)),OMX_ErrorIncorrectStateOperation);
                OMX_BASE_REQUIRE(((((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->format.audio.eEncoding == OMX_AUDIO_CodingPCM)&&(((OMX_PARAM_PORTDEFINITIONTYPE*)pComponentParameterStructure)->eDir == OMX_DirOutput)),OMX_ErrorBadParameter);
                OMX_BASE_REQUIRE(((OMX_PARAM_PORTDEFINITIONTYPE *)(pComponentParameterStructure))->nPortIndex == pComponentPrivate->portdefs[OUTPUT_PORT]->nPortIndex,OMX_ErrorBadParameter);
                TIMM_OSAL_Memcpy(pComponentPrivate->portdefs[OUTPUT_PORT], pComponentParameterStructure, sizeof(OMX_PARAM_PORTDEFINITIONTYPE));
            } else {
                
                tError = OMX_ErrorBadPortIndex;
            }
            
            break;
  
            /* client uses this to modify the format type of an port  */
        case OMX_IndexParamAudioPortFormat:
            AUDIODEC_DPRINT("\nOMX_IndexParamAudioPortFormat\n");

            OMX_BASE_CHK_VERSION(pComponentParameterStructure, OMX_AUDIO_PARAM_PORTFORMATTYPE, tError);

            if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nPortIndex ==
               pComponentPrivate->portdefs[INPUT_PORT]->nPortIndex) {

                if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nIndex >
                   pComponentPrivate->pPortFormat[INPUT_PORT]->nIndex) {
                    tError = OMX_ErrorNoMore;
                }
                else {
                    /*check whether the port has been disabled or not*/
                    OMX_BASE_REQUIRE(((pComponentPrivate->portdefs[INPUT_PORT]->bEnabled==OMX_FALSE)||(pComponentPrivate->tCurState==OMX_StateLoaded)),OMX_ErrorIncorrectStateOperation);
                    TIMM_OSAL_Memcpy(pComponentParameterStructure, pComponentPrivate->pPortFormat[INPUT_PORT], sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                }
            } else if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nPortIndex ==
                      pComponentPrivate->portdefs[OUTPUT_PORT]->nPortIndex){
                if(((OMX_AUDIO_PARAM_PORTFORMATTYPE *)(pComponentParameterStructure))->nIndex >
                   pComponentPrivate->pPortFormat[OUTPUT_PORT]->nIndex) {
                    tError = OMX_ErrorNoMore;
                }
                else {
                    /*check whether the port has been disabled or not*/
                    OMX_BASE_REQUIRE(((pComponentPrivate->portdefs[OUTPUT_PORT]->bEnabled==OMX_FALSE)||(pComponentPrivate->tCurState==OMX_StateLoaded)),OMX_ErrorIncorrectStateOperation);
                    TIMM_OSAL_Memcpy(pComponentParameterStructure, pComponentPrivate->pPortFormat[OUTPUT_PORT], sizeof(OMX_AUDIO_PARAM_PORTFORMATTYPE));
                }
            }else
            {
                tError = OMX_ErrorBadPortIndex;
            }
            break;
            /*Add more audio decoder specific indices here*/
        default:
            AUDIODEC_DPRINT("\nDefault\n");
            return OMX_BASE_GetParameter(hComponent, nParamIndex, pComponentParameterStructure);


    }/*Switch case ends*/

	
 EXIT:       
    return tError;

}/*Setparameter ends*/
OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_SetConfig(OMX_HANDLETYPE hComponent,
                                            OMX_INDEXTYPE nConfigIndex,
                                            OMX_PTR pComponentConfigStructure){
    OMX_ERRORTYPE tError = OMX_ErrorNone;
       
    tError = OMX_ErrorUnsupportedIndex;
    goto EXIT;
       
    EXIT:
        return tError;

}/*setconfig ends*/

OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_GetConfig(OMX_HANDLETYPE hComponent,
                                                OMX_INDEXTYPE nConfigIndex,
                                                OMX_PTR pComponentConfigStructure){

    OMX_ERRORTYPE tError = OMX_ErrorNone;
       
    tError = OMX_ErrorUnsupportedIndex;
    goto EXIT;
       
    EXIT:
        return tError;

}/*getconfig ends*/

OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                                     OMX_STRING cParameterName,
                                                     OMX_INDEXTYPE* pIndexType){

    OMX_ERRORTYPE tError = OMX_ErrorNone;
       
    tError = OMX_ErrorUnsupportedIndex;
    goto EXIT;
       
    EXIT:
        return tError;

}/*get extension index ends*/

/* ========================================================================= */
/*
 * @fn _OMX_AUDIO_DEC_ReturnPendingBuffers() - returns all the pending buffers in the
 *                                              (i/p or o/p)data or LCMLPending pipes
 * @param hComponent - handle of the component
 * @param nParamIndex - Component Index Port
 * @param pParamStruct - Component Parameter Structure to set
 * @return: OMX_ERRORTYPE
 *          OMX_ErrorNone on success
 *          !OMX_ErrorNone on any failure
 * @see         None 
 */
/* ========================================================================= */
OMX_ERRORTYPE _OMX_AUDIO_DEC_ReturnPendingBuffers(OMX_HANDLETYPE hComponent ,
                                            OMX_AUDIODEC_PIPE_TYPE pipe, OMX_U32 Dir)
{
    OMX_ERRORTYPE tError = OMX_ErrorNone;
    AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate;
    TIMM_OSAL_U32 elementsInPipe = 0;
    TIMM_OSAL_U32 elementsOutPipe = 0;
    TIMM_OSAL_U32 actualSize = 0;
    OMX_BUFFERHEADERTYPE* pInBufHeader = NULL;
    OMX_BUFFERHEADERTYPE* pOutBufHeader = NULL;
    
    /*check the i/p params*/    
    OMX_BASE_REQUIRE((hComponent != NULL), OMX_ErrorBadParameter);
    OMX_BASE_REQUIRE((((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate) != NULL, OMX_ErrorBadParameter);
            
    pComponentPrivate=(AUDIODEC_COMPONENT_PRIVATE *)(((OMX_COMPONENTTYPE*)hComponent)->pComponentPrivate);
        
    switch(pipe)
    {
        case DATA_PIPE:
            if(Dir == OMX_DirInput || Dir ==OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount((pComponentPrivate->pInDataPipe), &elementsInPipe);
                while(elementsInPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->pInDataPipe), 
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
                                       INPUT_PORT, pInBufHeader);
                }
            }
            if(Dir == OMX_DirOutput || Dir ==OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount((pComponentPrivate->pOutDataPipe), &elementsOutPipe);
                while(elementsOutPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->pOutDataPipe), 
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
                                       OUTPUT_PORT, pOutBufHeader);
                }
            }
        break;
        case LCML_DATA_PIPE:
            if(Dir == OMX_DirInput || Dir ==OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount((pComponentPrivate->LCMLParams->pInBufPendingWithLcmlPipe), 
                                                   &elementsInPipe);
                
                while(elementsInPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->LCMLParams->pInBufPendingWithLcmlPipe),
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
                                                          INPUT_PORT, 
                                                          pInBufHeader);
                }
            }
            if(Dir == OMX_DirOutput || Dir ==OMX_ALL)
            {
                TIMM_OSAL_GetPipeReadyMessageCount((pComponentPrivate->LCMLParams->pOutBufPendingWithLcmlPipe), 
                                                   &elementsOutPipe);
                while(elementsOutPipe)
                {
                    tError = TIMM_OSAL_ReadFromPipe
                             ((pComponentPrivate->LCMLParams->pOutBufPendingWithLcmlPipe),
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
                                                          OUTPUT_PORT, 
                                                          pOutBufHeader);
                }
            }
        break;
    }
EXIT:
    return tError;
}   
    
    

