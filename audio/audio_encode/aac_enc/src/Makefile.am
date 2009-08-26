## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libsrc.la
libsrc_la_SOURCES       = omx_audioenc_aacutils.c
libsrc_la_LIBADD        = @LTLIBOBJS@ $(OMXCORE_LIBS) $(OMXBASE_LIBS) $(MMOSAL_LIBS) $(LCML_LIBS)
libsrc_la_CFLAGS        = -I. -I$(top_srcdir)/base/inc -I$(top_srcdir)/aac_enc/inc $(OMXCORE_CFLAGS) $(OMXBASE_CFLAGS) $(MMOSAL_CFLAGS) $(LCML_CFLAGS)

