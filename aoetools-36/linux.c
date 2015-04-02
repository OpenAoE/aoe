/* linux.c: low level access routines for Linux
 * Copyright 2009, CORAID, Inc., and licensed under GPL v.2.
 */
#include "config.h"
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <features.h>    /* for the glibc version number */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>     /* the L2 protocols */
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>   /* The L2 protocols */
#endif

#include <sys/ioctl.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/fs.h>
#include <sys/stat.h>

#include "dat.h"
#include "fns.h"

static int
getindx(int s, char *name)	// return the index of device 'name'
{
	struct ifreq xx;
	int n;

	strcpy(xx.ifr_name, name);
	n = ioctl(s, SIOCGIFINDEX, &xx);
	if (n == -1)
		return -1;
	return xx.ifr_ifindex;
}

int
dial(char *eth)		// get us a raw connection to an interface
{
	int i;
	int n, s;
	struct sockaddr_ll sa;
	enum { aoe_type = 0x88a2 };

	memset(&sa, 0, sizeof sa);
	s = socket(PF_PACKET, SOCK_RAW, htons(aoe_type));
	if (s == -1) {
		perror("got bad socket");
		return -1;
	}
	i = getindx(s, eth);
	sa.sll_family = AF_PACKET;
	sa.sll_protocol = htons(0x88a2);
	sa.sll_ifindex = i;
	n = bind(s, (struct sockaddr *)&sa, sizeof sa);
	if (n == -1) {
		perror("bind funky");
		return -1;
	}
	return s;
}

int
getea(int s, char *name, uchar *ea)
{
	struct ifreq xx;
	int n;

	strcpy(xx.ifr_name, name);
	n = ioctl(s, SIOCGIFHWADDR, &xx);
	if (n == -1) {
		perror("Can't get hw addr");
		return 0;
	}
	memmove(ea, xx.ifr_hwaddr.sa_data, 6);
	return 1;
}
