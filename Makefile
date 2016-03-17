# makefile for out-of-kernel AoE build
# uses pre-configured 2.6 kernel sources
# 

.PHONY: copy default tools clean realclean realrealclean \
  install install_modobj install_nodev kerninst uninstall report

# export all directives to sub makes (aoetools)
export

# where the aoe-stat, aoe-discover, etc. commands will go
SBINDIR = /usr/sbin
MANDIR = /usr/share/man

# the directory in /dev to place device nodes
#DEVSUBDIR=
DEVSUBDIR=etherd

# where the devices nodes will be created
DEVDIR=/dev/$(DEVSUBDIR)

# number of shelves for which dev nodes are created
# (numbering starts at 0)
N_DEV_SHELVES=10

DYNDEVS := $(shell conf/detect-udev)

# number of slots per shelf for static minor device node numbers
NPERSHELF=16

# the number of partitions supported per device; default is 16
# (set to 1 for greater compatibility on systems with poor support
#  for large minor device numbers)
AOE_PARTITIONS=16

DRIVER_D = linux/drivers/block/aoe
SCRIPTS_D = linux/Documentation/aoe
TOOLS_D = aoetools-36

# Get the kernel version
KVER := $(shell uname -r)

# kernel sources for building third-party modules should be found here
KDIR := /lib/modules/$(KVER)/build

# INSTDIR is where to put the aoe module
INSTDIR := /lib/modules/$(KVER)/kernel/drivers/block/aoe

PWD	:= $(shell pwd)
EXTRA_CFLAGS := -DAOE_PARTITIONS=$(AOE_PARTITIONS) -DDEVSUBDIR='\"${DEVSUBDIR}\"'
EXTRA_CFLAGS += -DAOE_DYNDEVS=${DYNDEVS}
KMAK_FLAGS := \
	  CONFIG_ATA_OVER_ETH=m \
	  KDIR=${KDIR}

DUMMY := $(shell sh mak/cc-id.sh ${MAKE} -C $(KDIR) $(KMAK_FLAGS) SUBDIRS="$(PWD)/mak" modules)
DUMMY := $(shell sh mak/kernel-id.sh ${KDIR})
DUMMY := $(shell sh mak/src-id.sh ${DRIVER_D})

COPY_DIR = $(HOME)/build/aoe

default: tools ${DRIVER_D}/aoe.ko

${DRIVER_D}/aoe.ko : conf/done mak/src-id.ts
	$(MAKE) -C $(KDIR) $(KMAK_FLAGS) SUBDIRS="$(PWD)/$(DRIVER_D)" EXTRA_CFLAGS="${EXTRA_CFLAGS}" modules
	@if test "${DYNDEVS}" = "1"; then \
		echo "This aoe driver uses dynamic minor device numbers."; \
	else \
		echo "This aoe driver uses static minor device numbers."; \
	fi

# sanity checks and handling of compile-time resolvable compatibility issues
conf/done : mak/cc-id.ts mak/kernel-id.ts
	@perl -e '' || { \
	  echo "Error: the build system depends on perl being available" 1>&2; \
	  false; \
	}
	sh -c "cd $(DRIVER_D) && rm -f *.o *.ko core"
	@test -r "$(KDIR)/.config" || { \
	  echo "Error: $(KDIR) sources are not configured." 1>&2; \
	  false; \
	}
	@if ! test -r "$(KDIR)/include/asm" && \
	  ! test -r "$(KDIR)/include/generated/autoconf.h"; then \
	  echo "Error: $(KDIR) sources are not prepared." 1>&2; \
	  false; \
	fi
	@if ! test -r "$(KDIR)/scripts/mod/modpost"; then \
	  echo "Error: $(KDIR) scripts are not present." 1>&2; \
	  echo "  try running 'make scripts' in $(KDIR)" 1>&2; \
	  false; \
	fi
	@printf "ensuring compatibility ... "
	@cd conf && rm -rf *.o *.ko .tmp_versions .*.*o.cmd .*.*o.d *.mod.c
	@sh conf/compat.sh $(DRIVER_D) \
	  $(MAKE) -C $(KDIR) $(KMAK_FLAGS) SUBDIRS="$(PWD)/conf" C=0 modules
	echo > conf/done
	@sh mak/src-id.sh ${DRIVER_D}

copy:
	@cpio -p `pwd` < /dev/null 2>&0 || { \
	  echo Error: cpio is required for copy target 1>&2; \
	  exit 1; \
	}
	mkdir -p $(COPY_DIR)
	find . -name .git -type d -prune -o -print | cpio -p --verbose $(COPY_DIR)

tools:
	cd ${TOOLS_D} && make 
clean:
	rm -f kv.mak conf/kdir.last
	cd $(DRIVER_D) && rm -f *.o *.ko core
	cd conf && rm -rf *.o *.ko .tmp_versions .*.*o.cmd .*.*o.d *.mod.c
	cd ${TOOLS_D} && make clean

realclean: clean
	rm -f conf/make.log
	cd $(DRIVER_D) && rm -rf .tmp_versions .aoe*.*o.cmd .aoe*.*o.d aoe.mod.c
	cd $(DRIVER_D) && rm -rf Module.symvers
	cd conf && rm -f *.log Makefile

# revert conf patches
realrealclean: realclean
	-sh conf/compat.sh -R $(DRIVER_D) \
	  $(MAKE) -C $(KDIR) $(KMAK_FLAGS) SUBDIRS="$(PWD)/conf" modules
	cd $(DRIVER_D) && rm -rf *.orig *.rej

install_modobj: default
	@echo "Install directory is $(INSTDIR)"
	mkdir -p $(INSTDIR)
	install -m 644 $(DRIVER_D)/aoe.ko $(INSTDIR)
	/sbin/depmod -a

install_nodev: install_modobj
# punt if DEVSUBDIR isn't the default.  probably should have cleaner solution.
	@if test -r /etc/udev/udev.conf; then \
	  if test "$(DEVSUBDIR)" != "etherd" || \
	  ! conf=/etc/udev/udev.conf sh $(SCRIPTS_D)/udev-install.sh; then \
	    (echo ; echo "!! DID NOT INSTALL UDEV RULES FOR AOE"; echo) 1>&2; \
	  fi; \
	fi
	cd ${TOOLS_D} && make install
install: install_nodev
	@if test "${DYNDEVS}" = "0"; then \
		echo "creating static device nodes in ${DEVDIR}"; \
		n_partitions=$(AOE_PARTITIONS) \
		  n_shelves=${N_DEV_SHELVES} ${TOOLS_D}/aoe-mkdevs $(DEVDIR); \
	fi

# experts can put the driver in a kernel source tree with
# "make kerninst"
kerninst: conf/done mak/src-id.ts
	test -d ${KDIR}/drivers/block/aoe
	cp -b ${DRIVER_D}/*.[ch] ${KDIR}/drivers/block/aoe
	echo "#define AOE_PARTITIONS (${AOE_PARTITIONS})" \
	  >> ${KDIR}/drivers/block/aoe/aoe.h
	echo '#define DEVSUBDIR "${DEVSUBDIR}"' \
	  >> ${KDIR}/drivers/block/aoe/aoe.h
	echo '#define AOE_DYNDEVS (${DYNDEVS})' \
	  >> ${KDIR}/drivers/block/aoe/aoe.h
	echo 'aoe-objs += aoedbg.o' \
	  >> ${KDIR}/drivers/block/aoe/Makefile

uninstall:
	@echo "Removing Module from $(INSTDIR)"
	rm -f $(INSTDIR)/aoe.ko
	@echo "Removing $(DEVDIR) and components"
	rm -rf $(DEVDIR)
	/sbin/depmod -a

REPORTF := $(shell awk '$$2=="VERSION"{print $$3}' ${DRIVER_D}/aoe.h)
# remove quotes below
REPORTF := $(shell echo aoe6-${REPORTF}-report.tar)

report:
	tar cf ${REPORTF} conf/*.log
	gzip ${REPORTF}
	@echo The report file is "${REPORTF}.gz".
