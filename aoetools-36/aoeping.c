/* aoeping.c - userland aoe pinger
 * Copyright 2009, CORAID, Inc., and licensed under GPL v.2.
 *
 * run without arguments for usage
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <ctype.h>
#include <netinet/in.h>
#include <linux/hdreg.h>
#include <errno.h>
#include "dat.h"
#include "fns.h"

struct progopts {
	int shelf;
	int slot;
	char *netif;
	int verbose;
	int timeout;
	u32 tag;
	char *smart;
	char ata_ident;
	char pp_ataid;		/* pretty print ATA device identify response */
};

static struct progopts defaults = {
	.shelf = 0,
	.slot = 0,
	.netif = NULL,
	.verbose = 0,
	.timeout = 0,
	.tag = 0,
	.smart = NULL,
	.ata_ident = 0,
	.pp_ataid = 0,
};
static struct progopts opts;

struct smartcmd {
	char *name;		/* subcommand name from ATA spec */
	int cmd;		/* for features register */
	char data;		/* does this subcommand xfer data? */
};
static struct smartcmd smarts[] = {
	{ "read_data", 0xd0, SmartDataRet },
	// { "attr_autosave", 0xd2, 0 }, (unsupported b/c it overloads sector count)
	{ "offline_immediate", 0xd4, 0 },
	{ "read_log", 0xd5, SmartDataRet },
	{ "write_log", 0xd6, SmartDataPut },
	{ "enable", 0xd8, 0 },
	{ "disable", 0xd9, 0 },
	{ "return_status", 0xda, 0 },
};

static char *progname;
static int sfd;			/* raw socket file descriptor */
static uchar mac[6];

void
usage(void)
{
	fprintf(stderr,
		"usage:\t%s [options] {shelf} {slot} {netif}\n",
		progname);
	fprintf(stderr, "%s\n\t%s\n\t%s\n\t%s\n\t%s\n",
		"options:",
		"-i\tdo ATA device identify dump as raw hex",
		"-I\tdo ATA device identify print fields",
		"-v\tbe verbose",
		"-h\tshow this usage summary");
	fprintf(stderr, "%s\n\t%s\n\t%s\n\t%s\n",
		"options taking arguments:",
		"-s\ttimeout in seconds",
		"-S\tperform SMART command",
		"-t\tspecify number for starting AoE tag");
}

void
hex_print(FILE *out, uchar *buf, int n, char *sep)
{
	int i;
	int per_line = 16;

	for (i = 0; i < n;) {
		fprintf(out, "%02x%s", *buf++ & 0xff, sep);
		if (!(++i % per_line))
			putc('\n', out);
	}
}

void
find_blade(Conf *c, struct progopts *opts)
{
	int n;
	uchar buf[1400];
	u32 tag;

	Aoehdr *h = &c->h;

	memset(h, 0, sizeof *h);
	memset(h->dst, 0xff, sizeof h->dst);
	memmove(h->src, mac, sizeof h->src);
	
	h->type = htons(AOE_ETH_PROTO);
	h->flags = AoEver << 4;
	h->maj = htons(opts->shelf);
	h->min = opts->slot;
	h->cmd = Config;
	tag = htonl(opts->tag);
	memmove(h->tag, &tag, sizeof h->tag);
	c->bufcnt = 0;
	c->firmware = 0;
	c->scnt = 0;
	c->vercmd = Qread;
	c->len = htons(1024);
	memset(c->data, 0xED, sizeof c->data);
	if (write(sfd, c, sizeof *c) == -1) {
		perror("send config query");
		exit(EXIT_FAILURE);
	}
	for (;;) {
		n = read(sfd, buf, sizeof buf);
		if (n < 0) {
			perror("read network");
			exit(EXIT_FAILURE);
		}
		if (n < 60)
			continue;
		h = (Aoehdr *) buf;
		if (ntohs(h->type) != AOE_ETH_PROTO
			|| ntohs(h->maj) != opts->shelf
			|| h->min != opts->slot
			|| memcmp(h->tag, &tag, sizeof h->tag))
			continue;
		break;
	}
	if (opts->verbose) {
		puts("config query response:");
		hex_print(stdout, buf, n, " ");
		putchar('\n');
	}
	memcpy(c, buf, sizeof *c);
}

/* read a packet that was sent by the device that returned *c earlier
 */
int
aoe_pkt_read(uchar *buf, size_t siz, Conf *c, u32 tag)
{
	Aoehdr *h;
	int n;

	tag = htonl(tag);
	for (;;) {
		n = read(sfd, buf, siz);
		if (n < 0) {
			perror("read network");
			exit(EXIT_FAILURE);
		}
		if (n < 60)
			continue;
		h = (Aoehdr *) buf;
		if (ntohs(h->type) != AOE_ETH_PROTO
			|| h->maj != c->h.maj
			|| h->min != c->h.min
			|| memcmp(&tag, h->tag, sizeof h->tag))
			continue;
		break;
	}
	return n;
}

/* prepare a packet for doing ATA to a device that gave us Conf *c
 */
void
ata_prep(Ata *a, Conf *c, u32 tag)
{
	memset(a, 0, sizeof *a);
	memcpy(a->h.dst, c->h.src, sizeof a->h.dst);
	memcpy(a->h.src, mac, sizeof a->h.src);
	a->h.type = htons(AOE_ETH_PROTO);
	a->h.flags = AoEver << 4;
	a->h.maj = c->h.maj;
	a->h.min = c->h.min;
	a->h.cmd = ATAcmd;
	tag = htonl(tag);
	memmove(a->h.tag, &tag, sizeof a->h.tag);
}

/* pretty print ATA device identify text field
 * bytes have already been swapped
 */
void
pp_idtext(char *prefix, unsigned char *p, size_t len)
{
	int i;

	fputs(prefix, stdout);
	for (i = 0; i < len; ++i, ++p) {
		if (*p == '\0')
			break;
		if (!isgraph((int) *p) && *p != ' ')
			break;
		putchar(*p);
	}		
	putchar('\n');
}

int smart_supported(unsigned char *p)
{
	u16 w;

	p += 82 * 2;		/* skip to word 82 */
	w = *p++;
	w |= *p << 8;

	/* word 82 bit 0 is SMART support per ATA spec */
	return !!(w & 1);
}

void
disk_identify(Conf *c, struct progopts *opts, int *smart)
{
	int n;
	uchar buf[1400];
	Ata a;
	Ata *p;
	struct hd_driveid *id;

	ata_prep(&a, c, opts->tag);
	a.sectors = 1;
	a.cmd = ATAid_dev;
	a.lba[3] = 0xa0;

	if (write(sfd, &a, sizeof a) == -1) {
		perror("send ATA identify device");
		exit(EXIT_FAILURE);
	}

	n = aoe_pkt_read(buf, sizeof buf, c, opts->tag);
	p = (Ata *) buf;

	*smart = smart_supported(p->data);

	if (opts->ata_ident && !opts->pp_ataid) {
		puts("device identify response:");
		hex_print(stdout, p->data, 512, " ");
		return;
	}
	if (!opts->pp_ataid)
		return;
	
	for (n = 0; n < 1024; n += 2) {
		unsigned char ch;
		ch = p->data[n];
		p->data[n] = p->data[n+1];
		p->data[n+1] = ch;
	}
	id = (struct hd_driveid *) p->data;
	puts("device identify fields:");
	printf("vendor_specific_0: 0x%X\n", id->vendor0);
	printf("vendor_specific_1: 0x%X\n", id->vendor1);
	printf("vendor_specific_2: 0x%X\n", id->vendor2);
	pp_idtext("serial_number: ", id->serial_no, sizeof id->serial_no);
	pp_idtext("firmware_rev: ", id->fw_rev, sizeof id->fw_rev);
	pp_idtext("model: ", id->model, sizeof id->model);
}

struct smartcmd *
smartcmd_lookup(char *nam)
{
	int n = sizeof smarts / sizeof smarts[0];
	int i;

	for (i = 0; i < n; ++i) {
		char *p = strchr(nam, ':');

		if (p && !strncmp(smarts[i].name, nam, p - nam))
			return &smarts[i];
		else if (!strcmp(smarts[i].name, nam))
			return &smarts[i];
	}
	return nil;
}

void
smart_registers(Ata *a, char *opts, struct smartcmd *s)
{
	a->err = s->cmd;
	a->lba[1] = 0x4f;
	a->lba[2] = 0xc2;
	if (opts++)
		a->lba[0] = strtol(opts, NULL, 0);
}

int
show_smart_regs(Ata *a)
{
	if (a->err & ATAabrt) {
		fputs("SMART command aborted on target.\n",
		      stderr);
		return -1;
	}

	puts("ATA registers:");
	char *names[] = {
		"Features", "Sector Count",
		"LBA Low", "LBA Mid", "LBA High",
		"Status",
	};
	int regs[] = {
		a->err, a->sectors,
		a->lba[0], a->lba[1], a->lba[2],
		a->cmd,
	};
	int i;

	for (i = 0; i < sizeof regs / sizeof regs[0]; ++i)
		printf("%20s: 0x%02x\n", names[i], regs[i]);

	return 0;
}

void
smart(Conf *c, u32 tag, char *smart_cmd)
{
	int n;
	uchar buf[1400];
	Ata a;
	Ata *p;
	struct smartcmd *s = smartcmd_lookup(smart_cmd);

	if (!s) {
		fprintf(stderr,
			"%s Error: no such SMART command: %s\n",
			progname, smart_cmd);
		exit(EXIT_FAILURE);
	}

	ata_prep(&a, c, tag);
	a.sectors = !!s->data;	/* we only support one-sector data xfer */
	a.cmd = ATAsmart;
	smart_registers(&a, strchr(smart_cmd, ':'), s);
	if (s->data & SmartDataPut) {
		if (read(STDIN_FILENO, a.data, 512) == -1) {
			perror("reading smart data from stdin");
			exit(EXIT_FAILURE);
		}
		a.h.flags |= Write;
	}

	if (write(sfd, &a, sizeof a) == -1) {
		perror("send ATA identify device");
		exit(EXIT_FAILURE);
	}
	n = aoe_pkt_read(buf, sizeof buf, c, tag);
	p = (Ata *) buf;
	if (show_smart_regs(p) != 0)
		exit(EXIT_FAILURE);
	if (s->data & SmartDataRet) {
		puts("SMART data:");
		hex_print(stdout, p->data, 512, " ");
	}
}

void
bad_option(char c)
{
	fprintf(stderr, "%s Error: unrecognized option: ", progname);
	if (isprint(c))
		fprintf(stderr, "%c\n", c);
	else
		fprintf(stderr, "0x%02x\n", c & 0xff);
}

void
check_timeout(int secs)
{
	if (secs < 1) {
		fprintf(stderr,
			"%s Error: timeout seconds must be one or more\n",
			progname);
		exit(EXIT_FAILURE);
	}
}

void
init_opts(struct progopts *opts, int argc, char *argv[])
{
	int c;

	while ( (c = getopt(argc, argv, "hviIt:s:S:")) != -1) {
		switch (c) {
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'v':
			++opts->verbose;
			break;
		case 'i':
			opts->ata_ident = 1;
			break;
		case 'I':
			opts->ata_ident = 1;
			opts->pp_ataid = 1;
			break;
		case 't':
			opts->tag = atoi(optarg);
			break;
		case 's':
			opts->timeout = atoi(optarg);
			check_timeout(opts->timeout);
			break;
		case 'S':
			opts->smart = optarg;
			break;
		case '?':
			bad_option(optopt);
			usage();
			exit(EXIT_FAILURE);
			break;
		default:
			abort(); /* shouldn't happen */
		}
	}
	if (argc - optind != 3) {
		usage();
		exit(EXIT_FAILURE);
	}
	opts->shelf = atoi(argv[optind]);
	opts->slot = atoi(argv[optind+1]);
	opts->netif = argv[optind+2];
}

int
main(int argc, char *argv[])
{
	Conf c;
	int smartable = 0;

	opts = defaults;
	progname = strrchr(argv[0], '/');
	if (progname)
		progname += 1;
	else
		progname = argv[0];

	init_opts(&opts, argc, argv);
	opts.tag |= 1UL << 31;	/* set high bit for userland AoE */

	if (opts.verbose) {
		printf("tag: %x\neth: %s\nshelf: %u\nslot: %u\n",
		       opts.tag, opts.netif, opts.shelf, opts.slot);
		fflush(stdout);
	}
	sfd = dial(opts.netif);
	if (!getea(sfd, opts.netif, mac))
		exit(EXIT_FAILURE);
	
	alarm(opts.timeout);
	find_blade(&c, &opts);
	opts.tag += 1;
	alarm(0);
	if (opts.verbose) {
		printf("found e%d.%d with mac ",
			ntohs(c.h.maj), c.h.min);
		hex_print(stdout, c.h.src, sizeof c.h.src, "");
		putchar('\n');
		fflush(stdout);
	}

	if (opts.ata_ident || opts.smart) {
		alarm(opts.timeout);
		disk_identify(&c, &opts, &smartable);
		alarm(0);
		opts.tag += 1;
	}

	if (opts.smart) {
		if (!smartable) {
			fprintf(stderr,
				"Error: e%d.%d does not support SMART\n",
				ntohs(c.h.maj), c.h.min);
			exit(EXIT_FAILURE);
		}
		alarm(opts.timeout);
		smart(&c, opts.tag, opts.smart);
		alarm(0);
		opts.tag += 1;
	}

	return 0;
}
