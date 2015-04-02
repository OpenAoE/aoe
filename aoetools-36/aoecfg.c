/*
 * aoecfgstr.c - fiddle aoe configuration strings.
 * Copyright 2009, Erik Quanstrom, CORAID, Inc., Licenced under GPL v2
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <arpa/inet.h>
#include "dat.h"
#include "fns.h"

int	sfd;
uchar	mac[6];
int	timeout;
u16	shelf	= 0xffff;
uchar	slot	= 0xff;
char	*net 	= "eth0";
char	*cfgstr;
int	cfgstrlen;

char *errtab[7] = {
	"*GOK*",
	"*badcmd*",
	"*badarg*",
	"*baddev*",
	"*badcfg*",
	"*badvers*",
	"*GOK*",
};

void
resp(Conf *c)
{
	Aoehdr *h;
	char *s;
	int l;

	h = (Aoehdr*) c;
	if (h->flags & Error) {
		s = errtab[h->error & 7];
		l = strlen(s);
	} else {
		s = (char*) c->data;
		l = ntohs(c->len);
	}
	if (shelf != 0xffff && slot != 0xff)
		printf("%.*s\n", l, s);
	else
		printf("%d.%d\t%.*s\n", ntohs(h->maj), h->min, l, s);
}

int
readto(int fd, void *buf, int size)
{
	fd_set rfd;
	struct timeval tv;
	static int to;

	if (timeout == 0)
		goto f1;
	if (to == 0)
		to = time(0) + timeout;
	FD_ZERO(&rfd);
	FD_SET(fd, &rfd);

	tv.tv_sec = to - time(0);
	tv.tv_usec = 0;

	switch (select(fd+1, &rfd, 0, 0, &tv)) {
	case -1:
		perror("select");
		exit(1);
	case 0:
		exit(0);
	}
f1:
	return read(fd, buf, size);
}

u32
aoe_tag(void)
{
	u32 n;
	struct timeval t = { 0, 0 };

	/* Tag should be just enough to avoid conflicts with other
	 * aoeping and aoecfg processes, and should set high bit
	 * to avoid conflicting with in-kernel AoE.
	 */
	if (gettimeofday(&t, NULL)) {
		perror("gettimeofday");
		exit(1);
	}
	n = t.tv_usec | 1UL << 31;
	return htonl(n);
}

void
cfgquery(int cmd, int shelf, int slot)
{
	int n;
	Aoehdr *h;
	Conf *c;
	uchar buf[1024 + sizeof *h];
	u32 tag;

	c = (Conf*) buf;
	h = (Aoehdr*) c;
	memset(h, 0, sizeof *h);
	memset(h->dst, 0xff, sizeof h->dst);
	memmove(h->src, mac, sizeof h->src);
	
	h->type = htons(AOE_ETH_PROTO);
	h->flags = AoEver << 4;
	h->maj = htons(shelf);
	h->min = slot;
	h->cmd = Config;
	tag = aoe_tag();
	memmove(h->tag, &tag, sizeof h->tag);
	c->bufcnt = 0;
	c->firmware = 0;
	c->scnt = 0;
	c->vercmd = cmd;
	memmove(c->data, cfgstr, cfgstrlen);
	c->len = htons(cfgstrlen);
	if (write(sfd, c, sizeof *c) == -1) {
		perror("send config query");
		exit(1);
	}
	for (;;) {
		n = readto(sfd, buf, sizeof buf);
		if (n < 0) {
			perror("read network");
			exit(1);
		}
		if (n < 60)
			continue;
		h = (Aoehdr *) buf;
		if (ntohs(h->type) != AOE_ETH_PROTO)
			continue;
		if (ntohs(h->maj) == 0xffff || h->min == 0xff)
			continue;
		if (shelf != 0xffff && ntohs(h->maj) != shelf)
			continue;
		if (slot != 0xff && h->min != slot)
			continue;
		if (memcmp(h->tag, &tag, sizeof h->tag))
			continue;
		resp((Conf*) buf);
		if (shelf != 0xffff && slot != 0xff)
			break;
	}
}

void
usage(void)
{
	fputs("usage: aoecfg "
		"[-c cmd] [-s cfgstr] [-t timeout] "
		"[shelf slot] [net]\n", stderr);
	exit(1);
}

typedef struct{
	char	*s;
	int	cmd;
} Tab;

Tab tab[] = {
	{ "read",	Qread, },
	{ "test",	Qtest, },
	{ "prefix",	Qprefix, },
	{ "set",	Qset, },
	{ "fset",	Qfset, },
};

int
xlatecmd(char *s)
{
	int i;

	for (i = 0; i < nelem(tab); i++)
		if (strcmp(tab[i].s, s) == 0)
			return tab[i].cmd;
	usage();
	return -1;	// whine whine whine
}

int
main(int argc, char *argv[])
{
	int c, cmd;

	cmd = Qread;
	while ((c = getopt(argc, argv, "s:c:t:v")) != -1) {
		switch (c) {
		case 'c':
			cmd = xlatecmd(optarg);
			break;
		case 's':
			cfgstr = optarg;
			cfgstrlen = strlen(cfgstr);
			break;
		case 't':
			timeout = atoi(optarg);
			break;
		default:
			usage();
		}
	}

	c = argc - optind;
	argv += optind;
	if (c < 2)
		goto f1;
	if (c != 3 && c != 2)
		usage();
	shelf = strtoul(*argv++, 0, 0);
	slot = strtoul(*argv++, 0, 0);
f1:
	if (*argv) {
		net = *argv;
		if (isdigit((int) *net)) {
			fprintf(stderr,
				"aoecfg Error: \"%s\" %s\n", net,
				"is not a valid network interface");
			usage();
		}
	}
	sfd = dial(net);
	if (getea(sfd, net, mac) == 0)
		exit(1);
	cfgquery(cmd, shelf, slot);
	return 0;
}
