## Process this file with automake to produce Makefile.in

SUBDIRS                 = src

lib_LTLIBRARIES         = libLCML.la

libLCML_la_SOURCES  = 
libLCML_la_LDFLAGS  = -no-undefined
libLCML_la_LIBADD   = src/libsrc.la

libLCML_la_include_HEADERS = inc/LCML_CodecInterface.h inc/LCML_DspCodec.h inc/LCML_Types.h inc/usn.h
libLCML_la_includedir   = $(includedir)/omx/lcml

pkgconfig_DATA          = libLCML.pc
pkgconfigdir            = $(libdir)/pkgconfig
