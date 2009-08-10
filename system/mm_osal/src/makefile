# Debug Flags:
#     0 - NO DEBUG MESSAGES
#     1 - DEBUG MESSAGES are enabled

OMX_DEBUG ?= 0      # master switch: turn debug on or off

SRC=\
	timm_osal.c \
	timm_osal_events.c \
	timm_osal_memory.c \
	timm_osal_mutex.c \
	timm_osal_pipes.c \
	timm_osal_semaphores.c \
	timm_osal_task.c \
	timm_osal_trace.c

HSRC=$(wildcard ../inc/*)

OBJ=$(SRC:.c=.o)

include $(OMXROOT)/Master.mk

CFLAGS+=-I../inc -I$(OMXINCLUDEDIR) -I$(SYSTEMINCLUDEDIR) -I$(BRIDGEINCLUDEDIR) -Wall -fpic -pipe -DOMAP_2430 -DOMX_DEBUG

ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -O0
else
    CFLAGS += -O2
endif

ifeq ($(OMX_DEBUG), 1)
    CFLAGS += -DOMX_DEBUG=1
endif

OMXCORE_LIB=$(OMXLIBDIR)/libOMX_Core.so
COMPONENT_LIB=libmmosal.so

COMPONENT_TARGET=$(OMXLIBDIR)/$(COMPONENT_LIB)

all install:: $(COMPONENT_TARGET)

omxcore $(COMPONENT_TARGET): $(COMPONENT_LIB) $(OMXLIBDIR) $(OMXCORE_LIB)
	@echo "Installing $(COMPONENT_LIB)"
	cp -f $(COMPONENT_LIB) $(COMPONENT_TARGET)

$(COMPONENT_LIB): $(OBJ)
#	$(CROSS)gcc $(CFLAGS) -shared -o $(COMPONENT_LIB) $(OBJ) -L$(BRIDGELIBDIR) -lbridge -L$(OMXLIBDIR) -lpthread
	ar rs $(COMPONENT_LIB) $(OBJ)

$(SRC): $(HSRC)

clean:: 
	rm -f $(COMPONENT_LIB)
	rm -f $(OBJ)

distclean:: clean

clobber:: clean
	rm -f $(OMXLIBDIR)/$(COMPONENT_LIB)
