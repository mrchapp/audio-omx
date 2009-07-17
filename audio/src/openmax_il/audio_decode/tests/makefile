# Debug Flags:
#     0 - NO DEBUG MESSAGES
#     1 - DEBUG MESSAGES are enabled

OMX_DEBUG ?= 0      # master switch: turn debug on or off

BASEINCLUDEDIR=$(OMXROOT)/system/src/openmax_il/omx_base/inc
OSALINCLUDEDIR=$(OMXROOT)/system/src/openmax_il/mm_osal/inc


SRC=\
	tiomxplayer.c \
	tiomxplayerutils.c


HSRC=$(wildcard ../inc/*)

OBJ=$(SRC:.c=.o)

include $(OMXROOT)/Master.mk

CFLAGS += -DALSA=1 -I$(TARGETDIR)/include -lasound
CFLAGS += -I$(OMXINCLUDEDIR) -I$(SYSTEMINCLUDEDIR)

ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -O0
else
#    CFLAGS += -g -ggdb -Werror -DMEMWATCH -DMEMWATCH_STDIO
endif

ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -DOMX_DEBUG=1
endif


OMXCORE_LIB=$(OMXLIBDIR)/libOMX_Core.so
COMPONENT_TEST=tiomxplayer

COMPONENT_TARGET=$(OMXTESTDIR)/$(COMPONENT_TEST)

all install:: $(COMPONENT_TARGET)

omxcore $(COMPONENT_TARGET): $(OMXTESTDIR) $(COMPONENT_TEST) $(OMXLIBDIR) $(OMXCORE_LIB)  
	@echo "Installing $(COMPONENT_TEST)"
	cp -f $(COMPONENT_TEST) $(COMPONENT_TARGET)

$(COMPONENT_TEST): $(OBJ)
	$(CROSS)gcc $(CFLAGS) -o $(COMPONENT_TEST) $(OBJ) -L$(OMXLIBDIR) -ldl -lOMX_Core -lpthread

$(SRC): $(HSRC)

clean:: 
	rm -f $(COMPONENT_TEST)
	rm -f $(OBJ)

distclean:: clean

clobber:: clean
	rm -f $(OMXTESTDIR)/$(COMPONENT_TEST)
