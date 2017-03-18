# Mothballed Standalone AoE Driver

This repository contains the standalone, source-only distribution of
the aoe driver.  Current Linux kernels contain an aoe driver v85 or
greater, and that in-kernel aoe driver has all the features of this
one (and possibly more).

The standalone driver was useful when more experimental features were
being developed and when the popular Linux distributions lacked an
up-to-date aoe driver.  Today the distro kernels have caught up, and
after installing the aoetools, you can use the aoe driver in your
distro's kernel after doing `modprobe aoe`.

Using your own aoe driver is the recommended way.  As the upstream
kernel changes, this standalone driver will become difficult to use
without repeated additions to the compatibility system under `conf/`.
The 4.8.x kernel and greater are not supported.

We hope to include documentation here helping with tips on how to
modify the aoe drivers inside the most popular Linux distributions by
using their kernel source packages.

Meanwhile, the old README is still here as *README-old.md* for the
adventurous.