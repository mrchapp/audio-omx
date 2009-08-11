## Process this file with automake to produce Makefile.in

SUBDIRS                 = base aac_dec mp3_dec

lib_LTLIBRARIES         = libOMX.TI.AUDIO.DECODE.la

libOMX_TI_AUDIO_DECODE_la_SOURCES  = 
libOMX_TI_AUDIO_DECODE_la_LDFLAGS  = -no-undefined
libOMX_TI_AUDIO_DECODE_la_LIBADD   = base/libbase.la aac_dec/libaac_dec.la mp3_dec/libmp3_dec.la

libOMX_TI_AUDIO_DECODE_includedir   = $(includedir)/omx/audio_decode

pkgconfig_DATA          = libOMX.TI.AUDIO.DECODE.pc
pkgconfigdir            = $(libdir)/pkgconfig

