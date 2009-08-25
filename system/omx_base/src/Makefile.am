## Process this file with automake to produce Makefile.in

noinst_LTLIBRARIES      = libsrc.la
libsrc_la_SOURCES       = omx_base.c \
        omx_base_callbacks.c \
        omx_base_internal.c \
        omx_base_process.c
libsrc_la_LIBADD        = @LTLIBOBJS@ $(OMXCORE_LIBS) $(MMOSAL_LIBS) $(TTIF_LIBS)
libsrc_la_CFLAGS        = -I. -I$(top_srcdir)/inc $(OMXCORE_CFLAGS) $(MMOSAL_CFLAGS) $(TTIF_CFLAGS) $(TTIF_TRACES) -DSTATIC_TABLE

