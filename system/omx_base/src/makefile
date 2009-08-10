# Debug Flags:
#     0 - NO DEBUG MESSAGES
#     1 - DEBUG MESSAGES are enabled

OMX_DEBUG ?= 0      # master switch: turn debug on or off

SRC=\
	omx_base.c \
	omx_base_callbacks.c \
	omx_base_internal.c \
	omx_base_process.c 


HSRC=$(wildcard ../inc/*)

OBJ=$(SRC:.c=.o)

include $(OMXROOT)/Master.mk

CFLAGS+=-I../inc -I../../mm_osal/inc -I$(SYSTEMINCLUDEDIR) -I$(OMXINCLUDEDIR)
ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -O0
else
    CFLAGS += -O2
endif

ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -DOMX_DEBUG=1
endif

OMXCORE_LIB=$(OMXLIBDIR)/libOMX_Core.so
COMPONENT_LIB=libOMX_Base.so

COMPONENT_TARGET=$(OMXLIBDIR)/$(COMPONENT_LIB)

all install:: $(COMPONENT_TARGET)

omxcore $(COMPONENT_TARGET): $(COMPONENT_LIB) $(OMXLIBDIR) $(OMXCORE_LIB)
	@echo "Installing $(COMPONENT_LIB)"
	cp -f $(COMPONENT_LIB) $(COMPONENT_TARGET)

$(COMPONENT_LIB): $(OBJ)
	$(CROSS)gcc $(CFLAGS) -shared -o $(COMPONENT_LIB) $(OBJ) -L$(OMXLIBDIR) -lmmosal

$(SRC): $(HSRC)

clean:: 
	rm -f $(COMPONENT_LIB)
	rm -f $(OBJ)

distclean:: clean

clobber:: clean
	rm -f $(OMXLIBDIR)/$(COMPONENT_LIB)
