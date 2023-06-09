// license:BSD-3-Clause
// copyright-holders:Tomasz Slanina
/*
Lady Frog (c) 1990 Mondial Games
(there's  "(c) Alfa Tecnology" in the ROM)

Touche Me - Unknown Manfacturer (likely the same as Lady Frog)

driver by Tomasz Slanina

'N.Y. Captor' (TAITO) hardware , without sub cpu.

TODO:
- merge with flstory.cpp

Sound rom is 'borrowed' from NYC.
1.115 = a80_16.i26 + a80_17.i25

PCB Layout
|-------------------------------------------------|
|18MHz                          1       M5232     |
|                                          LM3900 |
|                               6116    Z80-2     |
|                     6116                   8MHz |
|                         8MHz        N5C090-60   |
|           2148                      AY-3-8910   |
|           2148                                  |
|                                         LM3900  |
|                                                 |
|                             Z80-1               |
|                 2148         2                  |
|                 2148        6264                |
|6      3         2148                           J|
|7      4                                        A|
|8      5                                        M|
|                                                M|
|           2148                   DSWB  DSWA    A|
|           2148                                  |
|           2148                                  |
|-------------------------------------------------|

Notes:
      Z80-1 clock: 4.000MHz
      Z80-2 clock: 4.000MHz
      AY-3-8910 clock: 2.000MHz
      OKI M5232 clock: 2.000MHz
      VSync: 60Hz
      HSync: 15.68kHz

      N5C090-60: iNTEL simple PLD (PLCC44), 100% compatible with Altera EP900

*/

#include "emu.h"
#include "ladyfrog.h"

#include "cpu/z80/z80.h"
#include "sound/ay8910.h"
#include "sound/dac.h"
#include "screen.h"
#include "speaker.h"


uint8_t ladyfrog_state::from_snd_r()
{
	m_snd_flag = 0;
	return m_snd_data;
}

void ladyfrog_state::to_main_w(uint8_t data)
{
	m_snd_data = data;
	m_snd_flag = 2;
}

void ladyfrog_state::sound_cpu_reset_w(uint8_t data)
{
	m_audiocpu->set_input_line(INPUT_LINE_RESET, (data & 1 ) ? ASSERT_LINE : CLEAR_LINE);
}

TIMER_CALLBACK_MEMBER(ladyfrog_state::nmi_callback)
{
	if (m_sound_nmi_enable)
		m_audiocpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
	else
		m_pending_nmi = 1;
}

void ladyfrog_state::sound_command_w(uint8_t data)
{
	m_soundlatch->write(data);
	machine().scheduler().synchronize(timer_expired_delegate(FUNC(ladyfrog_state::nmi_callback),this), data);
}

void ladyfrog_state::nmi_disable_w(uint8_t data)
{
	m_sound_nmi_enable = 0;
}

void ladyfrog_state::nmi_enable_w(uint8_t data)
{
	m_sound_nmi_enable = 1;
	if (m_pending_nmi)
	{
		m_audiocpu->pulse_input_line(INPUT_LINE_NMI, attotime::zero);
		m_pending_nmi = 0;
	}
}

void ladyfrog_state::unk_w(uint8_t data)
{
}

uint8_t ladyfrog_state::snd_flag_r()
{
	return m_snd_flag | 0xfd;
}

void ladyfrog_state::ladyfrog_map(address_map &map)
{
	map(0x0000, 0xbfff).rom();
	map(0xc000, 0xc07f).ram();
	map(0xc080, 0xc87f).rw(FUNC(ladyfrog_state::ladyfrog_videoram_r), FUNC(ladyfrog_state::ladyfrog_videoram_w)).share("videoram");
	map(0xd000, 0xd000).w(FUNC(ladyfrog_state::ladyfrog_gfxctrl2_w));
	map(0xd400, 0xd400).rw(FUNC(ladyfrog_state::from_snd_r), FUNC(ladyfrog_state::sound_command_w));
	map(0xd401, 0xd401).r(FUNC(ladyfrog_state::snd_flag_r));
	map(0xd403, 0xd403).w(FUNC(ladyfrog_state::sound_cpu_reset_w));
	map(0xd800, 0xd800).portr("DSW1");
	map(0xd801, 0xd801).portr("DSW2");
	map(0xd804, 0xd804).portr("INPUTS");
	map(0xd806, 0xd806).portr("SYSTEM");
	map(0xdc00, 0xdc9f).rw(FUNC(ladyfrog_state::ladyfrog_spriteram_r), FUNC(ladyfrog_state::ladyfrog_spriteram_w));
	map(0xdca0, 0xdcbf).rw(FUNC(ladyfrog_state::ladyfrog_scrlram_r), FUNC(ladyfrog_state::ladyfrog_scrlram_w)).share("scrlram");
	map(0xdcc0, 0xdcff).ram();
	map(0xdd00, 0xdeff).rw(FUNC(ladyfrog_state::ladyfrog_palette_r), FUNC(ladyfrog_state::ladyfrog_palette_w));
	map(0xd0d0, 0xd0d0).nopr(); /* code jumps to ASCII text "Alfa tecnology"  @ $b7 */
	map(0xdf03, 0xdf03).w(FUNC(ladyfrog_state::ladyfrog_gfxctrl_w));
	map(0xe000, 0xffff).ram();
}

void ladyfrog_state::ladyfrog_sound_map(address_map &map)
{
	map(0x0000, 0xbfff).rom();
	map(0xc000, 0xc7ff).ram();
	map(0xc800, 0xc801).nopw();
	map(0xc802, 0xc803).w("aysnd", FUNC(ay8910_device::address_data_w));
	map(0xc900, 0xc90d).w(m_msm, FUNC(msm5232_device::write));
	map(0xca00, 0xca00).nopw();
	map(0xcb00, 0xcb00).nopw();
	map(0xcc00, 0xcc00).nopw();
	map(0xd000, 0xd000).r(m_soundlatch, FUNC(generic_latch_8_device::read)).w(FUNC(ladyfrog_state::to_main_w));
	map(0xd200, 0xd200).nopr().w(FUNC(ladyfrog_state::nmi_enable_w));
	map(0xd400, 0xd400).w(FUNC(ladyfrog_state::nmi_disable_w));
	map(0xd600, 0xd600).nopr().w("dac", FUNC(dac_byte_interface::data_w));       /* signed 8-bit DAC - unknown read */
	map(0xe000, 0xefff).noprw();
}


static INPUT_PORTS_START( ladyfrog )

	PORT_START("DSW1")
	PORT_DIPNAME( 0x03, 0x02, DEF_STR( Lives ) )  PORT_DIPLOCATION("SW1:1,2")
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPSETTING(    0x02, "3" )
	PORT_DIPSETTING(    0x01, "2" )
	PORT_DIPSETTING(    0x03, "1" )
	PORT_DIPNAME( 0x04, 0x00, "Clear 'doors' after life lost" )  PORT_DIPLOCATION("SW1:3")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW1:4" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Free_Play ) )  PORT_DIPLOCATION("SW1:5")
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Allow_Continue ) )  PORT_DIPLOCATION("SW1:6")
	PORT_DIPSETTING(    0x20, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_A ) )  PORT_DIPLOCATION("SW1:7,8")
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_3C ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x03, 0x01, DEF_STR( Difficulty ) )  PORT_DIPLOCATION("SW2:1,2")
	PORT_DIPSETTING(    0x03, DEF_STR( Easy ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Normal ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Hard ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x04, 0x04, "SW2:3" )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_DIPUNKNOWN_DIPLOC( 0x10, 0x10, "SW2:5" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW2:6" )
	PORT_DIPUNKNOWN_DIPLOC( 0x40, 0x40, "SW2:7" )
	PORT_DIPUNKNOWN_DIPLOC( 0x80, 0x80, "SW2:8" )

	PORT_START("INPUTS")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_4WAY
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_4WAY
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_4WAY
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_4WAY

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON1 )
INPUT_PORTS_END


static INPUT_PORTS_START( toucheme )
	PORT_INCLUDE( ladyfrog )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Allow_Continue ) )  PORT_DIPLOCATION("SW1:3")
	PORT_DIPSETTING(    0x04, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )

	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW1:4" )
	PORT_DIPUNKNOWN_DIPLOC( 0x20, 0x20, "SW1:6" )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Coin_A ) )  PORT_DIPLOCATION("SW1:7,8")
	PORT_DIPSETTING(    0x40, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_4C ) )

	PORT_MODIFY("DSW2")
	PORT_DIPUNKNOWN_DIPLOC( 0x01, 0x01, "SW2:1" )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Demo_Sounds ) )  PORT_DIPLOCATION("SW2:2")
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPUNKNOWN_DIPLOC( 0x08, 0x08, "SW2:4" )
INPUT_PORTS_END


static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 0, 4 },
	{ 3, 2, 1, 0, 8+3, 8+2, 8+1, 8+0 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static const gfx_layout spritelayout =
{
	16,16,
	RGN_FRAC(1,2),
	4,
	{ RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4, 0, 4 },
	{ 3, 2, 1, 0, 8+3, 8+2, 8+1, 8+0,
			16*8+3, 16*8+2, 16*8+1, 16*8+0, 16*8+8+3, 16*8+8+2, 16*8+8+1, 16*8+8+0 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			16*16, 17*16, 18*16, 19*16, 20*16, 21*16, 22*16, 23*16 },
	64*8
};



static GFXDECODE_START( gfx_ladyfrog )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout,     0, 16 )
	GFXDECODE_ENTRY( "gfx1", 0, spritelayout, 256, 16 )
GFXDECODE_END


void ladyfrog_state::machine_start()
{
	save_item(NAME(m_tilebank));
	save_item(NAME(m_palette_bank));
	save_item(NAME(m_sound_nmi_enable));
	save_item(NAME(m_pending_nmi));
	save_item(NAME(m_snd_flag));
	save_item(NAME(m_snd_data));
}

void ladyfrog_state::machine_reset()
{
	m_tilebank = 0;
	m_palette_bank = 0;
	m_sound_nmi_enable = 0;
	m_pending_nmi = 0;
	m_snd_flag = 0;
	m_snd_data = 0;
}

void ladyfrog_state::ladyfrog(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(8'000'000)/2);
	m_maincpu->set_addrmap(AS_PROGRAM, &ladyfrog_state::ladyfrog_map);
	m_maincpu->set_vblank_int("screen", FUNC(ladyfrog_state::irq0_line_hold));

	Z80(config, m_audiocpu, XTAL(8'000'000)/2);
	m_audiocpu->set_addrmap(AS_PROGRAM, &ladyfrog_state::ladyfrog_sound_map);
	m_audiocpu->set_periodic_int(FUNC(ladyfrog_state::irq0_line_hold), attotime::from_hz(2*60));

	config.set_maximum_quantum(attotime::from_hz(6000));

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
//  screen.set_refresh_hz(60);
//  screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
//  screen.set_size(32*8, 32*8);
//  screen.set_visarea(0*8, 32*8-1, 2*8, 30*8-1); // black borders in ladyfrog gameplay are correct
	screen.set_raw(XTAL(8'000'000), 510, 0, 256, 262, 2*8, 30*8); // pixel clock appears to run at 8 MHz
	screen.set_screen_update(FUNC(ladyfrog_state::screen_update_ladyfrog));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_ladyfrog);
	PALETTE(config, m_palette).set_format(palette_device::xBGR_444, 512);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, m_soundlatch);

	ay8910_device &aysnd(AY8910(config, "aysnd", XTAL(8'000'000)/4));
	aysnd.port_a_write_callback().set(FUNC(ladyfrog_state::unk_w));
	aysnd.port_b_write_callback().set(FUNC(ladyfrog_state::unk_w));
	aysnd.add_route(ALL_OUTPUTS, "mono", 0.15);

	MSM5232(config, m_msm, XTAL(8'000'000)/4);
	m_msm->set_capacitors(1e-6, 1e-6, 1e-6, 1e-6, 1e-6, 1e-6, 1e-6, 1e-6);
	m_msm->add_route(0, "mono", 1.0);   // pin 28  2'-1
	m_msm->add_route(1, "mono", 1.0);   // pin 29  4'-1
	m_msm->add_route(2, "mono", 1.0);   // pin 30  8'-1
	m_msm->add_route(3, "mono", 1.0);   // pin 31 16'-1
	m_msm->add_route(4, "mono", 1.0);   // pin 36  2'-2
	m_msm->add_route(5, "mono", 1.0);   // pin 35  4'-2
	m_msm->add_route(6, "mono", 1.0);   // pin 34  8'-2
	m_msm->add_route(7, "mono", 1.0);   // pin 33 16'-2
	// pin 1 SOLO  8'       not mapped
	// pin 2 SOLO 16'       not mapped
	// pin 22 Noise Output  not mapped

	DAC_8BIT_R2R(config, "dac", 0).add_route(ALL_OUTPUTS, "mono", 0.25); // unknown DAC
}

void ladyfrog_state::toucheme(machine_config &config)
{
	ladyfrog(config);
	MCFG_VIDEO_START_OVERRIDE(ladyfrog_state,toucheme)
}


ROM_START( ladyfrog )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "2.107",   0x0000, 0x10000, CRC(fa4466e6) SHA1(08e5cc8e1d3c845bc9c253267f2683671bffa9f2) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1.115",   0x0000, 0x8000, CRC(b0932498) SHA1(13d90698f2682e64ff3597c9267ca9d33a6d62ba) ) /* NY Captor*/

	ROM_REGION( 0x60000, "gfx1", ROMREGION_INVERT )
	ROM_LOAD( "3.32",   0x30000, 0x10000, CRC(8a27fc0a) SHA1(36e0365776e61ef830451e6351eca6b6c742086f) )
	ROM_LOAD( "4.33",   0x40000, 0x10000, CRC(e1a137d3) SHA1(add8140a9366a0d343b611ced10c804d3fb04c03) )
	ROM_LOAD( "5.34",   0x50000, 0x10000, CRC(7816925f) SHA1(037a69243b35e1739e5d7288e279d0d4289c61ed) )
	ROM_LOAD( "6.8",    0x00000, 0x10000, CRC(61b3baaa) SHA1(d65a235dbbb96c11e8307aa457d1c06f20eb8d5a) )
	ROM_LOAD( "7.9",    0x10000, 0x10000, CRC(88aaff58) SHA1(dfb143ef452dec530adf8b35a50a82d08f47d107) )
	ROM_LOAD( "8.10",   0x20000, 0x10000, CRC(8c73baa1) SHA1(50fb408be181ef3c125dee23b04daeb010c9f276) )
ROM_END

ROM_START( toucheme )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "2.ic107",   0x0000, 0x10000, CRC(26f4580b) SHA1(59dc5799436cdaf68dfcb620cd9e2f6929ac7817) )

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1.ic115",   0x0000, 0x8000, CRC(902589aa) SHA1(d60088fc31a67fec91f908f671af77bb87a5e59c) )

	ROM_REGION( 0x60000, "gfx1", ROMREGION_INVERT )
	ROM_LOAD( "3.ic32",   0x30000, 0x10000, CRC(223b4435) SHA1(fb5a4096012093bae5fda213a5de317e63a88ec3) )
	ROM_LOAD( "4.ic33",   0x40000, 0x10000, CRC(96dcc2f3) SHA1(9c61f8161771e40ca41b6e102bc04583dc97cd0d) )
	ROM_LOAD( "5.ic34",   0x50000, 0x10000, CRC(b8667a6b) SHA1(288a5cbd8fc01b24822e89fbc1e6d7f45c181483) )
	ROM_LOAD( "6.ic8",    0x00000, 0x10000, CRC(d257382f) SHA1(9c459b90c9ddfe90de4a252f29a7bee809412b46) )
	ROM_LOAD( "7.ic9",    0x10000, 0x10000, CRC(feb1b974) SHA1(ffd4527472cdf655fbebebf4d3abb61962e54457) )
	ROM_LOAD( "8.ic10",   0x20000, 0x10000, CRC(fc6808bf) SHA1(f1f1b75a79dfdb500012f9b52c6364f0a13dce2d) )
ROM_END

ROM_START( touchemea )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD( "2.ic107",   0x0000, 0x10000, CRC(4e72312d) SHA1(a7d178608f05c87a53c650298b903bcf34b3b755) ) // sldh

	ROM_REGION( 0x10000, "audiocpu", 0 )
	ROM_LOAD( "1.ic115",   0x0000, 0x8000, CRC(902589aa) SHA1(d60088fc31a67fec91f908f671af77bb87a5e59c) )

	ROM_REGION( 0x60000, "gfx1", ROMREGION_INVERT )
	ROM_LOAD( "3.ic32",   0x30000, 0x10000, CRC(223b4435) SHA1(fb5a4096012093bae5fda213a5de317e63a88ec3) )
	ROM_LOAD( "4.ic33",   0x40000, 0x10000, CRC(96dcc2f3) SHA1(9c61f8161771e40ca41b6e102bc04583dc97cd0d) )
	ROM_LOAD( "5.ic34",   0x50000, 0x10000, CRC(b8667a6b) SHA1(288a5cbd8fc01b24822e89fbc1e6d7f45c181483) )
	ROM_LOAD( "6.ic8",    0x00000, 0x10000, CRC(d257382f) SHA1(9c459b90c9ddfe90de4a252f29a7bee809412b46) )
	ROM_LOAD( "7.ic9",    0x10000, 0x10000, CRC(feb1b974) SHA1(ffd4527472cdf655fbebebf4d3abb61962e54457) )
	ROM_LOAD( "8.ic10",   0x20000, 0x10000, CRC(fc6808bf) SHA1(f1f1b75a79dfdb500012f9b52c6364f0a13dce2d) )
ROM_END

GAME( 1990, ladyfrog,  0,        ladyfrog, ladyfrog, ladyfrog_state, empty_init, ORIENTATION_SWAP_XY, "Mondial Games", "Lady Frog", MACHINE_SUPPORTS_SAVE )

// toucheme art style is similar to ladyfrog, so it's probably the same manufacturer
GAME( 19??, toucheme,  0,        toucheme, toucheme, ladyfrog_state, empty_init, ORIENTATION_SWAP_XY, "<unknown>",     "Touche Me (set 1)",         MACHINE_SUPPORTS_SAVE )
GAME( 19??, touchemea, toucheme, toucheme, toucheme, ladyfrog_state, empty_init, ORIENTATION_SWAP_XY, "<unknown>",     "Touche Me (set 2, harder)", MACHINE_SUPPORTS_SAVE )
