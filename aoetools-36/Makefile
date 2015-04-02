# Makefile

# You can edit this file or override these variables on
# the commandline.  For example, "make install MANDIR=/tmp/man"
# would use defaults for all variables except ${MANDIR}.


# You can set this to something like /opt/aoetools-x
# if you want to install everything in one place.
#
# Note that even with ${PREFIX} set, the devices in
# /dev/etherd will be used unless you override ${DEVDIR}.
PREFIX = 

# Your aoe-driver device files should be in ${DEVDIR}.
DEVDIR = /dev/etherd

# The programs will be installed in ${SBINDIR}.
SBINDIR = ${PREFIX}/usr/sbin
MANDIR = ${PREFIX}/usr/share/man

# NPERSHELF is the number of slot addresses per shelf address.
# This number must match the same setting in the aoe driver.
#
# Older aoe drivers have NPERSHELF set to 10, and newer ones 
# use 16.
#
# You can see the setting in your driver like this in the driver
# source directory.  (Change into the "linux" directory if you're
# using the standalone driver from CORAID.)
#
#   grep 'NPERSHELF.*=' drivers/block/aoe/aoe.h
#
NPERSHELF=16

# end of user-configurable variables


# these scripts are created from the *.in files
CONF_SCRIPTS = aoe-discover aoe-interfaces aoe-mkshelf aoe-revalidate aoe-flush aoe-stat
PROGS = aoeping aoecfg aoe-sancheck
COMMANDS := ${CONF_SCRIPTS} aoe-mkdevs aoe-version coraid-update ${PROGS}
CFLAGS = -Wall -O -g
SANCHECKLIBS = -lpthread

AOE_PING_OBJ = aoeping.o linux.o
AOE_CFG_OBJ = aoecfg.o linux.o
SANCHECK_OBJ = aoe-sancheck.o

all : configure ${PROGS}
	@true

configure :
	@for f in ${CONF_SCRIPTS}; do \
	  sh -xc "sed -e 's!@devdir@!${DEVDIR}!g' -e 's!@npershelf@!${NPERSHELF}!g' $$f.in > $$f" || break; \
	done

# DESTDIR was put in for Rob Holland to make gentoo packaging easier
install : all
	mkdir -p ${DESTDIR}${SBINDIR}
	mkdir -p ${DESTDIR}${MANDIR}/man8
	@for f in ${COMMANDS}; do \
	  sh -xc "install -m 700 $$f ${DESTDIR}${SBINDIR}/$$f" || break; \
	  sh -xc "install -m 664 $$f.8 ${DESTDIR}${MANDIR}/man8/$$f.8" || break; \
	done

clean :
	rm -f ${CONF_SCRIPTS} ${AOE_PING_OBJ} ${AOE_CFG_OBJ} ${SANCHECK_OBJ} ${PROGS}

aoeping : ${AOE_PING_OBJ}
	${CC} ${CFLAGS} -o $@ ${AOE_PING_OBJ}
aoeping.o : aoeping.c dat.h fns.h
	${CC} ${CFLAGS} -o $@ -c $<
linux.o : linux.c config.h
	${CC} ${CFLAGS} -o $@ -c $<
aoecfg: ${AOE_CFG_OBJ}
	${CC} ${CFLAGS} -o $@ ${AOE_CFG_OBJ}
aoecfg.o : aoecfg.c dat.h fns.h
	${CC} ${CFLAGS} -o $@ -c $<
aoe-sancheck : ${SANCHECK_OBJ}
	-$(CC) $(CFLAGS) -o $@ ${SANCHECK_OBJ} $(SANCHECKLIBS)
aoe-sancheck.o : aoe-sancheck.c
	-$(CC) $(CFLAGS) -o $@ -c $<

