// license:BSD-3-Clause
// copyright-holders:Alex Pasadyn,Zsolt Vasvari,Aaron Giles
/*************************************************************************

    Gottlieb Exterminator hardware

*************************************************************************/
#ifndef MAME_GOTTLIEB_EXTERM_H
#define MAME_GOTTLIEB_EXTERM_H

#pragma once

#include "cpu/tms34010/tms34010.h"
#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "sound/ymopm.h"
#include "emupal.h"

class exterm_state : public driver_device
{
public:
	exterm_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_audioslave(*this, "audioslave"),
		m_soundlatch(*this, "soundlatch%u", 1),
		m_slave(*this, "slave"),
		m_ym2151(*this, "ymsnd"),
		m_nmi_timer(*this, "snd_nmi_timer"),
		m_master_videoram(*this, "master_videoram"),
		m_slave_videoram(*this, "slave_videoram"),
		m_dial(*this, "DIAL%u", 0U),
		m_input(*this, "P%u", 1U)
	{ }

	void exterm(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	required_device<tms34010_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<cpu_device> m_audioslave;
	required_device_array<generic_latch_8_device, 2> m_soundlatch;
	required_device<tms34010_device> m_slave;
	required_device<ym2151_device> m_ym2151;
	required_device<timer_device> m_nmi_timer;

	required_shared_ptr<uint16_t> m_master_videoram;
	required_shared_ptr<uint16_t> m_slave_videoram;

	required_ioport_array<2> m_dial;
	required_ioport_array<2> m_input;

	uint8_t m_aimpos[2]{};
	uint8_t m_trackball_old[2]{};
	uint8_t m_sound_control = 0U;
	uint16_t m_last = 0U;

	void host_data_w(offs_t offset, uint16_t data);
	uint16_t host_data_r(offs_t offset);
	template<uint8_t Which> uint16_t trackball_port_r();
	void output_port_0_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void sound_latch_w(uint8_t data);
	void sound_nmi_rate_w(uint8_t data);
	uint8_t sound_nmi_to_slave_r();
	void sound_control_w(uint8_t data);
	void ym2151_data_latch_w(uint8_t data);
	void exterm_palette(palette_device &palette) const;
	TIMER_DEVICE_CALLBACK_MEMBER(master_sound_nmi_callback);
	TMS340X0_SCANLINE_IND16_CB_MEMBER(scanline_update);
	TMS340X0_TO_SHIFTREG_CB_MEMBER(to_shiftreg_master);
	TMS340X0_FROM_SHIFTREG_CB_MEMBER(from_shiftreg_master);
	TMS340X0_TO_SHIFTREG_CB_MEMBER(to_shiftreg_slave);
	TMS340X0_FROM_SHIFTREG_CB_MEMBER(from_shiftreg_slave);
	void master_map(address_map &map);
	void slave_map(address_map &map);
	void sound_master_map(address_map &map);
	void sound_slave_map(address_map &map);
};

#endif // MAME_GOTTLIEB_EXTERM_H
