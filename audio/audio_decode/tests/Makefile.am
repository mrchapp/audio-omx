## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libtiomxplayer.la
libtiomxplayer_la_SOURCES = tiomxplayer.c tiomxplayerutils.c
libtiomxplayer_la_LIBADD  = @LTLIBOBJS@ $(OMXCORE_LIBS) $(OMXBASE_LIBS) $(MMOSAL_LIBS) $(LCML_LIBS) $(ALSA_LIBS)
libtiomxplayer_la_CFLAGS  = -I. $(OMXCORE_CFLAGS) $(OMXBASE_CFLAGS) $(MMOSAL_CFLAGS) $(LCML_CFLAGS) $(ALSA_CFLAGS)

