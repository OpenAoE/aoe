/* dat.h - aoetools type definitions and macros
 * Copyright 2009, CORAID, Inc., and licensed under GPL v.2.
 */

#define	nil	NULL
#define	nelem(x)	(sizeof(x)/sizeof((x)[0]))

typedef unsigned char uchar;
typedef struct Aoehdr Aoehdr;
typedef struct Ata Ata;
typedef struct Conf Conf;
typedef struct Ataregs Ataregs;

/* use C99's stdint.h for fixed-width types
 *
 * There's no guarantee an unsigned short is 16-bits wide,
 * but uint16_t is always correct.
 */
typedef uint64_t vlong;
typedef uint32_t u32;
typedef uint16_t u16;

struct Ataregs {
	vlong	lba;
	uchar	cmd;
	uchar	status;
	uchar	err;
	uchar	feature;
	uchar	sectors;
};

struct Aoehdr {
	uchar	dst[6];
	uchar	src[6];
	u16	type;
	uchar	flags;
	uchar	error;
	u16	maj;
	uchar	min;
	uchar	cmd;
	uchar	tag[4];
};

struct Ata {
	Aoehdr	h;
	uchar	aflag;
	uchar	err;
	uchar	sectors;
	uchar	cmd;
	uchar	lba[6];
	uchar	resvd[2];
	uchar	data[1024];
};

struct Conf {
	Aoehdr	h;
	u16	bufcnt;
	u16	firmware;
	uchar	scnt;
	uchar	vercmd;
	u16	len;
	uchar	data[1024];
};

enum {
	AOE_ETH_PROTO = 0x88a2,

	AoEver = 1,

	ATAcmd = 0,		// command codes
	Config,

	Resp = 1<<3,		// flags
	Error = 1<<2,

	BadCmd = 1,
	BadArg,
	DevUnavailable,
	ConfigErr,
	BadVersion,

	Write = 1<<0,
	Async = 1<<1,
	Device = 1<<4,
	Extend = 1<<6,

	Qread = 0,
	Qtest,
	Qprefix,
	Qset,
	Qfset,

	Nretries = 3,

	ATAid_dev = 0xec,	// ATA commands
	ATAsmart = 0xb0,

	ATAabrt = 0x4,		// ATA error bits

	SmartDataPut = 0x01,
	SmartDataRet = 0x10,
};
