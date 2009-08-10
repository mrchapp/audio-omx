# every thing is relative to where we are
export OMXROOT=$(shell /bin/pwd)

all::

# pull in the master "inherited" definitions
include $(OMXROOT)/Master.mk

subcomp=\
        target=$(3:$(1).%=%); export target; \
        if [ "$$target" = "$(3)" -o "$$target" = "%" ] ; then target=all; fi; \
        for i in $(2); do \
                (\
                if [ ! -d $$i ]; then continue; fi; \
                cd $$i; \
                $(MAKE) $$target; \
                ); \
                result=$$?; \
                if [ $$result -ne 0 ] ; then exit $$result; fi; \
        done; \
        exit $$result



DIRS= \
        system \
        audio
ifdef INST2
DIRS+= utilities/src/inst2
endif

$(BASETARGETS)::
        @$(call traverse_dirs,$(DIRS),$@)

ifdef OMX_PERF_INSTRUMENTATION
perf perf.%:
        $(call subcomp,perf,system/perf,$@)
else
perf perf.%:

endif

ifdef INST2
inst2 inst2.%:
        $(call subcomp,inst2,utilities/src/inst2,$@)
else
inst2 inst2.%:
endif

common common.%:
        $(call subcomp,common,system/common,$@)

core core.%: perf.% common.%
        $(call subcomp,core,system/omx_core,$@)

lcml lcml.%: core.%
        @$(call subcomp,lcml,system/lcml,$@)

resource_activity_monitor resource_activity_monitor.%: core.%
        @$(call subcomp,resource_activity_monitor,system/resource_manager/resource_activity_monitor,$@)

modem_app modem_app.%: core.%
        @$(call subcomp,modem_app,system/modem_app,$@)

clock clock.%: core.%
        @$(call subcomp,clock,system/clock_source,$@)

audio_manager audio_manager.%: core.%
        @$(call subcomp,audio_manager,system/audio_manager,$@)

resource_manager_proxy resource_manager_proxy.%: core.%
        @$(call subcomp,resource_manager_proxy,system/resource_manager_proxy,$@)

resource_manager resource_manager.%: core.% resource_activity_monitor.% omx_policy_manager.%
        @$(call subcomp,resource_manager,system/resource_manager,$@)

omx_policy_manager omx_policy_manager.%: core.%
        @$(call subcomp,omx_policy_manager,system/omx_policy_manager,$@)

mm_osal mm_osal.%: core.%
        $(call subcomp,mm_osal,system/mm_osal,$@)

omx_base omx_base.%: core.% mm_osal.% lcml.%
        $(call subcomp,omx_base,system/omx_base,$@)

jpeg_dec jpeg_dec.%: core.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,jpeg_dec,image/jpeg_dec,$@)

jpeg_enc jpeg_enc.%: core.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,jpeg_enc,image/jpeg_enc,$@)

camera_algo camera_algo.%:
        @$(call subcomp,camera_algo,../algo/make/linux,$@)

camera_algo_frmwk camera_algo_frmwk.%: camera_algo.%
        @$(call subcomp,camera_algo_frmwk,mm_isp,$@)

camera camera.%: core.% resource_manager_proxy.% resource_manager.% lcml.% clock.% post_processor.% camera_algo_frmwk.%
        @$(call subcomp,camera,video/camera,$@)

post_processor post_processor.%: core.% resource_manager_proxy.% resource_manager.% lcml.% clock.%
        @$(call subcomp,post_processor,video/post_processor,$@)

video_decode video_decode.%: core.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,video_decode,video/video_decode,$@)

video_encode video_encode.%: core.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,video_encode,video/video_encode,$@)

prepost_processor prepost_processor.%: core.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,prepost_processor,video/prepost_processor,$@)

audio_decode audio_decode.%: core.% omx_base.% lcml.%
        $(call subcomp,audio_decode,audio/audio_decode,$@)

g729_dec g729_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g729_dec,audio/g729_dec,$@)

g729_enc g729_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g729_enc,audio/g729_enc,$@)

g711_enc g711_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g711_enc,audio/g711_enc,$@)

g711_dec g711_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g711_dec,audio/g711_dec,$@)

g723_dec g723_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g723_dec,audio/g723_dec,$@)

g723_enc g723_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g723_enc,audio/g723_enc,$@)

g726_dec g726_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g726_dec,audio/g726_dec,$@)

g726_enc g726_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g726_enc,audio/g726_enc,$@)

gsmfr_dec gsmfr_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,gsmfr_dec,audio/gsmfr_dec,$@)

gsmfr_enc gsmfr_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,gsmfr_enc,audio/gsmfr_enc,$@)

gsmhr_dec gsmhr_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,gsmhr_dec,audio/gsmhr_dec,$@)

gsmhr_enc gsmhr_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,gsmhr_enc,audio/gsmhr_enc,$@)

pcm_enc pcm_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,pcm_enc,audio/pcm_enc,$@)

pcm_dec pcm_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,pcm_dec,audio/pcm_dec,$@)

nbamr_dec nbamr_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,nbamr_dec,audio/nbamr_dec,$@)

nbamr_enc nbamr_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,nbamr_enc,audio/nbamr_enc,$@)

ilbc_dec ilbc_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,ilbc_dec,audio/ilbc_dec,$@)

ilbc_enc ilbc_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,ilbc_enc,audio/ilbc_enc,$@)

imaadpcm_dec imaadpcm_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,imaadpcm_dec,audio/imaadpcm_dec,$@)

imaadpcm_enc imaadpcm_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,imaadpcm_enc,audio/imaadpcm_enc,$@)

mp3_dec mp3_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,mp3_dec,audio/mp3_dec,$@)

aac_dec aac_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,aac_dec,audio/aac_dec,$@)

aac_enc aac_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,aac_enc,audio/aac_enc,$@)

armaac_enc armaac_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,armaac_enc,audio/armaac_enc,$@)

armaac_dec armaac_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,armaac_dec,audio/armaac_dec,$@)

wbamr_enc wbamr_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,wbamr_enc,audio/wbamr_enc,$@)

wbamr_dec wbamr_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,wbamr_dec,audio/wbamr_dec,$@)

wma_dec wma_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,wma_dec,audio/wma_dec,$@)

g722_enc g722_enc.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g722_enc,audio/g722_enc,$@)

g722_dec g722_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.%
        @$(call subcomp,g722_dec,audio/g722_dec,$@)

audio_docs audio_docs.%:
        $(call subcomp,audio_docs,audio/docs/release/unit_test/23.x,$@)

video_docs video_docs.%:
        $(call subcomp,video_docs,video/docs/release/unit_test/23.x,$@)

docs docs.%: audio_docs.% video_docs.%

video_capture video_capture.%: core.% resource_manager_proxy.% resource_manager.% lcml.% camera.% video_encode.%
        @$(call subcomp,video_capture,application/video_capture,$@)

image_display image_display.%: core.% resource_manager_proxy.% resource_manager.% lcml.% jpeg_dec.% post_processor.% prepost_processor.% video_decode.%
        @$(call subcomp,image_display,application/image_display,$@)

image_capture image_capture.%: core.% resource_manager_proxy.% resource_manager.% lcml.% camera.% jpeg_enc.% post_processor.% prepost_processor.% video_encode.%
        @$(call subcomp,image_capture,application/image_capture,$@)

video_display video_display.%: core.% resource_manager_proxy.% resource_manager.% lcml.% video_decode.% post_processor.%
        @$(call subcomp,video_display,application/video_display,$@)

avplay avplay.%: core.% resource_manager_proxy.% resource_manager.% lcml.% video_decode.% post_processor.% nbamr_dec.% aac_dec.% wbamr_dec.% wma_dec.% rv_decode.% ragecko_dec.%
        @$(call subcomp,avplay,application/avplay,$@)

avrecord avrecord.%: core.% resource_manager_proxy.% resource_manager.% lcml.% camera.% video_encode.% nbamr_enc.% aac_enc.% wbamr_enc.%
        @$(call subcomp,avrecord,application/avrecord,$@)

rm_parser rm_parser.%: core.%
        @$(call subcomp,rm_parser,video/rm_parser,$@)

rm_rvparser rm_rvparser.%:
        @$(call subcomp,rm_rvparser,video/rm_rvparser,$@)

rv_decode rv_decode.%: core.% resource_manager_proxy.% resource_manager.% lcml.% rm_parser.% rm_rvparser.%
        @$(call subcomp,rv_decode,video/rv_decode,$@)

ragecko_dec ragecko_dec.%: core.% audio_manager.% resource_manager_proxy.% resource_manager.% lcml.% rm_parser.%
        @$(call subcomp,ragecko_dec ,audio/ragecko_dec,$@)
