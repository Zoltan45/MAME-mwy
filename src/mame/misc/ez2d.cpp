// license:BSD-3-Clause
// copyright-holders:Ivan Vangelista
/*

TODO:
- Jumps to PC=0xfb000 after the first 2 PCI dword configs, which points to empty 0xff opcodes.
  $3a000 contains an "= Award Decompression Bios =" header

Thanks to Guru for hardware infos and pics for Ez2dancer 2nd Move.
Later games in the series might run on newer, beefier hardware.

ASUS CUBX-103 motherboard
Intel Celeron 533 MHz CPU
128 MB PC100 DIMM RAM
Fujitsu MPD3043AT HD (4.3GB) - The ez2d2m dump in this driver comes from a different, bigger hd
Leadtek Winfast 3D S325 32MB Video Card (might not be the original one)
Sound Blaster Live CT4830 Sound Card
EZ2D-IOCARD-c
Hardlock E-Y-E security dongle

Other games thought to run on this or derived hardware:
Ez2Dancer series:
* Ez2Dancer 1st Move (2000)
* Ez2Dancer UK Move(2002)
* Ez2Dancer UK Move Special Edition (2003)
* Ez2Dancer SuperChina (2004)

Ez2DJ series:
* Ez2Dj The 1st Tracks (1999)
* Ez2DJ The 1st Tracks Special Edition (1999)
* Ez2DJ 2nd TraX: It Rules Once Again (2000)
* Ez2DJ 3rd TraX: Absolute Pitch (2001)
* Ez2DJ 4th TraX: Over Mind (2002)
* Ez2DJ Mini (2003)
* Ez2DJ Dual Platinum (2003)
* Ez2DJ Platinum: Limited Edition (2003)
* Ez2DJ Single (2004)
* Ez2DJ 6th TraX: Self Evolution (2004)
* Ez2DJ 7th TraX: Resistance (2007)
* Ez2DJ 7th TraX Class R Codename: Violet (2009)
* Ez2DJ 7th TraX: Bonus Edition (2011)
* Ez2DJ Azure Expression (2012)
* Ez2DJ Azure Expression: Integral Composition (2012)
*/

#include "emu.h"
#include "cpu/i386/i386.h"
#include "machine/pci.h"


namespace {

class ez2d_state : public driver_device
{
public:
	ez2d_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu")
	{ }

	void ez2d(machine_config &config);

private:
	required_device<cpu_device> m_maincpu;

	void ez2d_map(address_map &map);
};


void ez2d_state::ez2d_map(address_map &map)
{
	map(0x00000000, 0x0009ffff).ram();
	map(0x000e0000, 0x000fffff).rom().region("bios", 0x20000);
	map(0xfffc0000, 0xffffffff).rom().region("bios", 0);
}

static INPUT_PORTS_START( ez2d )
INPUT_PORTS_END


void ez2d_state::ez2d(machine_config &config)
{
	/* basic machine hardware */
	PENTIUM3(config, m_maincpu, 100'000'000); // actually a Celeron at 533 MHz
	m_maincpu->set_addrmap(AS_PROGRAM, &ez2d_state::ez2d_map);

	PCI_ROOT(config, "pci", 0);
	// ...
}

/***************************************************************************

  Game drivers

***************************************************************************/

ROM_START( ez2d2m )
	ROM_REGION32_LE(0x40000, "bios", 0) \
	ROM_LOAD("ez2dancer2ndmove_motherboard_v29c51002t_award_bios", 0x00000, 0x40000, CRC(02a5e84b) SHA1(94b341d268ce9d42597c68bc98c3b8b62e137205) ) // 29f020

	ROM_REGION( 0x10000, "vbios", 0 )
	// nVidia TNT2 Model 64 video BIOS (not from provided dump)
	// TODO: move to PCI device once we have one
	ROM_LOAD( "62090211.rom", 0x000000, 0x00b000, CRC(5669135b) SHA1(b704ce0d20b71e40563d12bcc45bd1240227be74) )

	DISK_REGION( "ide:0:hdd" )
	DISK_IMAGE( "ez2d2m", 0, SHA1(431f0bef3b81f83dad3818bca8994faa8ce9d5b7) )
ROM_END

} // anonymous namespace


GAME( 2001, ez2d2m, 0, ez2d, ez2d, ez2d_state, empty_init, ROT0, "Amuse World", "Ez2dancer 2nd Move",  MACHINE_IS_SKELETON )
