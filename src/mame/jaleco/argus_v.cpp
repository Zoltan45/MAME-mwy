// license:BSD-3-Clause
// copyright-holders:Yochizo
/***************************************************************************

Functions to emulate the video hardware of the machine.


BG RAM format [Argus and Butasan]
-----------------------------------------------------------------------------
 +0         +1
 xxxx xxxx  ---- ---- = 1st - 8th bits of tile number
 ---- ----  xx-- ---- = 9th and 10th bit of tile number
 ---- ----  --x- ---- = flip y
 ---- ----  ---x ---- = flip x
 ---- ----  ---- xxxx = color

BG RAM format [Valtric]
-----------------------------------------------------------------------------
 +0         +1
 xxxx xxxx  ---- ---- = 1st - 8th bits of tile number
 ---- ----  xx-- ---- = 9th and 10th bit of tile number
 ---- ----  --x- ---- = 11th bit of tile number
 ---- ----  ---- xxxx = color


Text RAM format [Argus, Valtric and Butasan]
-----------------------------------------------------------------------------
 +0         +1
 xxxx xxxx  ---- ---- = low bits of tile number
 ---- ----  xx-- ---- = high bits of tile number
 ---- ----  --x- ---- = flip y
 ---- ----  ---x ---- = flip x
 ---- ----  ---- xxxx = color


Sprite RAM format [Argus]
-----------------------------------------------------------------------------
 +11        +12        +13        +14        +15
 xxxx xxxx  ---- ----  ---- ----  ---- ----  ---- ---- = sprite y
 ---- ----  xxxx xxxx  ---- ----  ---- ----  ---- ---- = low bits of sprite x
 ---- ----  ---- ----  xx-- ----  ---- ----  ---- ---- = high bits of tile number
 ---- ----  ---- ----  --x- ----  ---- ----  ---- ---- = flip y
 ---- ----  ---- ----  ---x ----  ---- ----  ---- ---- = flip x
 ---- ----  ---- ----  ---- --x-  ---- ----  ---- ---- = high bit of sprite y
 ---- ----  ---- ----  ---- ---x  ---- ----  ---- ---- = high bit of sprite x
 ---- ----  ---- ----  ---- ----  xxxx xxxx  ---- ---- = low bits of tile number
 ---- ----  ---- ----  ---- ----  ---- ----  ---- x--- = BG1 / sprite priority (Argus only)
 ---- ----  ---- ----  ---- ----  ---- ----  ---- -xxx = color

Sprite RAM format [Valtric]
-----------------------------------------------------------------------------
 +11        +12        +13        +14        +15
 xxxx xxxx  ---- ----  ---- ----  ---- ----  ---- ---- = sprite y
 ---- ----  xxxx xxxx  ---- ----  ---- ----  ---- ---- = low bits of sprite x
 ---- ----  ---- ----  xx-- ----  ---- ----  ---- ---- = high bits of tile number
 ---- ----  ---- ----  --x- ----  ---- ----  ---- ---- = flip y
 ---- ----  ---- ----  ---x ----  ---- ----  ---- ---- = flip x
 ---- ----  ---- ----  ---- --x-  ---- ----  ---- ---- = high bit of sprite y
 ---- ----  ---- ----  ---- ---x  ---- ----  ---- ---- = high bit of sprite x
 ---- ----  ---- ----  ---- ----  xxxx xxxx  ---- ---- = low bits of tile number
 ---- ----  ---- ----  ---- ----  ---- ----  ---- xxxx = color

Sprite RAM format [Butasan]
-----------------------------------------------------------------------------
 +8         +9         +10        +11        +12
 ---- -x--  ---- ----  ---- ----  ---- ----  ---- ---- = flip y
 ---- ---x  ---- ----  ---- ----  ---- ----  ---- ---- = flip x
 ---- ----  ---- xxxx  ---- ----  ---- ----  ---- ---- = color ($00 - $0B)
 ---- ----  ---- ----  xxxx xxxx  ---- ----  ---- ---- = low bits of sprite x
 ---- ----  ---- ----  ---- ----  ---- ---x  ---- ---- = top bit of sprite x
 ---- ----  ---- ----  ---- ----  ---- ----  xxxx xxxx = low bits of sprite y
 +13        +14        +15
 ---- ---x  ---- ----  ---- ---- = top bit of sprite y
 ---- ----  xxxx xxxx  ---- ---- = low bits of tile number
 ---- ----  ---- ----  ---- xxxx = top bits of tile number

(*) Sprite size is defined by its offset.
    $F000 - $F0FF : 16x32    $F100 - $F2FF : 16x16
    $F300 - $F3FF : 16x32    $F400 - $F57F : 16x16
    $F580 - $F61F : 32x32    $F620 - $F67F : 64x64


Scroll RAM of X and Y coordinates [Argus, Valtric and Butasan]
-----------------------------------------------------------------------------
 +0         +1
 xxxx xxxx  ---- ---- = scroll value
 ---- ----  ---- ---x = top bit of scroll value


Video effect RAM ( $C30C )
-----------------------------------------------------------------------------
 +0
 ---- ---x  = BG enable bit
 ---- --x-  = grey scale effect or tile bank select.


Flip screen controller
-----------------------------------------------------------------------------
 +0
 x--- ----  = flip screen


BG0 palette intensity ( $C47F, $C4FF )
-----------------------------------------------------------------------------
 +0 (c47f)  +1 (c4ff)
 xxxx ----  ---- ---- = red intensity
 ---- xxxx  ---- ---- = green intensity
 ---- ----  xxxx ---- = blue intensity


(*) Things which are not emulated.
 - Color $000 - 00f, $01e, $02e ... are half transparent color.
 - Sprite priority bit may be present in Butasan. But I don't know
   what happens when it is set.

***************************************************************************/

#include "emu.h"
#include "argus.h"


/***************************************************************************
  Callbacks for the tilemap code
***************************************************************************/

template<int Gfx>
TILE_GET_INFO_MEMBER(argus_common_state::get_tx_tile_info)
{
	tile_index <<= 1;

	u8 lo = m_txram[tile_index];
	u8 hi = m_txram[tile_index + 1];

	tileinfo.set(Gfx,
			((hi & 0xc0) << 2) | lo,
			hi & 0x0f,
			TILE_FLIPYX((hi & 0x30) >> 4));
}

TILE_GET_INFO_MEMBER(argus_state::get_bg0_tile_info)
{
	// logical width is 65536(4096*16) but we load only 1024 pixel each
	// for reduce RAM usage
	tile_index = (((m_vrom_offset << 9) + tile_index) & 0x1ffff) << 1;
	int vrom_offset = (tile_index >> 3);
	tile_index = (m_vrom[0][vrom_offset & ~1] << 4) | ((m_vrom[0][vrom_offset | 1] & 0x7) << 12) | (tile_index & 0xf);

	u8 lo = m_vrom[1][tile_index];
	u8 hi = m_vrom[1][tile_index | 1];

	tileinfo.set(1,
			((hi & 0xc0) << 2) | lo,
			hi & 0x0f,
			TILE_FLIPYX((hi & 0x30) >> 4));
}

TILE_GET_INFO_MEMBER(argus_state::get_bg1_tile_info)
{
	tile_index <<= 1;

	u8 lo = m_bg1ram[tile_index];
	u8 hi = m_bg1ram[tile_index + 1];

	tileinfo.set(2,
			lo,
			hi & 0x0f,
			TILE_FLIPYX((hi & 0x30) >> 4));
}

TILE_GET_INFO_MEMBER(valtric_state::get_bg_tile_info)
{
	tile_index <<= 1;

	u8 lo = m_bg1ram[tile_index];
	u8 hi = m_bg1ram[tile_index + 1];

	tileinfo.set(1,
			((hi & 0xc0) << 2) | ((hi & 0x20) << 5) | lo,
			hi & 0x0f,
			0);
}

TILE_GET_INFO_MEMBER(butasan_state::get_tx_tile_info)
{
	tile_index <<= 1;

	u8 lo = m_butasan_txram[tile_index];
	u8 hi = m_butasan_txram[tile_index + 1];

	tileinfo.set(3,
			((hi & 0xc0) << 2) | lo,
			hi & 0x0f,
			TILE_FLIPYX((hi & 0x30) >> 4));
}

TILE_GET_INFO_MEMBER(butasan_state::get_bg0_tile_info)
{
	tile_index <<= 1;

	u8 lo = m_butasan_bg0ram[tile_index];
	u8 hi = m_butasan_bg0ram[tile_index + 1];

	tileinfo.set(1,
			((hi & 0xc0) << 2) | lo,
			hi & 0x0f,
			TILE_FLIPYX((hi & 0x30) >> 4));
}

TILE_GET_INFO_MEMBER(butasan_state::get_bg1_tile_info)
{
	int const tile = m_butasan_bg1ram[tile_index] | ((m_butasan_bg1_status & 2) << 7);

	tileinfo.set(2,
			tile,
			(tile & 0x80) >> 7,
			0);
}

TILEMAP_MAPPER_MEMBER(butasan_state::bg_scan)
{
	/* logical (col,row) -> memory offset */
	return (col & 0x0f) | ((row ^ 0x0f) << 4) | ((col & 0x10) << 5);
}

TILEMAP_MAPPER_MEMBER(butasan_state::tx_scan)
{
	/* logical (col,row) -> memory offset */
	return (col & 0x1f) | ((row ^ 0x1f) << 5);
}

/***************************************************************************
  Initialize and destroy video hardware emulation
***************************************************************************/

void argus_common_state::reset_common()
{
	m_bg_status = 0x01;
	m_flipscreen = 0;
	m_palette_intensity = 0;
}

void argus_state::video_start()
{
	/*                           info                     offset             w   h  col  row */
//  m_bg_tilemap[0] = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(argus_state::get_bg0_tile_info)),   TILEMAP_SCAN_COLS, 16, 16, 4096,    32); // full 65536 width tilemap
	m_bg_tilemap[0] = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(argus_state::get_bg0_tile_info)),   TILEMAP_SCAN_COLS, 16, 16, 1024/16, 32);
	m_bg_tilemap[1] = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(argus_state::get_bg1_tile_info)),   TILEMAP_SCAN_COLS, 16, 16, 32,      32);
	m_tx_tilemap    = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(argus_state::get_tx_tile_info<3>)), TILEMAP_SCAN_COLS,  8,  8, 32,      32);

	m_bg_tilemap[1]->set_transparent_pen(15);
	m_tx_tilemap->set_transparent_pen(15);

	save_item(NAME(m_bg_status));
	save_item(NAME(m_flipscreen));
	save_item(NAME(m_palette_intensity));
}

void argus_state::video_reset()
{
	m_bg_scrollx[0][0] = 0;
	m_bg_scrollx[0][1] = 0;
	reset_common();
}

void valtric_state::video_start()
{
	/*                           info                      offset             w   h  col  row */
	m_bg_tilemap[0] = nullptr;
	m_bg_tilemap[1] = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(valtric_state::get_bg_tile_info)),    TILEMAP_SCAN_COLS, 16, 16, 32, 32);
	m_tx_tilemap    = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(valtric_state::get_tx_tile_info<2>)), TILEMAP_SCAN_COLS,  8,  8, 32, 32);

	m_tx_tilemap->set_transparent_pen(15);

	m_screen->register_screen_bitmap(m_mosaicbitmap);

	save_item(NAME(m_bg_status));
	save_item(NAME(m_flipscreen));
	save_item(NAME(m_palette_intensity));
	save_item(NAME(m_valtric_mosaic));
	save_item(NAME(m_valtric_unknown));
	save_item(NAME(m_mosaic));
}

void valtric_state::video_reset()
{
	m_valtric_mosaic = 0x0f;
	reset_common();
}

void butasan_state::video_start()
{
	/*                           info                       offset             w   h  col  row */
	m_bg_tilemap[0] = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(butasan_state::get_bg0_tile_info)), tilemap_mapper_delegate(*this, FUNC(butasan_state::bg_scan)), 16, 16, 32, 32);
	m_bg_tilemap[1] = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(butasan_state::get_bg1_tile_info)), tilemap_mapper_delegate(*this, FUNC(butasan_state::bg_scan)), 16, 16, 32, 32);
	m_tx_tilemap    = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(butasan_state::get_tx_tile_info)),  tilemap_mapper_delegate(*this, FUNC(butasan_state::tx_scan)),  8,  8, 32, 32);

	m_bg_tilemap[1]->set_transparent_pen(15);
	m_tx_tilemap->set_transparent_pen(15);

	m_butasan_pagedram[0] = std::make_unique<u8[]>(0x1000);
	m_butasan_pagedram[1] = std::make_unique<u8[]>(0x1000);

	m_butasan_bg0ram     = &m_butasan_pagedram[0][0x000];
	m_butasan_bg0backram = &m_butasan_pagedram[0][0x800];
	m_butasan_txram      = &m_butasan_pagedram[1][0x000];
	m_butasan_txbackram  = &m_butasan_pagedram[1][0x800];

	save_item(NAME(m_bg_status));
	save_item(NAME(m_flipscreen));
	save_item(NAME(m_palette_intensity));
	save_pointer(NAME(m_butasan_pagedram[0]), 0x1000);
	save_pointer(NAME(m_butasan_pagedram[1]), 0x1000);
	save_item(NAME(m_butasan_page_latch));
	save_item(NAME(m_butasan_bg1_status));
	save_item(NAME(m_butasan_unknown));
}

void butasan_state::video_reset()
{
	m_butasan_page_latch = 0;
	m_butasan_bg1_status = 0x01;
	memset(m_butasan_pagedram[0].get(), 0, 0x1000);
	memset(m_butasan_pagedram[1].get(), 0, 0x1000);
	reset_common();
}


/***************************************************************************
  Functions for handler of MAP roms in Argus and palette color
***************************************************************************/

void argus_common_state::change_palette(int color, int lo_offs, int hi_offs)
{
	u8 lo = m_paletteram[lo_offs];
	u8 hi = m_paletteram[hi_offs];
	m_palette->set_pen_color(color, rgb_t(hi & 0x0f, pal4bit(lo >> 4), pal4bit(lo), pal4bit(hi >> 4)));
}

void argus_common_state::change_bg_palette(int color, int lo_offs, int hi_offs)
{
	u8 r,g,b,lo,hi,ir,ig,ib,ix;
	rgb_t rgb,irgb;

	/* red,green,blue intensities */
	ir = pal4bit(m_palette_intensity >> 12);
	ig = pal4bit(m_palette_intensity >>  8);
	ib = pal4bit(m_palette_intensity >>  4);
	ix = m_palette_intensity & 0x0f;

	irgb = rgb_t(ir,ig,ib);

	lo = m_paletteram[lo_offs];
	hi = m_paletteram[hi_offs];

	/* red,green,blue component */
	r = pal4bit(lo >> 4);
	g = pal4bit(lo);
	b = pal4bit(hi >> 4);

	/* Grey background enable */
	if (m_bg_status & 2)
	{
		u8 val = (r + g + b) / 3;
		rgb = rgb_t(val,val,val);
	}
	else
	{
		rgb = rgb_t(r,g,b);
	}

	rgb = m_blend->func(rgb,irgb,ix);

	m_palette->set_pen_color(color,rgb);
}


/***************************************************************************
  Memory handler
***************************************************************************/

void valtric_state::mosaic_w(u8 data)
{
	m_valtric_mosaic = data;
}

void argus_common_state::txram_w(offs_t offset, u8 data)
{
	m_txram[offset] = data;
	m_tx_tilemap->mark_tile_dirty(offset >> 1);
}

void argus_common_state::bg1ram_w(offs_t offset, u8 data)
{
	m_bg1ram[offset] = data;
	m_bg_tilemap[1]->mark_tile_dirty(offset >> 1);
}

void argus_state::bg_status_w(u8 data)
{
	if (m_bg_status != data)
	{
		m_bg_status = data;

		/* Gray / purple scale */
		if (m_bg_status & 2)
		{
			int offs;

			for (offs = 0x400; offs < 0x500; offs++)
			{
				change_bg_palette((offs - 0x400) + 0x080, offs, offs + 0x400);
			}
		}
	}
}

void valtric_state::bg_status_w(u8 data)
{
	if (m_bg_status != data)
	{
		m_bg_status = data;

		/* Gray / purple scale */
		if (m_bg_status & 2)
		{
			int offs;

			for (offs = 0x400; offs < 0x600; offs += 2)
			{
				change_bg_palette(((offs - 0x400) >> 1) + 0x100, offs & ~1, offs | 1);
			}
		}
	}
}

void butasan_state::bg0_status_w(u8 data)
{
	m_bg_status = data;
}

void butasan_state::bg1_status_w(u8 data)
{
	if (m_butasan_bg1_status != data)
	{
		m_butasan_bg1_status = data;

		/* Bank changed */
		m_bg_tilemap[1]->mark_all_dirty();
	}
}

void argus_common_state::flipscreen_w(u8 data)
{
	m_flipscreen = data & 0x80;
}

void argus_state::paletteram_w(offs_t offset, u8 data)
{
	int offs;

	m_paletteram[offset] = data;

	if (offset <= 0x0ff)                                /* sprite color */
	{
		offset &= 0x07f;

		change_palette(offset, offset, offset + 0x080);

		if (offset == 0x07f || offset == 0x0ff)
		{
			m_palette_intensity = m_paletteram[0x0ff] | (m_paletteram[0x07f] << 8);

			for (offs = 0x400; offs < 0x500; offs++)
				change_bg_palette((offs & 0xff) + 0x080, offs, offs + 0x400);
		}
	}
	else if ((offset >= 0x400 && offset <= 0x4ff) ||
				(offset >= 0x800 && offset <= 0x8ff))       /* BG0 color */
	{
		offs = offset & 0xff;
		offset = offs | 0x400;

		change_bg_palette(offs + 0x080, offset, offset + 0x400);
	}
	else if ((offset >= 0x500 && offset <= 0x5ff) ||
				(offset >= 0x900 && offset <= 0x9ff))       /* BG1 color */
	{
		offs = offset & 0xff;
		offset = offs | 0x500;

		change_palette(offs + 0x180, offset, offset + 0x400);
	}
	else if ((offset >= 0x700 && offset <= 0x7ff) ||
				(offset >= 0xb00 && offset <= 0xbff))       /* text color */
	{
		offs = offset & 0xff;
		offset = offs | 0x700;

		change_palette(offs + 0x280, offset, offset + 0x400);
	}
}

void valtric_state::paletteram_w(offs_t offset, u8 data)
{
	m_paletteram[offset] = data;

	if (offset <= 0x1ff)                            /* Sprite color */
	{
		change_palette(offset >> 1, offset & ~1, offset | 1);

		if (offset == 0x1fe || offset == 0x1ff)
		{
			int offs;

			m_palette_intensity = m_paletteram[0x1ff] | (m_paletteram[0x1fe] << 8);

			for (offs = 0x400; offs < 0x600; offs += 2)
				change_bg_palette(((offs & 0x1ff) >> 1) + 0x100, offs & ~1, offs | 1);
		}
	}
	else if (offset >= 0x400 && offset <= 0x5ff)        /* BG color */
	{
		change_bg_palette(((offset & 0x1ff) >> 1) + 0x100, offset & ~1, offset | 1);
	}
	else if (offset >= 0x600 && offset <= 0x7ff)        /* Text color */
	{
		change_palette(((offset & 0x1ff) >> 1) + 0x200, offset & ~1, offset | 1);
	}
}

void butasan_state::paletteram_w(offs_t offset, u8 data)
{
	m_paletteram[offset] = data;

	if (offset <= 0x1ff)                            /* BG0 color */
	{
		change_palette((offset >> 1) + 0x100, offset & ~1, offset | 1);
	}
	else if (offset <= 0x23f)                       /* BG1 color */
	{
		change_palette(((offset & 0x3f) >> 1) + 0x0c0, offset & ~1, offset | 1);
	}
	else if (offset >= 0x400 && offset <= 0x47f)    /* Sprite color */
	{                                               /* 16 colors */
		change_palette((offset & 0x7f) >> 1, offset & ~1, offset | 1);
	}
	else if (offset >= 0x480 && offset <= 0x4ff)    /* Sprite color */
	{                                               /* 8  colors */
		int offs = (offset & 0x070) | ((offset & 0x00f) >> 1);

		change_palette(offs + 0x040, offset & ~1, offset | 1);
		change_palette(offs + 0x048, offset & ~1, offset | 1);
	}
	else if (offset >= 0x600 && offset <= 0x7ff)    /* Text color */
	{
		change_palette(((offset & 0x1ff) >> 1) + 0x200, offset & ~1, offset | 1);
	}
	else if (offset >= 0x240 && offset <= 0x25f)    // dummy
		change_palette(((offset & 0x1f) >> 1) + 0xe0, offset & ~1, offset | 1);
	else if (offset >= 0x500 && offset <= 0x51f)    // dummy
		change_palette(((offset & 0x1f) >> 1) + 0xf0, offset & ~1, offset | 1);
}

void butasan_state::bg1ram_w(offs_t offset, u8 data)
{
	m_butasan_bg1ram[offset] = data;
	m_bg_tilemap[1]->mark_tile_dirty(offset);
}

void butasan_state::pageselect_w(u8 data)
{
	m_butasan_page_latch = data & 1;
}

u8 butasan_state::pagedram_r(offs_t offset)
{
	if (offset <= 0x07ff)
		return m_butasan_pagedram[m_butasan_page_latch][offset];
	else
		return m_butasan_pagedram[0][offset];
}

void butasan_state::pagedram_w(offs_t offset, u8 data)
{
	m_butasan_pagedram[m_butasan_page_latch][offset] = data;

	if (!m_butasan_page_latch)
	{
		if (offset <= 0x07ff)
			m_bg_tilemap[0]->mark_tile_dirty(offset >> 1);
	}
	else
	{
		if (offset <= 0x07ff)
			m_tx_tilemap->mark_tile_dirty(offset >> 1);
	}
}

void valtric_state::unknown_w(u8 data)
{
	m_valtric_unknown = data;
}

void butasan_state::unknown_w(u8 data)
{
	m_butasan_unknown = data;
}


/***************************************************************************
  Screen refresh
***************************************************************************/

#define bg_scrollx(layer) (m_bg_scrollx[layer][0] | (m_bg_scrollx[layer][1] << 8))
#define bg_scrolly(layer) (m_bg_scrolly[layer][0] | (m_bg_scrolly[layer][1] << 8))

void argus_common_state::bg_setting()
{
	machine().tilemap().set_flip_all(m_flipscreen ? TILEMAP_FLIPY|TILEMAP_FLIPX : 0);

	if (!m_flipscreen)
	{
		if (m_bg_tilemap[0] != nullptr)
		{
			if ((m_vrom[0] != nullptr) && (m_vrom[1] != nullptr))
			{
				if (m_vrom_offset != m_bg_scrollx[0][1])
				{
					m_vrom_offset = m_bg_scrollx[0][1];
					m_bg_tilemap[0]->mark_all_dirty();
				}
				m_bg_tilemap[0]->set_scrollx(0, m_bg_scrollx[0][0]);
				m_bg_tilemap[0]->set_scrolly(0, bg_scrolly(0));
			}
			else
			{
				m_bg_tilemap[0]->set_scrollx(0, bg_scrollx(0));
				m_bg_tilemap[0]->set_scrolly(0, bg_scrolly(0));
			}
		}
		m_bg_tilemap[1]->set_scrollx(0, bg_scrollx(1) & 0x1ff);
		m_bg_tilemap[1]->set_scrolly(0, bg_scrolly(1) & 0x1ff);
	}
	else
	{
		if (m_bg_tilemap[0] != nullptr)
		{
			if ((m_vrom[0] != nullptr) && (m_vrom[1] != nullptr))
			{
				if (m_vrom_offset != ((m_bg_scrollx[0][1] + 1) & 0xff))
				{
					m_vrom_offset = ((m_bg_scrollx[0][1] + 1) & 0xff);
					m_bg_tilemap[0]->mark_all_dirty();
				}
				m_bg_tilemap[0]->set_scrollx(0, m_bg_scrollx[0][0]);
				m_bg_tilemap[0]->set_scrolly(0, (bg_scrolly(0) + 256));
			}
			else
			{
				m_bg_tilemap[0]->set_scrollx(0, (bg_scrollx(0) + 256));
				m_bg_tilemap[0]->set_scrolly(0, (bg_scrolly(0) + 256));
			}
		}
		m_bg_tilemap[1]->set_scrollx(0, (bg_scrollx(1) + 256) & 0x1ff);
		m_bg_tilemap[1]->set_scrolly(0, (bg_scrolly(1) + 256) & 0x1ff);
	}
}

void argus_state::draw_sprites(bitmap_rgb32 &bitmap, const rectangle &cliprect, int priority)
{
	/* Draw the sprites */
	for (int offs = 0; offs < m_spriteram.bytes(); offs += 16)
	{
		if (!(m_spriteram[offs+15] == 0 && m_spriteram[offs+11] == 0xf0))
		{
			int sx, sy, tile, flipx, flipy, color, pri;

			sx = m_spriteram[offs+12]; if (m_spriteram[offs+13] & 0x01) sx -= 256;
			sy = m_spriteram[offs+11]; if (!(m_spriteram[offs+13] & 0x02)) sy -= 256;

			tile  = m_spriteram[offs+14] | ((m_spriteram[offs+13] & 0xc0) << 2);
			flipx = m_spriteram[offs+13] & 0x10;
			flipy = m_spriteram[offs+13] & 0x20;
			color = m_spriteram[offs+15] & 0x07;
			pri   = (m_spriteram[offs+15] & 0x08) >> 3;

			if (m_flipscreen)
			{
				sx = 240 - sx;
				sy = 240 - sy;
				flipx = !flipx;
				flipy = !flipy;
			}

			if (priority != pri)
				m_blend->drawgfx(
						*m_palette,
						m_gfxdecode->gfx(0),
						bitmap, cliprect,
						tile,
						color,
						flipx, flipy,
						sx, sy,
						15);
		}
	}
}

#if 1
void valtric_state::draw_mosaic(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	if (m_valtric_mosaic!=0x80)
	{
		m_mosaic=0x0f-(m_valtric_mosaic&0x0f);
		if (m_mosaic!=0) m_mosaic++;
		if (m_valtric_mosaic&0x80) m_mosaic*=-1;
	}

	if (m_mosaic==0)
		m_bg_tilemap[1]->draw(screen, bitmap, cliprect, 0, 0);
	else
	{
		m_bg_tilemap[1]->draw(screen, m_mosaicbitmap, cliprect, 0, 0);
		int step=m_mosaic;
		int c=0;
		int width = screen.width();
		int height = screen.height();

		if (m_mosaic<0)step*=-1;

		for (int y=0;y<width+step;y+=step)
			for (int x=0;x<height+step;x+=step)
			{
				if (y < height && x < width)
					c=m_mosaicbitmap.pix(y, x);

				if (m_mosaic<0)
					if (y+step-1<height && x+step-1< width)
						c = m_mosaicbitmap.pix(y+step-1, x+step-1);

				for (int yy=0;yy<step;yy++)
					for (int xx=0;xx<step;xx++)
					{
						if (xx+x < width && yy+y<height)
							bitmap.pix(y+yy, x+xx) = c;
					}
			}
	}
}
#else
void valtric_state::draw_mosaic(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	int step = 0x10 - (m_valtric_mosaic & 0x0f);

	if (step == 1)
		m_bg_tilemap[1]->draw(screen, bitmap, cliprect, 0, 0);
	else
	{
		m_bg_tilemap[1]->draw(screen, m_mosaicbitmap, cliprect, 0, 0);
		int c=0;
		int width = screen.width();
		int height = screen.height();

		for (int y = 0; y < width+step; y += step)
			for (int x = 0; x < height+step; x += step)
			{
				if (y < height && x < width)
					c = m_mosaicbitmap.pix(y, x);

				if (m_valtric_mosaic & 0x80)
					if (y+step-1 < height && x+step-1 < width)
						c = m_mosaicbitmap.pix(y+step-1, x+step-1);

				for (int yy = 0; yy < step; yy++)
					for (int xx = 0; xx < step; xx++)
					{
						if (xx+x < width && yy+y < height)
							bitmap.pix(y+yy, x+xx) = c;
					}
			}
	}
}
#endif

void valtric_state::draw_sprites(bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	/* Draw the sprites */
	for (int offs = 0; offs < m_spriteram.bytes(); offs += 16)
	{
		if (!(m_spriteram[offs+15] == 0 && m_spriteram[offs+11] == 0xf0))
		{
			int sx, sy, tile, flipx, flipy, color;

			sx = m_spriteram[offs+12]; if (m_spriteram[offs+13] & 0x01) sx -= 256;
			sy = m_spriteram[offs+11]; if (!(m_spriteram[offs+13] & 0x02)) sy -= 256;

			tile  = m_spriteram[offs+14] | ((m_spriteram[offs+13] & 0xc0) << 2);
			flipx = m_spriteram[offs+13] & 0x10;
			flipy = m_spriteram[offs+13] & 0x20;
			color = m_spriteram[offs+15] & 0x0f;

			if (m_flipscreen)
			{
				sx = 240 - sx;
				sy = 240 - sy;
				flipx = !flipx;
				flipy = !flipy;
			}

			m_blend->drawgfx(
					*m_palette,
					m_gfxdecode->gfx(0),
					bitmap, cliprect,
					tile,
					color,
					flipx, flipy,
					sx, sy,
					15);
		}
	}
}

void butasan_state::draw_sprites(bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	/* Draw the sprites */
	for (int offs = 0; offs < m_spriteram.bytes(); offs += 16)
	{
		int sx, sy, tile, flipx, flipy, color;
		int fx, fy;

		tile  = m_spriteram[offs+14] | ((m_spriteram[offs+15] & 0x0f) << 8);
		flipx = m_spriteram[offs+8] & 0x01;
		flipy = m_spriteram[offs+8] & 0x04;
		color = m_spriteram[offs+9] & 0x0f;

		sx = m_spriteram[offs+10];
		sy = m_spriteram[offs+12];

		if (m_spriteram[offs+11] & 0x01) sx-=256;
		if (m_spriteram[offs+13] & 0x01) sy-=256;

		sy = 240 - sy;

		if (m_flipscreen)
		{
			sx = 240 - sx;
			sy = 240 - sy;
			flipx = !flipx;
			flipy = !flipy;
		}

		fx = flipx;
		fy = flipy;

		{
			int i, j, td;

			if ((offs >= 0x100 && offs <= 0x2ff) || (offs >= 0x400 && offs <= 0x57f))
			{
				m_blend->drawgfx(
						*m_palette,
						m_gfxdecode->gfx(0),
						bitmap, cliprect,
						tile,
						color,
						flipx, flipy,
						sx, sy,
						7);
			}
			else if ((offs >= 0x000 && offs <= 0x0ff) || (offs >= 0x300 && offs <= 0x3ff))
			{
				for (i = 0; i <= 1; i++)
				{
					td = (fx) ? (1 - i) : i;

					m_blend->drawgfx(
							*m_palette,
							m_gfxdecode->gfx(0),
							bitmap, cliprect,
							tile + td,
							color,
							flipx, flipy,
							sx + i * 16, sy,
							7);
				}
			}
			else if (offs >= 0x580 && offs <= 0x61f)
			{
				for (i = 0; i <= 1; i++)
				{
					for (j = 0; j <= 1; j++)
					{
						if (fy)
							td = (fx) ? ((1 - i) * 2) + 1 - j : (1 - i) * 2 + j;
						else
							td = (fx) ? (i * 2) + 1 - j : i * 2 + j;

						m_blend->drawgfx(
								*m_palette,
								m_gfxdecode->gfx(0),
								bitmap, cliprect,
								tile + td,
								color,
								flipx, flipy,
								sx + j * 16, sy - i * 16,
								7);
					}
				}
			}
			else if (offs >= 0x620 && offs <= 0x67f)
			{
				for (i = 0; i <= 3; i++)
				{
					for (j = 0; j <= 3; j++)
					{
						if (fy)
							td = (fx) ? ((3 - i) * 4) + 3 - j : (3 - i) * 4 + j;
						else
							td = (fx) ? (i * 4) + 3 - j : i * 4 + j;

						m_blend->drawgfx(
								*m_palette,
								m_gfxdecode->gfx(0),
								bitmap, cliprect,
								tile + td,
								color,
								flipx, flipy,
								sx + j * 16, sy - i * 16,
								7);
					}
				}
			}
		}
	}
}


void butasan_state::log_vram()
{
#ifdef MAME_DEBUG
	int offs;

	if (machine().input().code_pressed(KEYCODE_M))
	{
		u8 *spriteram = &m_spriteram[0];
		int i;
		logerror("\nSprite RAM\n");
		logerror("---------------------------------------\n");
		logerror("       +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +a +b +c +d +e +f\n");
		for (offs = 0; offs < m_spriteram.bytes(); offs += 16)
		{
			for (i = 0; i < 16; i++)
			{
				if (i == 0)
				{
					logerror("%04x : ", offs + 0xf000);
					logerror("%02x ", spriteram[offs]);
				}
				else if (i == 7)
					logerror("%02x  ", spriteram[offs + 7]);
				else if (i == 15)
					logerror("%02x\n", spriteram[offs + 15]);
				else
					logerror("%02x ", spriteram[offs + i]);
			}
		}
		logerror("\nColor RAM\n");
		logerror("---------------------------------------\n");
		logerror("       +0 +1 +2 +3 +4 +5 +6 +7  +8 +9 +a +b +c +d +e +f\n");
		for (offs = 0; offs < 0xbf0; offs += 16)
		{
			for (i = 0; i < 16; i++)
			{
				if (i == 0)
				{
					logerror("%04x : ", offs + 0xc400);
					logerror("%02x ", m_paletteram[offs]);
				}
				else if (i == 7)
					logerror("%02x  ", m_paletteram[offs + 7]);
				else if (i == 15)
					logerror("%02x\n", m_paletteram[offs + 15]);
				else
					logerror("%02x ", m_paletteram[offs + i]);
			}
		}
	}
#endif
}

u32 argus_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	bg_setting();

	m_bg_tilemap[0]->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap, cliprect, 0);
	if (m_bg_status & 1)    /* Background enable */
		m_bg_tilemap[1]->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap, cliprect, 1);
	m_tx_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	return 0;
}

u32 valtric_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	bg_setting();

	if (m_bg_status & 1)    /* Background enable */
		draw_mosaic(screen, bitmap, cliprect);
	else
		bitmap.fill(m_palette->black_pen(), cliprect);
	draw_sprites(bitmap, cliprect);
	m_tx_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	return 0;
}

u32 butasan_state::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	bg_setting();

	if (m_bg_status & 1)    /* Background enable */
		m_bg_tilemap[0]->draw(screen, bitmap, cliprect, 0, 0);
	else
		bitmap.fill(m_palette->black_pen(), cliprect);
	if (m_butasan_bg1_status & 1) m_bg_tilemap[1]->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap, cliprect);
	m_tx_tilemap->draw(screen, bitmap, cliprect, 0, 0);

	log_vram();
	return 0;
}
