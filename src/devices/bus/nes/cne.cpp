// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/***********************************************************************************************************


 NES/Famicom cartridge emulation for C&E PCBs


 Here we emulate the following PCBs

 * C&E Decathlon [mapper 244]
 * C&E Feng Shen Bang [mapper 246]
 * C&E Sheng Huo Lie Zhuan [mapper 240]

 ***********************************************************************************************************/


#include "emu.h"
#include "cne.h"


#ifdef NES_PCB_DEBUG
#define VERBOSE (LOG_GENERAL)
#else
#define VERBOSE (0)
#endif
#include "logmacro.h"


//-------------------------------------------------
//  constructor
//-------------------------------------------------

DEFINE_DEVICE_TYPE(NES_CNE_DECATHL, nes_cne_decathl_device, "nes_cne_deca", "NES Cart C&E Decathlon PCB")
DEFINE_DEVICE_TYPE(NES_CNE_FSB,     nes_cne_fsb_device,     "nes_cne_fsb",  "NES Cart C&E Feng Shen Bang PCB")
DEFINE_DEVICE_TYPE(NES_CNE_SHLZ,    nes_cne_shlz_device,    "nes_cne_shlz", "NES Cart C&E Sheng Huo Lie Zhuan PCB")


nes_cne_decathl_device::nes_cne_decathl_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_nrom_device(mconfig, NES_CNE_DECATHL, tag, owner, clock)
{
}

nes_cne_fsb_device::nes_cne_fsb_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_nrom_device(mconfig, NES_CNE_FSB, tag, owner, clock)
{
}

nes_cne_shlz_device::nes_cne_shlz_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_nrom_device(mconfig, NES_CNE_SHLZ, tag, owner, clock)
{
}



void nes_cne_fsb_device::pcb_reset()
{
	prg32(0xff);
	chr8(0, m_chr_source);
}




/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

/*-------------------------------------------------

 C & E Bootleg Board for Decathlon

 Games: Decathlon

 Pretty simple mapper: writes to 0x8065-0x80a4 set prg32 to
 offset & 3; writes to 0x80a5-0x80e4 set chr8 to offset & 7

 iNES: mapper 244

 In MAME: Supported.

 -------------------------------------------------*/

void nes_cne_decathl_device::write_h(offs_t offset, u8 data)
{
	LOG("cne_decathl_w, offset: %04x, data: %02x\n", offset, data);

	if (offset >= 0x0065)
	{
		if (offset < 0x00a5)
			prg32((offset - 0x0065) & 0x03);
		else if (offset < 0x00e5)
			chr8((offset - 0x00a5) & 0x07, CHRROM);
	}
}

/*-------------------------------------------------

 C & E Bootleg Board for Fong Shen Bang

 Games: Fong Shen Bang - Zhu Lu Zhi Zhan

 Simple mapper: writes to 0x6000-0x67ff set PRG and CHR banks.
 Namely, 0x6000->0x6003 select resp. prg8_89, prg8_ab, prg8_cd
 and prg8_ef. 0x6004->0x6007 select resp. crh2_0, chr2_2,
 chr2_4 and chr2_6. In 0x6800-0x7fff lies WRAM. Battery backed?

 iNES: mapper 246

 In MAME: Supported.

 -------------------------------------------------*/

void nes_cne_fsb_device::write_m(offs_t offset, u8 data)
{
	LOG("cne_fsb write_m, offset: %04x, data: %02x\n", offset, data);

	if (offset < 0x0800)
	{
		switch (offset & 0x0007)
		{
			case 0x0000:
			case 0x0001:
			case 0x0002:
			case 0x0003:
				prg8_x(offset & 0x03, data);
				break;
			case 0x0004:
			case 0x0005:
			case 0x0006:
			case 0x0007:
				chr2_x((offset & 0x03) << 1, data, CHRROM);
				break;
		}
	}
	else
		device_nes_cart_interface::write_m(offset, data);
}

u8 nes_cne_fsb_device::read_m(offs_t offset)
{
	LOG("cne_fsb read_m, offset: %04x\n", offset);

	if (offset >= 0x0800)
		return device_nes_cart_interface::read_m(offset);

	return get_open_bus();
}

/*-------------------------------------------------

 C & E Bootleg Board for Sheng Huo Lie Zhuan

 Games: Jing Ke Xin Zhuan, Sheng Huo Lie Zhuan

 Simple Mapper: writes to 0x4020-0x5fff sets prg32 to
 data>>4 and chr8 to data&f. We currently do not map
 writes to 0x4020-0x40ff (to do: verify if this produces
 issues)

 iNES: mapper 240

 In MAME: Supported.

 -------------------------------------------------*/

void nes_cne_shlz_device::write_l(offs_t offset, u8 data)
{
	LOG("cne_shlz write_l, offset: %04x, data: %02x\n", offset, data);

	prg32(data >> 4);
	chr8(data & 0x0f, CHRROM);
}
