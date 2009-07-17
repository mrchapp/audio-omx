#include "tiomxplayer.h"

extern FILE* infile;

/** Initializes the event at a given value
 *
 * @param event the event to initialize
 * @param val the initial value of the event
 *
 */
void event_init(Event_t* event, unsigned int val) {
  pthread_cond_init(&event->cond, NULL);
  pthread_mutex_init(&event->mutex, NULL);
  event->semval = val;
}

/** Destroy the event
 *
 * @param event the event to destroy
 */
void event_deinit(Event_t* event) {
  pthread_cond_destroy(&event->cond);
  pthread_mutex_destroy(&event->mutex);
}

/** Decreases the value of the event. Blocks if the event
 * value is zero.
 *
 * @param event the event to decrease
 */
void event_block(Event_t* event) {
  pthread_mutex_lock(&event->mutex);
  while (event->semval == 0) {
    pthread_cond_wait(&event->cond, &event->mutex);
  }
  event->semval--;
  pthread_mutex_unlock(&event->mutex);
}

/** Increases the value of the event
 *
 * @param event the event to increase
 */
void event_wakeup(Event_t* event) {
  pthread_mutex_lock(&event->mutex);
  event->semval++;
  pthread_cond_signal(&event->cond);
  pthread_mutex_unlock(&event->mutex);
}

/** Reset the value of the event
 *
 * @param event the event to reset
 */
void event_reset(Event_t* event) {
  pthread_mutex_lock(&event->mutex);
  event->semval=0;
  pthread_mutex_unlock(&event->mutex);
}

/** Wait on the cond.
 *
 * @param event the event to wait
 */
void event_wait(Event_t* event) {
  pthread_mutex_lock(&event->mutex);
  pthread_cond_wait(&event->cond, &event->mutex);
  pthread_mutex_unlock(&event->mutex);
}

/** Signal the cond,if waiting
 *
 * @param event the event to signal
 */
void event_signal(Event_t* event) {
  pthread_mutex_lock(&event->mutex);
  pthread_cond_signal(&event->cond);
  pthread_mutex_unlock(&event->mutex);
}

/** Waits for the OMX component to change to the desired state
 *
 * @param pHandle Handle to the component
 * @param Desiredstate State to be reached
 * @param event the event to hold on
 *
 */
OMX_ERRORTYPE WaitForState(OMX_HANDLETYPE pHandle,
                           OMX_STATETYPE DesiredState,
                           Event_t* pevent){

  OMX_STATETYPE CurState = OMX_StateInvalid;
  OMX_ERRORTYPE eError = OMX_ErrorNone;

  eError = OMX_GetState(pHandle, &CurState);
  if (CurState == OMX_StateInvalid){
    APP_DPRINT("OMX_StateInvalid!\n");
    eError = OMX_ErrorInvalidState;
  }else if(CurState != DesiredState){
    event_wait(pevent);
  }
  APP_DPRINT("State changed to: %d\n",CurState);

  return eError;
}

/** Call to OMX_EmptyThisBuffer
 *
 * @param handle Handle to the component.
 * @param buffer IN buffer header pointer
 *
 */
int send_input_buffer(appPrivateSt* appPrvt,OMX_BUFFERHEADERTYPE *buffer){

  OMX_ERRORTYPE error = OMX_ErrorNone;
  static OMX_BOOL eos_flag = OMX_FALSE;
  int nread;
  static int drop_count = 0;

  if(!eos_flag){
    nread = fread(buffer->pBuffer,
                  1,
                  buffer->nAllocLen,
                  infile);
    buffer->nFilledLen = nread;
    buffer->nFlags |= 0;
    if(nread <= 0 ) {
      /*set the buffer flag*/
      buffer->nFlags |= OMX_BUFFERFLAG_EOS;
      eos_flag = OMX_TRUE;
      APP_DPRINT("EOS marked!\n");
    }

    /*APP_DPRINT("Send IN buffer %p %ld %x\n",buffer->pBuffer,
               buffer->nFilledLen,
               (unsigned int)buffer->nFlags);*/

    error = OMX_EmptyThisBuffer(appPrvt->phandle,buffer);
    if(error != OMX_ErrorNone){
      APP_DPRINT("Error on EmptyThisBuffer\n");
      return 1;
    }
  }else{
    drop_count++;
    if(drop_count == appPrvt->nIpBuf){
      /*APP_DPRINT("Resetting eos_flag\n");*/
      drop_count = 0;
      eos_flag = OMX_FALSE;
    }
  }
  return 0;
}

/** Configure pcm output params
 *
 * @param appPrvt appPrvt Handle to the app component structure.
 *
 */
int config_pcm(appPrivateSt* appPrvt){

  OMX_ERRORTYPE error = OMX_ErrorNone;

  OMX_INIT_STRUCT(appPrvt->pcm,OMX_AUDIO_PARAM_PCMMODETYPE);

  APP_DPRINT("call get_parameter for OMX_IndexParamAudioPcm\n");
  appPrvt->pcm->nPortIndex = OMX_DirOutput;
  error = OMX_GetParameter (appPrvt->phandle,
                            OMX_IndexParamAudioPcm,
                            appPrvt->pcm);
  if (error != OMX_ErrorNone) {
    APP_DPRINT("OMX_ErrorBadParameter in SetParameter\n");
    return 0;
  }

  appPrvt->pcm->nSize = sizeof(OMX_AUDIO_PARAM_PCMMODETYPE);
  appPrvt->pcm->nSamplingRate = appPrvt->samplerate;
  appPrvt->pcm->nChannels = appPrvt->channels;
  /* TODO configure other parameters */

  APP_DPRINT("call set_parameter for OMX_IndexParamAudioPcm\n");
  error = OMX_SetParameter (appPrvt->phandle,
                            OMX_IndexParamAudioPcm,
                            appPrvt->pcm);
  if (error != OMX_ErrorNone) {
    APP_DPRINT("OMX_ErrorBadParameter in SetParameter\n");
    return 0;
  }

  return 1;
}

/** Configure mp3 input params
 *
 * @param appPrvt appPrvt Handle to the app component structure.
 *
 */
int config_mp3(appPrivateSt* appPrvt){

  OMX_ERRORTYPE error = OMX_ErrorNone;

  OMX_INIT_STRUCT(appPrvt->mp3,OMX_AUDIO_PARAM_MP3TYPE);

  APP_DPRINT("call get_parameter for OMX_IndexParamAudioMp3\n");
  appPrvt->mp3->nPortIndex = OMX_DirInput;
  error = OMX_GetParameter (appPrvt->phandle,
                            OMX_IndexParamAudioMp3,
                            appPrvt->mp3);
  if (error != OMX_ErrorNone) {
    APP_DPRINT("OMX_ErrorBadParameter in SetParameter\n");
    return 1;
  }

  APP_DPRINT("call set_parameter for OMX_IndexParamAudioMp3\n");
  error = OMX_SetParameter (appPrvt->phandle,
                            OMX_IndexParamAudioMp3,
                            appPrvt->mp3);
  if (error != OMX_ErrorNone) {
    APP_DPRINT("OMX_ErrorBadParameter in SetParameter\n");
    return 1;
  }


  return 0;
}
/** Configure aac input params
 *
 * @param appPrvt Handle to the app component structure.
 *
 */
int config_aac(appPrivateSt* appPrvt){

  OMX_ERRORTYPE error = OMX_ErrorNone;

  OMX_INIT_STRUCT(appPrvt->aac, OMX_AUDIO_PARAM_AACPROFILETYPE);

  APP_DPRINT("call get_parameter for OMX_IndexParamAudioAac\n");
  appPrvt->aac->nPortIndex = OMX_DirInput;
  error = OMX_GetParameter (appPrvt->phandle,
                            OMX_IndexParamAudioAac,
                            appPrvt->aac);
  if (error != OMX_ErrorNone) {
    APP_DPRINT("OMX_ErrorBadParameter in SetParameter\n");
    return 1;
  }
  appPrvt->aac->eAACProfile = OMX_AUDIO_AACObjectLC;
  appPrvt->aac->nSampleRate = appPrvt->samplerate;
  appPrvt->aac->nChannels = appPrvt->channels;
  appPrvt->aac->eAACStreamFormat = OMX_AUDIO_AACStreamFormatMax;

  APP_DPRINT("call set_parameter for OMX_IndexParamAudioAac\n");
  error = OMX_SetParameter (appPrvt->phandle,
                            OMX_IndexParamAudioAac,
                            appPrvt->aac);
  if (error != OMX_ErrorNone) {
    APP_DPRINT("OMX_ErrorBadParameter in SetParameter\n");
    return 1;
  }


  return 0;
}

/** test_play: Plays till end of file
 *
 * @param appPrvt Handle to the app component structure.
 *
 */
int test_play(appPrivateSt *appPrvt){

  int i;
  OMX_ERRORTYPE error = OMX_ErrorNone;

  APP_DPRINT("Change state to Executing\n");
  error = OMX_SendCommand(appPrvt->phandle,
                          OMX_CommandStateSet,
                          OMX_StateExecuting,
                          NULL);
  if(error != OMX_ErrorNone){
    return 1;
  }
  error = WaitForState(appPrvt->phandle,
                       OMX_StateExecuting,
                       appPrvt->state);
  if(error != OMX_ErrorNone){
    return 1;
  }

  for (i=0; i < appPrvt->out_port->nBufferCountActual; i++) {
    APP_DPRINT( "Send OUT buffer %p\n",appPrvt->out_buffers[i]);
    error = OMX_FillThisBuffer(appPrvt->phandle,appPrvt->out_buffers[i]);
    if(error != OMX_ErrorNone) {
      APP_DPRINT("Warning: buffer (%p) dropped!\n",appPrvt->in_buffers[i]);
    }
  }
  for (i=0; i < appPrvt->in_port->nBufferCountActual; i++) {
    APP_DPRINT( "Send IN buffer %p\n",appPrvt->in_buffers[i]);
    error = send_input_buffer(appPrvt,appPrvt->in_buffers[i]);
    if(error) {
      APP_DPRINT("Warning: buffer (%p) not sent!\n",appPrvt->in_buffers[i]);
    }
  }

  /* Now wait for EOS.... */
  APP_DPRINT("Now wait for EOS to finish....\n");
  event_block(appPrvt->eos);

  return 0;
}

/** test_repeat: Multiple repetition till end of file
 *
 * @param appPrvt Handle to the app component structure.
 *
 */
int test_repeat(appPrivateSt *appPrvt){

  int i;
  int error = 0;

  for(i = 0;i < appPrvt->iterations;i++){
    sleep(2);
    APP_DPRINT("********PLAY FOR %d TIME********\n",(i+1));

    if((error = test_play(appPrvt))){
      APP_DPRINT("FAILED!!\n");
      return 1;
    }


    APP_DPRINT("Change state to Idle\n");
    error = OMX_SendCommand(appPrvt->phandle,
                            OMX_CommandStateSet,
                            OMX_StateIdle,
                            NULL);
    if(error != OMX_ErrorNone){
      return 1;
    }
    error = WaitForState(appPrvt->phandle,
                         OMX_StateIdle,
                         appPrvt->state);
    if(error != OMX_ErrorNone){
      return 1;
    }

    appPrvt->done_flag = OMX_FALSE;
    rewind(infile);

  }

  APP_DPRINT("Change state to Pause\n");
  error = OMX_SendCommand(appPrvt->phandle,
                          OMX_CommandStateSet,
                          OMX_StatePause,
                          NULL);
  if(error != OMX_ErrorNone){
    return 1;
  }
  error = WaitForState(appPrvt->phandle,
                       OMX_StatePause,
                       appPrvt->state);
  if(error != OMX_ErrorNone){
    return 1;
  }

  return 0;
}

/** test_repeat: Multiple repetition till end of file
 *
 * @param appPrvt Handle to the app component structure.
 *
 */
int test_pause_resume(appPrivateSt *appPrvt){

  int i;
  int error = 0;

  for(i = 0;i < 5;i++){
    sleep(2);
    APP_DPRINT("********PLAY FOR %d TIME********\n",(i+1));

    if((error = test_play(appPrvt))){
      APP_DPRINT("FAILED!!\n");
      return 1;
    }


    APP_DPRINT("Change state to Pause\n");
    error = OMX_SendCommand(appPrvt->phandle,
                            OMX_CommandStateSet,
                            OMX_StatePause,
                            NULL);
    if(error != OMX_ErrorNone){
      return 1;
    }
    error = WaitForState(appPrvt->phandle,
                         OMX_StatePause,
                         appPrvt->state);
    if(error != OMX_ErrorNone){
      return 1;
    }

    appPrvt->done_flag = OMX_FALSE;
    rewind(infile);

  }

  return 0;
}

/** test: Switch on different test cases
 *
 * @param appPrvt Handle to the app component structure.
 *
 */
int test(appPrivateSt *appPrvt){

  int error = 0;

  switch(appPrvt->tc){
  case 1:
    APP_DPRINT("********TEST PLAY BEGIN********\n");
    if(!(error = test_repeat(appPrvt))){
      APP_DPRINT("********TEST PLAY FINISHED********\n");
    }else{
      APP_DPRINT("********TEST PLAY EXITED********\n");
    }
    break;
  case 2:
    APP_DPRINT("********TEST PAUSE-RESUME BEGIN********\n");
    if(!(error = test_pause_resume(appPrvt))){
      APP_DPRINT("********TEST PAUSE-RESUME FINISHED********\n");
    }else{
      APP_DPRINT("********TEST PAUSE-RESUME EXITED********\n");
    }
    break;
    //TODO: ADD MORE TEST CASES
  default:
    break;
  }
  return error;
}

void alsa_setAudioParams(appPrivateSt *appPrvt) {
  int err;

  appPrvt->alsaPrvt->device = "default";            /* playback device */
  appPrvt->alsaPrvt->playback_handle = NULL;
  appPrvt->alsaPrvt->hw_params = NULL;
  appPrvt->alsaPrvt->sw_params = NULL;
  appPrvt->alsaPrvt->format = SND_PCM_FORMAT_S16_LE ;   /* sample format */
  appPrvt->alsaPrvt->frames = 0;

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&appPrvt->alsaPrvt->hw_params);
  /* Open PCM. The last parameter of this function is the mode. */
  if ((err = snd_pcm_open(&appPrvt->alsaPrvt->playback_handle, appPrvt->alsaPrvt->device,
                          SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    printf("Playback open error: %s\n", snd_strerror(err));
    exit(1);
  }
  /* Fill it in with default values. */
  snd_pcm_hw_params_any(appPrvt->alsaPrvt->playback_handle,
                        appPrvt->alsaPrvt->hw_params);
  /* Set the desired hardware parameters. */
  /* Interleaved mode */
  snd_pcm_hw_params_set_access(appPrvt->alsaPrvt->playback_handle,
                               appPrvt->alsaPrvt->hw_params,
                               SND_PCM_ACCESS_RW_INTERLEAVED);
  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(appPrvt->alsaPrvt->playback_handle,
                               appPrvt->alsaPrvt->hw_params,
                               SND_PCM_FORMAT_S16_LE);
  /* 1 channel for mono, 2 channels for stereo */
  snd_pcm_hw_params_set_channels(appPrvt->alsaPrvt->playback_handle,
                                 appPrvt->alsaPrvt->hw_params,
                                 appPrvt->channels);

  /* Sampling rate in bits/second */
  snd_pcm_hw_params_set_rate_near(appPrvt->alsaPrvt->playback_handle,
                                  appPrvt->alsaPrvt->hw_params,
                                  (unsigned int*)&appPrvt->samplerate,
                                  0);

  /* Write the parameters to the driver */
  if ((err = snd_pcm_hw_params (appPrvt->alsaPrvt->playback_handle,
                                appPrvt->alsaPrvt->hw_params)) < 0) {
    fprintf (stderr, "cannot set hwparameters (%s)\n", snd_strerror (err));
    exit (1);
  }

  /* Configure sw pcm params */
  if ((err = snd_pcm_sw_params_malloc(&appPrvt->alsaPrvt->sw_params)) < 0) {
    fprintf (stderr, "cannot allocate software parameters structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  /* Get current sw configuration for a PCM */
  if ((err = snd_pcm_sw_params_current (appPrvt->alsaPrvt->playback_handle,
                                        appPrvt->alsaPrvt->sw_params)) < 0) {
    fprintf (stderr, "cannot initialize software parameters structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  /* Set min avail frames to consider PCM ready */ 
  if ((err = snd_pcm_sw_params_set_avail_min (appPrvt->alsaPrvt->playback_handle,
                                              appPrvt->alsaPrvt->sw_params,
                                              OUT_BUFFER_SIZE)) < 0) {
    fprintf (stderr, "cannot set minimum available count (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  /* PCM is automatically started when playback frames available 
     to PCM are >= threshold */
  if ((err = snd_pcm_sw_params_set_start_threshold (appPrvt->alsaPrvt->playback_handle,
                                                    appPrvt->alsaPrvt->sw_params,
                                                    OUT_BUFFER_SIZE)) < 0) {
    fprintf (stderr, "cannot set start mode (%s)\n",
             snd_strerror (err));
    exit (1);
  }
  /* Install PCM sw configuration defined */
  if ((err = snd_pcm_sw_params (appPrvt->alsaPrvt->playback_handle,
                                appPrvt->alsaPrvt->sw_params)) < 0) {
    fprintf (stderr, "cannot set software parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }

}

void alsa_pcm_write(appPrivateSt *appPrvt, OMX_BUFFERHEADERTYPE* pBuffer)
{
  int err;
  int frameSize;
  int totalBuffer;

  frameSize = (appPrvt->pcm->nChannels * appPrvt->pcm->nBitPerSample) >> 3;

  if(pBuffer->nFilledLen < frameSize){
    APP_DPRINT("Data not enough! %ld\n",pBuffer->nFilledLen);
    return;
  }

  totalBuffer = pBuffer->nFilledLen/frameSize;

  err = snd_pcm_writei(appPrvt->alsaPrvt->playback_handle,
                       pBuffer->pBuffer, totalBuffer);
  /*err = snd_pcm_writei(appPrvt->alsaPrvt->playback_handle,
                       pBuffer->pBuffer, pBuffer->nFilledLen/(2*appPrvt->pcm->nChannels));*/
  if (err == -EPIPE) {
    APP_DPRINT("UNDERRUN\n");
    snd_pcm_prepare(appPrvt->alsaPrvt->playback_handle);
    //exit(1);
  } else if (err < 0) {
    APP_DPRINT("Error from writei: %s\n", snd_strerror(err));
    exit(1);
  }  else if (err != (int)(pBuffer->nFilledLen/(2*appPrvt->pcm->nChannels))) {
    /*APP_DPRINT("Short write, write %d frames\n", err);*/
  }
}
