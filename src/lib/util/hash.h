// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    hash.h

    Function to handle hash functions (checksums)

    Based on original idea by Farfetch'd

***************************************************************************/

#ifndef MAME_UTIL_HASH_H
#define MAME_UTIL_HASH_H

#pragma once

#include "hashing.h"
#include "utilfwd.h"

#include <memory>
#include <system_error>


//**************************************************************************
//  MACROS
//**************************************************************************

// use these to define compile-time internal-format hash strings
#define CRC(x)              "R" #x
#define SHA1(x)             "S" #x
#define NO_DUMP             "!"
#define BAD_DUMP            "^"


namespace util {

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// ======================> hash_collection

// a collection of the various supported hashes and flags
class hash_collection
{
	// forward declaration
	class hash_creator;

public:
	// hash types are identified by non-hex alpha values (G-Z)
	static constexpr char HASH_CRC = 'R';
	static constexpr char HASH_SHA1 = 'S';

	// common combinations for requests
	static char const *const HASH_TYPES_CRC;
	static char const *const HASH_TYPES_CRC_SHA1;
	static char const *const HASH_TYPES_ALL;

	// flags are identified by punctuation marks
	static constexpr char FLAG_NO_DUMP = '!';
	static constexpr char FLAG_BAD_DUMP = '^';

	// construction/destruction
	hash_collection();
	hash_collection(std::string_view string) : hash_collection() { from_internal_string(string); }
	hash_collection(const hash_collection &src) : hash_collection() { copyfrom(src); }

	// operators
	hash_collection &operator=(const hash_collection &src);
	bool operator==(const hash_collection &rhs) const noexcept;
	bool operator!=(const hash_collection &rhs) const noexcept { return !(*this == rhs); }

	// getters
	bool flag(char flag) const noexcept { return (m_flags.find_first_of(flag) != std::string::npos); }
	std::string hash_types() const;

	// hash manipulators
	void reset() noexcept;
	bool add_from_string(char type, std::string_view string) noexcept;
	bool remove(char type) noexcept;

	// CRC-specific helpers
	bool crc(uint32_t &result) const noexcept { result = m_crc32; return m_has_crc32; }
	void add_crc(uint32_t crc) noexcept { m_crc32 = crc; m_has_crc32 = true; }

	// SHA1-specific helpers
	bool sha1(sha1_t &result) const noexcept { result = m_sha1; return m_has_sha1; }
	void add_sha1(sha1_t sha1) noexcept { m_has_sha1 = true; m_sha1 = sha1; }

	// string conversion
	std::string internal_string() const;
	std::string macro_string() const;
	std::string attribute_string() const;
	bool from_internal_string(std::string_view string);

	// creation
	void compute(const uint8_t *data, uint32_t length, const char *types = nullptr);
	std::error_condition compute(random_read &stream, uint64_t offset, size_t length, size_t &actual, const char *types = nullptr);

private:
	// internal helpers
	std::unique_ptr<hash_creator> create(const char *types = nullptr);
	void copyfrom(const hash_collection &src);

	// internal state
	std::string             m_flags;
	bool                    m_has_crc32;
	crc32_t                 m_crc32;
	bool                    m_has_sha1;
	sha1_t                  m_sha1;
};


} // namespace util

#endif // MAME_UTIL_HASH_H
