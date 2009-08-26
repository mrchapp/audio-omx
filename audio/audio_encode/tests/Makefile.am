## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libtiomxrecorder.la
libtiomxrecorder_la_SOURCES = tiomxrecorder.c tiomxrecorderutils.c
libtiomxrecorder_la_LIBADD  = @LTLIBOBJS@ $(OMXCORE_LIBS) $(OMXBASE_LIBS) $(MMOSAL_LIBS) $(LCML_LIBS) $(ALSA_LIBS)
libtiomxrecorder_la_CFLAGS  = -I. $(OMXCORE_CFLAGS) $(OMXBASE_CFLAGS) $(MMOSAL_CFLAGS) $(LCML_CFLAGS) $(ALSA_CFLAGS)

