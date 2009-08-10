# Debug Flags:
#     0 - NO DEBUG MESSAGES
#     1 - DEBUG MESSAGES are enabled

OMX_DEBUG ?= 0      # master switch: turn debug on or off

BASEINCLUDEDIR=$(OMXROOT)/system/src/openmax_il/omx_base/inc
OSALINCLUDEDIR=$(OMXROOT)/system/src/openmax_il/mm_osal/inc
AACDIR=$(OMXROOT)/audio/src/openmax_il/aac_dec
MP3DIR=$(OMXROOT)/audio/src/openmax_il/mp3_dec

SRC=\
	omx_audiodecoder.c \
	omx_audiodecutils.c \
	$(MP3DIR)/src/omx_audiodec_mp3utils.c\
	$(AACDIR)/src/omx_audiodec_aacutils.c


HSRC=$(wildcard ../inc/*)   

OBJ=$(SRC:.c=.o)

include $(OMXROOT)/Master.mk


CFLAGS+=-I../inc -I$(AACDIR)/inc -I$(MP3DIR)/inc -I$(OMXINCLUDEDIR) -I$(SYSTEMINCLUDEDIR) -I$(BASEINCLUDEDIR) -I$(OSALINCLUDEDIR) -I$(BRIDGEINCLUDEDIR) -Wall -fpic -pipe -DOMX_DEBUG
ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -O0
else
    CFLAGS += -O2
endif

ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -DOMX_DEBUG=1
endif


OMXCORE_LIB=$(OMXLIBDIR)/libOMX_Core.so
COMPONENT_LIB=libOMX.TI.AUDIO.DECODE.so

COMPONENT_TARGET=$(OMXLIBDIR)/$(COMPONENT_LIB)

all install:: $(COMPONENT_TARGET)

omxcore $(COMPONENT_TARGET): $(COMPONENT_LIB) $(OMXLIBDIR) $(OMXCORE_LIB)
	@echo "Installing $(COMPONENT_LIB)"
	cp -f $(COMPONENT_LIB) $(COMPONENT_TARGET)

$(COMPONENT_LIB): $(OBJ)
	$(CROSS)gcc $(CFLAGS) -g -shared -o $(COMPONENT_LIB) $(OBJ) -L$(OMXLIBDIR) -lOMX_Base

$(SRC): $(HSRC)

clean:: 
	rm -f $(COMPONENT_LIB)
	rm -f $(OBJ)

distclean:: clean

clobber:: clean
	rm -f $(OMXLIBDIR)/$(COMPONENT_LIB)
