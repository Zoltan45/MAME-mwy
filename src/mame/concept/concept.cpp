// license:GPL-2.0+
// copyright-holders:Raphael Nabet, Brett Wyer
/*
    Corvus Concept driver

    Relatively simple 68k-based system

    * 256 or 512 kbytes of DRAM
    * 4kbytes of SRAM
    * 8kbyte boot ROM
    * optional MacsBugs ROM
    * two serial ports, keyboard, bitmapped display, simple sound, omninet
      LAN port (seems more or less similar to AppleTalk)
    * 4 expansion ports enable to add expansion cards, namely floppy disk
      and hard disk controllers (the expansion ports are partially compatible
      with Apple 2 expansion ports; DMA is not supported)

    Video: monochrome bitmapped display, 720*560 visible area (bitmaps are 768
      pixels wide in memory).  One interesting feature is the fact that the
      monitor can be rotated to give a 560*720 vertical display (you need to
      throw a switch and reset the machine for the display rotation to be taken
      into account, though).  One oddity is that the video hardware scans the
      display from the lower-left corner to the upper-left corner (or from the
      upper-right corner to the lower-left if the screen is flipped).
    Sound: simpler buzzer connected to the via shift register
    Keyboard: intelligent controller, connected through an ACIA.  See CCOS
      manual pp. 76 through 78. and User Guide p. 2-1 through 2-9.
    Clock: mm58174 RTC

    Raphael Nabet, Brett Wyer, 2003-2005
    Major reworking by R. Belmont 2012-2013 resulting in bootable floppies

    ACIA baud rates are 1.36% slower than normal by design. The clock division
    used as the BRG input for all three is about 1.818 MHz, not the standard
    1.8432 MHz. The schematics indicate a PCB option to leave the 74LS161 at
    U212 unpopulated and use a secondary XTAL as the baud rate clock. This
    XTAL is also specified as 1.818 MHz.
*/

#include "emu.h"
#include "concept.h"

#include "cpu/m68000/m68000.h"
#include "bus/a2bus/a2corvus.h"
#include "bus/a2bus/corvfdc01.h"
#include "bus/a2bus/corvfdc02.h"
#include "bus/rs232/rs232.h"
#include "machine/input_merger.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"

void concept_state::concept_memmap(address_map &map)
{
	map(0x000000, 0x000007).rom().region("maincpu", 0x010000);  /* boot ROM mirror */
	map(0x000008, 0x000fff).ram();                                     /* static RAM */
	map(0x010000, 0x011fff).rom().region("maincpu", 0x010000);  /* boot ROM */
	map(0x020000, 0x021fff).rom().region("macsbug", 0x0);       /* macsbugs ROM (optional) */
	map(0x030000, 0x03ffff).rw(FUNC(concept_state::io_r), FUNC(concept_state::io_w)).umask16(0x00ff);    /* I/O space */

	map(0x080000, 0x0fffff).ram().share("videoram"); /* .bankrw(2); */ /* DRAM */
}


static INPUT_PORTS_START( corvus_concept )

	PORT_START("KEY0")  /* port 0: keys 0x00 through 0x0f */
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(right)") PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3") PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9") PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("HOME") PORT_CODE(KEYCODE_HOME)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6") PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(",") PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("-") PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER_PAD)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(left)") PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1") PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7") PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(down)") PORT_CODE(KEYCODE_DOWN)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4") PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8") PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5") PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2") PORT_CODE(KEYCODE_2_PAD)

	PORT_START("KEY1")  /* port 1: keys 0x10 through 0x1f */
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("= +") PORT_CODE(KEYCODE_EQUALS)

	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("[ {") PORT_CODE(KEYCODE_OPENBRACE)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BACKSPACE") PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("] }") PORT_CODE(KEYCODE_CLOSEBRACE)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("\\ |") PORT_CODE(KEYCODE_BACKSLASH)

	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0 )") PORT_CODE(KEYCODE_0)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("/ ?") PORT_CODE(KEYCODE_SLASH)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("P") PORT_CODE(KEYCODE_P)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("- _") PORT_CODE(KEYCODE_MINUS)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("; :") PORT_CODE(KEYCODE_COLON)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("` ~") PORT_CODE(KEYCODE_BACKSLASH2)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("' \"") PORT_CODE(KEYCODE_QUOTE)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SHIFT (r)") PORT_CODE(KEYCODE_RSHIFT)

	PORT_START("KEY2")  /* port 2: keys 0x20 through 0x2f */
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F1") PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F2") PORT_CODE(KEYCODE_F2)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F3") PORT_CODE(KEYCODE_F3)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F4") PORT_CODE(KEYCODE_F4)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F5") PORT_CODE(KEYCODE_F5)

	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("4 $") PORT_CODE(KEYCODE_4)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("5 %") PORT_CODE(KEYCODE_5)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("R") PORT_CODE(KEYCODE_R)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("T") PORT_CODE(KEYCODE_T)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F") PORT_CODE(KEYCODE_F)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("G") PORT_CODE(KEYCODE_G)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("V") PORT_CODE(KEYCODE_V)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("B") PORT_CODE(KEYCODE_B)

	PORT_START("KEY3")  /* port 3: keys 0x30 through 0x3f */
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("2 @") PORT_CODE(KEYCODE_2)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("3 #") PORT_CODE(KEYCODE_3)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("W") PORT_CODE(KEYCODE_W)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("E") PORT_CODE(KEYCODE_E)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("S") PORT_CODE(KEYCODE_S)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("D") PORT_CODE(KEYCODE_D)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("X") PORT_CODE(KEYCODE_X)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("C") PORT_CODE(KEYCODE_C)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ESC") PORT_CODE(KEYCODE_ESC)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("1 !") PORT_CODE(KEYCODE_1)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("TAB") PORT_CODE(KEYCODE_TAB)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Q") PORT_CODE(KEYCODE_Q)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CAPS LOCK") PORT_CODE(KEYCODE_CAPSLOCK) PORT_TOGGLE
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("A") PORT_CODE(KEYCODE_A)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("SHIFT (l)") PORT_CODE(KEYCODE_LSHIFT)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Z") PORT_CODE(KEYCODE_Z)

	PORT_START("KEY4")  /* port 4: keys 0x40 through 0x4f */
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("6 ^") PORT_CODE(KEYCODE_6)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("7 &") PORT_CODE(KEYCODE_7)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("Y") PORT_CODE(KEYCODE_Y)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("U") PORT_CODE(KEYCODE_U)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("H") PORT_CODE(KEYCODE_H)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("J") PORT_CODE(KEYCODE_J)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("N") PORT_CODE(KEYCODE_N)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("M") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("CONTROL") PORT_CODE(KEYCODE_LCONTROL)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("FAST") PORT_CODE(KEYCODE_TILDE)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("COMMAND") PORT_CODE(KEYCODE_LALT)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(space)") PORT_CODE(KEYCODE_SPACE)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("ALT") PORT_CODE(KEYCODE_RALT)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("0") PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("00") PORT_CODE(KEYCODE_ASTERISK)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(".") PORT_CODE(KEYCODE_DEL_PAD)

	PORT_START("KEY5")  /* port 5: keys 0x50 through 0x5f */
	PORT_BIT(0x0001, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("8 *") PORT_CODE(KEYCODE_8)
	PORT_BIT(0x0002, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("9 (") PORT_CODE(KEYCODE_9)
	PORT_BIT(0x0004, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("I") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x0008, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("O") PORT_CODE(KEYCODE_O)
	PORT_BIT(0x0010, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("K") PORT_CODE(KEYCODE_K)
	PORT_BIT(0x0020, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("L") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x0040, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(", <") PORT_CODE(KEYCODE_COMMA)
	PORT_BIT(0x0080, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME(". >") PORT_CODE(KEYCODE_STOP)
	PORT_BIT(0x0100, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F6") PORT_CODE(KEYCODE_F6)
	PORT_BIT(0x0200, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F7") PORT_CODE(KEYCODE_F7)
	PORT_BIT(0x0400, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F8") PORT_CODE(KEYCODE_F8)
	PORT_BIT(0x0800, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F9") PORT_CODE(KEYCODE_F9)
	PORT_BIT(0x1000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("F10") PORT_CODE(KEYCODE_F10)
	PORT_BIT(0x2000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("(up)") PORT_CODE(KEYCODE_UP)
	PORT_BIT(0x4000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("???") PORT_CODE(KEYCODE_SLASH_PAD)
	PORT_BIT(0x8000, IP_ACTIVE_HIGH, IPT_KEYBOARD) PORT_NAME("BREAK") PORT_CODE(KEYCODE_PAUSE)

	PORT_START("DSW0")  /* port 6: on-board DIP switches */
	PORT_DIPNAME(0x01, 0x00, "Omninet Address bit 0")
	PORT_DIPSETTING(0x00, DEF_STR( Off ))
	PORT_DIPSETTING(0x01, DEF_STR( On ))
	PORT_DIPNAME(0x02, 0x02, "Omninet Address bit 1")
	PORT_DIPSETTING(0x00, DEF_STR( Off ))
	PORT_DIPSETTING(0x02, DEF_STR( On ))
	PORT_DIPNAME(0x04, 0x00, "Omninet Address bit 2")
	PORT_DIPSETTING(0x00, DEF_STR( Off ))
	PORT_DIPSETTING(0x04, DEF_STR( On ))
	PORT_DIPNAME(0x08, 0x00, "Omninet Address bit 3")
	PORT_DIPSETTING(0x00, DEF_STR( Off ))
	PORT_DIPSETTING(0x08, DEF_STR( On ))
	PORT_DIPNAME(0x10, 0x00, "Omninet Address bit 4")
	PORT_DIPSETTING(0x00, DEF_STR( Off ))
	PORT_DIPSETTING(0x10, DEF_STR( On ))
	PORT_DIPNAME(0x20, 0x00, "Omninet Address bit 5")
	PORT_DIPSETTING(0x00, DEF_STR( Off ))
	PORT_DIPSETTING(0x20, DEF_STR( On ))
	PORT_DIPNAME(0xc0, 0x00, "Type of Boot")
	PORT_DIPSETTING(0x00, "Prompt for type of Boot")        // Documentation has 0x00 and 0xc0 reversed per boot PROM
	PORT_DIPSETTING(0x40, "Boot from Omninet")
	PORT_DIPSETTING(0x80, "Boot from Local Disk")
	PORT_DIPSETTING(0xc0, "Boot from Diskette")

#if 0
	PORT_START("DISPLAY")   /* port 7: Display orientation */
	PORT_DIPNAME(0x01, 0x00, "Screen Orientation")
	PORT_DIPSETTING(0x00, "Horizontal")
	PORT_DIPSETTING(0x01, "Vertical")
#endif

INPUT_PORTS_END


void concept_a2_cards(device_slot_interface &device)
{
	device.option_add("fchdd", A2BUS_CORVUS);       // Corvus flat-cable HDD interface (see notes in a2corvus.c)
	device.option_add("fdc01", A2BUS_CORVFDC01);    // Corvus WD1793 floppy controller
	device.option_add("fdc02", A2BUS_CORVFDC02);    // Corvus NEC765 buffered floppy controller
}


void concept_state::corvus_concept(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 16.364_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &concept_state::concept_memmap);

	config.set_maximum_quantum(attotime::from_hz(60));

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_video_attributes(VIDEO_UPDATE_BEFORE_VBLANK);
	screen.set_raw(16.364_MHz_XTAL * 2, 944, 0, 720, 578, 0, 560);
	// Horizontal sync is 34.669 kHz; refresh rate is ~50 or ~60 Hz, jumper-selectable
	screen.set_screen_update(FUNC(concept_state::screen_update));
	screen.set_palette("palette");

	/* Is the palette black on white or white on black??? */
	PALETTE(config, "palette", palette_device::MONOCHROME);

	/* sound */
	SPEAKER(config, "mono").front_center();
	SPEAKER_SOUND(config, m_speaker).add_route(ALL_OUTPUTS, "mono", 1.00);

	/* rtc */
	MM58174(config, m_mm58174, 32.768_kHz_XTAL);

	/* via */
	MOS6522(config, m_via0, 16.364_MHz_XTAL / 16);
	m_via0->readpa_handler().set(FUNC(concept_state::via_in_a));
	m_via0->readpb_handler().set(FUNC(concept_state::via_in_b));
	m_via0->writepa_handler().set(FUNC(concept_state::via_out_a));
	m_via0->writepb_handler().set(FUNC(concept_state::via_out_b));
	m_via0->cb2_handler().set(FUNC(concept_state::via_out_cb2));
	m_via0->irq_handler().set(FUNC(concept_state::via_irq_func));

	/* ACIAs */
	MOS6551(config, m_acia0, 16.364_MHz_XTAL / 16);
	m_acia0->set_xtal(16.364_MHz_XTAL / 9);
	m_acia0->txd_handler().set("rs232a", FUNC(rs232_port_device::write_txd));

	MOS6551(config, m_acia1, 16.364_MHz_XTAL / 16);
	m_acia1->set_xtal(16.364_MHz_XTAL / 9);
	m_acia1->txd_handler().set("rs232b", FUNC(rs232_port_device::write_txd));

	MOS6551(config, m_kbdacia, 16.364_MHz_XTAL / 16);
	m_kbdacia->set_xtal(16.364_MHz_XTAL / 9);

	/* Apple II bus */
	A2BUS(config, m_a2bus, 0).set_space(m_maincpu, AS_PROGRAM);
	m_a2bus->nmi_w().set("iocint", FUNC(input_merger_device::in_w<0>));
	m_a2bus->irq_w().set("iocint", FUNC(input_merger_device::in_w<1>));
	A2BUS_SLOT(config, "sl1", m_a2bus, concept_a2_cards, nullptr);
	A2BUS_SLOT(config, "sl2", m_a2bus, concept_a2_cards, nullptr);
	A2BUS_SLOT(config, "sl3", m_a2bus, concept_a2_cards, nullptr);
	A2BUS_SLOT(config, "sl4", m_a2bus, concept_a2_cards, "fdc01");

	INPUT_MERGER_ANY_HIGH(config, "iocint").output_handler().set(FUNC(concept_state::ioc_interrupt));

	/* 2x RS232 ports */
	rs232_port_device &rs232a(RS232_PORT(config, "rs232a", default_rs232_devices, nullptr));
	rs232a.rxd_handler().set(m_acia0, FUNC(mos6551_device::write_rxd));
	rs232a.dcd_handler().set(m_acia0, FUNC(mos6551_device::write_dcd));
	rs232a.dsr_handler().set(m_acia0, FUNC(mos6551_device::write_dsr));
	rs232a.cts_handler().set(m_acia0, FUNC(mos6551_device::write_cts));

	rs232_port_device &rs232b(RS232_PORT(config, "rs232b", default_rs232_devices, nullptr));
	rs232b.rxd_handler().set(m_acia1, FUNC(mos6551_device::write_rxd));
	rs232b.dcd_handler().set(m_acia1, FUNC(mos6551_device::write_dcd));
	rs232b.dsr_handler().set(m_acia1, FUNC(mos6551_device::write_dsr));
	rs232b.cts_handler().set(m_acia1, FUNC(mos6551_device::write_cts));
}


ROM_START( concept )
	ROM_REGION16_BE(0x100000,"maincpu",0)   /* 68k rom and ram */

	// concept boot ROM
	ROM_SYSTEM_BIOS(0, "lvl8", "Level 8" )  // v0?
	ROMX_LOAD("bootl08h", 0x010000, 0x1000, CRC(ee479f51) SHA1(b20ba18564672196076e46507020c6d69a640a2f), ROM_BIOS(0) | ROM_SKIP(1))
	ROMX_LOAD("bootl08l", 0x010001, 0x1000, CRC(acaefd07) SHA1(de0c7eaacaf4c0652aa45e523cebce2b2993c437), ROM_BIOS(0) | ROM_SKIP(1))

	ROM_SYSTEM_BIOS(1, "lvl7", "Level 7" )  // v0? v1?
	ROMX_LOAD("cc07h", 0x010000, 0x1000, CRC(455abac8) SHA1(b12e1580220242d34eafed1b486cebd89e823c8b), ROM_BIOS(1) | ROM_SKIP(1))
	ROMX_LOAD("cc07l", 0x010001, 0x1000, CRC(107a3830) SHA1(0ea12ef13b0d11fcd83b306b3a1bb8014ba910c0), ROM_BIOS(1) | ROM_SKIP(1))

	ROM_SYSTEM_BIOS(2, "lvl6", "Level 6" )  // v0?
	ROMX_LOAD("cc06h", 0x010000, 0x1000, CRC(66b6b259) SHA1(1199a38ef3e94f695e8da6a7c80c6432da3cb80c), ROM_BIOS(2) | ROM_SKIP(1))
	ROMX_LOAD("cc06l", 0x010001, 0x1000, CRC(600940d3) SHA1(c3278bf23b3b1c35ea1e3da48a05e877862a8345), ROM_BIOS(2) | ROM_SKIP(1))

	ROM_REGION(0x400, "proms", 0)
	ROM_LOAD("map04a.bin", 0x000, 0x400, CRC(1ae0db9b) SHA1(cdb6f63bb08072b454b4704e62de51c483ede734) )

#if 0
	// version 1 lvl 7 release
	ROM_LOAD16_BYTE("bootl17h", 0x010000, 0x1000, CRC(6dd9718f))    // where does this come from?
	ROM_LOAD16_BYTE("bootl17l", 0x010001, 0x1000, CRC(107a3830) SHA1(0ea12ef13b0d11fcd83b306b3a1bb8014ba910c0))
#elif 0
	// version $F lvl 8 (development version found on a floppy disk along with
	// the source code)
	ROM_LOAD16_WORD("cc.prm", 0x010000, 0x2000, CRC(b5a87dab) SHA1(0da59af6cfeeb38672f71731527beac323d9c3d6))
#endif

	ROM_REGION16_BE(0x2000, "macsbug", 0)
	ROM_LOAD16_BYTE( "mb20h.bin",    0x000000, 0x001000, CRC(aa357112) SHA1(88211e5f59887928c557c27cdea674f48bf8eaf7) )
	ROM_LOAD16_BYTE( "mb20l.bin",    0x000001, 0x001000, CRC(b4b59de9) SHA1(3e8b8b5950b5359203c054f94af1fc5b8f0495b9) )
ROM_END

/*    YEAR  NAME     PARENT  COMPAT  MACHINE         INPUT           CLASS          INIT        COMPANY           FULLNAME */
COMP( 1982, concept, 0,      0,      corvus_concept, corvus_concept, concept_state, empty_init, "Corvus Systems", "Concept" , 0 )
