// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

    Intel 82371AB PCI IDE ISA Xcelerator (PIIX4)

    Part of the Intel 430TX chipset

    - Integrated IDE Controller
    - Enhanced DMA Controller based on two 82C37
    - Interrupt Controller based on two 82C59
    - Timers based on 82C54
    - USB
    - SMBus
    - Real Time Clock based on MC146818

***************************************************************************/

#include "emu.h"
#include "i82371ab.h"

#define LOG_ISA    (1U << 1)
#define LOG_IDE    (1U << 2)
#define LOG_USB    (1U << 3)
#define LOG_ACPI   (1U << 4)

#define VERBOSE (LOG_GENERAL | LOG_ISA | LOG_IDE | LOG_USB | LOG_ACPI)
//#define LOG_OUTPUT_FUNC osd_printf_info
#include "logmacro.h"

#define LOGISA(...)    LOGMASKED(LOG_ISA, __VA_ARGS__)
#define LOGIDE(...)    LOGMASKED(LOG_IDE, __VA_ARGS__)
#define LOGUSB(...)    LOGMASKED(LOG_USB, __VA_ARGS__)
#define LOGACPI(...)   LOGMASKED(LOG_ACPI, __VA_ARGS__)

DEFINE_DEVICE_TYPE(I82371AB, i82371ab_device, "i82371ab", "Intel 82371AB")


i82371ab_device::i82371ab_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: southbridge_extended_device(mconfig, I82371AB, tag, owner, clock)
	, pci_device_interface(mconfig, *this)
{
}



uint32_t i82371ab_device::pci_isa_r(device_t *busdevice, int offset, uint32_t mem_mask)
{
	uint32_t result = m_regs[0][offset] |
			m_regs[0][offset+1] << 8 |
			m_regs[0][offset+2] << 16|
			m_regs[0][offset+3] << 24;

	LOGISA("ISA read: [%02x] -> %08x & %08x\n", offset, result, mem_mask);

	return result;
}

void i82371ab_device::pci_isa_w(device_t *busdevice, int offset, uint32_t data, uint32_t mem_mask)
{
	uint32_t cdata = 0;
	int i;
	COMBINE_DATA(&cdata);

	LOGISA("ISA write: [%02x] <- %08x & %08x\n", offset, data, mem_mask);

	for(i = 0; i < 4; i++, offset++, cdata >>= 8)
	{
		switch (offset)
		{
			case 0x04:
				// clear reserved bits
				m_regs[0][offset] = cdata & 0x05;
				break;
			case 0x06:
				// set new status
				m_regs[0][offset] |= 0x80;
				break;
			case 0x07:
				m_regs[0][offset] |= 0x02;
				break;
		}
	}
}

uint32_t i82371ab_device::pci_ide_r(device_t *busdevice, int offset, uint32_t mem_mask)
{
	uint32_t result = m_regs[1][offset] |
			m_regs[1][offset+1] << 8 |
			m_regs[1][offset+2] << 16|
			m_regs[1][offset+3] << 24;

	LOGIDE("IDE read: [%02x] -> %08x & %08x\n", offset, result, mem_mask);

	return result;
}

void i82371ab_device::pci_ide_w(device_t *busdevice, int offset, uint32_t data, uint32_t mem_mask)
{
	uint32_t cdata = 0;
	int i;
	COMBINE_DATA(&cdata);

	LOGIDE("IDE write: [%02x] <- %08x & %08x\n", offset, data, mem_mask);

	for(i = 0; i < 4; i++, offset++, cdata >>= 8)
	{
		switch (offset)
		{
			case 0x04:
				// clear reserved bits
				m_regs[1][offset] = cdata & 0x05;
				break;
			case 0x06:
				// set new status
				m_regs[1][offset] |= 0x80;
				break;
			case 0x07:
				m_regs[1][offset] |= 0x02;
				break;
		}
	}
}

uint32_t i82371ab_device::pci_usb_r(device_t *busdevice, int offset, uint32_t mem_mask)
{
	uint32_t result = m_regs[2][offset] |
			m_regs[2][offset+1] << 8 |
			m_regs[2][offset+2] << 16|
			m_regs[2][offset+3] << 24;

	LOGUSB("USB read: [%02x] -> %08x & %08x\n", offset, result, mem_mask);

	return result;
}

void i82371ab_device::pci_usb_w(device_t *busdevice, int offset, uint32_t data, uint32_t mem_mask)
{
	uint32_t cdata = 0;
	int i;
	COMBINE_DATA(&cdata);

	LOGUSB("USB write: [%02x] <- %08x & %08x\n", offset, data, mem_mask);

	for(i = 0; i < 4; i++, offset++, cdata >>= 8)
	{
		switch (offset)
		{
			case 0x04:
				// clear reserved bits
				m_regs[2][offset] = cdata & 0x05;
				break;
			case 0x06:
				// set new status
				m_regs[2][offset] |= 0x80;
				break;
			case 0x07:
				m_regs[2][offset] |= 0x02;
				break;
		}
	}
}

uint32_t i82371ab_device::pci_acpi_r(device_t *busdevice, int offset, uint32_t mem_mask)
{
	uint32_t result = m_regs[3][offset] |
			m_regs[3][offset+1] << 8 |
			m_regs[3][offset+2] << 16|
			m_regs[3][offset+3] << 24;

	LOGACPI("ACPI read: [%02x] -> %08x & %08x\n", offset, result, mem_mask);

	return result;
}

void i82371ab_device::pci_acpi_w(device_t *busdevice, int offset, uint32_t data, uint32_t mem_mask)
{
	uint32_t cdata = 0;
	int i;
	COMBINE_DATA(&cdata);

	LOGACPI("ACPI write: [%02x] <- %08x & %08x\n", offset, data, mem_mask);

	for(i = 0; i < 4; i++, offset++, cdata >>= 8)
	{
		switch (offset)
		{
			case 0x04:
				// clear reserved bits
				m_regs[3][offset] = cdata & 0x05;
				break;
			case 0x06:
				// set new status
				m_regs[3][offset] |= 0x80;
				break;
			case 0x07:
				m_regs[3][offset] |= 0x02;
				break;
		}
	}
}

uint32_t i82371ab_device::pci_read(pci_bus_device *pcibus, int function, int offset, uint32_t mem_mask)
{
	switch (function)
	{
	case 0: return pci_isa_r(pcibus, offset, mem_mask);
	case 1: return pci_ide_r(pcibus, offset, mem_mask);
	case 2: return pci_usb_r(pcibus, offset, mem_mask);
	case 3: return pci_acpi_r(pcibus, offset, mem_mask);
	}

	LOG("read from undefined function %d\n", function);

	return 0;
}

void i82371ab_device::pci_write(pci_bus_device *pcibus, int function, int offset, uint32_t data, uint32_t mem_mask)
{
	switch (function)
	{
	case 0: pci_isa_w(pcibus, offset, data, mem_mask); break;
	case 1: pci_ide_w(pcibus, offset, data, mem_mask); break;
	case 2: pci_usb_w(pcibus, offset, data, mem_mask); break;
	case 3: pci_acpi_w(pcibus, offset, data, mem_mask); break;
	}
}

void i82371ab_device::remap(int space_id, offs_t start, offs_t end)
{
	m_isabus->remap(space_id, start, end);
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void i82371ab_device::device_start()
{
	southbridge_extended_device::device_start();
	// setup save states
	save_item(NAME(m_regs));
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void i82371ab_device::device_reset()
{
	southbridge_extended_device::device_reset();
	memset(m_regs, 0, sizeof(m_regs));
	uint32_t (*regs32)[64] = (uint32_t (*)[64])(m_regs);

	// isa
	regs32[0][0x00] = 0x71108086;
	regs32[0][0x04] = 0x00000000;
	regs32[0][0x08] = 0x06010000;
	regs32[0][0x0c] = 0x00800000;

	// ide
	regs32[1][0x00] = 0x71118086;
	regs32[1][0x04] = 0x02800000;
	regs32[1][0x08] = 0x01018000;
	regs32[1][0x0c] = 0x00000000;

	// usb
	regs32[2][0x00] = 0x71128086;
	regs32[2][0x04] = 0x02800000;
	regs32[2][0x08] = 0x0c030000;
	regs32[2][0x0c] = 0x00000000;

	// acpi
	regs32[3][0x00] = 0x71138086;
	regs32[3][0x04] = 0x02800000;
	regs32[3][0x08] = 0x06800000;
	regs32[3][0x0c] = 0x02800000;
}
