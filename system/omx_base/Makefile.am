## Process this file with automake to produce Makefile.in

SUBDIRS                 = src

lib_LTLIBRARIES         = libOMX_Base.la

libOMX_Base_la_SOURCES  = 
libOMX_Base_la_LDFLAGS  = -no-undefined
libOMX_Base_la_LIBADD   = src/libsrc.la

libOMX_Base_la_include_HEADERS = inc/omx_base.h inc/omx_base_internal.h inc/omx_base_utils.h
libOMX_Base_la_includedir   = $(includedir)/omx/base

pkgconfig_DATA          = libOMX_Base.pc
pkgconfigdir            = $(libdir)/pkgconfig
