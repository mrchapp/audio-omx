## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libsrc.la
libsrc_la_SOURCES       = LCML_DspCodec.c
libsrc_la_LIBADD        = @LTLIBOBJS@ $(OMXCORE_LIBS) $(DSPBRIDGE_LIBS) $(PERF_LIBS)
libsrc_la_CFLAGS        = -I. -I$(top_srcdir)/inc $(OMXCORE_CFLAGS) $(DSPBRIDGE_CFLAGS) $(PERF_CFLAGS) $(PERF_INSTRUMENTATION)

