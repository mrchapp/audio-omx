/* ====================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 * (c) Copyright Texas Instruments, Incorporated. All Rights Reserved.
 *
 * Use of this software is controlled by the terms and conditions found
 * in the license agreement under which this software has been supplied.
 * ==================================================================== */
/*
*  @file omx_base_internal.h
*  The omx_base_internal header file defines the interface of the base component.
*
*  @path \OMAPSW_SysDev\multimedia\omx\khronos1_1\omx_base\src
*
*  @rev  1.0
*/
/* --------------------------------------------------------------------------*/
/* =========================================================================
 *! Revision History
 *! =========================================================================
 *!17-April-2008 Rabbani Patan rabbani@ti.com: Initial Version
 * =========================================================================*/

#ifndef _OMX_BASE_INTERNAL_H_
#define _OMX_BASE_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** ------ User code goes here ------**/
/** ------compilation control switches -----**/
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
/** ----- system and platform files -------**/
/**------ program files -------------------**/

/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/**--------data declarations -----------------------------------**/
/**--------function prototypes ---------------------------------**/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/**--------macros ----------------------------------------------**/

/**=========================================================================**/
/**
 * Port Specific Macros
 */
/**=========================================================================**/
#define PORT_IS_ENABLED(pPort)                 \
        (pPort->tPortDefParams.bEnabled == OMX_TRUE)

#define PORT_IS_BEING_DISABLED(pPort)          \
	    (pPort->bIsTransientToDisabled == OMX_TRUE)

#define PORT_IS_POPULATED(pPort)               \
       (pPort->tPortDefParams.bPopulated == OMX_TRUE)

#define PORT_IS_TUNNEL(pPort)                  \
        (pPort->unTunnelFlags & PORT_IS_TUNNELED)

#define PORT_IS_BUFFER_SUPPLIER(pPort)          \
        (pPort->unTunnelFlags & PORT_IS_SUPPLIER)

#define PORT_IS_TUNNELED_N_BUFFER_SUPPLIER(pPort) \
  (pPort->unTunnelFlags == (PORT_IS_TUNNELED | PORT_IS_SUPPLIER))

/**--------data declarations -----------------------------------**/

#define OMX_BASE_TUNNEL_TIMEOUT 20

#define OMX_BASE_TUNNEL_SLEEPTIME 5

#define CMDEVENT  (0x00000100)

#define DATAEVENT (0x00000200)

#define ENDEVENT  (0x00000020)

/******************************************************************************
* Enumerated Types
******************************************************************************/

/**=========================================================================**/
/** PORT_TUNNEL_CONFIG
 *    describes the tunnel configurations of a port.
 *
 * @param PORT_NOT_TUNNELED : Assigned when the port is not Tunneled

 * @param PORT_IS_TUNNELED  : Assigned when the port is tunneled
 *
 * @param PORT_IS_SUPPLIER  : Assigned if the port is tunneled supplier
 */
/**=========================================================================**/
typedef enum PORT_TUNNEL_CONFIG {
   PORT_NOT_TUNNELED = 0,
   PORT_IS_TUNNELED  = 0x0001,
   PORT_IS_SUPPLIER  = 0x0002
}PORT_TUNNEL_CONFIG;

/*******************************************************************************
* Strutures
*******************************************************************************/

/**=========================================================================**/
/** OMX_BASE_CMDPARAMS
 *  Structure contains general information given by
 *  OpenMax request(from SendCommand)
 *
 * @param tCmdType      :  Command for the comp to execute
 *
 * @param unParam       :  parameter for the command to be executed
 *
 * @param pCmdData      :  ptr that contains implementation specific data
 */
/**=========================================================================**/
typedef struct OMX_BASE_CMDPARAMS
{
   OMX_COMMANDTYPE tCmdType;
   OMX_U32 unParam;
   OMX_PTR pCmdData;
}OMX_BASE_CMDPARAMS;

/**=========================================================================**/
/**  OMX_BUFHDR_STATUS_FLAG - BufferHeader Status Flag Enumeration
 *
 * @param OMX_BUFFER_FREE     :Buffer is free
 *
 * @param OMX_BUFFER_ALLOCATED:Buffer is Allocated
 *
 * @param OMX_BUFFER_ASSIGNED :Buffer Assigned by the client.
 *
 * @param OMX_HEADER_ALLOCATED:Buffer Header is Allocated.
 */
/**=========================================================================**/
 typedef enum OMX_BUFHDR_STATUS_FLAG {
   OMX_BUFFER_FREE         = 0,
   OMX_BUFFER_ALLOCATED    = 0x0001,
   OMX_BUFFER_ASSIGNED     = 0x0002,
   OMX_HEADER_ALLOCATED    = 0x0004
} OMX_BUFHDR_STATUS_FLAG;

/**=========================================================================**/
/** OMX_BASE_PORTTYPE -Structure that describes the elements of an port
 *
 *  @param hTunnelComp           :  Handle of the Tunneled Component
 *
 *  @param unTunnelFlags         :  tunnel status of a port.
 *
 *  @param unTunnelPort          :  Tunneled port number
 *
 *  @param unTunnelBufs          :  Number of buffers to be Tunneled
 *
 *  @param unNumAssignedBufs     :  Number of Buffers Assigned on each port
 *
 *  @param bIsPortflushed        :  indicates that the port is being flushed
 *
 *  @param bIsTransientToEnabled :  port is in transition to enabled
 *
 *  @param bIsTransientToDisabled:  port is in transition to disabled
 *
 *  @param tBufferSupplier       :  type of supplier incase of tunneling
 *
 *  @param tPortDefParams        :  port definition setting of an port
 *
 *  @param pBufStateAllocated    :  status of the bufferheader
 *                                  (Assigned or Allocated)
 *
 *  @param pBufferList           :  Array of buffer headers
 *
 *  @param pBufAllocSem          :  semaphore that locks the execution
 *                                  for the buffers to be allocated.
 *
 *  @param pDataPipe             :  data pipe
 */
/**=========================================================================**/
typedef struct OMX_BASE_PORTTYPE {
   OMX_HANDLETYPE hTunnelComp;
   OMX_U32 unTunnelFlags;
   OMX_U32 unTunnelPort;
   OMX_U32 unTunnelBufs;
   OMX_U32 unNumAssignedBufs;
   OMX_BOOL bIsPortflushed;
   OMX_BOOL bIsTransientToEnabled;
   OMX_BOOL bIsTransientToDisabled;
   OMX_BUFFERSUPPLIERTYPE tBufferSupplier;
   OMX_PARAM_PORTDEFINITIONTYPE tPortDefParams;
   OMX_BUFHDR_STATUS_FLAG *pBufStateAllocated;
   OMX_BUFFERHEADERTYPE **pBufferlist;
   OMX_PTR pBufAllocSem;
   OMX_PTR dataPipe;
}OMX_BASE_PORTTYPE;

/**=========================================================================**/
/** OMX_BASE_INTERNALTYPE
 *  structure contains private elements of the Base Component
 *
 * @param ports               : describes the info related to each port
 *
 * @param tCbInfo             : Application callbacks
 *
 * @param cmdPipe             : CommandPipe Used to Send Commands
 *
 * @param pThreadId           : ThreadID incase of Active context
 *
 * @param pEventToProcesshread: event ot Invoke the thread
 *
 * @param nNumBuffers         : total no of buffer of an component
 *
 * @param hEvent              : handle of anEvent
 *
 * @param fpInvokeProcessFunction : FunctionPointer to Invoke the ProcessFunction
 */
/**=========================================================================**/
typedef struct OMX_BASE_INTERNALTYPE{
   OMX_BASE_PORTTYPE **ports;
   OMX_CALLBACKTYPE tCbInfo;
   OMX_U32 nNumBuffers;
   OMX_PTR cmdPipe;
   OMX_PTR pThreadId;
   OMX_PTR pEventToProcessThread;
   OMX_HANDLETYPE hEvent;
   OMX_ERRORTYPE (*fpInvokeProcessFunction)(OMX_HANDLETYPE hComponent,
                                              OMX_U32 retrivedEvent);
}OMX_BASE_INTERNALTYPE;

/**---Port (Init, Deinit, Enable and Disable) Related - function prototypes -*/
/**=========================================================================**/
/**
 * @fn _OMX_BASE_InitializePorts()
 *     This is an internal function used by the
 *     Base Component. This function Allocates memory for the given
 *     number of ports(by Derived component)  and Initialies them.
 *
 *  @ param [in] hComponent : Handle of the Component to access
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_InitializePorts(OMX_HANDLETYPE hComponent);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_DeinitalizePorts()
 *     This is an Internal function used by the Base Component.
 *     This function deintializes all the ports Allocated while the component
 *     is initiailzes,  since the component is getting de-initialized.
 *
 *  @ param [in] hComponent : Handle of the Component to access
 *
 *   @ see omx_base_internal.h
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_DeinitalizePorts(OMX_HANDLETYPE hComponent);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_DisablePort()
 *     This is an Internal Function used by the
 *     Base component to Disable a port/s.This function is called only
 *     under the following conditions.
 *     1. If the port that the IL Client is disabling is a Non-Supplier port,
 *        then IL client should have returned all the buffers it is holding
 *        to the supplier port via ETB/FTB if tunneling
 *        or EBD/FBD if not tunneled.
 *     2. If the port that the IL client is disabling is a supplier port
 *        with buffers Allocated, then the Non-Supplier port should have
 *        returned all the buffers via  ETB/FTB.
 *
 *  @ param [in]    hComponent : Handle of the Component to access
 *
 *  @ param [in]    nParam     : Index of the por to disable
 *
 * @ see omx_base_internal.h
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_DisablePort(OMX_HANDLETYPE hComponent,
                                    OMX_U32 nParam);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_EnablePort()
 *    This is an Internal function used by the Base Component
 *    to enable a port/s. This function allocates the buffers if the port
 *    is tunneled and buffer supplier.
 *
 *  @ param [in] hComponent : Handle of the Component
 *
 *  @ Param [in] nParam     : Index of the port to be enabled
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_EnablePort(OMX_HANDLETYPE hComponent,
                                   OMX_U32 nParam);

/**-- Port Compatblity(Verify tunnelConnection)- function prototypes--------**/
/**=========================================================================**/
/**
 * @fn _OMX_BASE_VerifyTunnelConnection()
 *     Internal function used by the  "BasecomponentTunnelRequest" to
 *     check for the compatablity between tunneled ports for tunneling.
 *
 *  @ param [in]    pPort         : Pointer to the OMX_BASE_PORTTYPE
 *
 *  @ param [in]    hTunneledComp : Handle of the Tunneled Component
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_VerifyTunnelConnection(OMX_BASE_PORTTYPE *pPort,
                                           OMX_HANDLETYPE hTunneledComp);

/**---------------EventHandler Callbacks  - function prototypes ------------**/
/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleEventNotify1()
 *     This is an Internal Function used by the
 *     Base component. This functions  is used to return the Events given by
 *     OMX_SendCommand for following.
 *     1. State transition from loaded to idle 2. command port enable
 *
 *  @ param [in]    hComponent  : Handle of the Component to access
 *
 *  @ param [in]    nPortIndex  : Index of the port
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleEventNotify1(OMX_HANDLETYPE hComponent,
                                           OMX_U32 nPortID);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleEventNotify2()
 *     This is an Internal Function used by the Base component.
 *     This is used to return the Events given by SendCommand for following.
 *     1. State transition from idle to loaded   and  2. command port disable
 *
 *  @ param [in]  hComponent  : Handle of the Component to access
 *
 *  @ param [in]  nPortIndex  : Index of the port
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleEventNotify2(OMX_HANDLETYPE hComponent,
                                           OMX_U32 nPortIndex);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleEventNotify3()
 *     This function is an Internal Function used by the Base Component.
 *     this function is used to return an EventHandler
 *     callbacks after returning   a call from the derived Component.
 *
 *  @ param [in]  hComponent   : Handle of the Component to access
 *
 *  @ param [in]  cmdParams    : General info given by the SendCommand
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleEventNotify3(OMX_HANDLETYPE hComponent,
                                        OMX_BASE_CMDPARAMS *cmdEvent);

/**-----------------EventHandler Callbacks  - function prototypes ----------**/
/**=========================================================================**/
/**
 * @fn _OMX_BASE_HandleStateTransition()
 *     This is an Internal Function used by used by the Base Component.
 *     This function handles the command sent by OMX_SendCommand to
 *     trnasition the component from current state to the New state.
 *
 *  @ param [in] hComponent :  Handle of the Component to access
 *
  * @ param [in] cmdParams  :  General info given by the SendCommand
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_HandleStateTransition(
                                OMX_HANDLETYPE hComponent,
                                OMX_BASE_CMDPARAMS *cmdEvent);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_AllocateResources()
 *     Internal Function used to Allocate the
 *     resources required  to the specified port number.
 *
 *  @ param [in] hComponent  : Handle of the Component to access
 *
 *  @ param [in] nIndex      : Index of the port to Allocate Resources
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_AllocateResources(OMX_HANDLETYPE hComponent,
                                          OMX_U32 nIndex);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_DeAllocateResources()
 *     Internal Function used to De-Allocate/Freeup
 *     the resources to the specifed port.
 *
 *  @ param [in] hComponent  :  Handle of the Component to access
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_DeAllocateResources(OMX_HANDLETYPE hComponent,
                                            OMX_U32 nIndex);



/**-----Allocation/De-Allocation/Flushing Buffers --------------------------**/
/**=========================================================================**/
/**
 * @fn _OMX_BASE_InitializeBuffer()
 *     This is an Interna function used by the
 *     Base Component. This function intializes the give buffer header
 *
 *  @ param [in] hComponent     :  Handle of the Component
 *
 *  @ Param [in] pBufHeader     :  Pointer to an Buffer Header
 *
 *  @ Param [in] nPortIndex     :  Port Index
 *
 *  @ Param [in] pAppPrivate    :  Pointer to an Application Private Data
 *
 *  @ Param [in] nSizeBytes     :  Number of Bytes
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_InitializeBuffer(OMX_HANDLETYPE hComponent,
                                    OMX_BUFFERHEADERTYPE* pBufHeader,
                                         OMX_U32 nPortIndex,
                                         OMX_PTR pAppPrivate,
                                         OMX_U32 nSizeBytes);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_AllocateTunnelBuffer()
 *     This is an Internal Function used only by
 *    the Base Component.  This function allocates buffers only for the
 *    tunneled port and calls OMX_UseBuffer of the tunneled component.
 *
 *  @ param [in] hComponent   : Handle of the Component to access
 *
 *  @ Param [in] nPortIndex   : Index of the port to allocate buffers
 *
 *  @ Param [in] nSizeBytes   : Number of bytes to Allocate
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_AllocateTunnelBuffer(OMX_HANDLETYPE hComponent,
                                             OMX_U32 nPortIndex,
                                             OMX_U32 nSizeBytes);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_FreeTunnelBuffer()
 *    This is an Internal Function used by the Base Component.
 *    This function deallocates the buffer and calls
 *    an OMX_FreeBuffer  tunneled port.
 *
 *  @ param [in] hComponent   :   Handle of the Component
 *
 *  @ Param [in] nPortIndex   :   Port Index
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_FreeTunnelBuffer(OMX_HANDLETYPE hComponent,
                                         OMX_U32 nPortIndex);

/**=========================================================================**/
/**
 * @fn _OMX_BASE_FlushBuffers()
 *     This is an Internal Function used by the
 *     Base Component.  his function returns buffers back to the IL client incase of
 *     Non-Tunneling  via EBD/FBD  or alls ETB/FTB of the corresponding
 *     tunneled comp in case of tunneling.
 *
 *  @ param [in] hComponent    :  Handle of the Component
 *
 *  @ Param [in] nPortIndex    :  Port Index
 */
/**=========================================================================**/
OMX_ERRORTYPE _OMX_BASE_FlushBuffers(OMX_HANDLETYPE hComponent,
                                     OMX_U32 nPortIndex);

/**-Hooks from Derived Component to Base Component  - function prototypes -**/
/**=========================================================================**/
/**
 * @fn OMX_BASE_Cb_ReturnDataNotify()
 *     This is the callback function called by the
 *     Derived Component to the Base Component to reutn data notification.
 *     This method now checks for the port if not tunneled which communicates
 *     with the IL Client and returns buffers back to the IL client by calling EBD/FBD,
 *     if port is tunneled calls EmptyThisBuffer/FillThisBuffer on the corresponding
 *     tunneled port.
 *
 *  @ param [in] hComponent  :  Handle of the Component that executes the call
 *
 *  @ param [in] nPortIndex  :   Index of the port
 *
 *  @ param [in] pBuffer     :  pointer to an OMX_BUFFERHEADERTYPE structure
 *
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_CB_ReturnDataNotify(OMX_HANDLETYPE hComponent,
                                           OMX_S32 nPortIndex,
                                           OMX_BUFFERHEADERTYPE* pBuffer);

/**=========================================================================**/
/**
* @fn OMX_BASE_Cb_ReturnEventNotify()
*     This is the Callback function called by the
*     Derived Component to the Base Component to notify the Aysnchornous Events.
*     This method now generates an EventHandler Notification to the IL client.
*
* @  param [in] hComponent : Handle of the Component to access
*
*  @ param [in] eEvent     : Event that the component wants to notify the app
*
*  @ param [in] EventData1 : This can be OMX_ERRORTYPE for an error event and
*                            will be an OMX_COMMANDTYPE for a command complete
*                            event,OMX_INDEXTYPE for OMX_PortSettingsChanged event.
*
*  @ param [in] EventData2 : This can hold extra information related to the Event.
*                            can be OMX_STATETYPE for a state change command or
*                            port index for a port settings changed event.
*
*  @ param [in] pEventData : pointer to an additional event-specific data
*/
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_CB_ReturnEventNotify(OMX_HANDLETYPE hComponent,
                                            OMX_EVENTTYPE eEvent,
                                            OMX_U32 EventData1,
                                            OMX_U32 EventData2,
                                            OMX_PTR pEventData);

/**---------Thred Related   - function prototypes -------------------------**/
/**=========================================================================**/
/**
 * @fn OMX_BASE_ThreadFunctionEntry() -
 *     Function entry in case of Active context.
 */
/**=========================================================================**/
/*void OMX_BASE_ThreadFunctionEntry(OMX_U32 nArgs, void* arg);*/
/*Linux mods*/
void * OMX_BASE_ThreadFunctionEntry (void* arg);


/**=========================================================================**/
/**
 * @fn OMX_BASE_PROCESS_TriggerEvent() -
 *
 *  @ param [in] hComponent    : Handle of the Component
 *
 *  @ Param [in] unEventToSet  : Event that need to be set to invoke the Thread
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_PROCESS_TriggerEvent(OMX_HANDLETYPE hComponent,
                                            OMX_U32 unEventToSet);

/**=========================================================================**/
/**
 * @fn OMX_BASE_PROCESS_Events() -
 *
 *  @ param [in] hComponent      : Handle of the Component
 *
 *  @ Param [in] unRetrivedEvent : Event to be processed
 */
/**=========================================================================**/
OMX_ERRORTYPE OMX_BASE_PROCESS_Events(OMX_HANDLETYPE hComponent,
                                      OMX_U32 unRetrivedEvent);

/******************************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 *******************************************************************************/
/**------------------------------------------------ data declarations ---------**/
/**---------------------------------  function prototypes ---------------------**/
/**----------------------------------------------------   macros     ----------**/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OMX_BASE_INTERNAL_H_ */




