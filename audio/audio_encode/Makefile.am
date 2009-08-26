## Process this file with automake to produce Makefile.in

SUBDIRS                 = base aac_enc

lib_LTLIBRARIES         = libOMX.TI.AUDIO.ENCODE.la

libOMX_TI_AUDIO_ENCODE_la_SOURCES  = 
libOMX_TI_AUDIO_ENCODE_la_LDFLAGS  = -no-undefined
libOMX_TI_AUDIO_ENCODE_la_LIBADD   = base/libbase.la aac_enc/libaac_enc.la

if BUILD_TESTS
  SUBDIRS               += tests
  bin_PROGRAMS           = tiomxrecorder
  tiomxrecorder_SOURCES    = 
  tiomxrecorder_LDFLAGS    = -no-undefined
  tiomxrecorder_LDADD      = tests/libtiomxrecorder.la
endif

libOMX_TI_AUDIO_ENCODE_la_includedir   = $(includedir)/omx/audio_encode

pkgconfig_DATA          = libOMX.TI.AUDIO.ENCODE.pc
pkgconfigdir            = $(libdir)/pkgconfig

