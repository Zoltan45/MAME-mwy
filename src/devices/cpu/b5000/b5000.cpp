// license:BSD-3-Clause
// copyright-holders:hap
/*

  Rockwell B5000 MCU core implementation

*/

#include "emu.h"
#include "b5000.h"

#include "b5000d.h"


DEFINE_DEVICE_TYPE(B5000, b5000_cpu_device, "b5000", "Rockwell B5000")


// constructor
b5000_cpu_device::b5000_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	b5000_base_device(mconfig, B5000, tag, owner, clock, 9, address_map_constructor(FUNC(b5000_cpu_device::program_map), this), 6, address_map_constructor(FUNC(b5000_cpu_device::data_map), this))
{ }


// internal memory maps
void b5000_cpu_device::program_map(address_map &map)
{
	map(0x000, 0x1ff).rom();
}

void b5000_cpu_device::data_map(address_map &map)
{
	map(0x00, 0x3f).ram();
}


// disasm
std::unique_ptr<util::disasm_interface> b5000_cpu_device::create_disassembler()
{
	return std::make_unique<b5000_disassembler>();
}


//-------------------------------------------------
//  execute
//-------------------------------------------------

void b5000_cpu_device::execute_one()
{
	switch (m_op & 0xf0)
	{
		case 0x40: op_lax(); break;
		case 0x60:
			if (m_op != 0x6f)
				op_adx();
			else
				op_read();
			break;

		case 0x80: case 0x90: case 0xa0: case 0xb0: op_tra0(1); break;
		case 0xc0: case 0xd0: case 0xe0: case 0xf0: op_tra1(1); break;

		default:
			switch (m_op & 0xfc)
			{
		case 0x04: op_tdin(); break;
		case 0x08: op_tm(); break;
		case 0x10: op_sm(); break;
		case 0x14: op_rsm(); break;
		case 0x18: op_ret(1); break;

		case 0x20: op_lb(7); break;
		case 0x24: op_lb(10); break;
		case 0x28: op_lb(9); break;
		case 0x2c: op_lb(8); break;
		case 0x3c: op_lb(0); break;

		case 0x30: case 0x34: op_tl(); break;

		case 0x50: op_lda(); break;
		case 0x54: op_excp(); break;
		case 0x58: op_exc0(); break;
		case 0x5c: op_excm(); break;
		case 0x70: op_add(); break;
		case 0x78: op_comp(); break;
		case 0x7c: op_tam(); break;

		default:
			switch (m_op)
			{
		case 0x00: op_nop(); break;
		case 0x01: op_tc(); break;
		case 0x02: op_tkb(); break;
		case 0x03: op_tkbs(1); break;
		case 0x39: op_rsc(); break;
		case 0x3b: op_sc(); break;
		case 0x74: op_kseg(); break;
		case 0x77: op_atbz(1); break; // ATB

		default: op_illegal(); break;
			}
			break; // 0xff

			}
			break; // 0xfc
	}
}

bool b5000_cpu_device::op_canskip(u8 op)
{
	// TL and ATB are unskippable
	return ((op & 0xf8) != 0x30) && (op != 0x77);
}

bool b5000_cpu_device::op_is_lb(u8 op)
{
	return ((op & 0xf0) == 0x20) || ((op & 0xfc) == 0x3c);
}
