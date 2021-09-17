/*
 * Copyright (C) 2012 Luka Perkov <luka@openwrt.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <malloc.h>
#include <miiphy.h>
#include <switch.h>
#include <netdev.h>

#define BITS(_s, _n)  (((1UL << (_n)) - 1) << _s)

#define AR8216_REG_CTRL			0x0000
#define   AR8216_CTRL_REVISION		BITS(0, 8)
#define   AR8216_CTRL_VERSION		BITS(8, 8)

#define AR8216_PROBE_RETRIES		10

static void split_addr(u32 regaddr, u16 *r1, u16 *r2, u16 *page)
{
	regaddr >>= 1;
	*r1 = regaddr & 0x1e;

	regaddr >>= 5;
	*r2 = regaddr & 0x7;

	regaddr >>= 3;
	*page = regaddr & 0x1ff;
}

static int ar8216_mii_read(struct mii_dev *bus, u32 reg)
{
	u16 r1, r2, page;
	u16 lo, hi;

	split_addr(reg, &r1, &r2, &page);

	bus->write(bus, 0x18, MDIO_DEVAD_NONE, 0, page);
	__udelay(1000);

	lo = bus->read(bus, 0x10 | r2, MDIO_DEVAD_NONE, r1);
	hi = bus->read(bus, 0x10 | r2, MDIO_DEVAD_NONE, r1 + 1);

	return (hi << 16) | lo;
}

static void ar8216_mii_write(struct mii_dev *bus, u16 reg, u32 val)
{
	u16 r1, r2, r3;
	u16 lo, hi;

	split_addr((u32) reg, &r1, &r2, &r3);

	bus->write(bus, 0x18, MDIO_DEVAD_NONE, 0, r3);
	__udelay(1000);

	lo = val & 0xffff;
	hi = (u16) (val >> 16);
	bus->write(bus, 0x10 | r2, MDIO_DEVAD_NONE, r1 + 1, hi);
	bus->write(bus, 0x10 | r2, MDIO_DEVAD_NONE, r1, lo);
}

static int ar8216_probe(struct switch_device *dev)
{
	struct mii_dev *bus = dev->bus;
	u32 val;
	u16 id;

	val = ar8216_mii_read(bus, AR8216_REG_CTRL);
	if (val == ~0)
		return 1;

	id = val & (AR8216_CTRL_REVISION | AR8216_CTRL_VERSION);

	switch (id) {
		case 0x0101:
			return 0;
		default:
			return 1;
	}
}

static void ar8216_setup(struct switch_device *dev)
{
	struct mii_dev *bus = dev->bus;

	ar8216_mii_write(bus, 0x200, 0x200);
	ar8216_mii_write(bus, 0x300, 0x200);
	ar8216_mii_write(bus, 0x400, 0x200);
	ar8216_mii_write(bus, 0x500, 0x200);
	ar8216_mii_write(bus, 0x600, 0x7d);
	ar8216_mii_write(bus, 0x38, 0xc000050e);
	ar8216_mii_write(bus, 0x104, 0x4004);
	ar8216_mii_write(bus, 0x60, 0xffffffff);
	ar8216_mii_write(bus, 0x64, 0xaaaaaaaa);
	ar8216_mii_write(bus, 0x68, 0x55555555);
	ar8216_mii_write(bus, 0x6c, 0x0);
	ar8216_mii_write(bus, 0x70, 0x41af);
}

static struct switch_driver ar8216_drv = {
	.name = "ar8216",
};

void switch_ar8216_init(void)
{
	/* for archs with manual relocation */
	ar8216_drv.probe = ar8216_probe;
	ar8216_drv.setup = ar8216_setup;

	switch_driver_register(&ar8216_drv);
}
