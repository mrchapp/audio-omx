/* ============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* =========================================================================== */
/**
* @file OMX_AudioDecUtils.h
*
* This is an header file for an Audio Decoder that is fully
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
/* ------ compilation control switches ------------ */
#ifndef OMX_AUIDIODECUTILS_H
#define OMX_AUIDIODECUTILS_H

/*For Debugging*/
//#define AUDIODEC_DEBUG
#undef AUDIODEC_DEBUG

#ifdef  AUDIODEC_DEBUG
#define AUDIODEC_DPRINT(...)  fprintf(stdout, "%s %d::  ",__FUNCTION__, __LINE__); \
    fprintf(stdout, __VA_ARGS__);                                       \
    fprintf(stdout, "\n");
#elseif OMX_LOG_OVER_TTIF
#define AUDIODEC_DPRINT(...)  OMX_BASE_Trace(__VA_ARGS__);
#else
#define AUDIODEC_DPRINT(...)
#endif


/******************************************************************************
 * INCLUDE FILES
 *****************************************************************************/
/** ------------------------ system and platform files ---------------------**/

/**-------------------------program files ----------------------------------**/
#include <OMX_Component.h>
#include <omx_base.h>
#ifdef RESOURCE_MANAGER_ENABLED
#include <ResourceManagerProxyAPI.h>
#endif
#include "omx_audiodec_mp3utils.h"
#include "omx_audiodec_aacutils.h"

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/**----------------------------------data declarations ----------------------**/
/**----------------------------------function prototypes --------------------**/
/**-----------------------------------macros --------------------------------**/

/******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 *****************************************************************************/
/**------------------------------ macros -----------------------------------**/

/* Default start port number */
#define DEFAULT_START_PORT_NUM 0

/* default number of ports */
#define NUM_OF_PORTS 2

/*default i/p port index*/
#define INPUT_PORT 0

/*default o/p port index*/
#define OUTPUT_PORT 1

/*max number of buffers*/
#define MAX_NUM_OF_BUFS 10

/*default context type*/
#define DEFAULT_CONTEXT_TYPE OMX_BASE_CONTEXT_PASSIVE

/* defines the major version of the Component */
#define OMX_AUDIO_DEC_COMP_VERSION_MAJOR 1
/* defines the minor version of the Component */
#define OMX_AUDIO_DEC_COMP_VERSION_MINOR 1
/* defines the version Revision of the Component */
#define OMX_AUDIO_DEC_COMP_VERSION_REVISION 0
/* defines the version Step of the Component */
#define OMX_AUDIO_DEC_COMP_VERSION_STEP 0

/* Group Id of the Audio DEC Comp */
#define OMX_AUDIO_DEC_GROUPID 0
/*defines the stack size*/
#define OMX_AUDIO_DEC_TASK_STACKSIZE (2*1024)
/* Priority of the OMX AUDIO DEC Comp  */
#define OMX_AUDIO_DEC_TASK_PRIORITY 1

/*default num of i/p buffers*/
#define DEC_DEFAULT_INPUT_BUFFER_COUNT 1
/*default num of o/p buffers*/
#define DEC_DEFAULT_OUTPUT_BUFFER_COUNT 1
/*default i/p buffer size*/
#define DEC_DEFAULT_INPUT_BUFFER_SIZE 4096
/*default o/p buffer size*/
#define DEC_DEFAULT_OUTPUT_BUFFER_SIZE 8192
#define OMX_CODECSTOP 2

/* For Cache alignment*/
#define DSP_CACHE_ALIGNMENT 256
/* For Cache alignment*/
#define EXTRA_BYTES 128

 /* ==================================================================== */
 /*IUALG_Cmd_Audio_DEC:is passed as one of the parameter to control the codec
 */
 /* ==================================================================== */
typedef enum {
  IUALG_CMD_STOP 	  = 0,
  IUALG_CMD_PAUSE	  = 1,
  IUALG_CMD_GETSTATUS     = 2,
  IUALG_CMD_SETSTATUS     = 3,
}IUALG_Cmd_Audio_DEC;

 /* ==================================================================== */
 /*IAUDIO_PcmFormat:is passed as one of the parameter to control the codec
 */
 /* ==================================================================== */
typedef enum {
  EAUDIO_BLOCK =0, /*It is used in DASF mode*/
  EAUDIO_INTERLEAVED /*It specifies interleaved format of SN*/
}TAUDIO_PcmFormat;

/* ======================================================================= */
/*AUDIO_DEC_COMPONENT_ROLE:This enum is used to select the role specific functions
*/
/* ======================================================================= */

typedef enum AUDIO_DEC_COMPONENT_ROLE{
  AAC_DECODE,
  MP3_DECODE,
  UNDEFINED_ROLE
}AUDIO_DEC_COMPONENT_ROLE;

/* ======================================================================= */
/**UAlgInBufParamStruct: This struct is passed with input buffers that
 * are sent to DSP.
 */
/* ==================================================================== */
typedef struct {
  /* Set to 1 if buffer is last buffer */
  unsigned short bLastBuffer;
  /* Not used in new MP3 SN use only for AAC SN */
  unsigned short bConcealBuffer;
  /* New parameter added at MP3 SN */
  unsigned long ulNumInFrames;
}UAlgInBufParamStruct;
/* ======================================================================= */
/**UAlgOutBufParamStruct: This is passed with output buffer to DSP.
 */
/* ==================================================================== */
typedef struct {
  unsigned long ulFrameCount;
}UAlgOutBufParamStruct;

/* ==================================================================== */
/* USN_AudioCodecParams: This contains the information which need to send to Codec
	 on DSP */
/* ==================================================================== */
typedef struct USN_AudioCodecParams{
  /* Specifies the sample frequency */
  unsigned long ulSamplingFreq;
  /* Specifies the UUID */
  unsigned long unUUID;
  /* Specifies the audio format */
  unsigned short unAudioFormat;
}USN_AudioCodecParams;
/* ==================================================================== */
/*LCMLParams:struct contains all the parameters related to LCML
*/
/* ==================================================================== */
typedef struct LCMLParams{
  USN_AudioCodecParams *pParams;/*Codec parameters*/
  OMX_HANDLETYPE pLcmlHandle;/*LCML hanldle*/
  LCML_STRMATTR  *strmAttr;
  OMX_PTR pInBufPendingWithLcmlPipe;/*used to keep the i/p bufferheaders taht are sent to LCML for processing*/
  OMX_PTR pOutBufPendingWithLcmlPipe;/*used to keep the o/p bufferheaders taht are sent to LCML for processing*/
}LCMLParams;
/* ========================================================================= */
/* OMX_AUDIODEC_PIPE_TYPE - To differentiate among the two types of data pipes
 *                          used by this component. Currently this enumeration
 *                          is used only by the
 *                          OMX_AUDIO_DEC_ReturnPendingBuffers() function.
 */
/* ========================================================================= */
typedef enum OMX_AUDIODEC_PIPE_TYPE{
  DATA_PIPE,
  LCML_DATA_PIPE
}OMX_AUDIODEC_PIPE_TYPE;
/*===============================================================================*/
/**PRIVATE STRUCTRE VARIABLES
*/
/* ==================================================================== */
DERIVEDSTRUCT(AUDIODEC_COMPONENT_PRIVATE, OMX_BASE_PRIVATETYPE)
#define AUDIODEC_COMPONENT_PRIVATE_FIELDS OMX_BASE_PRIVATETYPE_FIELDS \
    /* This is component handle */ \
    OMX_COMPONENTTYPE *pHandle; \
    /*struct contains the ROLE information*/\
    OMX_PARAM_COMPONENTROLETYPE componentRole;\
    AUDIO_DEC_COMPONENT_ROLE eComponentRole; \
    /*to store the i/pbufferheadres that are sent to derived componnet for processing*/\
    OMX_PTR pInDataPipe; \
    /*to store the o/pbufferheadres that are sent to derived componnet for processing*/\
    OMX_PTR pOutDataPipe; \
    /*struct contains the INFO related to pcmparams */\
    OMX_AUDIO_PARAM_PCMMODETYPE *pcmParams; \
    /*struct contains the info related to the port formats*/\
    OMX_AUDIO_PARAM_PORTFORMATTYPE **pPortFormat;\
    /*handle for the RMPrxy call back*/\
    /*RMPROXY_CALLBACKTYPE rmproxyCallback;*/                           \
    /*pointers to the auxilary info of the buffer that need to be sent to the LCML*/\
    UAlgInBufParamStruct *pIpParam;\
    /*pointers to the auxilary info of the buffer that need to be sent to the LCML*/\
    UAlgOutBufParamStruct *pOpParam;\
    /*OMX_BOOL dasfmode;*/\
    /*flag to bypass DSP when an empty buffer is sent for processing*/  \
    OMX_BOOL bBypassDSP;                                                \
    /*flag to handle port disable and StateIdle commands*/              \
    OMX_U16 bNoIdleOnStop;\
    /*flag to indicate that EOF has been arrived at i/p*/\
    OMX_BOOL bIsEOFSent;\
    /*flag to indicate that Marked buffer has arrived at i/p*/\
    OMX_BOOL bMarkDataIn;\
    OMX_STRING* sDeviceString;\
    /*OMX_U32 streamID;*/ \
    /**Keep buffer timestamp*/\
    OMX_TICKS BufTimeStamp[MAX_NUM_OF_BUFS];  \
    /** Index to arrBufTimeStamp[], used for input buffer timestamps */\
    OMX_U8 nIpBufindex;\
    /* Index to arrBufTimeStamp[], used for output buffer timestamps  */\
    OMX_U8 nOpBufindex;\
    /*LCML parameters*/\
    LCMLParams* LCMLParams;\
    /*union containinig the Audiodec i/p parameters*/\
    union {\
        AACDECParams* aacdecParams;\
        MP3DECParams* mp3decParams;\
    }codec_params;
 ENDSTRUCT(AUDIODEC_COMPONENT_PRIVATE)


/*===============================================================================*/
/*FUNCTIONS
*/
/*===============================================================================*/

OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_GetParameter(OMX_HANDLETYPE hComponent,
                                              	OMX_INDEXTYPE nParamIndex,
                                              	OMX_PTR pComponentParameterStructure);

OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_SetParameter(OMX_HANDLETYPE hComponent,
                                              	OMX_INDEXTYPE nParamIndex,
                                              	OMX_PTR pComponentParameterStructure);

OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_SetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nConfigIndex,
                                             OMX_PTR pComponentConfigStructure);

OMX_ERRORTYPE OMX_AUDIO_DEC_COMMON_GetConfig(OMX_HANDLETYPE hComponent,
                                             OMX_INDEXTYPE nConfigIndex,
                                             OMX_PTR pComponentConfigStructure);

OMX_ERRORTYPE OMX_AUDIO_ENC_COMMON_GetExtensionIndex(OMX_HANDLETYPE hComponent,
                                                     OMX_STRING cParameterName,
                                                     OMX_INDEXTYPE* pIndexType);

OMX_ERRORTYPE _OMX_AUDIO_DEC_ReturnPendingBuffers(OMX_HANDLETYPE hComponent ,
                                                  OMX_AUDIODEC_PIPE_TYPE pipe, OMX_U32 Dir);
void OMX_AUDIO_DEC_StoreInputTimeStamp( AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate,
                                        OMX_BUFFERHEADERTYPE* pBufHeader );

void OMX_AUDIO_DEC_RetrieveOutputTimeStamp( AUDIODEC_COMPONENT_PRIVATE *pComponentPrivate,
                                            OMX_BUFFERHEADERTYPE* pBufHeader );
#endif
