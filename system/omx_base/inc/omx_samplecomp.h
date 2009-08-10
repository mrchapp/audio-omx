/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*   @file  omx_samplecomp.h
*   This file contains methods that provides the functionality for the sample component.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_sample
*
*   @rev  1.0
*
*/
/* -------------------------------------------------------------------------- */
/* ========================================================================= 
 *! Revision History
 *! ========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * ========================================================================= */

#ifndef _OMX_SAMPLE_COMPONENT_H
#define _OMX_SAMPLE_COMPONENT_H

#ifdef _cplusplus
extern "C"   {
#endif /* _cplusplus */

/* User code goes here */
/* ------compilation control switches ----------------------------------------*/
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/* ----- system and platform files ----------------------------*/ 
/*-------program files ----------------------------------------*/

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 
 ****************************************************************/
 
 /* defines the major version of the sample Component */
 #define OMX_SAMPLE_COMP_VERSION_MAJOR 1

 /*  defines the minor version of the Sample component  */
 #define OMX_SAMPLE_COMP_VERSION_MINOR 1

 /* defiens the revision of the Sample component */
 #define OMX_SAMPLE_COMP_VERSION_REVISION 0

 /* defines the step version of the Sample component */
 #define OMX_SAMPLE_COMP_VERSION_STEP 0
 
 /* Maximum Number of ports for the Sample Comp */
 #define  OMX_SAMPLE_NUM_PORTS     2

 /* Deafult portstartnumber of sample comp */
 #define  DEFAULT_START_PORT_NUM 0
 
 /* Input port Index for the Sample OMX Comp */
 #define OMX_SAMPLE_INPUT_PORT 0

 /* Output port Index for the Sample OMX Comp */
 #define OMX_SAMPLE_OUTPUT_PORT 1

 /* Default context Type as HOST CONTEXT(0) */
 #define DEFAULT_CONTEXT_TYPE OMX_BASE_CONTEXT_PASSIVE 

 /* Priority of the OMX Sample Comp  */
 #define OMX_SAMPLE_GROUPPRIORITY 245

 /* Stack Size of the OMX Sample Comp*/
 #define OMX_SAMPLE_STACKSIZE (1024)
 
 /* Group Id of the Sample Comp */
 #define OMX_SAMPLE_GROUPID 0

 /* default input buffer count */
 #define OMX_SAMPLE_DEFAULT_INPUT_BUFFER_COUNT 2

 /* deafult input buffer size */
 #define OMX_SAMPLE_DEFAULT_INPUT_BUFFER_SIZE 32

 /* default input buffer count */
 #define OMX_SAMPLE_DEFAULT_OUTPUT_BUFFER_COUNT 2

 /* deafult input buffer size */
 #define OMX_SAMPLE_DEFAULT_OUTPUT_BUFFER_SIZE 32
 
/*--------data declarations -----------------------------------*/
 
/*******************************************************************************
* Enumerated Types
*******************************************************************************/

/*******************************************************************************
* Strutures
*******************************************************************************/

/* =================================================================== */
/* OMX_SAMPLE_COMP_PVTTYPE - Structure for the private elements of the component 
 *         This structur includes all the private elements of the Base Component 
 *
 *   Define all derived Component Specifc fields over here 
 */
/* ==================================================================== */
DERIVEDSTRUCT(OMX_SAMPLE_PVTTYPE, OMX_BASE_PRIVATETYPE)
#define OMX_SAMPLE_PVTTYPE_FIELDS OMX_BASE_PRIVATETYPE_FIELDS \
    OMX_S32 privateData; 
ENDSTRUCT(OMX_SAMPLE_PVTTYPE)

/*--------macros ----------------------------------------------*/
/*--------function prototypes ---------------------------------*/

/* OMX Sample Component Init */
OMX_ERRORTYPE OMX_ComponentInit(OMX_HANDLETYPE hComponent);

/* Callback from Base to Derived to Notify Command */
OMX_ERRORTYPE OMX_TI_SAMPLE_CommandNotify(OMX_HANDLETYPE hComponent, 
                                          OMX_COMMANDTYPE Cmd,
                                          OMX_U32 nParam, 
                                          OMX_PTR pCmdData);

/* Callback from Base to Derived to Notify Data */
OMX_ERRORTYPE OMX_TI_SAMPLE_DataNotify(OMX_HANDLETYPE hComponent, 
                                              OMX_U32 nPortIndex,
                                              OMX_BUFFERHEADERTYPE* pBuffer);

/* Function to set the parameters of the Sample Component*/
OMX_ERRORTYPE OMX_TI_SAMPLE_SetParameter(OMX_HANDLETYPE hComponent,
                                         OMX_INDEXTYPE nParamIndex,
                                         OMX_PTR pParamStruct);

/* Function to get the parameters from the component */
OMX_ERRORTYPE OMX_TI_SAMPLE_GetParameter(OMX_HANDLETYPE hComponent,
                                         OMX_INDEXTYPE nParamIndex,
                                         OMX_PTR pParamStruct);

/* Function to set the Configurations of the Component */
OMX_ERRORTYPE OMX_TI_SAMPLE_SetConfig(OMX_HANDLETYPE hComponent,
                                      OMX_INDEXTYPE nIndex,
                                      OMX_PTR pConfigData);

/* Function to get the Configurations of the component */
OMX_ERRORTYPE OMX_TI_SAMPLE_GetConfig(OMX_HANDLETYPE hComponent,
                                      OMX_INDEXTYPE nIndex,
                                      OMX_PTR pConfigData);

/* Function to deinitalize the sample component */
OMX_ERRORTYPE OMX_TI_SAMPLE_ComponentDeinit(OMX_HANDLETYPE hComponent);

/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/

#ifdef _cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_SAMPLE_COMPONENT_H */


