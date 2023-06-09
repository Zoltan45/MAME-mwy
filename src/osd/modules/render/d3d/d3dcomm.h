// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  d3dcomm.h - Common Win32 Direct3D structures
//
//============================================================

#ifndef MAME_RENDER_D3D_D3DCOMM_H
#define MAME_RENDER_D3D_D3DCOMM_H

#pragma once

// lib/util
#include "bitmap.h"

#include <windows.h>
#include <d3d9.h>
#include <wrl/client.h>

#include <cstring>
#include <memory>
#include <vector>


//============================================================
//  CONSTANTS
//============================================================

#define MAX_BLOOM_COUNT 15 // shader model 3.0 support up to 16 samplers, but we need the last for the original texture
#define HALF_BLOOM_COUNT 8

//============================================================
//  FORWARD DECLARATIONS
//============================================================

class texture_info;
class renderer_d3d9;
struct d3d_base;

//============================================================
//  TYPE DEFINITIONS
//============================================================

class vec2f
{
public:
	vec2f()
	{
		memset(&c, 0, sizeof(float) * 2);
	}
	vec2f(float x, float y)
	{
		c.x = x;
		c.y = y;
	}

	vec2f operator+(const vec2f& a) const
	{
		return vec2f(c.x + a.c.x, c.y + a.c.y);
	}

	vec2f operator-(const vec2f& a) const
	{
		return vec2f(c.x - a.c.x, c.y - a.c.y);
	}

	struct
	{
		float x, y;
	} c;
};

class d3d_texture_manager
{
public:
	d3d_texture_manager(renderer_d3d9 &d3d, IDirect3D9 *d3dobj);

	void                    update_textures();

	void                    create_resources();
	void                    delete_resources();

	texture_info *          find_texinfo(const render_texinfo *texture, uint32_t flags);
	uint32_t                texture_compute_hash(const render_texinfo *texture, uint32_t flags);

	D3DFORMAT               get_yuv_format() const { return m_yuv_format; }

	DWORD                   get_texture_caps() const { return m_texture_caps; }
	DWORD                   get_max_texture_aspect() const { return m_texture_max_aspect; }
	DWORD                   get_max_texture_width() const { return m_texture_max_width; }
	DWORD                   get_max_texture_height() const { return m_texture_max_height; }

	texture_info *          get_default_texture() const { return m_default_texture; }

	renderer_d3d9 &         get_d3d() const { return m_renderer; }

	std::vector<std::unique_ptr<texture_info> > m_texture_list;  // list of active textures

private:
	renderer_d3d9 &         m_renderer;
	D3DFORMAT               m_yuv_format;               // format to use for YUV textures

	DWORD                   m_texture_caps;             // textureCaps field
	DWORD                   m_texture_max_aspect;       // texture maximum aspect ratio
	DWORD                   m_texture_max_width;        // texture maximum width
	DWORD                   m_texture_max_height;       // texture maximum height

	bitmap_rgb32            m_default_bitmap;           // experimental: default bitmap
	texture_info *          m_default_texture;          // experimental: default texture
};


/* texture_info holds information about a texture */
class texture_info
{
public:
	texture_info(d3d_texture_manager &manager, const render_texinfo *texsource, int prescale, uint32_t flags);
	~texture_info();

	render_texinfo &        get_texinfo() { return m_texinfo; }

	int                     get_width() const { return m_rawdims.c.x; }
	int                     get_height() const { return m_rawdims.c.y; }
	int                     get_xscale() const { return m_xprescale; }
	int                     get_yscale() const { return m_yprescale; }

	uint32_t                get_flags() const { return m_flags; }

	void                    set_data(const render_texinfo *texsource, uint32_t flags);

	uint32_t                get_hash() const { return m_hash; }

	void                    increment_frame_count() { m_cur_frame++; }
	void                    mask_frame_count(int mask) { m_cur_frame %= mask; }

	int                     get_cur_frame() const { return m_cur_frame; }

	IDirect3DTexture9 *     get_finaltex() const { return m_d3dfinaltex.Get(); }

	vec2f &                 get_uvstart() { return m_start; }
	vec2f &                 get_uvstop() { return m_stop; }
	vec2f &                 get_rawdims() { return m_rawdims; }

private:
	using IDirect3DTexture9Ptr = Microsoft::WRL::ComPtr<IDirect3DTexture9>;
	using IDirect3DSurface9Ptr = Microsoft::WRL::ComPtr<IDirect3DSurface9>;

	void prescale();
	void compute_size(int texwidth, int texheight);
	void compute_size_subroutine(int texwidth, int texheight, int* p_width, int* p_height);

	inline void copyline_palette16(uint32_t *dst, const uint16_t *src, int width, const rgb_t *palette, int xborderpix);
	inline void copyline_rgb32(uint32_t *dst, const uint32_t *src, int width, const rgb_t *palette, int xborderpix);
	inline void copyline_argb32(uint32_t *dst, const uint32_t *src, int width, const rgb_t *palette, int xborderpix);
	inline void copyline_yuy16_to_yuy2(uint16_t *dst, const uint16_t *src, int width, const rgb_t *palette);
	inline void copyline_yuy16_to_uyvy(uint16_t *dst, const uint16_t *src, int width, const rgb_t *palette);
	inline void copyline_yuy16_to_argb(uint32_t *dst, const uint16_t *src, int width, const rgb_t *palette);

	d3d_texture_manager &   m_texture_manager;          // texture manager pointer

	renderer_d3d9 &         m_renderer;                 // renderer pointer

	const uint32_t          m_hash;                     // hash value for the texture
	const uint32_t          m_flags;                    // rendering flags
	render_texinfo          m_texinfo;                  // copy of the texture info
	const int               m_type;                     // what type of texture are we?
	vec2f                   m_start;                    // beggining UV coordinates
	vec2f                   m_stop;                     // ending UV coordinates
	vec2f                   m_rawdims;                  // raw dims of the texture
	int                     m_xprescale, m_yprescale;   // X/Y prescale factor
	int                     m_xborderpix, m_yborderpix; // X/Y border pixels
	int                     m_cur_frame;                // what is our current frame?
	IDirect3DTexture9Ptr    m_d3dtex;                   // Direct3D texture pointer
	IDirect3DSurface9Ptr    m_d3dsurface;               // Direct3D offscreen plain surface pointer
	IDirect3DTexture9Ptr    m_d3dfinaltex;              // Direct3D final (post-scaled) texture
};

/* poly_info holds information about a single polygon/d3d primitive */
class poly_info
{
public:
	void init(D3DPRIMITIVETYPE type, uint32_t count, uint32_t numverts,
				uint32_t flags, texture_info *texture, uint32_t modmode,
				float prim_width, float prim_height, uint32_t tint)
	{
		m_type = type;
		m_count = count;
		m_numverts = numverts;
		m_flags = flags;
		m_texture = texture;
		m_modmode = modmode;
		m_prim_width = prim_width;
		m_prim_height = prim_height;
		m_tint = tint;
	}

	D3DPRIMITIVETYPE        type() const { return m_type; }
	uint32_t                count() const { return m_count; }
	uint32_t                numverts() const { return m_numverts; }
	uint32_t                flags() const { return m_flags; }

	texture_info *          texture() const { return m_texture; }
	DWORD                   modmode() const { return m_modmode; }

	float                   prim_width() const { return m_prim_width; }
	float                   prim_height() const { return m_prim_height; }

	DWORD                   tint() const { return m_tint; }

private:
	D3DPRIMITIVETYPE        m_type;         // type of primitive
	uint32_t                m_count;        // total number of primitives
	uint32_t                m_numverts;     // total number of vertices
	uint32_t                m_flags;        // rendering flags

	texture_info *          m_texture;      // pointer to texture info
	DWORD                   m_modmode;      // texture modulation mode

	float                   m_prim_width;   // used by quads
	float                   m_prim_height;  // used by quads

	uint32_t                m_tint;         // color tint for primitive
};

/* vertex describes a single vertex */
struct vertex
{
	float       x, y, z;                    // X,Y,Z coordinates
	float       rhw;                        // RHW when no HLSL, padding when HLSL
	D3DCOLOR    color;                      // diffuse color
	float       u0, v0;                     // texture stage 0 coordinates
	float       u1, v1;                     // additional info for vector data
};


/* d3d_render_target is the information about a Direct3D render target chain */
class d3d_render_target
{
public:
	// construction/destruction
	d3d_render_target()
		: target_width(0)
		, target_height(0)
		, width(0)
		, height(0)
		, screen_index(0)
		, bloom_count(0)
	{
	}

	~d3d_render_target();

	bool init(renderer_d3d9 *d3d, int source_width, int source_height, int target_width, int target_height, int screen_index);
	int next_index(int index) { return ++index > 1 ? 0 : index; }

	// real target dimension
	int target_width;
	int target_height;

	// only used to identify/find the render target
	int width;
	int height;

	int screen_index;

	Microsoft::WRL::ComPtr<IDirect3DSurface9> target_surface[2];
	Microsoft::WRL::ComPtr<IDirect3DTexture9> target_texture[2];
	Microsoft::WRL::ComPtr<IDirect3DSurface9> source_surface[2];
	Microsoft::WRL::ComPtr<IDirect3DTexture9> source_texture[2];

	Microsoft::WRL::ComPtr<IDirect3DSurface9> cache_surface;
	Microsoft::WRL::ComPtr<IDirect3DTexture9> cache_texture;

	Microsoft::WRL::ComPtr<IDirect3DSurface9> bloom_surface[MAX_BLOOM_COUNT];
	Microsoft::WRL::ComPtr<IDirect3DTexture9> bloom_texture[MAX_BLOOM_COUNT];

	float bloom_dims[MAX_BLOOM_COUNT][2];

	int bloom_count;
};

#endif // MAME_RENDER_D3D_D3DCOMM_H
