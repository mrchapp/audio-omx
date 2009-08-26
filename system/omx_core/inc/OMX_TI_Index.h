/*=========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright Texas Instruments, Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied.
 * ==========================================================================*/
/**
 *  @file  omx_ti_index.h
 *         This file contains the vendor(TI) specific indexes
 *
 *  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_core\inc
 *
 *  @rev 1.0
 */

/*==============================================================
 *! Revision History
 *! ============================
 *! 20-Dec-2008 x0052661@ti.com, initial version
 *================================================================*/

#ifndef _OMX_TI_INDEX_H_
#define _OMX_TI_INDEX_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************
 *   INCLUDE FILES
 ******************************************************************/
#include <omx_types.h>

/*******************************************************************
 * EXTERNAL REFERENCE NOTE: only use if not found in header file
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */
/*******************************************************************
 * PUBLIC DECLARATIONS: defined here, used elsewhere
 *******************************************************************/
/*----------         function prototypes      ------------------- */
/*----------         data declarations        ------------------- */

typedef enum OMX_TI_INDEXTYPE {

    OMX_IndexConfigAutoPauseAfterCapture = OMX_IndexAutoPauseAfterCapture,

    /* Vendor specific area for storing indices */
    OMX_TI_IndexConfigChannelName = ((OMX_INDEXTYPE)OMX_IndexVendorStartUnused + 1), /**< reference: OMX_CONFIG_CHANNELNAME */

    OMX_TI_IndexParamJPEGUncompressedMode,      /**< reference: OMX_JPEG_PARAM_UNCOMPRESSEDMODETYPE */
    OMX_TI_IndexParamJPEGCompressedMode,        /**< reference: OMX_JPEG_PARAM_COMPRESSEDMODETYPE */

    /* H264 Encoder Indices*/
    OMX_TI_IndexParamVideoDataMode,             /**< reference: OMX_TI_VIDEO_PARAM_DATAMODE */
    OMX_TI_IndexParamVideoAspectRatio,          /**< reference: OMX_TI_VIDEO_PARAM_ASPECT_RATIO */
    OMX_TI_IndexParamBitStreamControl,          /**< reference: OMX_TI_VIDEO_PARAM_BITSTREAM_CONTROL */
    OMX_TI_IndexConfigInterCoding,              /**< reference: OMX_TI_VIDEO_CONFIG_INTER_CODING */
    OMX_TI_IndexParamPOCType,                   /**< reference: OMX_TI_VIDEO_PARAM_POCTYPE */

    /* Camera Indices */
    OMX_IndexParamSensorSelect,                 /**< reference: OMX_CONFIG_SENSORSELECTTYPE */
    OMX_IndexConfigFlickerCancel,               /**< reference: OMX_CONFIG_FLICKERCANCELTYPE */
    OMX_IndexConfigSensorCal,                   /**< reference: OMX_CONFIG_SENSORCALTYPE */
    OMX_IndexParamSceneMode,                    /**< reference: OMX_CONFIG_SCENEMODETYPE */

    OMX_IndexConfigDigitalZoomSpeed,            /**< reference: OMX_CONFIG_DIGITALZOOMSPEEDTYPE */
    OMX_IndexConfigDigitalZoomTarget,           /**< reference: OMX_CONFIG_DIGITALZOOMTARGETTYPE */

    OMX_IndexConfigCommonScaleQuality,          /**< reference: OMX_CONFIG_SCALEQUALITYTYPE */

    OMX_IndexConfigCommonDigitalZoomQuality,    /**< reference: OMX_CONFIG_SCALEQUALITYTYPE */

    OMX_IndexConfigOpticalZoomSpeed,            /**< reference: OMX_CONFIG_DIGITALZOOMSPEEDTYPE */
    OMX_IndexConfigOpticalZoomTarget,           /**< reference: OMX_CONFIG_DIGITALZOOMTARGETTYPE */

    OMX_IndexConfigSmoothZoom,                  /**< reference: OMX_CONFIG_SMOOTHZOOMTYPE */

    OMX_IndexConfigBlemish,                     /**< reference: OMX_CONFIG_BLEMISHTYPE */

    OMX_IndexConfigExtCaptureMode,              /**< reference: OMX_CONFIG_EXTCAPTUREMODETYPE */

    OMX_IndexConfigDigitalFlash,                /**< reference: OMX_CONFIG_BOOLEANTYPE */
    OMX_IndexConfigPrivacyIndicator,            /**< reference: OMX_CONFIG_BOOLEANTYPE */

    OMX_IndexConfigTorchMode,                   /**< reference: OMX_CONFIG_TORCHMODETYPE */

    OMX_IndexConfigSlowSync,                    /**< reference: OMX_CONFIG_BOOLEANTYPE */

    OMX_IndexConfigFocusAssist,                 /**< reference: OMX_CONFIG_BOOLEANTYPE */

    OMX_IndexConfigImageFocusLock,              /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */
    OMX_IndexConfigImageWhiteBalanceLock,       /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */
    OMX_IndexConfigImageExposureLock,           /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */
    OMX_IndexConfigImageAllLock,                /**< reference: OMX_IMAGE_CONFIG_LOCKTYPE */

    OMX_IndexConfigImageDeNoiseLevel,           /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
    OMX_IndexConfigSharpeningLevel,             /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
    OMX_IndexConfigDeBlurringLevel,             /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
    OMX_IndexConfigChromaCorrection,            /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */
    OMX_IndexConfigDeMosaicingLevel,            /**< reference: OMX_IMAGE_CONFIG_PROCESSINGLEVELTYPE */

    OMX_IndexConfigCommonWhiteBalanceGain,      /**< reference: OMX_CONFIG_WHITEBALGAINTYPE */

    OMX_IndexConfigCommonRGB2RGB,               /**< reference: OMX_CONFIG_COLORCONVERSIONTYPE_II */
    OMX_IndexConfigCommonRGB2YUV,               /**< reference: OMX_CONFIG_COLORCONVERSIONTYPE_II */

    OMX_IndexConfigCommonGammaTable,            /**< reference: OMX_CONFIG_GAMMATABLETYPE */

    OMX_IndexConfigImageFaceDetection,          /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
    OMX_IndexConfigImageBarcodeDetection,       /**< reference: OMX_CONFIG_EXTRADATATYPE */
    OMX_IndexConfigImageSmileDetection,         /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
    OMX_IndexConfigImageBlinkDetection,         /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
    OMX_IndexConfigImageFrontObjectDetection,   /**< reference: OMX_CONFIG_EXTRADATATYPE */
    OMX_IndexConfigHistogramMeasurement,        /**< reference: OMX_CONFIG_HISTOGRAMTYPE */
    OMX_IndexConfigDistanceMeasurement,         /**< reference: OMX_CONFIG_EXTRADATATYPE */

    OMX_IndexConfigSnapshotToPreview,           /**< reference: OMX_CONFIG_BOOLEANTYPE */

    OMX_IndexParamEXIF,                         /**< reference: OMX_CONFIG_EXTRADATATYPE */
    OMX_IndexParamJpegMaxSize,                  /**< reference: OMX_IMAGE_JPEGMAXSIZE */

    OMX_IndexConfigRestartMarker,               /**< reference: OMX_CONFIG_BOOLEANTYPE */

    OMX_IndexParamImageStampOverlay,            /**< reference: OMX_PARAM_IMAGESTAMPOVERLAYTYPE */
    OMX_IndexParamThumbnail,                    /**< reference: OMX_PARAM_THUMBNAILTYPE */
    OMX_IndexConfigImageStabilization,          /**< reference: OMX_CONFIG_BOOLEANTYPE */
    OMX_IndexConfigRedEyeRemoval,               /**< reference: OMX_CONFIG_REDEYEREMOVALTYPE */
    OMX_IndexParamHighISONoiseFiler,            /**< reference: OMX_CONFIG_BOOLEANTYPE */
    OMX_IndexParamLensDistortionCorrection,     /**< reference: OMX_CONFIG_BOOLEANTYPE */
    OMX_IndexParamLocalBrightnessAndContrast,   /**< reference: OMX_CONFIG_BOOLEANTYPE */
    OMX_IndexParamVideoCaptureYUVRange,         /**< reference: OMX_PARAM_VIDEOYUVRANGETYPE */

    OMX_IndexConfigCommonYUV2RGB,               /**< reference: TODO: define type */
    OMX_IndexConfigExtCapturing,                /**< reference: OMX_CONFIG_EXTCAPTURING */
    OMX_IndexConfigFocusRegion,                 /**< reference: OMX_CONFIG_EXTFOCUSREGIONTYPE */
    OMX_IndexConfigImageMotionEstimation,       /**< reference: OMX_CONFIG_OBJDETECTIONTYPE */
    OMX_IndexParamProcessingOrder,              /**< reference: OMX_CONFIGPROCESSINGORDERTYPE */
    OMX_IndexParamFrameStabilisation,           /**< reference: OMX_CONFIG_BOOLEANTYPE */
    OMX_IndexParamVideoNoiseFilter              /**< reference: OMX_PARAM_VIDEONOISEFILTERTYPE */
} OMX_TI_INDEXTYPE;



typedef enum OMX_TI_ERRORTYPE
{
    /* Vendor specific area for storing indices */
    
    /*Control attribute is pending - Dio_Dequeue will not work until attribute 
    is retreived*/
    OMX_TI_WarningAttributePending = (OMX_S32)((OMX_ERRORTYPE)OMX_ErrorVendorStartUnused + 1),
    /*Attribute buffer size is insufficient - reallocate the attribute buffer*/
    OMX_TI_WarningInsufficientAttributeSize,
    /*EOS buffer has been received*/
    OMX_TI_WarningEosReceived
}OMX_TI_ERRORTYPE;

/*******************************************************************
 * PRIVATE DECLARATIONS: defined here, used only here
 *******************************************************************/
/*----------          data declarations        ------------------- */
/*----------          function prototypes      ------------------- */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_TI_INDEX_H_ */

