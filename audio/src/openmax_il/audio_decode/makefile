

all::

# pull in the master "inherited" definitions
include $(OMXROOT)/Master.mk

DIRS= \
	src

EXTRA=\

$(BASETARGETS)::
	@$(call traverse_dirs,$(DIRS),$@)
