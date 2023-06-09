// license:BSD-3-Clause
// copyright-holders:Vas Crabb
/***************************************************************************

    msdib.h

    Microsoft Device-Independent Bitmap file loading.

***************************************************************************/
#ifndef MAME_LIB_UTIL_MSDIB_H
#define MAME_LIB_UTIL_MSDIB_H

#pragma once

#include "bitmap.h"
#include "utilfwd.h"

#include <cstdint>


namespace util {

/***************************************************************************
    CONSTANTS
***************************************************************************/

// Error types
enum class msdib_error
{
	NONE,
	OUT_OF_MEMORY,
	FILE_ERROR,
	BAD_SIGNATURE,
	FILE_TRUNCATED,
	FILE_CORRUPT,
	UNSUPPORTED_FORMAT
};

msdib_error msdib_verify_header(random_read &fp) noexcept;
msdib_error msdib_read_bitmap(random_read &fp, bitmap_argb32 &bitmap) noexcept;
msdib_error msdib_read_bitmap_data(random_read &fp, bitmap_argb32 &bitmap, std::uint32_t length, std::uint32_t dirheight = 0U) noexcept;

} // namespace util

#endif // MAME_LIB_UTIL_MSDIB_H
