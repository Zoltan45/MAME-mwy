// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/***********************************************************************************************************


 NES/Famicom cartridge emulation for Bandai PT-554 PCBs


 Here we emulate the following Bandai PT-554 PCB (a CNROM PCB + LPC / PARCOR speech synthesis chip)

 ***********************************************************************************************************/


#include "emu.h"
#include "pt554.h"
#include "speaker.h"


#ifdef NES_PCB_DEBUG
#define VERBOSE (LOG_GENERAL)
#else
#define VERBOSE (0)
#endif
#include "logmacro.h"


//-------------------------------------------------
//  constructor
//-------------------------------------------------

DEFINE_DEVICE_TYPE(NES_BANDAI_PT554, nes_bandai_pt554_device, "nes_bandai_pt554", "NES Cart Bandai PT-554 PCB")


nes_bandai_pt554_device::nes_bandai_pt554_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_cnrom_device(mconfig, NES_BANDAI_PT554, tag, owner, clock)
	, m_samples(*this, "samples")
{
}



/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

/*-------------------------------------------------

 Bandai PT-554 board emulation

 This is used by Aerobics Studio. It is basically a CNROM board
 with an Mitsubishi M50805 LPC / PARCOR speech synthesis chip
 with internal tables stored in ROM which have not yet been dumped.

 iNES: mapper 3

 -------------------------------------------------*/

void nes_bandai_pt554_device::write_m(offs_t offset, u8 data)
{
	LOG("Bandai PT-554 Sound write, data: %02x\n", data);

	// the actual chip starts speech synthesis when SYNC is held low >18µs
	if (!BIT(data, 6))
		m_samples->start(data & 0x07, data & 0x07);
}

static const char *const pt554_sample_names[] =
{
	"*ftaerobi",
	"00",   // hello, let's go
	"01",   // rest
	"02",   // good
	"03",   // hai
	"04",   // four
	"05",   // three
	"06",   // two
	"07",   // one
	nullptr
};


//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void nes_bandai_pt554_device::device_add_mconfig(machine_config &config)
{
	// additional sound hardware
	SPEAKER(config, "addon").front_center();

	SAMPLES(config, m_samples);
	m_samples->set_channels(8);
	m_samples->set_samples_names(pt554_sample_names);
	m_samples->add_route(ALL_OUTPUTS, "addon", 0.50);
}
