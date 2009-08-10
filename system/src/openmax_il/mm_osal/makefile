

all::

# pull in the master "inherited" definitions
include $(OMXROOT)/Master.mk

DIRS= \
	src \
	inc 

EXTRA=\

$(BASETARGETS)::
	@$(call traverse_dirs,$(DIRS),$@)
