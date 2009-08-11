/* =============================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ============================================================================ */
/**
 * @file omx_audiodec_mp3utils.h
 *
 * This is an header file for an MP3 Decoder that is fully
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
#ifndef OMX_AUDIODEC_MP3UTILS_H
#define OMX_AUDIODEC_MP3UTILS_H

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
 * @def    MP3DEC_USN_DLL_NAME   USN DLL name
 */
/* ======================================================================= */

#define MP3DEC_DLL_NAME "/lib/dsp/mp3dec_sn.dll64P"
/* ======================================================================= */
/**
 * @def    MP3DEC_DLL_NAME   MP3 Dec Decoder socket node DLL name
 */
/* ======================================================================= */

#define MP3DEC_USN_DLL_NAME "/lib/dsp/usn.dll64P"
/* ======================================================================= */
/**
 * @def    OMX_MP3DEC_DEFAULT_SEGMENT    Default segment ID for the LCML
 */
/* ======================================================================= */

#define OMX_MP3DEC_DEFAULT_SEGMENT (0)
/* ======================================================================= */
/**
 * @def    OMX_MP3DEC_SN_TIMEOUT    Timeout value for the socket node
 */
/* ======================================================================= */

#define OMX_MP3DEC_SN_TIMEOUT (-1)
/* ======================================================================= */
/**
 * @def    OMX_MP3DEC_SN_PRIORITY   Priority for the socket node
 */
/* ======================================================================= */

#define OMX_MP3DEC_SN_PRIORITY (10)

/* ======================================================================= */
/**
 * @def    STREAM_COUNT_MP3DEC   Number of streams
 */
/* ======================================================================= */
#define STREAM_COUNT_MP3DEC 2
/* ======================================================================= */
/**
 * @def    MP3D_NUM_INPUT_BUFFERS   Default number of input buffers*
 */
/* ======================================================================= */

#define MP3D_NUM_INPUT_BUFFERS 1
/* ======================================================================= */
/**
 * @def    MP3D_INPUT_BUFFER_SIZE   Default input buffer size
 */
/* ======================================================================= */

#define MP3D_INPUT_BUFFER_SIZE 4*1024 //4096
/* ======================================================================= */
/**
 * @def    MP3DEC_CPU_USAGE for Resource Mannager (MHZ)
 */
/* ======================================================================= */

#define MP3DEC_CPU_USAGE 50

/* Stream types supported*/
#define MONO_STREAM_MP3DEC                   1
#define STEREO_INTERLEAVED_STREAM_MP3DEC     2
#define STEREO_NONINTERLEAVED_STREAM_MP3DEC  3

/*extended custom specific index*/
typedef enum OMX_INDEXAUDIOTYPE_MP3DEC {
    OMX_IndexCustomMp3DecHeaderInfoConfig = 0xFF000001,
    OMX_IndexCustomMp3DecStreamInfoConfig,
    OMX_IndexCustomMp3DecDataPath
}OMX_INDEXAUDIOTYPE_MP3DEC;


/*IAUDIO_PcmFormat: This value is used by DSP.*/
#if 0
typedef enum {
    EAUDIO_BLOCK =0, /*It is used in DASF mode*/
    EAUDIO_INTERLEAVED /*It specifies interleaved format of SN*/
}TAUDIO_Mp3Format;
#endif
/* ==================================================================== */
/*MP3DEC_UALGParams:contains the parameters that are necessary to send to the SN
 */
/* ==================================================================== */
typedef struct {
    OMX_U32    size;
    long       lOutputFormat; /*To set interleaved/Block format*/
  //long       DownSampleSbr;
  //long       iEnablePS;
    long       lSamplingRateIdx;
    long       nProfile;
    long       bRawFormat;
} MP3DEC_UALGParams;

/* ==================================================================== */
/*MP3DECParams:contains all the parameters that are specific to MP3
 */
/* ==================================================================== */
typedef struct MP3DECParams{
    OMX_BOOL Framemode;/*flag to indiacate either frame mode or non frame mode*/
    OMX_U32 nProfile; /*indicates the profile*/
    OMX_U32 parameteric_stereo;/*flag for Parametric_stereo*/
    OMX_U32 SBR;/*flag for SBR*/
    OMX_U32 nOpBit;/*to set the bit rate*/
    OMX_BOOL SendAfterEOS;/*flag used to initialize the SN after the EOS has been sent*/
    MP3DEC_UALGParams *MP3DEC_UALGParam;/*parameters necessary to the SN*/
    OMX_AUDIO_PARAM_MP3TYPE* mp3Params;/*MP3 specific parameters*/
}MP3DECParams;

/*Function ProtoTypes*/

OMX_ERRORTYPE OMX_MP3_DEC_GetParameter (OMX_HANDLETYPE hComponent,
                                        OMX_INDEXTYPE nParamIndex,
                                        OMX_PTR pComponentParameterStructure);

OMX_ERRORTYPE OMX_MP3_DEC_SetParameter (OMX_HANDLETYPE hComponent,
                                        OMX_INDEXTYPE nParamIndex,
                                        OMX_PTR pComponentParameterStructure);

OMX_ERRORTYPE OMX_MP3_DEC_SetConfig (OMX_HANDLETYPE hComponent,
                                     OMX_INDEXTYPE nConfigIndex,
                                     OMX_PTR pComponentConfigStructure);

OMX_ERRORTYPE OMX_MP3_DEC_GetConfig (OMX_HANDLETYPE hComponent,
                                     OMX_INDEXTYPE nConfigIndex,
                                     OMX_PTR pComponentConfigStructure);

OMX_ERRORTYPE OMX_MP3_DEC_GetExtensionIndex(OMX_IN  OMX_HANDLETYPE hComponent,
                                            OMX_IN  OMX_STRING cParameterName,
                                            OMX_OUT OMX_INDEXTYPE* pIndexType);

OMX_ERRORTYPE OMX_MP3_DEC_InitParams(OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_MP3_DEC_Fill_LCMLInitParams(OMX_HANDLETYPE pComponent,
                                              LCML_DSP *plcml_Init, OMX_U16 arr[]);

OMX_ERRORTYPE OMX_MP3_DEC_SN_InitParams(OMX_HANDLETYPE hComponent);

OMX_ERRORTYPE OMX_MP3DEC_DeInit(OMX_HANDLETYPE hComponent);

int _OMX_MP3DEC_GetSampleRateIndexL( const int aRate);

#endif
