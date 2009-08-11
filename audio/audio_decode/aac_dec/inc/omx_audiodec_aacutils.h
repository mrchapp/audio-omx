/* =============================================================================
*             Texas Instruments OMAP(TM) Platform Software
*  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
*
*  Use of this software is controlled by the terms and conditions found
*  in the license agreement under which this software has been supplied.
* ============================================================================ */
/**
* @file omx_omxauiodec_aacutils.h
*
* This is an header file for an AAC Decoder that is fully
* compliant with the OMX Audio specification 1.5.
* This the file that the application that uses OMX would include
* in its code.
*
* @path 
*
* @rev 
*/
/* --------------------------------------------------------------------------- */
/* --------------------------------------------------------------------------- */
/*-----------  User code goes here ------------------*/
/* ------ compilation control switches ------------ */
#ifndef OMX_AUDIODEC_AACUTILS_H
#define OMX_AUDIODEC_AACUTILS_H

/******************************************************************************
 * INCLUDE FILES
 *****************************************************************************/
/** ------------------------ system and platform files ---------------------**/

/**-------------------------program files ----------------------------------**/
#include <OMX_Component.h>
#include "LCML_DspCodec.h"

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

/* ======================================================================= */
/**
 * @def    AACDEC_USN_DLL_NAME   USN DLL name
 */
/* ======================================================================= */

#define AACDEC_DLL_NAME "/lib/dsp/mpeg4aacdec_sn.dll64P"
/* ======================================================================= */
/**
 * @def    AACDEC_DLL_NAME   AAC Dec Decoder socket node DLL name
 */
/* ======================================================================= */

#define AACDEC_USN_DLL_NAME "/lib/dsp/usn.dll64P"
/* ======================================================================= */
/**
 * @def    OMX_AACDEC_DEFAULT_SEGMENT    Default segment ID for the LCML
 */
/* ======================================================================= */

#define OMX_AACDEC_DEFAULT_SEGMENT (0)
/* ======================================================================= */
/**
 * @def    OMX_AACDEC_SN_TIMEOUT    Timeout value for the socket node
 */
/* ======================================================================= */

#define OMX_AACDEC_SN_TIMEOUT (-1)
/* ======================================================================= */
/**
 * @def    OMX_AACDEC_SN_PRIORITY   Priority for the socket node
 */
/* ======================================================================= */

#define OMX_AACDEC_SN_PRIORITY (10)

/* ======================================================================= */
/**
 * @def    STREAM_COUNT_AACDEC   Number of streams
 */
/* ======================================================================= */
#define STREAM_COUNT_AACDEC 2
/* ======================================================================= */
/**
 * @def    AACD_NUM_INPUT_BUFFERS   Default number of input buffers*
 */
/* ======================================================================= */

#define AACD_NUM_INPUT_BUFFERS 1
/* ======================================================================= */
/**
 * @def    AACD_INPUT_BUFFER_SIZE   Default input buffer size
 */
/* ======================================================================= */

#define AACD_INPUT_BUFFER_SIZE 4096
/* ======================================================================= */
/**
 * @def    AACDEC_CPU_USAGE for Resource Mannager (MHZ)
 */
/* ======================================================================= */

#define AACDEC_CPU_USAGE 50 

#define PARAMETRIC_STEREO_AACDEC 1
  
#define AACDEC_SBR_CONTENT 0x601 /*flag detection*/ 
  
#define AACDEC_PS_CONTENT 0x602 /*flag detection*/

 
 /* Stream types supported*/
#define MONO_STREAM_AACDEC                   1
#define STEREO_INTERLEAVED_STREAM_AACDEC     2
#define STEREO_NONINTERLEAVED_STREAM_AACDEC  3

/*extended custom specific index*/
 typedef enum OMX_INDEXAUDIOTYPE_AACDEC {
	 OMX_IndexCustomAacDecHeaderInfoConfig = 0xFF000001,
	 OMX_IndexCustomAacDecStreamIDConfig,
	 OMX_IndexCustomAacDecDataPath
 }OMX_INDEXAUDIOTYPE_AACDEC;


 /*IAUDIO_PcmFormat: This value is used by DSP.*/
#if 0
 typedef enum {
	 EAUDIO_BLOCK =0, /*It is used in DASF mode*/
	 EAUDIO_INTERLEAVED /*It specifies interleaved format of SN*/
 }TAUDIO_AacFormat;
#endif
 /* ==================================================================== */
 /*IUALG_Cmd_AAC_DEC:is passed as one of the parameter to control the codec
 */
 /* ==================================================================== */
#if 0 /* changed to omx_audiodecutil.h */
typedef enum {
	 IUALG_CMD_STOP 	  = 0,
	 IUALG_CMD_PAUSE	  = 1,
	 IUALG_CMD_GETSTATUS      = 2,
	 IUALG_CMD_SETSTATUS      = 3,
	 IUALG_CMD_USERCMDSTART_AACDEC	 = 100
 }IUALG_Cmd_AAC_DEC;
 #endif
 /* ==================================================================== */
/*MPEG4AACDEC_UALGParams:contains the parameters that are necessary to send to the SN
*/
/* ==================================================================== */
typedef struct {
	OMX_U32    size;
	long	   lOutputFormat; /*To set interleaved/Block format*/
	long	   DownSampleSbr;
	long	   iEnablePS;
	long	   lSamplingRateIdx;
	long	   nProfile;
	long	   bRawFormat;
} MPEG4AACDEC_UALGParams;

/* ==================================================================== */
/*AACDECParams:contains all the parameters that are specific to AAC
*/
/* ==================================================================== */
typedef struct AACDECParams{
    OMX_BOOL Framemode;/*flag to indiacate either frame mode or non frame mode*/
    OMX_U32 nProfile; /*indicates the profile*/
    OMX_U32 parameteric_stereo;/*flag for Parametric_stereo*/
    OMX_U32 SBR;/*flag for SBR*/
    OMX_U32 nOpBit;/*to set the bit rate*/
    OMX_BOOL SendAfterEOS;/*flag used to initialize the SN after the EOS has been sent*/
    MPEG4AACDEC_UALGParams *AACDEC_UALGParam;/*parameters necessary to the SN*/
    OMX_AUDIO_PARAM_AACPROFILETYPE* aacParams;/*AAC specific parameters*/
}AACDECParams;
 
/*Function ProtoTypes*/

OMX_ERRORTYPE OMX_AAC_DEC_GetParameter (OMX_HANDLETYPE hComponent,
                                       OMX_INDEXTYPE nParamIndex,
                                       OMX_PTR pComponentParameterStructure);

OMX_ERRORTYPE OMX_AAC_DEC_SetParameter (OMX_HANDLETYPE hComponent,
                                       OMX_INDEXTYPE nParamIndex,
                                       OMX_PTR pComponentParameterStructure);

OMX_ERRORTYPE OMX_AAC_DEC_SetConfig (OMX_HANDLETYPE hComponent,
                                OMX_INDEXTYPE nConfigIndex,
                                OMX_PTR pComponentConfigStructure);

OMX_ERRORTYPE OMX_AAC_DEC_GetConfig (OMX_HANDLETYPE hComponent,
                                OMX_INDEXTYPE nConfigIndex,
                                OMX_PTR pComponentConfigStructure);

OMX_ERRORTYPE OMX_AAC_DEC_GetExtensionIndex(OMX_IN  OMX_HANDLETYPE hComponent,
            									OMX_IN  OMX_STRING cParameterName,
            									OMX_OUT OMX_INDEXTYPE* pIndexType);

OMX_ERRORTYPE OMX_AAC_DEC_InitParams(OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_AAC_DEC_Fill_LCMLInitParams(OMX_HANDLETYPE pComponent,
                                  LCML_DSP *plcml_Init, OMX_U16 arr[]);
                                  
OMX_ERRORTYPE OMX_AAC_DEC_SN_InitParams(OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_AACDEC_DeInit(OMX_HANDLETYPE hComponent);

int _OMX_AACDEC_GetSampleRateIndexL( const int aRate);

#endif
