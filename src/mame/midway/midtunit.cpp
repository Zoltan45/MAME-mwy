// license:BSD-3-Clause
// copyright-holders:Alex Pasadyn, Zsolt Vasvari, Ernesto Corvi, Aaron Giles
// thanks-to:Kurt Mahan
/*************************************************************************

    Midway T-unit system

    driver by Alex Pasadyn, Zsolt Vasvari, Kurt Mahan, Ernesto Corvi,
    and Aaron Giles

    Games supported:
        * Mortal Kombat (T-unit version)
		* Mortal Kombat +
        * Mortal Kombat II
        * Mortal Kombat II +
        * NBA Jam
        * NBA Jam Tournament Edition
        * NBA Jam Tournament Edition Rewind
        * Judge Dredd (prototype)

    Known bugs:
        * page flipping seems off in NBA Jam (or else there's a blank-the
            screen bit we're missing)

**************************************************************************/


#include "emu.h"
#include "midtunit.h"

#include "machine/nvram.h"
#include "machine/watchdog.h"

#include "screen.h"
#include "speaker.h"




/*************************************
 *
 *  Memory maps
 *
 *************************************/

void midtunit_base_state::main_map(address_map &map)
{
	map.unmap_value_high();
	map(0x00000000, 0x003fffff).rw(m_video, FUNC(midtunit_video_device::midtunit_vram_r), FUNC(midtunit_video_device::midtunit_vram_w));
	map(0x01000000, 0x013fffff).ram();
	map(0x01400000, 0x0141ffff).rw(FUNC(midtunit_base_state::cmos_r), FUNC(midtunit_base_state::cmos_w)).share(m_nvram);
	map(0x01480000, 0x014fffff).w(FUNC(midtunit_base_state::cmos_enable_w));
	map(0x01600000, 0x0160000f).portr("IN0");
	map(0x01600010, 0x0160001f).portr("IN1");
	map(0x01600020, 0x0160002f).portr("IN2");
	map(0x01600030, 0x0160003f).portr("DSW");
	map(0x01800000, 0x0187ffff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x01a80000, 0x01a800ff).rw(m_video, FUNC(midtunit_video_device::dma_r), FUNC(midtunit_video_device::dma_w));
	map(0x01b00000, 0x01b0001f).w(m_video, FUNC(midtunit_video_device::midtunit_control_w));
//  map(0x01c00060, 0x01c0007f).w(FUNC(midtunit_base_state::cmos_enable_w));
	map(0x01d81060, 0x01d8107f).w("watchdog", FUNC(watchdog_timer_device::reset16_w));
	map(0x01f00000, 0x01f0001f).w(m_video, FUNC(midtunit_video_device::midtunit_control_w));
	map(0x02000000, 0x07ffffff).r(m_video, FUNC(midtunit_video_device::midtunit_gfxrom_r)).share("video");
	map(0x1f800000, 0x1fffffff).rom().region("maincpu", 0); // mirror used by MK
	map(0xff800000, 0xffffffff).rom().region("maincpu", 0);
}

void midtunit_adpcm_state::main_adpcm_map(address_map &map)
{
	main_map(map);

	map(0x01d00000, 0x01d0001f).r(FUNC(midtunit_adpcm_state::sound_state_r));
	map(0x01d01020, 0x01d0103f).rw(FUNC(midtunit_adpcm_state::sound_r), FUNC(midtunit_adpcm_state::sound_w));
}

void mk2_state::mk2_map(address_map &map)
{
	main_map(map);

	map(0x01d00000, 0x01d0001f).r(FUNC(mk2_state::dcs_state_r));
	map(0x01d01020, 0x01d0103f).rw(FUNC(mk2_state::dcs_r), FUNC(mk2_state::dcs_w));
}



/*************************************
 *
 *  Input ports
 *
 *************************************/

static INPUT_PORTS_START( mk )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 High Punch") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Block") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 High Kick") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 High Punch") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Block") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 High Kick") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) // Slam Switch
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P2 Low Punch") PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P2 Low Kick") PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P2 Block 2") PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P1 Low Punch") PORT_PLAYER(1)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P1 Low Kick") PORT_PLAYER(1)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P1 Block 2") PORT_PLAYER(1)

	PORT_START("IN2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW")
	PORT_DIPNAME( 0x0001, 0x0001, "Test Switch" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0002, 0x0000, "Counters" )
	PORT_DIPSETTING(      0x0002, "One" )
	PORT_DIPSETTING(      0x0000, "Two" )
	PORT_DIPNAME( 0x007c, 0x007c, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x007c, "USA-1" )
	PORT_DIPSETTING(      0x003c, "USA-2" )
	PORT_DIPSETTING(      0x005c, "USA-3" )
	PORT_DIPSETTING(      0x001c, "USA-4" )
	PORT_DIPSETTING(      0x006c, "USA-ECA" )
	PORT_DIPSETTING(      0x000c, "USA-Free Play" )
	PORT_DIPSETTING(      0x0074, "German-1" )
	PORT_DIPSETTING(      0x0034, "German-2" )
	PORT_DIPSETTING(      0x0054, "German-3" )
	PORT_DIPSETTING(      0x0014, "German-4" )
	PORT_DIPSETTING(      0x0064, "German-5" )
	PORT_DIPSETTING(      0x0024, "German-ECA" )
	PORT_DIPSETTING(      0x0004, "German-Free Play" )
	PORT_DIPSETTING(      0x0078, "French-1" )
	PORT_DIPSETTING(      0x0038, "French-2" )
	PORT_DIPSETTING(      0x0058, "French-3" )
	PORT_DIPSETTING(      0x0018, "French-4" )
	PORT_DIPSETTING(      0x0068, "French-ECA" )
	PORT_DIPSETTING(      0x0008, "French-Free Play" )
	PORT_DIPNAME( 0x0080, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x0080, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )
	PORT_DIPNAME( 0x0100, 0x0000, "Skip Post Test")
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0800, 0x0800, "Comic Book Offer" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0800, DEF_STR( On ))
	PORT_DIPNAME( 0x1000, 0x1000, "Attract Sound" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x1000, DEF_STR( On ))
	PORT_DIPNAME( 0x2000, 0x2000, "Low Blows" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x2000, DEF_STR( On ))
	PORT_DIPNAME( 0x4000, 0x4000, "Blood" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x4000, DEF_STR( On ))
	PORT_DIPNAME( 0x8000, 0x8000, "Violence" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x8000, DEF_STR( On ))
INPUT_PORTS_END


static INPUT_PORTS_START( mk2 )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 High Punch") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Block") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 High Kick") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 High Punch") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Block") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 High Kick") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) // Slam Switch
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x0600, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_VOLUME_DOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_VOLUME_UP )
	PORT_BIT( 0x6000, IP_ACTIVE_LOW, IPT_UNUSED )
	//PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_PLAYER(1)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED ) //Renamed to unused because without it the game seemed to hold P1 Block down-someone with more experience should check

	PORT_START("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P1 Low Punch") PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P1 Low Kick") PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P1 Block 2") PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P2 Low Punch") PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("P2 Low Kick") PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("P2 Block 2") PORT_PLAYER(2)
	PORT_BIT( 0xff80, IP_ACTIVE_LOW, IPT_UNUSED )

	/*Note-the real MK2 board has a special cable designed for SF2 cab conversions that has the 2 SF2
	      Medium punch/kick buttons as block buttons for MK2. The secondary block button registers in test mode,
	      but does not have an indicator light show up. During gameplay, the second block only functions temporarily.
	  You can hold the button, but the character will only take a block position for 1 second. This is correct behavior.*/

	PORT_START("DSW")
	PORT_DIPNAME( 0x0001, 0x0001, "Test Switch" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0002, 0x0000, "Counters" )
	PORT_DIPSETTING(      0x0002, "One" )
	PORT_DIPSETTING(      0x0000, "Two" )
	PORT_DIPNAME( 0x007c, 0x007c, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x007c, "USA-1" )
	PORT_DIPSETTING(      0x003c, "USA-2" )
	PORT_DIPSETTING(      0x005c, "USA-3" )
	PORT_DIPSETTING(      0x001c, "USA-4" )
	PORT_DIPSETTING(      0x006c, "USA-ECA" )
	PORT_DIPSETTING(      0x000c, "USA-Free Play" )
	PORT_DIPSETTING(      0x0074, "German-1" )
	PORT_DIPSETTING(      0x0034, "German-2" )
	PORT_DIPSETTING(      0x0054, "German-3" )
	PORT_DIPSETTING(      0x0014, "German-4" )
	PORT_DIPSETTING(      0x0064, "German-5" )
	PORT_DIPSETTING(      0x0024, "German-ECA" )
	PORT_DIPSETTING(      0x0004, "German-Free Play" )
	PORT_DIPSETTING(      0x0078, "French-1" )
	PORT_DIPSETTING(      0x0038, "French-2" )
	PORT_DIPSETTING(      0x0058, "French-3" )
	PORT_DIPSETTING(      0x0018, "French-4" )
	PORT_DIPSETTING(      0x0068, "French-ECA" )
	PORT_DIPSETTING(      0x0008, "French-Free Play" )
	PORT_DIPNAME( 0x0080, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x0080, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )
	PORT_DIPNAME( 0x0100, 0x0000, "Circuit Boards" )
	PORT_DIPSETTING(      0x0100, "2" )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPNAME( 0x0200, 0x0000, "Powerup Test" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0200, DEF_STR( On ))
	PORT_DIPNAME( 0x0400, 0x0400, "Bill Validator" )
	PORT_DIPSETTING(      0x0000, "Installed" )
	PORT_DIPSETTING(      0x0400, "Not Present" )
	PORT_DIPNAME( 0x0800, 0x0800, "Comic Book Offer" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0800, DEF_STR( On ))
	PORT_DIPNAME( 0x1000, 0x1000, "Attract Sound" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x1000, DEF_STR( On ))
	PORT_DIPNAME( 0x2000, 0x2000, "Low Blows" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x2000, DEF_STR( On ))
	PORT_DIPNAME( 0x4000, 0x4000, "Blood" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x4000, DEF_STR( On ))
	PORT_DIPNAME( 0x8000, 0x8000, "Violence" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x8000, DEF_STR( On ))
INPUT_PORTS_END


static INPUT_PORTS_START( jdreddp )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Jump") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 Punch") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Kick") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P1 Crouch") PORT_PLAYER(1)
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Jump") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 Punch") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 Kick") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P2 Crouch") PORT_PLAYER(2)

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) // Slam Switch
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_VOLUME_DOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_VOLUME_UP )
	PORT_BIT( 0xe000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P3 Jump") PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P3 Punch") PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P3 Kick") PORT_PLAYER(3)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("P3 Crouch") PORT_PLAYER(3)
	PORT_BIT( 0xff00, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW")       // DS1
	PORT_DIPNAME( 0x0001, 0x0001, "Test Switch" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unused )) //listed as 'Powerup Test' in service mode, does nothing
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0020, 0x0020, "Blood" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0020, DEF_STR( On ))
	PORT_DIPNAME( 0x0040, 0x0040, "Validator" )
	PORT_DIPSETTING(      0x0000, "Installed" )
	PORT_DIPSETTING(      0x0040, DEF_STR( None ) )
	PORT_DIPNAME( 0x0080, 0x0080, "Freeze" ) //listed as 2/3 player in service mode
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ))
	PORT_DIPSETTING(      0x0000, DEF_STR( On ))
	PORT_DIPNAME( 0x0300, 0x0000, "Coin Counters" )
//  PORT_DIPSETTING(      0x0300, "1 Counter, 1 count/coin" )
	PORT_DIPSETTING(      0x0200, "1 Counter, Totalizing" )
	PORT_DIPSETTING(      0x0100, "2 Counters, 1 count/coin" )
	PORT_DIPSETTING(      0x0000, "1 Counter, 1 count/coin" )
	PORT_DIPNAME( 0x0c00, 0x0c00, "Country" )
	PORT_DIPSETTING(      0x0c00, DEF_STR( USA ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( French ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( German ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Unused ))
	PORT_DIPNAME( 0x7000, 0x5000, DEF_STR( Coinage ))
	PORT_DIPSETTING(      0x7000, "1" )
	PORT_DIPSETTING(      0x3000, "2" )
	PORT_DIPSETTING(      0x5000, "3" ) // the game reads this as Skip Power up Test and Coinage 3
	PORT_DIPSETTING(      0x1000, "4" )
	PORT_DIPSETTING(      0x6000, "ECA" )
//  PORT_DIPSETTING(      0x4000, DEF_STR( Unused ))
//  PORT_DIPSETTING(      0x2000, DEF_STR( Unused ))
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ))
	PORT_DIPNAME( 0x8000, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x8000, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )
INPUT_PORTS_END


static INPUT_PORTS_START( nbajam )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Shoot / Block") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 Pass / Steal") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Turbo") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Shoot / Block") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 Pass / Steal") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 Turbo") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) // Slam Switch
	PORT_SERVICE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_VOLUME_DOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_VOLUME_UP )
	PORT_BIT( 0xe000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P3 Shoot / Block") PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P3 Pass / Steal") PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P3 Turbo") PORT_PLAYER(3)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P4 Shoot / Block") PORT_PLAYER(4)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P4 Pass / Steal") PORT_PLAYER(4)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P4 Turbo") PORT_PLAYER(4)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW")       // DS1
	PORT_DIPNAME( 0x0001, 0x0001, "Test Switch" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0000, "Powerup Test" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, "Video Clips" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, "Dollar Bill Validator" )
	PORT_DIPSETTING(      0x0000, "Installed" )
	PORT_DIPSETTING(      0x0040, "Not Present" )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Players ) )
	PORT_DIPSETTING(      0x0000, "2" )
	PORT_DIPSETTING(      0x0080, "4" )
	PORT_DIPNAME( 0x0300, 0x0300, "Coin Counters" )
	PORT_DIPSETTING(      0x0300, "1 Counter, 1 count/coin" )
	PORT_DIPSETTING(      0x0200, "1 Counter, Totalizing" )
	PORT_DIPSETTING(      0x0100, "2 Counters, 1 count/coin" )
//  PORT_DIPSETTING(      0x0000, "1 Counter, 1 count/coin" )
	PORT_DIPNAME( 0x0c00, 0x0c00, "Country" )
	PORT_DIPSETTING(      0x0c00, DEF_STR( USA ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( French ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( German ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x7000, 0x7000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x7000, "1" )
	PORT_DIPSETTING(      0x3000, "2" )
	PORT_DIPSETTING(      0x5000, "3" )
	PORT_DIPSETTING(      0x1000, "4" )
	PORT_DIPSETTING(      0x6000, "ECA" )
//  PORT_DIPSETTING(      0x4000, DEF_STR( Unused ) )
//  PORT_DIPSETTING(      0x2000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x8000, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x8000, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )
INPUT_PORTS_END


static INPUT_PORTS_START( nbajamte )
	PORT_START("IN0")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P1 Shoot / Block") PORT_PLAYER(1)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P1 Pass / Steal") PORT_PLAYER(1)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P1 Turbo") PORT_PLAYER(1)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(2)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P2 Shoot / Block") PORT_PLAYER(2)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P2 Pass / Steal") PORT_PLAYER(2)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P2 Turbo") PORT_PLAYER(2)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_TILT ) // Slam Switch
	PORT_SERVICE_NO_TOGGLE( 0x0010, IP_ACTIVE_LOW )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_COIN3 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN4 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_START3 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_START4 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_VOLUME_DOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_VOLUME_UP )
	PORT_BIT( 0xe000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("IN2")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(3)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P3 Shoot / Block") PORT_PLAYER(3)
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P3 Pass / Steal") PORT_PLAYER(3)
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P3 Turbo") PORT_PLAYER(3)
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_8WAY PORT_PLAYER(4)
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("P4 Shoot / Block") PORT_PLAYER(4)
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("P4 Pass / Steal") PORT_PLAYER(4)
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("P4 Turbo") PORT_PLAYER(4)
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("DSW")// DS1
	PORT_DIPNAME( 0x0001, 0x0001, "Test Switch" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0000, "Powerup Test" )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, "Dollar Bill Validator" )
	PORT_DIPSETTING(      0x0000, "Installed" )
	PORT_DIPSETTING(      0x0040, "Not Present" )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Players ) )
	PORT_DIPSETTING(      0x0000, "2" )
	PORT_DIPSETTING(      0x0080, "4" )
	PORT_DIPNAME( 0x0300, 0x0300, "Coin Counters" )
	PORT_DIPSETTING(      0x0300, "1 Counter, 1 count/coin" )
	PORT_DIPSETTING(      0x0200, "1 Counter, Totalizing" )
	PORT_DIPSETTING(      0x0100, "2 Counters, 1 count/coin" )
//  PORT_DIPSETTING(      0x0000, "1 Counter, 1 count/coin" )
	PORT_DIPNAME( 0x0c00, 0x0c00, "Country" )
	PORT_DIPSETTING(      0x0c00, DEF_STR( USA ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( French ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( German ) )
//  PORT_DIPSETTING(      0x0000, DEF_STR( Unused ) )
	PORT_DIPNAME( 0x7000, 0x7000, DEF_STR( Coinage ) )
	PORT_DIPSETTING(      0x7000, "1" )
	PORT_DIPSETTING(      0x3000, "2" )
	PORT_DIPSETTING(      0x5000, "3" )
	PORT_DIPSETTING(      0x1000, "4" )
	PORT_DIPSETTING(      0x6000, "ECA" )
//  PORT_DIPSETTING(      0x4000, DEF_STR( Unused ) )
//  PORT_DIPSETTING(      0x2000, DEF_STR( Unused ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0x8000, 0x0000, "Coinage Source" )
	PORT_DIPSETTING(      0x8000, "Dipswitch" )
	PORT_DIPSETTING(      0x0000, "CMOS" )
INPUT_PORTS_END



/*************************************
 *
 *  Machine drivers
 *
 *************************************/

void midtunit_base_state::tunit_core(machine_config &config)
{
	constexpr XTAL CPU_CLOCK = 50_MHz_XTAL;
	constexpr XTAL PIXEL_CLOCK = 8_MHz_XTAL/2;

	MIDTUNIT_VIDEO(config, m_video, m_palette);
	m_video->dma_irq_cb().set_inputline(m_maincpu, 0);

	// basic machine hardware
	TMS34010(config, m_maincpu, CPU_CLOCK);
	m_maincpu->set_halt_on_reset(false);     // halt on reset
	m_maincpu->set_pixel_clock(PIXEL_CLOCK); // pixel clock
	m_maincpu->set_pixels_per_clock(2);      // pixels per clock
	m_maincpu->set_scanline_ind16_callback(m_video, FUNC(midtunit_video_device::scanline_update));  // scanline updater (indexed16)
	m_maincpu->set_shiftreg_in_callback(m_video, FUNC(midtunit_video_device::to_shiftreg));         // write to shiftreg function
	m_maincpu->set_shiftreg_out_callback(m_video, FUNC(midtunit_video_device::from_shiftreg));      // read from shiftreg function
	m_maincpu->set_screen("screen");

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	WATCHDOG_TIMER(config, "watchdog");

	// video hardware
	PALETTE(config, m_palette).set_format(palette_device::xRGB_555, 32768);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	// from TMS340 registers
	screen.set_raw(PIXEL_CLOCK * 2, 506, 100, 500, 289, 20, 274);
	screen.set_screen_update("maincpu", FUNC(tms34010_device::tms340x0_ind16));
	screen.set_palette(m_palette);
}


void midtunit_adpcm_state::tunit_adpcm(machine_config &config)
{
	tunit_core(config);

	// basic machine hardware
	m_maincpu->set_addrmap(AS_PROGRAM, &midtunit_adpcm_state::main_adpcm_map);

	SPEAKER(config, "speaker").front_center();

	WILLIAMS_ADPCM_SOUND(config, m_adpcm_sound, 0).add_route(ALL_OUTPUTS, "speaker", 1.0);
}


void mk2_state::mk2(machine_config &config)
{
	tunit_core(config);

	// basic machine hardware
	m_maincpu->set_addrmap(AS_PROGRAM, &mk2_state::mk2_map);

	m_video->set_gfx_rom_large(true);

	SPEAKER(config, "speaker").front_center();

	DCS_AUDIO_2K(config, m_dcs, 0);
	m_dcs->set_maincpu_tag(m_maincpu);
	m_dcs->add_route(0, "speaker", 1.0);
}



/*************************************
 *
 *  ROM definitions
 *
 *************************************/

ROM_START( mk )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   // sound CPU
	ROM_LOAD( "sl1_mortal_kombat_u3_sound_rom.u3", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "sl1_mortal_kombat_u12_sound_rom.u12", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(                                      0x40000, 0x40000 )
	ROM_LOAD( "sl1_mortal_kombat_u13_sound_rom.u13", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(                                      0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l5_mortal_kombat_t-unit_uj12_game_rom.uj12", 0x00000, 0x80000, CRC(f4990bf2) SHA1(796ec84d37c8d20ca36d6439c14dee626fb8481e) )
	ROM_LOAD16_BYTE( "l5_mortal_kombat_t-unit_ug12_game_rom.ug12", 0x00001, 0x80000, CRC(b06aeac1) SHA1(f66655eeab67c8cf5e496ae42dbae54d6400586f) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug14_game_rom.ug14", 0x000000, 0x80000, CRC(9e00834e) SHA1(2b97b63f52ba1dba6af6ae56c223519a52b2ab9d) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj14_game_rom.uj14", 0x000001, 0x80000, CRC(f4b0aaa7) SHA1(4cc6ee34c89e3cde325ad24b29511f70ae6a5a72) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug19_game_rom.ug19", 0x000002, 0x80000, CRC(2d8c7ba1) SHA1(f891d6eb618dbf3e77f02e0f93da216e20571905) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj19_game_rom.uj19", 0x000003, 0x80000, CRC(33b9b7a4) SHA1(e8ceca4c049e1f55d480a03ff793b595bd04d344) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug16_game_rom.ug16", 0x200000, 0x80000, CRC(52c9d1e5) SHA1(7b1880fca0a11544782b70365c7dd96381ac48e7) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj16_game_rom.uj16", 0x200001, 0x80000, CRC(c94c58cf) SHA1(974d75667eee779497325d5be8df937f15417edf) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug20_game_rom.ug20", 0x200002, 0x80000, CRC(2f7e55d3) SHA1(bda6892ee6fcb46959e4d0892bbe7d9fc6072dd3) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj20_game_rom.uj20", 0x200003, 0x80000, CRC(eae96df0) SHA1(b40532312ba61e4065abfd733dd0c93eecad48e9) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug17_game_rom.ug17", 0x400000, 0x80000, CRC(e34fe253) SHA1(6b010bee795c1770297c9557ded1fe83425857f2) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj17_game_rom.uj17", 0x400001, 0x80000, CRC(a56e12f5) SHA1(5db637c4710990cd06bb0069714b19621532e431) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug22_game_rom.ug22", 0x400002, 0x80000, CRC(b537bb4e) SHA1(05a447deee2e89b49bdb3ca2161a021d7ec5f11e) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj22_game_rom.uj22", 0x400003, 0x80000, CRC(5e12523b) SHA1(468f93ef9bb6addb45c1c939d24b6511f255426a) )
ROM_END


ROM_START( mkp )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   // sound CPU
	ROM_LOAD( "sl1_mortal_kombat_u3_sound_rom.u3", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "sl1_mortal_kombat_u12_sound_rom.u12", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(                                      0x40000, 0x40000 )
	ROM_LOAD( "sl1_mortal_kombat_u13_sound_rom.u13", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(                                      0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "mkp_uj12_game_rom.uj12", 0x00000, 0x80000, NO_DUMP )
	ROM_LOAD16_BYTE( "mkp_ug12_game_rom.ug12", 0x00001, 0x80000, NO_DUMP )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug14_game_rom.ug14", 0x000000, 0x80000, CRC(9e00834e) SHA1(2b97b63f52ba1dba6af6ae56c223519a52b2ab9d) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj14_game_rom.uj14", 0x000001, 0x80000, CRC(f4b0aaa7) SHA1(4cc6ee34c89e3cde325ad24b29511f70ae6a5a72) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug19_game_rom.ug19", 0x000002, 0x80000, CRC(2d8c7ba1) SHA1(f891d6eb618dbf3e77f02e0f93da216e20571905) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj19_game_rom.uj19", 0x000003, 0x80000, CRC(33b9b7a4) SHA1(e8ceca4c049e1f55d480a03ff793b595bd04d344) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug16_game_rom.ug16", 0x200000, 0x80000, CRC(52c9d1e5) SHA1(7b1880fca0a11544782b70365c7dd96381ac48e7) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj16_game_rom.uj16", 0x200001, 0x80000, CRC(c94c58cf) SHA1(974d75667eee779497325d5be8df937f15417edf) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug20_game_rom.ug20", 0x200002, 0x80000, CRC(2f7e55d3) SHA1(bda6892ee6fcb46959e4d0892bbe7d9fc6072dd3) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj20_game_rom.uj20", 0x200003, 0x80000, CRC(eae96df0) SHA1(b40532312ba61e4065abfd733dd0c93eecad48e9) )

	ROM_LOAD32_BYTE( "mkp_ug17_game_rom.ug17", 0x400000, 0x80000, NO_DUMP )
	ROM_LOAD32_BYTE( "mkp_uj17_game_rom.uj17", 0x400001, 0x80000, NO_DUMP )
	ROM_LOAD32_BYTE( "mkp_ug22_game_rom.ug22", 0x400002, 0x80000, NO_DUMP )
	ROM_LOAD32_BYTE( "mkp_uj22_game_rom.uj22", 0x400003, 0x80000, NO_DUMP )
ROM_END


ROM_START( mkr4 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   // sound CPU
	ROM_LOAD( "sl1_mortal_kombat_u3_sound_rom.u3", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "sl1_mortal_kombat_u12_sound_rom.u12", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(                                      0x40000, 0x40000 )
	ROM_LOAD( "sl1_mortal_kombat_u13_sound_rom.u13", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(                                      0xc0000, 0x40000 )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l4_mortal_kombat_t-unit_uj12_game_rom.uj12", 0x00000, 0x80000, CRC(a1b6635a) SHA1(22d396cc9c1e3a14cb01d196de6d3e864f7afc55) )
	ROM_LOAD16_BYTE( "l4_mortal_kombat_t-unit_ug12_game_rom.ug12", 0x00001, 0x80000, CRC(aa94f7ea) SHA1(bd8957bf52f73b49767cc78fec84ed1109a37701) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug14_game_rom.ug14", 0x000000, 0x80000, CRC(9e00834e) SHA1(2b97b63f52ba1dba6af6ae56c223519a52b2ab9d) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj14_game_rom.uj14", 0x000001, 0x80000, CRC(f4b0aaa7) SHA1(4cc6ee34c89e3cde325ad24b29511f70ae6a5a72) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug19_game_rom.ug19", 0x000002, 0x80000, CRC(2d8c7ba1) SHA1(f891d6eb618dbf3e77f02e0f93da216e20571905) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj19_game_rom.uj19", 0x000003, 0x80000, CRC(33b9b7a4) SHA1(e8ceca4c049e1f55d480a03ff793b595bd04d344) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug16_game_rom.ug16", 0x200000, 0x80000, CRC(52c9d1e5) SHA1(7b1880fca0a11544782b70365c7dd96381ac48e7) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj16_game_rom.uj16", 0x200001, 0x80000, CRC(c94c58cf) SHA1(974d75667eee779497325d5be8df937f15417edf) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug20_game_rom.ug20", 0x200002, 0x80000, CRC(2f7e55d3) SHA1(bda6892ee6fcb46959e4d0892bbe7d9fc6072dd3) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj20_game_rom.uj20", 0x200003, 0x80000, CRC(eae96df0) SHA1(b40532312ba61e4065abfd733dd0c93eecad48e9) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug17_game_rom.ug17", 0x400000, 0x80000, CRC(e34fe253) SHA1(6b010bee795c1770297c9557ded1fe83425857f2) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj17_game_rom.uj17", 0x400001, 0x80000, CRC(a56e12f5) SHA1(5db637c4710990cd06bb0069714b19621532e431) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug22_game_rom.ug22", 0x400002, 0x80000, CRC(b537bb4e) SHA1(05a447deee2e89b49bdb3ca2161a021d7ec5f11e) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj22_game_rom.uj22", 0x400003, 0x80000, CRC(5e12523b) SHA1(468f93ef9bb6addb45c1c939d24b6511f255426a) )
ROM_END


ROM_START( mktturbo )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 )   // sound CPU
	ROM_LOAD( "sl1_mortal_kombat_u3_sound_rom.u3", 0x10000, 0x40000, CRC(c615844c) SHA1(5732f9053a5f73b0cc3b0166d7dc4430829d5bc7) )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "sl1_mortal_kombat_u12_sound_rom.u12", 0x00000, 0x40000, CRC(258bd7f9) SHA1(463890b23f17350fb9b8a85897b0777c45bc2d54) )
	ROM_RELOAD(                                      0x40000, 0x40000 )
	ROM_LOAD( "sl1_mortal_kombat_u13_sound_rom.u13", 0x80000, 0x40000, CRC(7b7ec3b6) SHA1(6eec1b90d4a4855f34a7ebfbf93f3358d5627db4) )
	ROM_RELOAD(                                      0xc0000, 0x40000 )

	// A 'NIBBLE BOARD' daughtercard holding a GAL16V8A-2SP, 27C040 EPROM and a 9.8304MHz XTAL plugs into the UG12 socket
	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "kombo-rom-uj-12.bin", 0x00000, 0x80000, CRC(7a441f2d) SHA1(3b731bcbd73721ea0cc20157ec5181d25922523c) )
	ROM_LOAD16_BYTE( "kombo-rom-ug-12.bin", 0x00001, 0x80000, CRC(45bed5a1) SHA1(dba2c21878925afdcaf61520c18ebefd5e9617db) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug14_game_rom.ug14", 0x000000, 0x80000, CRC(9e00834e) SHA1(2b97b63f52ba1dba6af6ae56c223519a52b2ab9d) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj14_game_rom.uj14", 0x000001, 0x80000, CRC(f4b0aaa7) SHA1(4cc6ee34c89e3cde325ad24b29511f70ae6a5a72) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug19_game_rom.ug19", 0x000002, 0x80000, CRC(2d8c7ba1) SHA1(f891d6eb618dbf3e77f02e0f93da216e20571905) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj19_game_rom.uj19", 0x000003, 0x80000, CRC(33b9b7a4) SHA1(e8ceca4c049e1f55d480a03ff793b595bd04d344) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug16_game_rom.ug16", 0x200000, 0x80000, CRC(52c9d1e5) SHA1(7b1880fca0a11544782b70365c7dd96381ac48e7) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj16_game_rom.uj16", 0x200001, 0x80000, CRC(c94c58cf) SHA1(974d75667eee779497325d5be8df937f15417edf) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug20_game_rom.ug20", 0x200002, 0x80000, CRC(2f7e55d3) SHA1(bda6892ee6fcb46959e4d0892bbe7d9fc6072dd3) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj20_game_rom.uj20", 0x200003, 0x80000, CRC(eae96df0) SHA1(b40532312ba61e4065abfd733dd0c93eecad48e9) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug17_game_rom.ug17", 0x400000, 0x80000, CRC(e34fe253) SHA1(6b010bee795c1770297c9557ded1fe83425857f2) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj17_game_rom.uj17", 0x400001, 0x80000, CRC(a56e12f5) SHA1(5db637c4710990cd06bb0069714b19621532e431) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_ug22_game_rom.ug22", 0x400002, 0x80000, CRC(b537bb4e) SHA1(05a447deee2e89b49bdb3ca2161a021d7ec5f11e) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_t-unit_uj22_game_rom.uj22", 0x400003, 0x80000, CRC(5e12523b) SHA1(468f93ef9bb6addb45c1c939d24b6511f255426a) )
ROM_END


ROM_START( mk2 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l3.1_mortal_kombat_ii_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(cf100a75) SHA1(c5cf739fdb08e311f47794eb93a8d34d4bc11cde) ) // Revision 3.1
	ROM_LOAD16_BYTE( "l3.1_mortal_kombat_ii_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(582c7dfd) SHA1(f32bd1213ce70f74caa97a2047815cf4baee56b5) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2p )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )
	ROM_LOAD16_BYTE( "mk2p.u2", 0x000000, 0x80000, CRC(65d11dd7) SHA1(bb94cca5a35f8f285d32314f8762c3c52ec48f5b) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )
	ROM_LOAD16_BYTE( "mk2p.uj12", 0x00000, 0x80000, CRC(05ff15a9) SHA1(23e9c47644c756d7568b6e5122801b19cf594e9b) )
	ROM_LOAD16_BYTE( "mk2p.ug12", 0x00001, 0x80000, CRC(b6d8ff5c) SHA1(ad974cc0dcdf35c7175f7cb8c620b58b82843523) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "mk2p.ug16", 0x400000, 0x100000, CRC(b2af2798) SHA1(3d679fb59753b72f961fe60a65e4e03a76b64266) )
	ROM_LOAD32_BYTE( "mk2p.uj16", 0x400001, 0x100000, CRC(d70dd149) SHA1(1add61f22a68c0bd8dba3fa0da18f5ecc7f51c6f) )
	ROM_LOAD32_BYTE( "mk2p.ug20", 0x400002, 0x100000, CRC(d05e970a) SHA1(0f07d72c0ae9ed77c79b4a39abbc047df68a6b1d) )
	ROM_LOAD32_BYTE( "mk2p.uj20", 0x400003, 0x100000, CRC(0f9c9a12) SHA1(6c593ab037f9e203e50a13bef1cdbba765a45845) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2r32e )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "uj12.l32e", 0x00000, 0x80000, CRC(43f773a6) SHA1(a97b75bac2793f99738abcbd4054f2b860aff574) ) // Revision 3.2 Euro
	ROM_LOAD16_BYTE( "ug12.l32e", 0x00001, 0x80000, CRC(dcde9619) SHA1(72b39bd68eff5938cd87d3388074172a07bda816) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2r31e )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "uj12.l31e", 0x00000, 0x80000, CRC(f64306d1) SHA1(b1fb8d59400a411498a56a740a7b35e4687ecebd) ) // Revision 3.1 Euro
	ROM_LOAD16_BYTE( "ug12.l31e", 0x00001, 0x80000, CRC(4adeae7e) SHA1(4c9e5c7df3f86cc5c97c7fb70d4acca71d65cab5) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2r30 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l3_mortal_kombat_ii_game_rom_uj12.uj12.l30", 0x00000, 0x80000, CRC(93440895) SHA1(e81735db939cd12b3836c7b9507a087e6899cdbd) ) // Revision 3.0
	ROM_LOAD16_BYTE( "l3_mortal_kombat_ii_game_rom_ug12.ug12.l30", 0x00001, 0x80000, CRC(6153c2d8) SHA1(7b12eecc830f770a9c605a7e8376c8e719c33678) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2r21 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l2.1_mortal_kombat_ii_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(d6a35699) SHA1(17feee7886108d6f946bf04669479d35c2edac76) ) // Revision 2.1
	ROM_LOAD16_BYTE( "l2.1_mortal_kombat_ii_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(aeb703ff) SHA1(e94cd9e6feb45e3de85661ca12452aff6e14d3ae) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2r20 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l2_mortal_kombat_ii_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(72071550) SHA1(af0fb357e423eb054d32a1b2b216fb18437939ed) ) // Revision 2.0
	ROM_LOAD16_BYTE( "l2_mortal_kombat_ii_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(86c3ce65) SHA1(09d4dd6905911d8febe516f018e445657e929959) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2r14 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l1.4_mortal_kombat_ii_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(6d43bc6d) SHA1(578ea9c60fa94689d6ae583b86769cd56d8db311) ) // Revision 1.4
	ROM_LOAD16_BYTE( "l1.4_mortal_kombat_ii_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(42b0da21) SHA1(94ef25b04c35b4c26b692c2c3c5f68ba747bef49) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END

ROM_START( mk2r11 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l1.1_mortal_kombat_ii_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(01daff19) SHA1(8b14bf823ecb60c391688c106a52f141f1d291b5) ) // Revision 1.1
	ROM_LOAD16_BYTE( "l1.1_mortal_kombat_ii_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(54042eb7) SHA1(cda2f940b9c74989450611e6319e7cdadc05c627) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END

ROM_START( mk2r42 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "mk242j12.bin", 0x00000, 0x80000, CRC(c7fb1525) SHA1(350be1a6f6da3a6b42764cfceae196696482def2) )
	ROM_LOAD16_BYTE( "mk242g12.bin", 0x00001, 0x80000, CRC(443d0e0a) SHA1(20e69c266cda59be92d7cd6423f6e03ad65226eb) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2r91 )
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "uj12.l91", 0x00000, 0x80000, CRC(41953903) SHA1(f72f92beb32e724d37e5f951b24539902dc16a9f) )
	ROM_LOAD16_BYTE( "ug12.l91", 0x00001, 0x80000, CRC(c07f745a) SHA1(049a18bc162274c897cae695032f32c851e57330) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


ROM_START( mk2chal ) // Known as the Challenger Hack because the version number has been replaced with "CHALLENGER.." in the Test Menu
	ROM_REGION16_LE( 0xc00000, "dcs", ROMREGION_ERASEFF )   // sound data
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u2.u2", 0x000000, 0x80000, CRC(5f23d71d) SHA1(54c2afef243759e0f3dbe2907edbc4302f5c8bad) )
	ROM_RELOAD(                                             0x100000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u3.u3", 0x200000, 0x80000, CRC(d6d92bf9) SHA1(397351c6b707f2595e36360471015f9fa494e894) )
	ROM_RELOAD(                                             0x300000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u4.u4", 0x400000, 0x80000, CRC(eebc8e0f) SHA1(705ab63ff7672a4857d546afda6dca4973cce1ad) )
	ROM_RELOAD(                                             0x500000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u5.u5", 0x600000, 0x80000, CRC(2b0b7961) SHA1(1cdc64aab74d14afbd8c3531e3d0bd49271a281f) )
	ROM_RELOAD(                                             0x700000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u6.u6", 0x800000, 0x80000, CRC(f694b27f) SHA1(d43e38a124665f49ebb4ffc5a55e8f19a1a64686) )
	ROM_RELOAD(                                             0x900000, 0x80000 )
	ROM_LOAD16_BYTE( "l1_mortal_kombat_ii_sound_rom_u7.u7", 0xa00000, 0x80000, CRC(20387e0a) SHA1(505d05173b2a1f1ee3ebc2898ccd3a95c98dd04a) )
	ROM_RELOAD(                                             0xb00000, 0x80000 )
	// su8 and su9 are unpopulated

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "immortal_kombat_ii_j-12.uj12", 0x00000, 0x80000, CRC(2d5c04e6) SHA1(85947876319c86bdcdeccda99ae1ddbcfb212484) ) // labeled IMMORTAL KOMBAT II J-12
	ROM_LOAD16_BYTE( "immortal_kombat_ii_g-12.ug12", 0x00001, 0x80000, CRC(3e7a4bad) SHA1(9a8ad99e09badcea7f2bcf80a649c96a883a0463) ) // labeled IMMORTAL KOMBAT II G-12

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug14.ug14", 0x000000, 0x100000, CRC(01e73af6) SHA1(6598cfd704cc92a7f358a0e1f1c973ab79dcc493) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj14.uj14", 0x000001, 0x100000, CRC(d4985cbb) SHA1(367865da7efae38d83de3c0868d02a705177ae63) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug19.ug19", 0x000002, 0x100000, CRC(fec137be) SHA1(f11ecb8a7993f5c4f4449564b4911f69bd6e9bf8) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj19.uj19", 0x000003, 0x100000, CRC(2d763156) SHA1(06536006da49ab5fb6b75b25f801b83fad000ff5) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug16.ug16", 0x400000, 0x100000, CRC(8ba6ae18) SHA1(465fe907de4a1e502180c4e41642998dd3abc8e6) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj16.uj16", 0x400001, 0x100000, CRC(39d885b4) SHA1(2251826d247c3c6df421124718401fb35a672f83) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug20.ug20", 0x400002, 0x100000, CRC(809118c1) SHA1(86153e648834c749e34573151cd4fee403a81962) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj20.uj20", 0x400003, 0x100000, CRC(b96824f0) SHA1(d42b122f9a57da330192abc7e5f97abc4065d718) )

	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug17.ug17", 0x800000, 0x100000, CRC(937d8620) SHA1(8b9f80a460b124a747a6d1495b53f01f580e28f1) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj17.uj17", 0x800001, 0x100000, CRC(218de160) SHA1(87aea173720d2a33d8183903f4fe8ba1d47e3348) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_ug22.ug22", 0x800002, 0x100000, CRC(154d53b1) SHA1(58ff0aa59101f40a9a3b5fbae1c904d0b0b31612) )
	ROM_LOAD32_BYTE( "l1_mortal_kombat_ii_game_rom_uj22.uj22", 0x800003, 0x100000, CRC(8891d785) SHA1(fd460df1ef8f4306ea42f7dc41488a80fd2c8f53) )
ROM_END


/*
    equivalences for the extension board version (same contents, split in half)

    ROM_LOAD( "ug14.l1",  0x000000, 0x080000, CRC(74f5aaf1) SHA1(065e162392d4a305eb3787332f60e7f87a31a1d7) )
    ROM_LOAD( "ug16.l11", 0x080000, 0x080000, CRC(1cf58c4c) SHA1(7a672d61fa275871e82e94a9f5d3695e87b580bd) )
    ROM_LOAD( "u8.l1",    0x200000, 0x080000, CRC(56e22ff5) SHA1(d245bd3fecea09f6c9c5e1991958fcbc48a24771) )
    ROM_LOAD( "u11.l1",   0x280000, 0x080000, CRC(559ca4a3) SHA1(f48e0f8f2238bb52e251fd0e48838afdb5c625da) )
    ROM_LOAD( "ug17.l1",  0x100000, 0x080000, CRC(4202d8bf) SHA1(5126e7c59f677392fb02f1195dab43ee5ad232bb) )
    ROM_LOAD( "ug18.l1",  0x180000, 0x080000, CRC(a3deab6a) SHA1(b529a8b51dc70b0550b36d74e15c9efb10323833) )

    ROM_LOAD( "uj14.l1",  0x300000, 0x080000, CRC(869a3c55) SHA1(e51f36c3bd88af4ac922ad45f0e25211efd598aa) )
    ROM_LOAD( "uj16.l11", 0x380000, 0x080000, CRC(c70cf053) SHA1(e120f63c04c22016967b555194c91a79b39444cb) )
    ROM_LOAD( "u9.l1",    0x500000, 0x080000, CRC(67da0769) SHA1(8c98b78f6325e2fd1151141cef5ec51cc71ec9ce) )
    ROM_LOAD( "u10.l1",   0x580000, 0x080000, CRC(69000ac3) SHA1(0c173032095f8e0abfc17008440afcdd1cde8f68) )
    ROM_LOAD( "uj17.l1",  0x400000, 0x080000, CRC(ec3e1884) SHA1(e4e58c53526dc7af6127000e6e495879eb939d60) )
    ROM_LOAD( "uj18.l1",  0x480000, 0x080000, CRC(c9f5aef4) SHA1(0459206a1ab296a13905da9b59ca33d2ed311bb4) )

    ROM_LOAD( "u6.l1",    0x600000, 0x080000, CRC(8d4c496a) SHA1(0fe09f74dbd5c62206d8d355ef2ea1bfdbf31451) )
    ROM_LOAD( "u13.l11",  0x680000, 0x080000, CRC(7fb20a45) SHA1(b6f648d9ea3e7a11df95447bf466a55310b27dd9) )
    ROM_LOAD( "ug19.l1",  0x800000, 0x080000, CRC(d6c1f75e) SHA1(b8900b9e92136f23f6b64821e7561dfdd5530c8e) )
    ROM_LOAD( "ug20.l1",  0x880000, 0x080000, CRC(19a33cff) SHA1(5e5beb389fe1791a61b91fd05764de229e94725f) )
    ROM_LOAD( "ug22.l1",  0x700000, 0x080000, CRC(db6cfa45) SHA1(2d32dd2b206f77b0740a24143aa816e3d3a4df76) )
    ROM_LOAD( "ug23.l1",  0x780000, 0x080000, CRC(bfd8b656) SHA1(76d7a4e82d8a28eba5c6adc8490954e099353784) )

    ROM_LOAD( "u7.l1",    0x900000, 0x080000, CRC(3988aac8) SHA1(6b4a61226ee824870c2aec38533564d4c48dfe1a) )
    ROM_LOAD( "u12.l11",  0x980000, 0x080000, CRC(2ef12cc6) SHA1(1585242f3dcfdc8129a1f83995b9945ffa11ccad) )
    ROM_LOAD( "uj19.l1",  0xb00000, 0x080000, CRC(4eed6f18) SHA1(bb049c784fdcb0de332917f6e1fe48aab18a8b23) )
    ROM_LOAD( "uj20.l1",  0xb80000, 0x080000, CRC(337b1e20) SHA1(bb76b7a7fab529bc6cc07305b71d9345de3789be) )
    ROM_LOAD( "uj22.l1",  0xa00000, 0x080000, CRC(a6546b15) SHA1(756210dda34dbe7784a291a5f76c2b60cc214b3c) )
    ROM_LOAD( "uj23.l1",  0xa80000, 0x080000, CRC(45867c6f) SHA1(cf65f906c1f7f5b73cb71e4ae4e944646ef0bf98) )
*/
/*

Notes about NBA JAM's sound PCB & sound ROMs

At least one PCB has been found with labels in the following format:

NBA JAM       U3
SOUND ROM
(c)WMS INC     $ FC0A
   2-10-93    REV SLA2

NBA JAM       U13
SOUND ROM
(c)WMS INC     $ 9858
   2-1-93     REV SLA1

NBA JAM       U12
SOUND ROM
(c)WMS INC     $ 304A
   2-1-93     REV SLA1

Most commonly labels as ("L2" is printed in large black strip perpendicular to the rest of the print):

L2 NBA JAM
   U3  SOUND ROM
   (c) 1993 MIDWAY
   MANUFACTURING CO.

L2 NBA JAM
   U12  SOUND ROM
   (c) 1993 MIDWAY
   MANUFACTURING CO.

L2 NBA JAM
   U13  SOUND ROM
   (c) 1993 MIDWAY
   MANUFACTURING CO.


NOTE: The current sound ROMs match the L1 checksums listed above. Midway would commonly raise all ROMs to the same revision.
      The program ROMs for L2 report as 2-10-93 like the L2 sound ROM's label. Therefor there might be a L1 revision of the
      U3 sound ROM dated 2-1-93 specific to the L1 program ROM set which report as 2-1-93 like the 2 listed above sound ROMs.
*/

ROM_START( nbajam )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l2_nba_jam_u3_sound_rom.u3", 0x010000, 0x20000, CRC(3a3ea480) SHA1(d12a45cba5c35f046b176661d7877fa4fd0e6c13) )
	ROM_RELOAD(                              0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_u12_sound_rom.u12", 0x000000, 0x80000, CRC(b94847f1) SHA1(e7efa0a379bfa91fe4ffb75f07a5dfbfde9a96b4) ) // may be labeled as L2 revision
	ROM_LOAD( "l1_nba_jam_u13_sound_rom.u13", 0x080000, 0x80000, CRC(b6fe24bd) SHA1(f70f75b5570a2b368ebc74d2a7d264c618940430) ) // may be labeled as L2 revision

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l3_nba_jam_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(b93e271c) SHA1(b0e9f055376a4a4cd1115a81f71c933903c251b1) )
	ROM_LOAD16_BYTE( "l3_nba_jam_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(407d3390) SHA1(a319bc890d94310e44fe2ec98bfc95665a662701) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(8591c572) SHA1(237bab2e93abf438a84be3603505db5de59922af) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(d2e554f1) SHA1(139aa39bd48b8605058ece188f9f5e6793561fcb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(44fd6221) SHA1(1d6754bf2c24950080523f66b77407931babba29) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(f9cebbb6) SHA1(6202e490bc5658bd0741422f841540fcd037cfee) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(6f921886) SHA1(72542249ca6602dc4816952765c1810f064ff394) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(b2e14981) SHA1(5cec9b7fcaa6d0ce5bff689541fc98db435c5b5f) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(ab05ed89) SHA1(4153d098fbaeac963d93f26dcd9d8bc33a48a734) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(59a95878) SHA1(b95165987853f164842ab2b5895ea95484a1d78b) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(5162d3d6) SHA1(14d377977510b7793e4006a7a5089dbfd785d7d1) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(fdee0037) SHA1(3bcc740f4bdb3236822cd6e7ed06241804351cca) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(7b934c7a) SHA1(a6992fb3c50429ac4fa15bd91612ae0c0b8f961d) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(427d2eee) SHA1(4985e3dd9c9e1bedd5a900958bf549656debd494) )
ROM_END


ROM_START( nbajamr2 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l2_nba_jam_u3_sound_rom.u3", 0x010000, 0x20000, CRC(3a3ea480) SHA1(d12a45cba5c35f046b176661d7877fa4fd0e6c13) ) // sound program updated 2-10-93
	ROM_RELOAD(                              0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_u12_sound_rom.u12", 0x000000, 0x80000, CRC(b94847f1) SHA1(e7efa0a379bfa91fe4ffb75f07a5dfbfde9a96b4) ) // may be labeled as L2 revision
	ROM_LOAD( "l1_nba_jam_u13_sound_rom.u13", 0x080000, 0x80000, CRC(b6fe24bd) SHA1(f70f75b5570a2b368ebc74d2a7d264c618940430) ) // may be labeled as L2 revision

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l2_nba_jam_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(0fe80b36) SHA1(fe6b21dc9b393b25c511b2914b568fa92301d749) )
	ROM_LOAD16_BYTE( "l2_nba_jam_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(5d106315) SHA1(e2cddd9ed6771e77711e3a4f25fe2d07712d954e) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(8591c572) SHA1(237bab2e93abf438a84be3603505db5de59922af) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(d2e554f1) SHA1(139aa39bd48b8605058ece188f9f5e6793561fcb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(44fd6221) SHA1(1d6754bf2c24950080523f66b77407931babba29) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(f9cebbb6) SHA1(6202e490bc5658bd0741422f841540fcd037cfee) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(6f921886) SHA1(72542249ca6602dc4816952765c1810f064ff394) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(b2e14981) SHA1(5cec9b7fcaa6d0ce5bff689541fc98db435c5b5f) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(ab05ed89) SHA1(4153d098fbaeac963d93f26dcd9d8bc33a48a734) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(59a95878) SHA1(b95165987853f164842ab2b5895ea95484a1d78b) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(5162d3d6) SHA1(14d377977510b7793e4006a7a5089dbfd785d7d1) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(fdee0037) SHA1(3bcc740f4bdb3236822cd6e7ed06241804351cca) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(7b934c7a) SHA1(a6992fb3c50429ac4fa15bd91612ae0c0b8f961d) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(427d2eee) SHA1(4985e3dd9c9e1bedd5a900958bf549656debd494) )
ROM_END


ROM_START( nbajamr1 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l2_nba_jam_u3_sound_rom.u3", 0x010000, 0x20000, CRC(3a3ea480) SHA1(d12a45cba5c35f046b176661d7877fa4fd0e6c13) ) // missing undumped L1 ROM - should be dated 2/1/93
	ROM_RELOAD(                              0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_u12_sound_rom.u12", 0x000000, 0x80000, CRC(b94847f1) SHA1(e7efa0a379bfa91fe4ffb75f07a5dfbfde9a96b4) )
	ROM_LOAD( "l1_nba_jam_u13_sound_rom.u13", 0x080000, 0x80000, CRC(b6fe24bd) SHA1(f70f75b5570a2b368ebc74d2a7d264c618940430) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l1_nba_jam_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(4db672ec) SHA1(bb329c552473179f617d3bd038f47fb69d060b55) )
	ROM_LOAD16_BYTE( "l1_nba_jam_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(ed1df3f7) SHA1(36b0c47758a205719dbef169f0af3e761f557b99) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(8591c572) SHA1(237bab2e93abf438a84be3603505db5de59922af) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(d2e554f1) SHA1(139aa39bd48b8605058ece188f9f5e6793561fcb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(44fd6221) SHA1(1d6754bf2c24950080523f66b77407931babba29) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(f9cebbb6) SHA1(6202e490bc5658bd0741422f841540fcd037cfee) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(6f921886) SHA1(72542249ca6602dc4816952765c1810f064ff394) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(b2e14981) SHA1(5cec9b7fcaa6d0ce5bff689541fc98db435c5b5f) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(ab05ed89) SHA1(4153d098fbaeac963d93f26dcd9d8bc33a48a734) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(59a95878) SHA1(b95165987853f164842ab2b5895ea95484a1d78b) )

	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(5162d3d6) SHA1(14d377977510b7793e4006a7a5089dbfd785d7d1) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(fdee0037) SHA1(3bcc740f4bdb3236822cd6e7ed06241804351cca) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(7b934c7a) SHA1(a6992fb3c50429ac4fa15bd91612ae0c0b8f961d) )
	ROM_LOAD32_BYTE( "l1_nba_jam_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(427d2eee) SHA1(4985e3dd9c9e1bedd5a900958bf549656debd494) )
ROM_END


ROM_START( nbajamp2 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "p1_nba_jam_u3_sound_rom.u3", 0x010000, 0x20000, CRC(3d13633c) SHA1(b9597c352f56d67e5fdc958319285bbed0fcfbea) ) // all sound ROMs labeled as P1
	ROM_RELOAD(                              0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "p1_nba_jam_u12_sound_rom.u12", 0x000000, 0x80000, CRC(009aad42) SHA1(6cdef52ca565919626475a9ef5f264c55b899ea6) )
	ROM_LOAD( "p1_nba_jam_u13_sound_rom.u13", 0x080000, 0x80000, CRC(248800c2) SHA1(c6d7cd7841d751ee188c7bfd1ebbed380a18116e) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "p2_nba_jam_game_rom_uj12.uj12", 0x00000, 0x40000, CRC(4ebdf669) SHA1(8ab0d6084ed39ea7872aa5749148500ab1f1f692) ) // program ROMs labeled as P2
	ROM_RELOAD(                                       0x80000, 0x40000 )
	ROM_LOAD16_BYTE( "p2_nba_jam_game_rom_ug12.ug12", 0x00001, 0x40000, CRC(8d6098b6) SHA1(ca2e9be3ae77b379e8aa83b5ef7fda9fdaa3903c) )
	ROM_RELOAD(                                       0x80001, 0x40000 )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(39e16e0b) SHA1(9cd5b4b74d5bdf89a348d37a235df7988f91a133) ) // all graphics ROMs labeled as P1
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(a9ef8b67) SHA1(4d7faf6c0d4fdf98d33c9a01221e15cd5bbdaa88) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a88b961c) SHA1(28d087acedeba67413bcc3fd26a872459fa27161) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(a19d9889) SHA1(72189d96feb74880078c951a7b87bf363442564f) )

	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(946b2ab0) SHA1(e29812d223a21dbbc84e3d07682d512a05162b56) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(46e11687) SHA1(6c720cbac8e29f65c1d83a4dca7a9711440bdcfb) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(d62be814) SHA1(a7d6b5e29611a4eb98da8b9a6f6b67192e6be80b) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf8081a5) SHA1(f910a8a100bdd4920e35e831f02614746f9b6b7a) )

	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(5e286f81) SHA1(feea0cd29a1f3f3bcb8ccc7507e9ad448abf8130) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(a86775e2) SHA1(e3a90a50fd1163808292988bdd903d13e096cba1) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(b4ad0c2f) SHA1(e2cd8bfa485e61398ff934c1c9a942670dc07249) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(5b1bb97d) SHA1(5df8c356fb19aed63f440daa9d5dc427e4a856e5) )

	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(5acf3792) SHA1(05d3bc54903b2cc710437570590bf97bd54804f4) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(e00f906a) SHA1(519c5eeb3e83364f71147f91a999d6ea1cdbd74a) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(d7f199f6) SHA1(f752652a1531291fc538c3b03f04062d1a886f14) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(5f87a4cf) SHA1(ac3c5abe68225c3a7878b93f9f44bdcb1d5f734e) )
ROM_END


ROM_START( nbajamp1 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "p1_nba_jam_u3_sound_rom.u3", 0x010000, 0x20000, CRC(3d13633c) SHA1(b9597c352f56d67e5fdc958319285bbed0fcfbea) ) // all sound ROMs labeled as P1
	ROM_RELOAD(                              0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "p1_nba_jam_u12_sound_rom.u12", 0x000000, 0x80000, CRC(009aad42) SHA1(6cdef52ca565919626475a9ef5f264c55b899ea6) )
	ROM_LOAD( "p1_nba_jam_u13_sound_rom.u13", 0x080000, 0x80000, CRC(248800c2) SHA1(c6d7cd7841d751ee188c7bfd1ebbed380a18116e) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "p1_nba_jam_game_rom_uj12.uj12", 0x00000, 0x40000, CRC(c0faf310) SHA1(f37d796ab8b06861853fe24a3b4ccfb27a3832b5) ) // program ROMs labeled as P1
	ROM_RELOAD(                                       0x80000, 0x40000 )
	ROM_LOAD16_BYTE( "p1_nba_jam_game_rom_ug12.ug12", 0x00001, 0x40000, CRC(5ee68e03) SHA1(eec97375e3ad9b1aa5d2a6793289fa0c002d1343) )
	ROM_RELOAD(                                       0x80001, 0x40000 )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(39e16e0b) SHA1(9cd5b4b74d5bdf89a348d37a235df7988f91a133) ) // all graphics ROMs labeled as P1
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(a9ef8b67) SHA1(4d7faf6c0d4fdf98d33c9a01221e15cd5bbdaa88) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a88b961c) SHA1(28d087acedeba67413bcc3fd26a872459fa27161) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(a19d9889) SHA1(72189d96feb74880078c951a7b87bf363442564f) )

	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(946b2ab0) SHA1(e29812d223a21dbbc84e3d07682d512a05162b56) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(46e11687) SHA1(6c720cbac8e29f65c1d83a4dca7a9711440bdcfb) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(d62be814) SHA1(a7d6b5e29611a4eb98da8b9a6f6b67192e6be80b) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf8081a5) SHA1(f910a8a100bdd4920e35e831f02614746f9b6b7a) )

	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(5e286f81) SHA1(feea0cd29a1f3f3bcb8ccc7507e9ad448abf8130) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(a86775e2) SHA1(e3a90a50fd1163808292988bdd903d13e096cba1) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(b4ad0c2f) SHA1(e2cd8bfa485e61398ff934c1c9a942670dc07249) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(5b1bb97d) SHA1(5df8c356fb19aed63f440daa9d5dc427e4a856e5) )

	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(5acf3792) SHA1(05d3bc54903b2cc710437570590bf97bd54804f4) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(e00f906a) SHA1(519c5eeb3e83364f71147f91a999d6ea1cdbd74a) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(d7f199f6) SHA1(f752652a1531291fc538c3b03f04062d1a886f14) )
	ROM_LOAD32_BYTE( "p1_nba_jam_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(5f87a4cf) SHA1(ac3c5abe68225c3a7878b93f9f44bdcb1d5f734e) )
ROM_END


ROM_START( nbajamte )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l4_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(d7c21bc4) SHA1(e05f0299b955500df6a08b1c0b24b932a9cdfa6a) ) // sldh - rev 4.0 3/23/94
	ROM_LOAD16_BYTE( "l4_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(7ad49229) SHA1(e9ceedb0e620809d8a4d42087d806aa296a4cd59) ) // sldh - rev 4.0 3/23/94

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamte4 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l4_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(f94074f8) SHA1(0d669a38f33b000ec12352ae15ebdd7849b6ad50) ) // sldh - rev 4.0 3/03/94
	ROM_LOAD16_BYTE( "l4_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(2c55890b) SHA1(839492d50474a54a434090a5f06548963773aec7) ) // sldh - rev 4.0 3/03/94

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamte3 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l3_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(8fdf77b4) SHA1(1a8a178b19d0b8e7a5fd2ddf373a4279321440d0) ) // sldh - rev 3.0 3/04/94
	ROM_LOAD16_BYTE( "l3_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(656579ed) SHA1(b038fdc814ebc8d203724fdb2f7501d40f1dc21f) ) // sldh - rev 3.0 3/04/94

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamte3a )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l3_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(83f03079) SHA1(2aa95339edb8b50b38f0842c960ca2adee4db5dd) ) // sldh - rev 3.0 2/26/94
	ROM_LOAD16_BYTE( "l3_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(121ccb3a) SHA1(c5e76f34b222f33e7af957bd57b45d30f43cb012) ) // sldh - rev 3.0 2/26/94

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamte2 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l2.1_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(d009aa29) SHA1(2f9317d3f89488a3593637a37eea4ac68dd1067b) )
	ROM_LOAD16_BYTE( "l2.1_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(6c3bfb6a) SHA1(e05cbe33661fb37a929c6a75d9e0f3469cc81d2d) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamte2a )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l2_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(eaa6fb32) SHA1(8c8c0c6ace2b98679d7fe90e1f9284bdf0e14eaf) )
	ROM_LOAD16_BYTE( "l2_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(5a694d9a) SHA1(fb74e4242d9adba03f24a81451ea06e8d9b4af96) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamte1 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "l1_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(a9f555ad) SHA1(34f5fc1b003ef8acbb2b38fbacd58d018d20ab1b) )
	ROM_LOAD16_BYTE( "l1_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(bd4579b5) SHA1(c893cff931f1e60a1d0d29d2719f514d92fb3490) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


// romset contained only the program ROMs and PCB pics are available, so ideally every other one should be checked if another PCB ever shows up
// not marking them as BAD_DUMP as they pass the ROM test
ROM_START( nbajamtep2 )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "p2_nba_jam_tournament_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(f90f7450) SHA1(ecc2b801edd1e0529fe0e52471b7876f748cf296) )
	ROM_LOAD16_BYTE( "p2_nba_jam_tournament_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(a0d9d49a) SHA1(8fac949b9707d821e35ad2f2decb67b5bab28b40) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamten )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(                                         0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "l1_nba_jam_tournament_u12_sound_rom.u12", 0x000000, 0x80000, CRC(4fac97bc) SHA1(bd88d8c3edab0e35ad9f9350bcbaa17cda61d87a) )
	ROM_LOAD( "l1_nba_jam_tournament_u13_sound_rom.u13", 0x080000, 0x80000, CRC(6f27b202) SHA1(c1f0db15624d1e7102ce9fd1db49ccf86e8611d6) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "nani-uj12.bin", 0x00000, 0x80000, CRC(a2662e74) SHA1(7a6c18464446baf3d279013eb95bf862b5b3be70) )
	ROM_LOAD16_BYTE( "nani-ug12.bin", 0x00001, 0x80000, CRC(40cda5b1) SHA1(2ff51f830aa86f6456c626666e221be1f7bfbfa2) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(c7ce74d0) SHA1(93861cd909e0f28ed112096d6f9fc57d0d31c57c) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(905ad88b) SHA1(24c336ccc0e2ac0ee96a34ad6fe4aa7464de0009) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(8a48728c) SHA1(3684099b4934b027336c319c77d9e0710b8c22dc) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(bf263d61) SHA1(b5b59e8df55f8030eff068c1d8b07dad8521bf5d) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(9401be62) SHA1(597413a8a1eb66a7ad89af2f548fa3062e5e8efb) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(8a852b9e) SHA1(604c7f4305887e9505320630027765ea76607c58) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(3b05133b) SHA1(f6067abb92b8751afe7352a4f1b1a22c9528002b) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(39791051) SHA1(7aa02500ddacd31fca04044a22a38f36452ca300) )

	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(6fd08f57) SHA1(5b7031dffc88374c5bfdf3021aa01ec4e28d0631) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(4eb73c26) SHA1(693bf45f777da8e55b7bcd8699ea5bd711964941) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(854f73bc) SHA1(242cc8ce28711f6f0787524a1070eb4b0956e6ae) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(f8c30998) SHA1(33e2f982d74e9f3686b1f4a8172c49fb8b604cf5) )
ROM_END


ROM_START( nbajamter )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) /* sound CPU */
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(             							0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD( "nba_jam_te_rewind_sound_rom.u12", 0x000000, 0x80000, CRC(cd5d4532) SHA1(63ff4b5b22e02ee90cdcb745acc6bf9dbc1c3140) )
	ROM_LOAD( "nba_jam_te_rewind_sound_rom.u13", 0x080000, 0x80000, CRC(e92fb0d3) SHA1(bac12a69f74051cb97326c05eb77d88acdf81a63) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   /* 34010 code */
	ROM_LOAD16_BYTE( "nba_jam_te_rewind_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(9b3fc483) SHA1(4e7b6b85d9972063ebdb08ad2c860e082b6e886b) )
	ROM_LOAD16_BYTE( "nba_jam_te_rewind_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(18e75204) SHA1(e33eaa969dbc7ac19641238fc62d2af4d75bddef) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(b9a07a6f) SHA1(0b9410ae9d761928b828449f45302f0951a6a8af) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(ffa7db04) SHA1(984a1011f378d7e77acf43f285f7c72f035fc7ee) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(67c8646b) SHA1(a0584e79cdd5c83cd62bda09470f67082a670fe1) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(71d028f8) SHA1(6c9d3cb6b937a53af8be0c7c5e209fb8740d6d9a) )
								
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(34c6bdb8) SHA1(fe613fc3256eafc368ba566dd989c05293df1829) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(3af5b32e) SHA1(397d2524a5749f8e8419280cc3c48ec3081834b5) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(d41234d2) SHA1(9dab65c7843758572634c4805227fee1850b0349) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(42196c84) SHA1(7f30204a4764b4afd38cf82f8be909c9ee5a0a0b) )
								
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(fe18a6ef) SHA1(58b8bbf257b3de5d7536421dda302e2d93f51999) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(9a6d36de) SHA1(1bb24ff5fdbdc1f7265da0ba43e3d96ab589548f) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(8d1af1a6) SHA1(4de21240ec0ffce9346e75b9719d9ae71bfbdb8e) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(b5bf66f9) SHA1(de12e47e44f9cf7c611d674fe6c9b3466fde2081) )
ROM_END


ROM_START( nbajamterx )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) /* sound CPU */
	ROM_LOAD(  "l1_nba_jam_tournament_u3_sound_rom.u3", 0x010000, 0x20000, CRC(d4551195) SHA1(e8908fbe4339fb8c93f7e74113dfd25dda1667ea) )
	ROM_RELOAD(             							0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  /* ADPCM */
	ROM_LOAD( "nba_jam_te_rewind_sound_rom.u12", 0x000000, 0x80000, CRC(cd5d4532) SHA1(63ff4b5b22e02ee90cdcb745acc6bf9dbc1c3140) )
	ROM_LOAD( "nba_jam_te_rewind_x_sound_rom.u13", 0x080000, 0x80000, CRC(c4cbede2) SHA1(8b1a26ee852281de1564af35fc3605a89eb0b7dd) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   /* 34010 code */
	ROM_LOAD16_BYTE( "nba_jam_te_rewind_x_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(383cd3ec) SHA1(ab1cfa38af37f36a8701b7b7a26f1d242050ad45) )
	ROM_LOAD16_BYTE( "nba_jam_te_rewind_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(18e75204) SHA1(e33eaa969dbc7ac19641238fc62d2af4d75bddef) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(04bb9f64) SHA1(9e1a8c37e14cb6fe67f4aa3caa9022f356f1ca64) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(b34b7af3) SHA1(0abb74d2f414bc9da0380a81beb134f3a87c1a0a) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(a8f22fbb) SHA1(514208a9d6d0c8c2d7847cc02d4387eac90be659) )
	ROM_LOAD32_BYTE( "l1_nba_jam_tournament_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(8130a8a2) SHA1(f23f124024285d07d8cf822817b62e42c38b82db) )

	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(b9a07a6f) SHA1(0b9410ae9d761928b828449f45302f0951a6a8af) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(ffa7db04) SHA1(984a1011f378d7e77acf43f285f7c72f035fc7ee) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(67c8646b) SHA1(a0584e79cdd5c83cd62bda09470f67082a670fe1) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(71d028f8) SHA1(6c9d3cb6b937a53af8be0c7c5e209fb8740d6d9a) )
								
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(34c6bdb8) SHA1(fe613fc3256eafc368ba566dd989c05293df1829) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(3af5b32e) SHA1(397d2524a5749f8e8419280cc3c48ec3081834b5) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(d41234d2) SHA1(9dab65c7843758572634c4805227fee1850b0349) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(42196c84) SHA1(7f30204a4764b4afd38cf82f8be909c9ee5a0a0b) )
								
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(fe18a6ef) SHA1(58b8bbf257b3de5d7536421dda302e2d93f51999) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(9a6d36de) SHA1(1bb24ff5fdbdc1f7265da0ba43e3d96ab589548f) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(8d1af1a6) SHA1(4de21240ec0ffce9346e75b9719d9ae71bfbdb8e) )
	ROM_LOAD32_BYTE( "nba_jam_te_rewind_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(b5bf66f9) SHA1(de12e47e44f9cf7c611d674fe6c9b3466fde2081) )
ROM_END


ROM_START( jdreddp )
	ROM_REGION( 0x50000, "adpcm:cpu", 0 ) // sound CPU
	ROM_LOAD(  "t1_judge_dredd_sound_rom_u3.u3", 0x010000, 0x20000, CRC(6154d108) SHA1(54328455ec22ba815de85aa3bfe6405353c64f5c) )
	ROM_RELOAD(                                  0x030000, 0x20000 )

	ROM_REGION( 0x100000, "adpcm:oki", 0 )  // ADPCM
	ROM_LOAD( "t1_judge_dredd_sound_rom_u12.u12", 0x000000, 0x80000, CRC(ef32f202) SHA1(16aea085e63496dec259291de1a64fbeab52f039) )
	ROM_LOAD( "t1_judge_dredd_sound_rom_u13.u13", 0x080000, 0x80000, CRC(3dc70473) SHA1(a3d7210301ff0579889009a075092115d9bf0600) )

	ROM_REGION16_LE( 0x100000, "maincpu", 0 )   // 34010 code
	ROM_LOAD16_BYTE( "t1_judge_dredd_game_rom_uj12.uj12", 0x00000, 0x80000, CRC(7e5c8d5a) SHA1(65c0e887fea01846426067adfc4cf60dce4a1e24) )
	ROM_LOAD16_BYTE( "t1_judge_dredd_game_rom_ug12.ug12", 0x00001, 0x80000, CRC(a16b8a4a) SHA1(77abb31e7cb3b66c63ef7c1874d8544ae9a02667) )

	ROM_REGION( 0xc00000, "video", 0 )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug14.ug14", 0x000000, 0x80000, CRC(468484d7) SHA1(87e3b87051e3afff097333af90efa0eb4dd61a35) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj14.uj14", 0x000001, 0x80000, CRC(fe6ec0ec) SHA1(3e3b1774e1c5cf6629fbd3aeff36cadff1adfbf9) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug19.ug19", 0x000002, 0x80000, CRC(e076c08e) SHA1(9b52470feac66b258e62e53dfd6a6a74c1e47ac1) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj19.uj19", 0x000003, 0x80000, CRC(bd8cffe0) SHA1(7690bfa82ab5c2c102dc5c6e60628f341b83a77b) )

	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug16.ug16", 0x200000, 0x80000, CRC(1d7f12b6) SHA1(beb864615a6c554097377a2f2b6dfe361c1fb084) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj16.uj16", 0x200001, 0x80000, CRC(31d4a71b) SHA1(703448956968f1913e5755a6aedf0f7d15ea4a4e) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug20.ug20", 0x200002, 0x80000, CRC(7b8c370a) SHA1(e6562782519610447657d0850481b1f9fd7c08b3) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj20.uj20", 0x200003, 0x80000, CRC(8fc7bfb9) SHA1(c3c31ea641a6e304b060a7938e2ac473db8a7aab) )

	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug17.ug17", 0x400000, 0x80000, CRC(b6d83d74) SHA1(e0e71f691af5b55fb4153a6b80d3055641cb7cf4) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj17.uj17", 0x400001, 0x80000, CRC(ddc76f0b) SHA1(8f3091c6a5ec1488fcd296e75bbd0572f1a4485c) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug22.ug22", 0x400002, 0x80000, CRC(6705d5b3) SHA1(da304ea33cd20c118b97147fe603237fe5940732) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj22.uj22", 0x400003, 0x80000, CRC(7438295e) SHA1(dbc28a9273897d50abf8e7bebe0753949365eb42) )

	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug18.ug18", 0x600000, 0x80000, CRC(c8a45e01) SHA1(6d63a977c30d5f421baf48db55da90c75032a75f) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj18.uj18", 0x600001, 0x80000, CRC(3e16e7a9) SHA1(f517d42594225b06d70404f29e44dc144ad87a72) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_ug23.ug23", 0x600002, 0x80000, CRC(0c9edbc4) SHA1(bb3926a992efd1923d64c5bc615dac39867f426d) )
	ROM_LOAD32_BYTE( "t1_judge_dredd_game_rom_uj23.uj23", 0x600003, 0x80000, CRC(86ea157d) SHA1(9189e07abc73b601a26ae8aaf6d49ed87d1befca) )
ROM_END



/*************************************
 *
 *  Game drivers
 *
 *************************************/

GAME( 1992, mk,         0,        tunit_adpcm, mk,       midtunit_adpcm_state, init_mktunit,  ROT0, "Midway",   "Mortal Kombat (rev 5.0 T-Unit 03/19/93)", MACHINE_SUPPORTS_SAVE )
GAME( 1992, mkr4,       mk,       tunit_adpcm, mk,       midtunit_adpcm_state, init_mktunit,  ROT0, "Midway",   "Mortal Kombat (rev 4.0 T-Unit 02/11/93)", MACHINE_SUPPORTS_SAVE )
GAME( 1992, mktturbo,   mk,       tunit_adpcm, mk,       midtunit_adpcm_state, init_mkturbo,  ROT0, "hack",     "Mortal Kombat (Turbo Ninja T-Unit 03/19/93, hack)", MACHINE_SUPPORTS_SAVE )
GAME( 2020, mkp,        mk,       tunit_adpcm, mk,       midtunit_adpcm_state, init_mktunit,  ROT0, "Team Plus","Mortal Kombat + (alpha)", MACHINE_SUPPORTS_SAVE )

GAME( 1993, mk2,        0,        mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L3.1)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r32e,    mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L3.2, European)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r31e,    mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L3.1, European)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r30,     mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L3.0)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r21,     mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L2.1)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r20,     mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L2.0)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r14,     mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L1.4)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r11,     mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "Midway",   "Mortal Kombat II (rev L1.1)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r42,     mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "hack",     "Mortal Kombat II (rev L4.2, hack)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2r91,     mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "hack",     "Mortal Kombat II (rev L9.1, hack)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, mk2chal,    mk2,      mk2,         mk2,      mk2_state,            init_mk2,      ROT0, "hack",     "Mortal Kombat II Challenger (hack)", MACHINE_SUPPORTS_SAVE ) // program ROMs labeled as IMMORTAL KOMBAT II

GAME( 2018, mk2p,       mk2,      mk2,		   mk2,      mk2_state,            init_mk2,      ROT0, "Team Plus","Mortal Kombat II + (Beta II)", MACHINE_SUPPORTS_SAVE )

GAME( 1993, jdreddp,    0,        tunit_adpcm, jdreddp,  midtunit_adpcm_state, init_jdreddp,  ROT0, "Midway",   "Judge Dredd (rev TA1 7/12/92, location test)", MACHINE_SUPPORTS_SAVE )

GAME( 1993, nbajam,     0,        tunit_adpcm, nbajam,   midtunit_adpcm_state, init_nbajam,   ROT0, "Midway",   "NBA Jam (rev 3.01 4/07/93)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, nbajamr2,   nbajam,   tunit_adpcm, nbajam,   midtunit_adpcm_state, init_nbajam,   ROT0, "Midway",   "NBA Jam (rev 2.00 2/10/93)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, nbajamr1,   nbajam,   tunit_adpcm, nbajam,   midtunit_adpcm_state, init_nbajam,   ROT0, "Midway",   "NBA Jam (rev 1.00 2/1/93)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, nbajamp2,   nbajam,   tunit_adpcm, nbajam,   midtunit_adpcm_state, init_nbajam,   ROT0, "Midway",   "NBA Jam (proto v 2.00 1/24/93)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, nbajamp1,   nbajam,   tunit_adpcm, nbajam,   midtunit_adpcm_state, init_nbajam,   ROT0, "Midway",   "NBA Jam (proto v 1.01 1/23/93)", MACHINE_SUPPORTS_SAVE )

GAME( 1994, nbajamte,   0,        tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (rev 4.0 3/23/94)", MACHINE_SUPPORTS_SAVE )
GAME( 1994, nbajamte4,  nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (rev 4.0 3/03/94)", MACHINE_SUPPORTS_SAVE )
GAME( 1994, nbajamte3,  nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (rev 3.0 3/04/94)", MACHINE_SUPPORTS_SAVE )
GAME( 1994, nbajamte3a, nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (rev 3.0 2/26/94)", MACHINE_SUPPORTS_SAVE )
GAME( 1994, nbajamte2,  nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (rev 2.1 2/06/94)", MACHINE_SUPPORTS_SAVE )
GAME( 1994, nbajamte2a, nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (rev 2.0 1/28/94)", MACHINE_SUPPORTS_SAVE )
GAME( 1994, nbajamte1,  nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (rev 1.00 1/17/94)", MACHINE_SUPPORTS_SAVE )
GAME( 1993, nbajamtep2, nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (proto 2.00 12/17/93)", MACHINE_SUPPORTS_SAVE )
GAME( 1995, nbajamten,  nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "Midway",   "NBA Jam Tournament Edition (Nani Edition, rev 5.2 8/11/95, prototype)", MACHINE_SUPPORTS_SAVE )

GAME( 2022, nbajamter,	nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "ponlork",  "NBA Jam T.E. Rewind ver 1.1", MACHINE_SUPPORTS_SAVE )
GAME( 2022, nbajamterx,	nbajamte, tunit_adpcm, nbajamte, midtunit_adpcm_state, init_nbajamte, ROT0, "ponlork",  "NBA Jam T.E. Rewind ver X", MACHINE_SUPPORTS_SAVE )