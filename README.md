# AoE Initiator Driver for Linux

## Introduction

This software is an AoE (ATA over Ethernet) storage initiator for
Linux kernels 2.6.18 and up.

The version number of this driver is unrelated to the version numbers
of Linux kernels.  Just use the latest aoe driver with your 2.6 or 3.x
Linux kernel.

[This driver is available on github.](https://github.com/OpenAoE/aoe)

Please see linux/Documentation/aoe.txt for basic information and
EtherDrive-2.6-HOWTO.sgml for more.

The aoetools (available separately at sourceforge.net) are required
for this aoe driver.  A compatible version is currently bundled with
this driver, but if you update the version that appears as "TOOLS_D"
in the Makefile, you can generally use something newer.


## Installation

After extracting aoetools, to build the ATA over Ethernet driver, just
type `make`.

Then, as root, do `make install` to make the driver available to your
system.  You may have to install the kernel sources provided by your
Linux distribution if you don't have them already.

The Makefile builds an aoe module for the currently-running kernel by
default.  The kernel sources in /lib/modules/`uname -r`/build are
used.  These sources should already be configured (e.g., with `make
menuconfig` or `make oldconfig`) and ready to be used for building.
The easiest way to make it ready for building is to do a `make` in
that directory.

If you need to build a module for a different kernel, make sure those
kernel sources are properly configured and ready for building.  Then
tell make where to find the kernel sources and where to install the
aoe module.

Here's an example where I'm running 2.6.16.62, and building an aoe
module for a different kernel, 2.6.32.60.  The make commands are each
typed as one long line, without a backslash.

    $ cd /tmp/aoe6-50
    $ uname -r
    2.6.16.62
    $ make KDIR=~/kernel/linux-2.6.32.60 \
      INSTDIR=/lib/modules/2.6.32.60/kernel/drivers/block/aoe 
    $ su -
    
    # cd /tmp/aoe6-50
    # make install KDIR=~ecashin/kernel/linux-2.6.32.60 \
      INSTDIR=/lib/modules/2.6.32.60/kernel/drivers/block/aoe 

If you have installed a previous version of the aoe driver, it is best
to get rid of any old files that might get in the way.  These two
commands remove old device files and any aoe modules for the running
kernel.

    rm -rf /dev/etherd
    find /lib/modules/`uname -r` -name aoe.ko -print0 | xargs -0 rm

If udev is detected, an aoe driver is built that uses dynamic minor
device numbers in order to support the maximum number of possible AoE
target addresses.  What that means to you is that if you plan to use
udev, it should be running when you build the aoe driver.

If udev is not detected, the makefile builds an old-fashioned aoe
driver (that uses predictable minor numbers for a smaller number of
AoE devices) and creates enough device files for ten EtherDrive
shelves.  If you want more device files, specify it on the make
command line.  For example, to create device files for fifteen
shelves, use the following command.

    make install N_DEV_SHELVES=15

If you are not running udev and you don't want the makefile to create
device nodes, you can do a `make install_nodev` instead of a `make
install`.

There is optional support for having only one partition per aoe
device.  This feature may be useful on systems that do not support
large minor device numbers well, e.g., debian sarge.

Some versions of mdadm do not handle large minor device numbers well.
To use this compatibility feature set the AOE_PARTITIONS variable in
the Makefile to 1.


## Support Information

Maintainer:

	Ed L. Cashin
	ecashin@acm.org

Original Author:

	Sam Hopkins

There is an aoetools-discuss mailing list at aoetools.sourceforge.net.


## Additional Information

Please see the linux/Documentation/aoe/aoe.txt and the other files in
that directory.

* [Linux kernel download site](http://www.kernel.org/)
* [Linux kernel howto (building, configuring, etc)](http://www.linuxdocs.org/HOWTOs/Kernel-HOWTO.html)
