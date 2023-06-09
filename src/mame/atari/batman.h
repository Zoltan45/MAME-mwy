// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Atari Batman hardware

*************************************************************************/
#ifndef MAME_ATARI_BATMAN_H
#define MAME_ATARI_BATMAN_H

#pragma once

#include "atarijsa.h"
#include "atarimo.h"
#include "atarivad.h"
#include "emupal.h"
#include "screen.h"
#include "tilemap.h"

class batman_state : public driver_device
{
public:
	batman_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_screen(*this, "screen"),
		m_jsa(*this, "jsa"),
		m_vad(*this, "vad"),
		m_mob(*this, "vad:mob")
	{ }

	void init_batman();
	void batman(machine_config &config);

protected:
	virtual void machine_start() override;
	void latch_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	TILE_GET_INFO_MEMBER(get_alpha_tile_info);
	TILE_GET_INFO_MEMBER(get_playfield_tile_info);
	TILE_GET_INFO_MEMBER(get_playfield2_tile_info);
	uint32_t screen_update_batman(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void main_map(address_map &map);

private:
	required_device<cpu_device> m_maincpu;
	required_device<screen_device> m_screen;
	required_device<atari_jsa_iii_device> m_jsa;
	required_device<atari_vad_device> m_vad;
	required_device<atari_motion_objects_device> m_mob;

	uint16_t          m_latch_data = 0U;
	uint8_t           m_alpha_tile_bank = 0U;

	static const atari_motion_objects_config s_mob_config;
};

#endif // MAME_ATARI_BATMAN_H
