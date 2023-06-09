// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/***********************************************************************************************************


 NES/Famicom cartridge emulation for RacerMate PCBs


 Here we emulate the UNL-RACERMATE PCB [mapper 168]

 TODO:
 - save VRAM
 - emulate the bike controller?

 ***********************************************************************************************************/


#include "emu.h"
#include "racermate.h"


#ifdef NES_PCB_DEBUG
#define VERBOSE (LOG_GENERAL)
#else
#define VERBOSE (0)
#endif
#include "logmacro.h"


//-------------------------------------------------
//  constructor
//-------------------------------------------------

DEFINE_DEVICE_TYPE(NES_RACERMATE, nes_racermate_device, "nes_racermate", "NES Cart Racermate PCB")


nes_racermate_device::nes_racermate_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: nes_nrom_device(mconfig, NES_RACERMATE, tag, owner, clock)
	, m_latch(0)
{
}



void nes_racermate_device::device_start()
{
	common_start();
	save_item(NAME(m_latch));
}

void nes_racermate_device::pcb_reset()
{
	prg16_89ab(0);
	prg16_cdef(m_prg_chunks - 1);
	chr4_0(0, CHRRAM);
	chr4_4(0, CHRRAM);

	m_latch = 0;
}



/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

/*-------------------------------------------------

 Board UNL-RACERMATE

 In MAME: *VERY* preliminary support. Also, it seems that this
 board saves to battery the CHRRAM!!!

 -------------------------------------------------*/

void nes_racermate_device::update_banks()
{
	chr4_4(m_latch & 0x0f, m_chr_source);
	prg16_89ab(m_latch >> 6);
}

void nes_racermate_device::write_h(offs_t offset, uint8_t data)
{
	LOG("racermate write_h, offset: %04x, data: %02x\n", offset, data);

	if (offset == 0x3000)
	{
		m_latch = data;
		update_banks();
	}
}
