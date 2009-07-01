/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*  @file omx_base.h
*  This file omx_base header file contains the data and function prototypes
*  for the OMX1.1 Base Component which basically defines the interface
*  to be used by the Derived compnent. This Header file needs to be included
*  by all the OMXDerived Components which uses the Base Component.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_base\inc\
*
*  @rev  1.0
*/
/* -------------------------------------------------------------------------- */
/**============================================================================
 *! Revision History
 *! ===========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * =========================================================================**/

/*-----------  User code goes here ------------------*/
/* ------ compilation control switches ------------ */
#ifndef _OMX_BASE_H_
#define _OMX_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/******************************************************************************
 * INCLUDE FILES
 *****************************************************************************/
/** ------------------------ system and platform files ---------------------**/
/**-------------------------program files ----------------------------------**/

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/**----------------------------------data declarations ----------------------**/
/**----------------------------------function prototypes --------------------**/
/**-----------------------------------macros --------------------------------**/

/******************************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 *****************************************************************************/
 #define OMX_NOPORT 0xFFFFFFFE

 #define OMX_BASE_CMDPIPE_ELEMENTS 4

/**------------------------------ macros -----------------------------------**/
/**=========================================================================**/
/**
 * @def Used by the Derived Components to extend the
 *           Base Component Private Structure
 *
 *  Usage Rules:
 *
 *  1)  Include this File
 *  2)  If you don't extend(inherit) start your structure with STRUCT
 *  3)  If you want to Extend(inherit) few elements, start your Structure
 *      with DERIVEDSTRUCT (structure_name , inherited _structure_name)
 *  4)  End your Structure with ENDSTRUCT(structure_name)
 *  5)  Define your Structure variables with a #define structure_name_FIELDS
 *      inherited_structure_name_FIELDS  inside your structure and
 *      always add a backslash at the end of the file (except last)
 */
/**=========================================================================**/
#define STRUCT(a) typedef struct a a; \
 struct a {
#define DERIVEDSTRUCT(a, b) typedef struct a a; \
 struct a {
#define ENDSTRUCT(a) a##_FIELDS };

/**--------------------------data declarations -----------------------------**/
/******************************************************************************
* Data Types
******************************************************************************/

/***************************************************************************
* Enumerated Types
****************************************************************************/

/**=========================================================================**/
/**OMX_BASE_CONTEXT_TYPE - Context Type Enumeration
 *
 * @param OMX_BASE_CONTEXT_PASSIVE   : Passive/Application/Host   context
 *
 * @param OMX_BASE_CONTEXT_ACTIVE    : ACTIVE/Remote/Seperate  Context
 */
/**=========================================================================**/
typedef enum OMX_BASE_CONTEXT_TYPE {
    OMX_BASE_CONTEXT_PASSIVE = 0,
    OMX_BASE_CONTEXT_ACTIVE
}OMX_BASE_CONTEXT_TYPE;

/******************************************************************************
* Strutures
******************************************************************************/

/**=========================================================================**/
/** OMX_BASE_PARAMSFROMDERIVED -
 *  Structure contains the information which needs to be
 *  filled and passed from the Derived Component to Base Component.
 *
 * @param cComponentName :  Ptrto NULL terminated string with the Component Name.
 *
 * @param ctxType        :  Context type (Active/Passive) which the
 *                          component must be initialized.
 *
 * @param tComVersion    :  Specific version of an Component.
 *
 * @param unStackSize    :  Unsigned Stack Size, to create a task
 *
 * @param unGroupPrioirty:  Component Prioirty which is common
 *                          across a  group of components.
 *
 * @param unGroupID      :  ID of a group of components that share
 *                          the same logical chain
 *
 * @param tPortParams    :  Params of an port that contains short
 *                          description of Avaialable no of ports.
 *
 * @param pTunPref       :  Array of tunnel preference pointers
 *
 * @param fpCommandNotify:  function pointer for Base to Notify
 *                          Commands to the Derived component.
 *
 * @param fpDataNotify   :  function pointer for Base to Notify
 *                          Data(i/p or o/p buffers)to the Derived component.
 */
/**=========================================================================**/
typedef struct OMX_BASE_PARAMSFROMDERIVED{
   OMX_STRING cComponentName;
   OMX_BASE_CONTEXT_TYPE ctxType;
   OMX_VERSIONTYPE tComVersion;
   OMX_U32 unStackSize;
   OMX_U32 unGroupPrioirty;
   OMX_U32 unGroupID;
   OMX_PORT_PARAM_TYPE tPortParams;
   OMX_TUNNELSETUPTYPE **pTunPref;
   OMX_ERRORTYPE (*fpCommandNotify)(OMX_HANDLETYPE hComponent,
                                    OMX_COMMANDTYPE Cmd,
                                    OMX_U32 nParam, OMX_PTR pCmdData);
   OMX_ERRORTYPE (*fpDataNotify)(OMX_HANDLETYPE hComponent,
                                 OMX_U32 nPortIndex,
                                 OMX_BUFFERHEADERTYPE* pBuffer);
}OMX_BASE_PARAMSFROMDERIVED;

/**=========================================================================**/
/** OMX_BASE_PVTTYPE
 *            Structure contain all the Private Elements of the Base Component
 *            This strucutre can be extended by derived component's in their
 *            derived component private structure.
 *
 * @param hOMXBaseInt       : Handle for the Base Component's Internal elements
 *
 * @param pMark             : Pointerto MarkType holds the private data
 *                            associated with a mark request
 *
 * @param tDerToBase        : parameters as input from derived to base component
 *
 * @param tCurState         : Current State of the Component
 *
 * @param tNewState         : New State of the Component
 *
 * @param pMutex            : pointer to an Mutex to protect resources
 *
 * @param fpReturnDataNotify: function pointer to return data notification from
                              Derived to Base Component.
 *
 * @param fpReturnEventNotify:function pointer to return event notification
 *                            from derived to Base Component.
 */
/**========================================================================**/
STRUCT(OMX_BASE_PRIVATETYPE)
 #define OMX_BASE_PRIVATETYPE_FIELDS                   \
 OMX_PTR hOMXBaseInt;                                  \
 OMX_MARKTYPE *pMark;                                  \
 OMX_BASE_PARAMSFROMDERIVED tDerToBase;                \
 OMX_PARAM_PORTDEFINITIONTYPE **portdefs;              \
 OMX_STATETYPE tCurState;                              \
 OMX_STATETYPE tNewState;                              \
 OMX_PTR pMutex;                                       \
 OMX_ERRORTYPE (*fpReturnDataNotify)(OMX_HANDLETYPE hComponent,  \
            OMX_S32 nPortIndex, OMX_BUFFERHEADERTYPE* pBuffer);  \
 OMX_ERRORTYPE (*fpReturnEventNotify)(OMX_HANDLETYPE hComponent, \
                       OMX_EVENTTYPE eEvent, OMX_U32 EventData1, \
                       OMX_U32 EventData2, OMX_PTR pEventData);
ENDSTRUCT(OMX_BASE_PRIVATETYPE)

/**---------------------------------  macros --------------------------------**/
/**--------------------------------- function prototypes-----------------**/

/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentInit()
 *     This method is the first call into a Base component
 *     and is used to  perform one time initialization specific to a component
 *     called by the Derived comp's init function. This method MUST fill in
 *     all the function pointers into the handle.This component should allocate,
 *     fill in the Internal data structure and Initialize the number of ports
 *     given by the Derived component.
 *
 *  @ param hComponent   :     Handle of the Component to be Accessed.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentInit(OMX_IN OMX_HANDLETYPE hComponent);

/**=========================================================================**/
/**
 * @fn OMX_BASE_SetCallbacks()
 *     SetCallbacks method will allow the core to
 *     transfer the callback structure from the IL Client to the Component.
 *
 *  @ calltype         :  Blocking call
 *
 *  @ param hComponent :  Handle of the component that executes the call
 *
 *  @ param pCallbacks :  pointer to an OMX_CALLBACKTYPE structure that is
 *                        used to provide the callback information to the Component.
 *
 *  @ param pAppData   :  A pointer to a value that the IL client has defined
 *                        that allows the callback in the IL client to
 *                        determine the context of the call
 *
 *  @ PreRequisites    :  Component Shall be in OMX_StateLoaded State.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_SetCallbacks(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_CALLBACKTYPE* pCallbacks,
                OMX_IN  OMX_PTR pAppData);

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetComponentVersion()
 *     This method is used to query the component and returns info about it.
 *
 *  @ calltype                :  Blocking call
 *
 *  @ param hComponent        :  handle of the component
 *
 *  @ param cComponentName    :  Pointer to a component name string
 *
 *  @ Param pComponentVersion :  Pointer to an OpenMax version structure that
 *                               indicates the comp version.
 *
 *  @ Param pSpecVersion      :  pointer to an OMX Specification Version Information
 *
 *  @ Param pComponentUUID    :  Pointer to an Component UUID
 *
 *  @ PreRequisites         :  None
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetComponentVersion(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_OUT OMX_STRING pComponentName,
                OMX_OUT OMX_VERSIONTYPE* pComponentVersion,
                OMX_OUT OMX_VERSIONTYPE* pSpecVersion,
                OMX_OUT OMX_UUIDTYPE* pComponentUUID)  ;

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetState()
 *     This method invokes the component to get the current status of the component
 *
 * @ calltype          :  Blocking call
 *
 *  @ param hComponent :  Handle of the component that executes the call
 *
 *  @ Param pState     :  A pointer to the location that receives the state. The value
 *                        returned is one of the OMX_STATETYPE members
 *
 *  @ PreRequisites    :  None
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetState(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_OUT OMX_STATETYPE* pState);

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetParameter()
 *     This method will get one of the current parameter
 *     settings from the component. nParamIndex specifies which strucutre is
 *     requested from the component. Caller shall provide memory for the
 *     strcutre  and populate nSize and nVersion fileds before invoking this macro.
 *     List of supported indices are :
 *
 *  @ param OMX_IndexParamAudioInit    :   Retrieve number of Audio ports in a comp
 *  @ param OMX_IndexParamVideoInit    :   Retrieve number of video ports in a comp
 *  @ param OMX_IndexParamImageInit    :   Retrieve number of image ports in a comp
 *  @ param OMX_IndexParamOtherInit    :   Retrieve number of other ports in a comp
 *
 *  @ param OMX_IndexParamPortDefinition    :   Retrieve port settings
 *
 *  @ param OMX_IndexParamPriorityMgmt      : Get priority of the Comp group
 *
 *  @ param OMX_IndexParamCompBufferSupplier:   Get buffer supplier settings
 *
 *  @ calltype           :  Blocking call
 *
 *  @ param hComponent   :  Handle of the component that executes the call
 *
 *  @ param nParamIndex  :  The index of the structure to be filled.
 *                          This value is from the OMX_INDEXTYPE enumeration.
 *
 *  @ param pParamStruct :  ptr to the IL client-allocated structure that the comp fills
 *
 *  @ PreRequisites      :  Can be called in any state except the OMX_StateInvalid State.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetParameter(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_IN OMX_INDEXTYPE nParamIndex,
                OMX_INOUT OMX_PTR pParamStruct);

/**=========================================================================**/
/**
 * @fn OMX_BASE_SetParameter()
 *     This method will send a parameter structure
*      to a component.nParamIndex indicates which strucutre is passed to
*      the comp.Caller shall provide memory for the correct structure and shall
*      fill in the nSize and nVersion fields before invoking this macro.
*      List of supported indices are
*
*  @ param OMX_IndexParamPortDefinition     : set the port settings
*
*  @ param OMX_IndexParamPriorityMgmt   : set the priority for a group of components
*
*  @ param OMX_IndexParamCompBufferSupplier : set the buffer supplier parameters
*
*  @ calltype           : Blocking call
*
*  @ param hComponent   : Handle of the component
*
*  @ param nParamIndex  : The index of the structure that is to be sent. This value
*                         is from the OMX_INDEXTYPE enumeration.
*
*  @ param pParamStruct : A pointer to the IL client-allocated structure that
*                         the component uses for initialization.
*
*  @ PreRequisites      : Invokes only when the component is in OMX_StateLoaded
*                         or on port that is disabled
*/
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_SetParameter(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_INDEXTYPE nParamIndex,
                OMX_IN  OMX_PTR pParamStruct);

/**========================================================================**/
/**
 * @fn OMX_BASE_SendCommand()
 *     This method sends command to a component.
 *     If the component executes the command successfully then it generates an
 *     OMX_EventCmdComplete callback to notify the IL Client and if the
 *     component fails to execute the command, comp generates an
 *     OMX_EventError and passes the appropriate error as parameter.
 *
 *  @ calltype          :  Non-Blocking call
 *
 *  @ param hComponent  :  Handle of the component
 *
 *  @ param Cmd         :  Command for the component to execute
 *
 *  @ Param nParam      :  Integer parameter for the command that is to be executed
 *
 *  @ Param pCmdData    :  A pointer that contains implementation-specific data that
 *                         cannot be represented with the numeric parameter nParam.
 *
 *  @ PreRequisites     :  None
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_SendCommand(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_COMMANDTYPE Cmd,
                OMX_IN  OMX_U32 nParam,
                OMX_IN  OMX_PTR pCmdData);

/**=========================================================================**/
/**
 * @fn OMX_BASE_AllocateBuffer()
 *     This method will reuqest the comp to allocate
 *     a new buffer and buffer header. Component allocate the buffer
 *     and bufferheader and return a  pointer to the buffhdr. This macro
 *     cannot be used to allocate buffers for tunneled ports.
 *
 *  @ calltype         : Blocking call
 *
 *  @ param hComponent : Handle of the component
 *
 *  @ Param ppBuffer   : A pointer to a pointer of an OMX_BUFFERHEADERTYPE
 *                       structure that receives the  pointer to the buffer header
 *
 *  @ Param nPortIndex : Port Index
 *
 *  @ Param pAppPrivate: Initializes the pAppPrivate member of the buffer header structure.
 *
 *  @ Param nSizeBytes : The size of the buffer to allocate.
 *
 *  @ PreRequisites    : Comp shall be in the OMX_StateLoaded or WaitForResources state.
 *                       or on a port to which the call applies shall be disabled.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_AllocateBuffer(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_INOUT OMX_BUFFERHEADERTYPE** ppBuffer,
                OMX_IN OMX_U32 nPortIndex,
                OMX_IN OMX_PTR pAppPrivate,
                OMX_IN OMX_U32 nSizeBytes);

/**=========================================================================**/
/**
 * @fn OMX_BASE_UseBuffer()
 *     This method requests the component to use a buffer
 *     already allocated by the ILClient or a buffer already supplied by a tunneled
 *     component.This allocates the bufferheader and populates it with given
 *     input parameters.
 *
 *  @ calltype            :  Blocking call
 *
 *  @ param hComponent    :  Handle of the component
 *
 *  @ Param ppBufferHdr   :  pointer to a pointer of an OMX_BUFFERHEADERTYPE structure
 *                           that receives the pointer to the buffer header.
 *
 *  @ Param nPortIndex    :  The index of the port that will use the specified buffer. This index
 *                           is relative to the component that owns the port.
 *
 *  @ Param pAppPrivate   :  A pointer that refers to an implementation-specific memory area
 *                           that is under responsibility of the supplier of the buffer
 *
 *  @ Param nSizeBytes    :  Size of the buffer in bytes
 *
 *  @ Param pBuffer       :  A pointer to the memory buffer area to be used.
 *
 *  @ PreRequisites       :  The comp shall be in the OMX_StateLoaded or WaitForResources
 *                           state , or on a port which the call applies shall be disabled.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_UseBuffer(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_IN OMX_U32 nPortIndex,
                OMX_IN OMX_PTR pAppPrivate,
                OMX_IN OMX_U32 nSizeBytes,
                OMX_IN OMX_U8* pBuffer);

/**=========================================================================**/
/**
 * @fn OMX_BASE_FreeBuffer()
 *     This method is used to release a buffer and buffer
 *     header from a component. Base Component frees only buffer header if it is
 *     allocated only the buffer header. and shall free both the buffer and
 *     buffer header if it is allocated both buffer and buf hdr.
 *
 *  @ calltype         : Blocking call
 *
 *  @ param hComponent : Handle of the component that executes the call
 *
 *  @ Param nPortIndex : The index of the port that is using the specified buffer
 *
 *  @ Param pBuffer    : A pointer to an OMX_BUFFERHEADERTYPE structure
 *                       used to provide or receive the pointer to the buffer header
 *
 *  @ PreRequisites    : Component should be in the OMX_StateIdle state
 *                       or the port should be disabled
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_FreeBuffer(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_U32 nPortIndex,
                OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

/**=========================================================================**/
/**
 * @fn OMX_BASE_EmptyThisBuffer()
 *     This method will send a buffer full of data to
 *     an input port of a component. When  a port is Non-Tunneled, buffers sent to
 *     OMX_EmptyThisBuffer are returned to the ILClient with the EmptyBufferDone
 *     callback once it is emptied. When a port is tunneled, buffers sent to
 *     OMX_EmptyThisBuffer are sent to the tunneled port.
 *
 *  @ calltype          : Non-Blocking call
 *
 *  @ param hComponent  : Handle of the component
 *
 *  @ Param pBufHeader  : A pointer to an OMX_BUFFERHEADERTYPE structure
 *                        that is used to provide or receive the pointer to the buffer header.
 *                        The buffer header shall specify the index of the input
 *                        port that receives the buffer
 *
 *  @ PreRequisites :Component needs to be in OMX_StateExecuting or OMX_StatePause state.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_EmptyThisBuffer(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_IN OMX_BUFFERHEADERTYPE* pBufHeader);

/**=========================================================================**/
/**
 * @fn OMX_BASE_FillThisBuffer()
 *     This method will send an empty buffer to an o/p port
 *     of a component. When port is tunneled, buffers sent to OMX_fillThisBuffer
 *     return  to the IL Client with the FillBuffeDone callback once they have filled.
 *     when a port is Tunneled, buffers sent to the OMX_FillThisBuffer are sent to
 *     the  tunneled port once they are filled .
 *
 *  @ calltype          :  Non-Blocking call
 *
 *  @ param hComponent	:  Handle of the component
 *
 *  @ Param pBufHeader  :  A pointer to an OMX_BUFFERHEADERTYPE structure used to
 *                         provide or receive the  pointer to the buffer header.
                           buffers i.e  shall specify the index  of the input
 *                         port that receives the buffer.
 *
 *  @ PreRequisites :  Component needs to be in OMX_StateExecuting or OMX_StatePause state.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_FillThisBuffer(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_BUFFERHEADERTYPE* pBufHeader);

/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentTunnelRequest()
 *     This method will interact with the
 *     another OpenMAX component to determine if tunneling is possible
 *     and to setup the tunneling if it is possible.
 *
 *  @ calltype           :  Blocking call
 *
 *  @ param hComponent   :  The handle of the target component of the RequestTunnel call
 *                          and one of the components that will participate in the tunnel.
 *
 *  @ Param nPort        :  The index of the port belonging to hComp that will
 *                          participate in tunnel
 *
 *  @ Param hTunneledComp:  Handle of the Tunneled Component
 *
 *  @ Param nTunneledPort:  index of the port belonging to hTunneledComp
 *                          that participates in the tunnel
 *
 *  @ Param pTunnelSetup :  structure that contains data for the tunneling
 *                          negotiation between comp
 *
 *  @ PreRequisites :     Component shall be in OMX_StateLoaded state.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentTunnelRequest(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_U32 nPort,
                OMX_IN  OMX_HANDLETYPE hTunneledComp,
                OMX_IN  OMX_U32 nTunneledPort,
                OMX_INOUT  OMX_TUNNELSETUPTYPE* pTunnelSetup);

/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentDeInit()
 *     Derived Component calls this function to dispose of a component.
 *
 * @ calltype          :  Blocking call
 *
 *  @ param hComponent :  Handle of the component that executes the call
 *
 *  @ PreRequisites    :  ILclient executes this function regardless of
 *                        component's state, However executing ComponentDeinit
 *                        when the comp is in loaded state is recommended
 *                        for proper shutdown.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentDeInit(
                OMX_IN OMX_HANDLETYPE hComponent);

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetConfig()
 *     This methos will get a configuration structure from a component.
 *
 *  @ calltype          :    Blocking call
 *
 *  @ param hComponent  :    Handle of the component that executes the call
 *
 *  @ param nIndex      :    The index of the structure to be filled. This
 *                           value is from the OMX_INDEXTYPE enumeration.
 *
 *  @ param pComponentConfigStructure :A pointer to the IL client-allocated
 *                                      structure that the component fills.
 *
 *  @ PreRequisites : invoked when the comp is in any state except OMX_StateInvalid state.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetConfig(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_INDEXTYPE nIndex,
                OMX_INOUT OMX_PTR pComponentConfigStructure);

/**=========================================================================**/
/**
 * @fn OMX_BASE_SetConfig()
 *     This method will send one of the configurations strucutre to a component.
 *
 * @ calltype          :  Blocking call
 *
 *  @ param hComponent :  Handle of the component
 *
 *  @ param nIndex     :  The index of the structure that is to be sent. This
 *                        value is from  the OMX_INDEXTYPE enumeration.
 *
 *  @ param pComponentConfigStructure  : A pointer to the IL client-allocated struct
 *                                       that the component uses for initialization.
 *
 *  @ PreRequisites  : invoked when the comp is in any state except OMX_StateInvalid
 */
/**=========================================================================**/
 OMX_ERRORTYPE OMX_BASE_SetConfig(
                OMX_IN  OMX_HANDLETYPE hComponent,
                OMX_IN  OMX_INDEXTYPE nIndex,
                OMX_IN  OMX_PTR pComponentConfigStructure);

/**=========================================================================**/
/**
 * @fn OMX_BASE_UseEGLImage()
 *     This method enables an OMX IL component to
 *     use as a buffer, the image  already allocated via EGL. EGLImages are
 *     designed for sharing data  between rendering based EGL interfaces,
 *     such as OpenGL ES and OpenVG.
 *
 *
 *  @ calltype           :  Blocking call
 *
 *  @ param hComponent   :  Handle of the component
 *
 *  @ param ppBufferHdr  :  A pointer to a pointer of an OMX_BUFFERHEADERTYPE
 *                          structure that receives the pointer to the buffer header.
 *
 *  @ param nPortIndex   :  The index of the port that will use the specified buffer.
 *
 *  @ param pAppPrivate  :  A pointer that refers to an implementation-specific
 *                          memory area that is under responsibility of the
 *                          upplier of the buffer.
 *
 *  @ param eglImage     :  The handle of the EGLImage to use as a buffer
*                           on the specified port.
 *
 *  @ preRequisites      :  Comp shall be in OMX_StateLoaded or
 *                          OMX_StateWaitForResources state,  or the
*                           port to which the call applies shall be disabled.
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_UseEGLImage(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_INOUT OMX_BUFFERHEADERTYPE** ppBufferHdr,
                OMX_IN OMX_U32 nPortIndex,
                OMX_IN OMX_PTR pAppPrivate,
                OMX_IN void* eglImage);

/**=========================================================================**/
/**
 * @fn OMX_BASE_GetExtensionIndex()
 *     This method will invoke a component to
 *     translate  from a  standardized OpenMAX or vendor-specific extension string
 *     for a configuration or a parameter into an OpenMAX structure index.
 *
 *  @ calltype            : Blocking call
 *
 *  @ param hComponent    : Handle of the component
 *
 *  @ param cParameterName: An OMX_STRING value that shall be less than 128 char
 *                          long including the trailing null byte. The component will
 *                          translate this string into a configuration index.
 *
 *  @ param pIndexType    : A pointer to the OMX_INDEXTYPE structure that is to
 *                          receive the index value
 *
 *  @ PreRequisite  : Invoked when the comp is in any state except OMX_StateInvalid
 *
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_GetExtensionIndex(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_IN OMX_STRING  cParameterName,
                OMX_OUT OMX_INDEXTYPE *pIndexType);

/**=========================================================================**/
/**
 * @fn OMX_BASE_ComponentRoleEnum()
 *     This method allows the IL Core to query a component for all the roles it supports.
 *
 *  @ caltype             :     Blocking call
 *
 *  @ param hComponent    :     Handle of the component
 *
 *  @ Param cRole         :     Name of the Specified Role
 *
 *  @ Param nIndex        :     Index of the Role being queried
 *
 *  @ PreRequisites       :     None
 *
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_ComponentRoleEnum(
                OMX_IN OMX_HANDLETYPE hComponent,
                OMX_OUT OMX_U8 *cRole,
                OMX_IN OMX_U32 nIndex);

/******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
******************************************************************************/
/**---------------------------------------- data declarations -----------------**/
/**--------------------------------------- function prototypes ----------------**/
/**--------------------------------------------  macro   ----------------------**/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_BASE_H_ */

