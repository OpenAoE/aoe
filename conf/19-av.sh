f=linux/drivers/block/aoe/aoenet.c

nd_net="`fgrep 'if (ifp->nd_net != &init_net)' $f`"
test "$nd_net" && { echo nd_net; exit 0; }

dev_net="`fgrep 'if (dev_net(ifp) != &init_net)' $f`"
test "$dev_net" && { echo dev_net; exit 0; }

echo nons
