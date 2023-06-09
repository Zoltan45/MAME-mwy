// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/***********************************************************************************************************


 NES/Famicom cartridge emulation for TXC PCBs

 Here we emulate the following PCBs

 * TXC 01-22000-400 [mapper 36]
 * TXC 22111 [mapper 132]
 * TXC Du Ma Racing [mapper 172]
 * TXC Mahjong Block [mapper 172]
 * TXC Commandos (and many more) [mapper 241]

 TODO:
 - Does Commandos indeed use this board?


 ***********************************************************************************************************/


#include "emu.h"
#include "txc.h"


#ifdef NES_PCB_DEBUG
#define VERBOSE (LOG_GENERAL)
#else
#define VERBOSE (0)
#endif
#include "logmacro.h"


//-------------------------------------------------
//  constructor
//-------------------------------------------------

DEFINE_DEVICE_TYPE(NES_TXC_22211,      nes_txc_22211_device,     "nes_txc_22211",   "NES Cart TXC 22211 PCB")
DEFINE_DEVICE_TYPE(NES_TXC_DUMARACING, nes_txc_dumarc_device,    "nes_txc_dumarc",  "NES Cart TXC Du Ma Racing PCB")
DEFINE_DEVICE_TYPE(NES_TXC_MJBLOCK,    nes_txc_mjblock_device,   "nes_txc_mjblock", "NES Cart TXC Mahjong Block PCB")
DEFINE_DEVICE_TYPE(NES_TXC_STRIKEW,    nes_txc_strikew_device,   "nes_txc_strikew", "NES Cart TXC 01-22000-400 PCB")
DEFINE_DEVICE_TYPE(NES_TXC_COMMANDOS,  nes_txc_commandos_device, "nes_txc_comm",    "NES Cart TXC Cart Commandos PCB") // and others


nes_txc_22211_device::nes_txc_22211_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: nes_nrom_device(mconfig, type, tag, owner, clock)
{
}

nes_txc_22211_device::nes_txc_22211_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: nes_nrom_device(mconfig, NES_TXC_22211, tag, owner, clock)
{
}

nes_txc_dumarc_device::nes_txc_dumarc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: nes_txc_22211_device(mconfig, NES_TXC_DUMARACING, tag, owner, clock)
{
}

nes_txc_mjblock_device::nes_txc_mjblock_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: nes_txc_22211_device(mconfig, NES_TXC_MJBLOCK, tag, owner, clock)
{
}

nes_txc_strikew_device::nes_txc_strikew_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_nrom_device(mconfig, NES_TXC_STRIKEW, tag, owner, clock)
{
}

nes_txc_commandos_device::nes_txc_commandos_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: nes_nrom_device(mconfig, NES_TXC_COMMANDOS, tag, owner, clock)
{
}




void nes_txc_22211_device::device_start()
{
	common_start();
	save_item(NAME(m_reg));
}

void nes_txc_22211_device::pcb_reset()
{
	prg32(0);
	chr8(0, m_chr_source);
	m_reg[0] = m_reg[1] = m_reg[2] = m_reg[3] = 0;
}

void nes_txc_strikew_device::device_start()
{
	common_start();
	save_item(NAME(m_reg));
}

void nes_txc_strikew_device::pcb_reset()
{
	prg32(0);
	chr8(0, m_chr_source);
	m_reg[0] = m_reg[1] = m_reg[2] = m_reg[3] = 0;
}




/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

/*-------------------------------------------------

 Bootleg Board 22211 by TXC (Type A)

 Games: Creatom

 Info from NEStopia: this mapper features write to four
 registers (0x4100-0x4103). The third one is used to select
 PRG and CHR banks.

 iNES: mapper 132

 In MAME: Supported.

 -------------------------------------------------*/

void nes_txc_22211_device::write_l(offs_t offset, uint8_t data)
{
	LOG("TXC 22111 write_l, offset: %04x, data: %02x\n", offset, data);

	if (offset < 4)
		m_reg[offset & 0x03] = data;
}

uint8_t nes_txc_22211_device::read_l(offs_t offset)
{
	LOG("TXC 22111 read_l, offset: %04x\n", offset);

	if (offset == 0x0000)
		return (m_reg[1] ^ m_reg[2]) | 0x40;
	else
		return 0x00;
}

void nes_txc_22211_device::write_h(offs_t offset, uint8_t data)
{
	LOG("TXC 22111 write_h, offset: %04x, data: %02x\n", offset, data);

	prg32(m_reg[2] >> 2);
	chr8(m_reg[2], CHRROM);
}

/*-------------------------------------------------

 Bootleg Board 22211 by TXC (Type B)

 Games: 1991 Du Ma Racing

 This mapper is basically the same as Type A. Only difference is
 in the way CHR banks are selected (see below)

 iNES: mapper 172

 In MAME: Supported.

 -------------------------------------------------*/

void nes_txc_dumarc_device::write_h(offs_t offset, uint8_t data)
{
	LOG("TXC Du Ma Racing write_h, offset: %04x, data: %02x\n", offset, data);

	prg32(m_reg[2] >> 2);
	chr8(bitswap<2>(data ^ m_reg[2], 4, 5), CHRROM);
}

/*-------------------------------------------------

 Bootleg Board 22211 by TXC (Type C)

 Games: Mahjong Block, Xiao Ma Li

 This mapper is basically the same as 132 too. Only difference is
 in 0x4100 reads which expect also bit 0 to be set

 iNES: mapper 172

 In MAME: Supported.

 -------------------------------------------------*/

uint8_t nes_txc_mjblock_device::read_l(offs_t offset)
{
	LOG("TXC mjblock read_l, offset: %04x\n", offset);

	if (offset == 0x0000)
		return (m_reg[1] ^ m_reg[2]) | 0x41;
	else
		return 0x00;
}

/*-------------------------------------------------

 TXC 01-22000-400 Board

 Games: F-15 City War, Policeman, Puzzle, Strike Wolf,
 Venice Beach Volley

 iNES: mapper 36

 In MAME: Supported.

 -------------------------------------------------*/

u8 nes_txc_strikew_device::read_l(offs_t offset)
{
	LOG("TXC 01-22000-400 read_l, offset: %04x\n", offset);

	offset += 0x100;
	if (offset & 0x100)
		return (get_open_bus() & 0xcf) | m_reg[0] << 4;

	return get_open_bus();
}

void nes_txc_strikew_device::write_l(offs_t offset, u8 data)
{
	LOG("TXC 01-22000-400 write_l, offset: %04x, data: %02x\n", offset, data);

	offset += 0x100;
	switch (offset & 0x103)
	{
		case 0x100:
			if (m_reg[3] & 1)
				m_reg[0] = (m_reg[0] + 1) & 0x03;
			else if (m_reg[1] & 1)
				m_reg[0] = m_reg[2] ^ 0x03;
			else
				m_reg[0] = m_reg[2];
			break;
		case 0x101:
		case 0x102:
		case 0x103:
			m_reg[offset & 0x03] = BIT(data, 4, 2);
			break;
	}

	if (offset & 0x200)
		chr8(data, CHRROM);
}

void nes_txc_strikew_device::write_h(offs_t offset, u8 data)
{
	LOG("TXC 01-22000-400 write_h, offset: %04x, data: %02x\n", offset, data);

	prg32(m_reg[0]);
}

/*-------------------------------------------------

 Bootleg Board MXMDHTWO by TXC

 Games: Commandos, Journey to the West, Ma Bu Mi Zhen &
 Qu Wei Cheng Yu Wu, Si Lu Chuan Qi

 Simple Mapper: writes to 0x8000-0xffff sets the prg32 bank.
 Not sure if returning 0x50 for reads in 0x4100-0x5000 is correct.

 iNES: mapper 241

 In MAME: Supported.

 -------------------------------------------------*/

uint8_t nes_txc_commandos_device::read_l(offs_t offset)
{
	return 0x50;
}

void nes_txc_commandos_device::write_h(offs_t offset, uint8_t data)
{
	LOG("TXC Commandos write_h, offset: %04x, data: %02x\n", offset, data);

	prg32(data);
}
