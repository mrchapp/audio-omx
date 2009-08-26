/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */

/* =========================================================================== */
/**
* @file omx_audioencoderutils.h
*
* This is an header file for an Audio Encoder that is fully
* compliant with the OMX Audio specification 1.5.
* This the file that the application that uses OMX would include
* in its code.
*
* @path
*
* @rev
*/
/* -------------------------------------------------------------------------- */
/*-----------  User code goes here ------------------*/

#ifndef OMX_AUDIOENCODERUTILS_H
#define OMX_AUDIOENCODERUTILS_H

/*This is used specifically in the case when the component is moving from paused
  to executing state. We send this as the port index parameter so that the data
  notify function will not expect a buffer header but will directly start
  taking buffers from the i/p and o/p data pipes*/
#define PAUSED_TO_EXECUTING 100



/*Print traces for debugging*/
/*#define DEBUG*/
#ifdef DEBUG
    #define DPRINT printf
#else
    #define DPRINT(...)
#endif




#define DEFAULT_START_PORT_NUM 0

#define DEFAULT_CONTEXT_TYPE    OMX_BASE_CONTEXT_PASSIVE

#define OMX_AUDIO_ENC_COMP_VERSION_MAJOR 1

#define OMX_AUDIO_ENC_COMP_VERSION_MINOR 1

#define OMX_AUDIO_ENC_COMP_VERSION_REVISION 0

#define OMX_AUDIO_ENC_COMP_VERSION_STEP 0

#define OMX_AUDIO_ENC_GROUPID 0

#define OMX_AUDIO_ENC_TASK_STACKSIZE 1024*4  /*4KB randomly set for now*/

#define OMX_AUDIO_ENC_TASK_PRIORITY 15  /*Random value*/

#define NUM_OF_PORTS 2

#define AUDIOENC_INPUT_PORT 0

#define AUDIOENC_OUTPUT_PORT 1




/* ========================================================================= */
/* OMX_AUDIOENC_COMMON_INDEXTYPE - Vendor specific indices. Have currently kept
 *                                 0x7F000001 to 0x7F000100 for common audio
 *                                 encode indices. Codec specific indices start
 *                                 from 0x7F000101. This range may be changed
 *                                 if required. However vendor specific indices
 *                                 have to remain within 0x7F000001 - 0x7FFFFFFF.
 *
 * @param OMX_IndexCustomAudioEncStreamIDConfig    :Stream ID configuration.
 */
/* ========================================================================= */
typedef enum OMX_AUDIOENC_COMMON_INDEXTYPE
{
    OMX_IndexCustomAudioEncStreamIDConfig = 0x7F000001,
}OMX_AUDIOENC_COMMON_INDEXTYPE;



/* ========================================================================= */
/* OMX_AUDIOENC_COMPONENT_ROLE - Component Role Enumeration
 *
 * @param UNDEFINED_ROLE        :Undefined Role.
 *
 * @param AAC_ENCODE            :AAC encoding.
 */
/* ========================================================================= */
typedef enum OMX_AUDIOENC_COMPONENT_ROLE
{
    UNDEFINED_ROLE,
    AAC_ENCODE
}OMX_AUDIOENC_COMPONENT_ROLE;



/* ========================================================================= */
/* OMX_AUDIOENC_PIPE_TYPE - To differentiate among the two types of data pipes
 *                          used by this component. Currently this enumeration
 *                          is used only by the
 *                          OMX_AUDIO_ENC_ReturnPendingBuffers() function.
 *
 * @param DATA_PIPE            :pInDataPipe or pOutDataPipe.
 *
 * @param LCML_DATA_PIPE       :pInBufPendingWithLcmlPipe or
 *                              pOutBufPendingWithLcmlPipe.
 */
/* ========================================================================= */
typedef enum OMX_AUDIOENC_PIPE_TYPE
{
    DATA_PIPE,
    LCML_DATA_PIPE
}OMX_AUDIOENC_PIPE_TYPE;



/* =================================================================== */
/* OMX_AUDIOENC_PRIVATE_PIPE_PTRS - Structure contains pointers to the various
 *                                  internal data pipes used by the component.
 *
 * @param pInDataPipe                 :Queues all input buffer headers received
 *                                     by the component before they are sent to
 *                                     the lcml layer.
 *
 * @param pOutDataPipe                :Queues all output buffer headers received
 *                                     by the component before they are sent to
 *                                     the lcml layer.
 *
 * @param pInBufPendingWithLcmlPipe   :Queues up all the input buffer headers
 *                                     that are currently with the lcml layer.
 *
 * @param pOutBufPendingWithLcmlPipe  :Queues up all the output buffer headers
 *                                     that are currently with the lcml layer.
 */
/* ==================================================================== */
typedef struct OMX_AUDIOENC_PRIVATE_PIPE_PTRS
{
    OMX_PTR pInDataPipe;
    OMX_PTR pOutDataPipe;
    OMX_PTR pInBufPendingWithLcmlPipe;
    OMX_PTR pOutBufPendingWithLcmlPipe;
}OMX_AUDIOENC_PRIVATE_PIPE_PTRS;



/* =================================================================== */
/* OMX_AUDIOENC_PRIVATE_LCML_PARAMS - Structure for the LCML specific members to
 *                                    be used by the audio encode component
 *                                    private structure.
 *
 * @param ptrLibLCML          :Pointer to LCML library - this is the pointer
 *                             returned by dlopen() call.
 *
 * @param pLcmlHandle         :Handle received from LCML GetHandle function.
 *
 * @param bLcmlParamsFreed    :Indicates whether all LCML specific parameters
 *                             have been freed or not.
 */
/* ==================================================================== */
typedef struct OMX_AUDIOENC_PRIVATE_LCML_PARAMS
{
     /*Ptr to lcml library*/
    OMX_PTR ptrLibLCML;
    OMX_HANDLETYPE pLcmlHandle;
    OMX_BOOL bLcmlParamsFreed;
}OMX_AUDIOENC_PRIVATE_LCML_PARAMS;



/* =================================================================== */
/* OMX_AUDIOENC_COMPONENT_PRIVATE - Structure for the private elements of the
 *                                  audio encode component. This structure
 *                                  includes all the private elements of the
 *                                  Base Component
 *
 *   All derived Component Specifc fields are as defined below
 *
 * @param pHandle                            :Component Handle.
 *
 * @param componentRole                      :Component role structure.
 *
 * @param eComponentRole                     :Enumeration for component role.
 *                                            Used in various switch cases.
 *
 * @param tPipePtrs                          :Structure contains pointers to the
 *                                            various data pipes used by the component.
 *
 * @param tLcmlParams                        :Structure contains lcml specific
 *                                            members to be used by the component.
 *
 * @param rmproxyCallback                    :Function pointer to Resource
 *                                            Manager callback function.
 *
 * @param streamID                           :Stream ID.
 *
 * @param sDeviceString                      :Device string - will be initialised
 *                                            on setting the role, can be set
 *                                            via a SetConfig call.
 *
 * @param bNoIdleOnStop                      :Indicates not to transition to idle
 *                                            when codec stops. Used when outport
 *                                            port is disabled while the component
 *                                            is in the executing state.
 *
 * @param inEOSReceived                      :Indicates that an EOS has been received.
 *
 * @param bReturnOutputBufWithoutProcessing  :Will be set when an empty input buffer
 *                                            is received. In this case both the
 *                                            input and the corresponding output
 *                                            buffers are returned as it is.
 *
 * @param tAudioPortFormat                   :Audio port format structure.
 *
 * @param bAudioPortFormatSet                :If the user sets audio port format
 *                                            before setting the role, it is not
 *                                            set to default value.
 *
 * @param pcmParams                          :PCM specific parameters
 *                                           (only on input port for encoders).
 *
 * @param bpcmParamsSet                      :If the user sets pcm params before
 *                                            setting the role, it is
 *                                            not set to default value.
 *
 * @param bportdefsSet                       :If the user sets portdefs(some
 *                                            members of portdefs are role specific)
 *                                            before setting the role, it is not
 *                                            set to default value.
 *
 * @param union                              :This union contains private structures
 *                                            of various codecs. Depending on the
 *                                            role, the corresponding structure
 *                                            will be initialised.
 */
/* ==================================================================== */

DERIVEDSTRUCT(OMX_AUDIOENC_COMPONENT_PRIVATE, OMX_BASE_PRIVATETYPE)
#define OMX_AUDIOENC_COMPONENT_PRIVATE_FIELDS OMX_BASE_PRIVATETYPE_FIELDS \
    OMX_COMPONENTTYPE *pHandle; \
    OMX_PARAM_COMPONENTROLETYPE componentRole; \
    OMX_AUDIOENC_COMPONENT_ROLE eComponentRole; \
    OMX_AUDIOENC_PRIVATE_PIPE_PTRS tPipePtrs; \
    OMX_AUDIOENC_PRIVATE_LCML_PARAMS tLcmlParams; \
/*    RMPROXY_CALLBACKTYPE rmproxyCallback; */\
    OMX_U32 streamID; \
    OMX_STRING* sDeviceString; \
    OMX_BOOL bNoIdleOnStop; \
    OMX_U16 inEOSReceived; \
    OMX_BOOL bReturnOutputBufWithoutProcessing; \
    OMX_AUDIO_PARAM_PORTFORMATTYPE tAudioPortFormat[NUM_OF_PORTS]; \
    OMX_BOOL bAudioPortFormatSet[NUM_OF_PORTS]; \
    OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams; \
    OMX_BOOL bpcmParamsSet; \
    OMX_BOOL bportdefsSet[NUM_OF_PORTS]; \
    union \
    { \
        OMX_AUDIOENC_AAC_PRIVATE_PARAMS *pAacPrivateParams; \
    };
	ENDSTRUCT(OMX_AUDIOENC_COMPONENT_PRIVATE)



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_COMMON_GetExtensionIndex() - Returns index for vendor
 *                                                specific settings common to
 *                                                audio encoding.
 *
 * @ see OMX_core.h
 *
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                                     OMX_STRING cParameterName,
                                                     OMX_INDEXTYPE* pIndexType);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_COMMON_GetParameter() - This function gets the parameters
 *                                           common to audio encoding. This
 *                                           function internally calls the base
 *                                           GetParameter function.
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
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetParameter(OMX_HANDLETYPE hComponent,
                                           OMX_INDEXTYPE nParamIndex,
                                           OMX_PTR pComponentParameterStructure);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_COMMON_SetParameter() - This function sets the parameters
 *                                           common to audio encoding. This
 *                                           function internally calls the base
 *                                           SetParameter function.
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
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_SetParameter(OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE nParamIndex,
                                          OMX_PTR pComponentParameterStructure);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_COMMON_GetConfig() - This function gets the Configurations
 *                                        common to audio encoding.
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
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nConfigIndex,
                                             OMX_PTR pComponentConfigStructure);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_COMMON_SetConfig() - This function sets the Configurations
 *                                        common to audio encoding.
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
OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_SetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nConfigIndex,
                                             OMX_PTR pComponentConfigStructure);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_GetLCMLHandle() - This function loads the LCML library and
 *                                     calls the lcml get handle function.
 *
 *  @ param pComponentPrivate        :Handle of the component private structure.
 *
 *  @ retval OMX_HANDLETYPE
 */
/* ===========================================================================*/
OMX_HANDLETYPE OMX_AUDIO_ENC_GetLCMLHandle
               (OMX_AUDIOENC_COMPONENT_PRIVATE* pComponentPrivate);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_ReturnPendingBuffers() - This function will return all
 *                                            buffers pending in any data pipe
 *                                            depending on the parameters.
 *
 *  @ param pComponentPrivate        :Handle of the component private structure.
 *
 *  @ param pipe                     :DATA_PIPE or LCML_DATA_PIPE.
 *
 *  @ param dir                      :Input, output or both.
 *
 *  @ retval OMX_ERRORTYPE
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_AUDIO_ENC_ReturnPendingBuffers
                             (OMX_AUDIOENC_COMPONENT_PRIVATE* pComponentPrivate,
                              OMX_AUDIOENC_PIPE_TYPE pipe, OMX_U32 dir);


#endif