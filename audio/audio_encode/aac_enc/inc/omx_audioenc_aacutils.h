/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */

/* =========================================================================== */
/**
* @file omx_audioenc_aacutils.h
*
* This is an header file for an AAC Encoder that is fully
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


#ifndef OMX_AUDIOENC_AACUTILS_H
#define OMX_AUDIOENC_AACUTILS_H

#define MAX_NUM_OF_BUFS 5


/*#defines from omap3 aac code*/

#define MPEG4AACENC_MAX_OUTPUT_FRAMES 24

#define NUM_AACENC_INPUT_BUFFERS 1

#define NUM_AACENC_OUTPUT_BUFFERS 1

#define INPUT_AACENC_BUFFER_SIZE 8192

#define OUTPUT_AACENC_BUFFER_SIZE 9200

#define OMX_AACENC_DEFAULT_SEGMENT (0)

#define OMX_AACENC_SN_PRIORITY (10)



/* ========================================================================= */
/* OMX_AUDIOENC_AAC_INDEXTYPE - Vendor specific indices for AAC encoding. Have
 *                              currently kept 0x7F000001 to 0x7F000100 for
 *                              common audio encode indices. AAC specific indices
 *                              start from 0x7F000101. This range may be changed
 *                              if required. However vendor specific indices
 *                              have to remain within 0x7F000001 - 0x7FFFFFFF.
 *
 * @param OMX_IndexCustomAacEncHeaderInfoConfig    :Header info configuration.
 *
 * @param OMX_IndexCustomAacEncFramesPerOutBuf     :Frames per o/p buffer
 *                                                  configuration.
 *
 * @param OMX_IndexCustomAacEncDataPath            :Data path configuration.
 */
/* ========================================================================= */
typedef enum OMX_AUDIOENC_AAC_INDEXTYPE
{
    OMX_IndexCustomAacEncHeaderInfoConfig = 0x7F000101,
    OMX_IndexCustomAacEncFramesPerOutBuf,
    OMX_IndexCustomAacEncDataPath
}OMX_AUDIOENC_AAC_INDEXTYPE;



/* The following eight enumerations and structures have been taken as is from
   the omap3 code */

typedef enum
{
    IUALG_CMD_STOP          = 0,
    IUALG_CMD_PAUSE         = 1,
    IUALG_CMD_GETSTATUS     = 2,
    IUALG_CMD_SETSTATUS     = 3,
    IUALG_CMD_USERCMDSTART  = 100
}IUALG_Cmd;

typedef enum
{
  AACENC_OBJ_TYP_LC           = 2,   /*!< AAC Low Complexity  */
  AACENC_OBJ_TYP_HEAAC        = 5,   /*!< HE AAC              */
  AACENC_OBJ_TYP_PS           = 29   /*!< AAC LC + SBR + PS   */
} AACENC_OBJ_TYP;

/*! \enum  AACENC_TRANSPORT_TYPE */
/*! \brief Enumerated type for output file format */
typedef enum
{
  AACENC_TT_RAW    = 0,              /*!< Raw Output Format   */
  AACENC_TT_ADIF   = 1,              /*!< ADIF File Format    */
  AACENC_TT_ADTS   = 2               /*!< ADTS File Format    */
} AACENC_TRANSPORT_TYPE;

typedef struct AACENC_AudioCodecParams
{
    unsigned long  iSamplingRate;
    unsigned long  iStrmId;
    unsigned short iAudioFormat;
}AACENC_AudioCodecParams;

/* ======================================================================= */
/** MPEG4AACENC_UALGParams
 *
 * @param lOutputFormat - To set interleaved/Block format
 * @param DownSampleSbr -
 */
/* ==================================================================== */
typedef struct
{
    long size;           /* size of this structure */
    long bitRate;        /* Average bit rate in bits per second */
    long sampleRate;     /* Samplling frequency in Hz */
    long numChannels;    /* Number of Channels: IAUDIO_ChannelId */

}MPEG4AUDENC_UALGdynamicparams;

typedef struct MPEG4AACENC_UALGParams
{
    int size;
    MPEG4AUDENC_UALGdynamicparams audenc_dynamicparams;
                                    /*!< generic encoder dynamic parameters  */

    /* For RESET Command */
    OMX_BOOL        useTns;       /*!< Flag for activating TNS feature*/
    OMX_BOOL        usePns;       /*!< Flag for activating PNS feature*/
    AACENC_OBJ_TYP          outObjectType;  /*!< Output Object Type LC/HE/HEv2*/
    AACENC_TRANSPORT_TYPE   outFileFormat;  /*!< Output File Format*/

} MPEG4AACENC_UALGParams;

typedef struct
{
    OMX_BOOL bLastBuffer;
}AACENC_UAlgInBufParamStruct;

typedef struct
{
    unsigned long unNumFramesEncoded;
    unsigned long unFrameSizes[MPEG4AACENC_MAX_OUTPUT_FRAMES];
}AACENC_UAlgOutBufParamStruct;
/*       ----       */



typedef struct OMX_AUDIOENC_AAC_PRIVATE_PARAMS
{
    OMX_AUDIO_PARAM_AACPROFILETYPE *aacParams;
    OMX_U16 File_Format;
    OMX_U16 framesPerOutBuf;
    OMX_U16 bitRateMode;
    MPEG4AACENC_UALGParams* ptAlgDynParams;
    AACENC_AudioCodecParams* pParams;
    AACENC_UAlgInBufParamStruct inBufParam[MAX_NUM_OF_BUFS];
    OMX_U16 posInBufParam;
    AACENC_UAlgOutBufParamStruct outBufParam[MAX_NUM_OF_BUFS];
    OMX_U16 posOutBufParam;
}OMX_AUDIOENC_AAC_PRIVATE_PARAMS;



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_GetExtensionIndex() - Returns index for vendor specific
 *                                             settings specific to AAC encoding.
 *                                             This function internally calls
 *                                             OMX_AUDIO_ENC_COMMON_GetExtensionIndex
 *
 * @ see OMX_core.h
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                                  OMX_STRING cParameterName,
                                                  OMX_INDEXTYPE* pIndexType);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_GetParameter() - This function gets the parameters
 *                                        specific to AAC encoding. This function
 *                                        internally calls the common audio
 *                                        GetParameter function.
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
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_GetParameter (OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE nParamIndex,
                                          OMX_PTR pComponentParameterStructure);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_SetParameter() - This function Sets the parameters
 *                                        specific to AAC encoding. This function
 *                                        internally calls the common audio
 *                                        SetParameter function.
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
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_SetParameter (OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE nParamIndex,
                                          OMX_PTR pComponentParameterStructure);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_SetConfig() - This function sets the Configurations
 *                                     specific to AAC encoding. This function
 *                                     internally calls the common audio
 *                                     SetConfig function.
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
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_SetConfig(OMX_HANDLETYPE hComponent,
                                          OMX_INDEXTYPE nConfigIndex,
                                          OMX_PTR pComponentConfigStructure);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_InitParams() - This function initializes certain
 *                                      parameters specific to AAC encodinig.
 *                                      It is called internally by SetParameter
 *                                      after the client sets the role to AAC.
 *
 *  @ calltype                         :Non - Blocking call.
 *
 *  @ param hComponent                 :Handle of the component.
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_InitParams(OMX_HANDLETYPE hComponent);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_FillLcmlInitParams() - This function fills initialization
 *                                              parameters, buffer deatils  etc
 *                                              to LCML structure. It is called
 *                                              internally when component moves
 *                                              from loaded to idle state.
 *
 *  @ calltype            :Non - Blocking call.
 *
 *  @ param hComponent    :Handle of the component.
 *
 *  @ param plcml_Init    :LCML structure that has to be filled by the component.
 *
 *  @ param arr           :Filled by the component and passed as a member of the
 *                         plcml_Init structure.
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_FillLCMLInitParams(OMX_HANDLETYPE hComponent,
                                         LCML_DSP *plcml_Init, OMX_U16 arr[]);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_SendParamsToCodecForTransitionToExecuting() -
 *                       This function sets certain codec parameters via
 *                       ControlCodec calls before the component transitions
 *                       to executinig state.
 *
 *  @ param hComponent    :Handle of the component.
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_SendParamsToCodecForTransitionToExecuting
              (OMX_HANDLETYPE hComponent);



/* ===========================================================================*/
/*
 * @fn OMX_AUDIO_ENC_AAC_FreeLCMLInitParams() - Frees up memory allocated for
 *                                              LCML structures.
 *
 *  @ param hComponent    :Handle of the component.
 */
/* ===========================================================================*/
OMX_ERRORTYPE OMX_AUDIO_ENC_AAC_FreeLCMLInitParams(OMX_HANDLETYPE pComponent);

#endif