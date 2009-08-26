#ifndef _TIOMXPLAYER_H_
#define _TIOMXPLAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Used for tiomxplayerutils.c */
#include <pthread.h>
#include <sys/time.h>

#include <error.h>
#include <OMX_Core.h>
#include <OMX_Component.h>
#include <unistd.h>
#include <getopt.h>
#include <alsa/asoundlib.h>

#define IN_BUFFER_SIZE 1024*2
#define OUT_BUFFER_SIZE 1024*8

#define CACHE_ALIGNMENT 128
#define EXTRA_BYTES 256

#define FILE_MODE 0
#define ALSA_MODE 1
#define DASF_MODE 2

#define APP_DEBUG
#ifdef APP_DEBUG
#define APP_DPRINT(...) fprintf(stderr,"%d %s-",__LINE__,__func__);	\
  fprintf(stderr,__VA_ARGS__);
#else
    #define APP_DPRINT(...)
#endif

/**=========================================================================**/
/**
 * @def OMX_INIT_STRUCT - Initializes the data structure using a ptr to
 *  structure This initialization sets up the nSize and nVersin fields of the
 *  structure
 *=========================================================================**/
#define OMX_INIT_STRUCT(_s_, _name_)            \
  memset((_s_), 0x0, sizeof(_name_));           \
  (_s_)->nSize = sizeof(_name_);                \
  (_s_)->nVersion.s.nVersionMajor = 0x1;        \
  (_s_)->nVersion.s.nVersionMinor = 0x1;        \
  (_s_)->nVersion.s.nRevision  = 0x0;           \
  (_s_)->nVersion.s.nStep   = 0x0;

typedef struct Event_t{
  pthread_cond_t cond;
  pthread_mutex_t mutex;
  unsigned int semval;
}Event_t;

typedef struct{
  char *device ;           
  snd_pcm_t *playback_handle;
  snd_pcm_hw_params_t *hw_params;
  snd_pcm_sw_params_t *sw_params;
  snd_pcm_format_t format;
  snd_pcm_uframes_t frames;
}alsaPrvtSt;

typedef struct{
  OMX_HANDLETYPE phandle;
  OMX_PARAM_PORTDEFINITIONTYPE *in_port;
  OMX_PARAM_PORTDEFINITIONTYPE *out_port;
  OMX_BUFFERHEADERTYPE **in_buffers;
  OMX_BUFFERHEADERTYPE **out_buffers;
  OMX_AUDIO_PARAM_PCMMODETYPE *pcm;
  OMX_AUDIO_PARAM_MP3TYPE *mp3;
  OMX_AUDIO_PARAM_AACPROFILETYPE *aac;
  OMX_PARAM_COMPONENTROLETYPE *pCompRoleStruct;
  OMX_AUDIO_CODINGTYPE eEncoding;
  OMX_U8 mode;
  OMX_U16 iterations;
  OMX_U8 channels;
  OMX_U32 samplerate;
  OMX_U16 nIpBuf;
  OMX_U16 IpBufSize;
  OMX_U16 nOpBuf;
  OMX_U16 OpBufSize;
  OMX_BOOL done_flag;
  OMX_U8 tc;
  Event_t *eos;
  Event_t *flush;
  Event_t *state;
  alsaPrvtSt *alsaPrvt;
}appPrivateSt;

/** Initializes the event at a given value
 *
 * @param event the event to initialize
 *
 * @param val the initial value of the event
 */
void event_init(Event_t* event, unsigned int val);

/** Destroy the event
 *
 * @param event the event to destroy
 */
void event_deinit(Event_t* event);

/** Decreases the value of the event. Blocks if the event
 * value is zero.
 *
 * @param event the event to decrease
 */
void event_block(Event_t* event);

/** Increases the value of the event
 *
 * @param event the event to increase
 */
void event_wakeup(Event_t* event);

/** Reset the value of the event
 *
 * @param event the event to reset
 */
void event_reset(Event_t* event);

/** Wait on the condition.
 *
 * @param event the event to wait
 */
void event_wait(Event_t* event);

/** Signal the condition,if waiting
 *
 * @param event the event to signal
 */
void event_signal(Event_t* event);

/** 
 *
 * @param 
 */
void alsa_setAudioParams(appPrivateSt *appPrvt) ;

/** 
 *
 * @param 
 */
void alsa_pcm_write(appPrivateSt *appPrvt, OMX_BUFFERHEADERTYPE* pBuffer);
/** Call to OMX_EmptyThisBuffer
 *
 * @param handle Handle to the component.
 * @param buffer IN buffer header pointer
 *
 */
int send_input_buffer(appPrivateSt* appPrvt,
                      OMX_BUFFERHEADERTYPE* buffer);
/** test: Switch to different test cases
 *
 * @param handle Handle to the component.
 *
 */
int test(appPrivateSt* appPrvt);

/** Waits for the OMX component to change to the desired state
 *
 * @param pHandle Handle to the component
 * @param Desiredstate State to be reached
 * @param event the event to hold on
 *
 */
OMX_ERRORTYPE WaitForState(OMX_HANDLETYPE pHandle,
                           OMX_STATETYPE DesiredState,
                           Event_t* pevent);
/** Configure mp3 input params
 *
 * @param appPrvt Aplication private variables
 *
 */
int config_mp3(appPrivateSt* appPrvt);
/** Configure aac input params
 *
 * @param appPrvt Aplication private variables
 *
 */
int config_aac(appPrivateSt* appPrvt);
/** Configure pcm output params
 *
 * @param appPrvt Aplication private variables
 *
 */
int config_pcm(appPrivateSt* appPrvt);

#endif
