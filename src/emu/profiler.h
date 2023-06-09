// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    profiler.h

    Functions to manage profiling of MAME execution.

****************************************************************************

    Profiling is scope-based. To start profiling, put a profiler scope
    object on the stack. To end profiling, just end the scope:

    {
        auto scope = g_profiler.start(PROFILER_VIDEO);

        your_work_here();
    }

    the profiler handles a FILO list so calls may be nested.

***************************************************************************/

#ifndef MAME_EMU_PROFILER_H
#define MAME_EMU_PROFILER_H

#pragma once

#include "emufwd.h"

#include "attotime.h"

#include "eminline.h" // for get_profile_ticks()
#include "osdcore.h"

#include <cstdio>
#include <cstdlib>
#include <string>


//**************************************************************************
//  CONSTANTS
//**************************************************************************

enum profile_type
{
	PROFILER_DEVICE_FIRST = 0,
	PROFILER_DEVICE_MAX = PROFILER_DEVICE_FIRST + 256,
	PROFILER_DRC_COMPILE,
	PROFILER_MEM_REMAP,
	PROFILER_MEMREAD,
	PROFILER_MEMWRITE,
	PROFILER_VIDEO,
	PROFILER_DRAWGFX,
	PROFILER_COPYBITMAP,
	PROFILER_TILEMAP_DRAW,
	PROFILER_TILEMAP_DRAW_ROZ,
	PROFILER_TILEMAP_UPDATE,
	PROFILER_BLIT,
	PROFILER_SOUND,
	PROFILER_TIMER_CALLBACK,
	PROFILER_INPUT,             // input.cpp and inptport.cpp
	PROFILER_MOVIE_REC,         // movie recording
	PROFILER_LOGERROR,          // logerror
	PROFILER_LUA,               // LUA
	PROFILER_EXTRA,             // everything else

	// the USER types are available to driver writers to profile
	// custom sections of the code
	PROFILER_USER1,
	PROFILER_USER2,
	PROFILER_USER3,
	PROFILER_USER4,
	PROFILER_USER5,
	PROFILER_USER6,
	PROFILER_USER7,
	PROFILER_USER8,

	PROFILER_PROFILER,
	PROFILER_IDLE,
	PROFILER_TOTAL
};
DECLARE_ENUM_INCDEC_OPERATORS(profile_type)



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


// ======================> real_profiler_state

class real_profiler_state
{
public:
	class scope
	{
	private:
		real_profiler_state &m_host;
		bool m_active;

	public:
		scope(scope const &) = delete;
		scope &operator=(scope const &) = delete;

		scope(scope &&that) noexcept : m_host(that.m_host), m_active(that.m_active)
		{
			that.m_active = false;
		}

		scope(real_profiler_state &host, profile_type type) noexcept : m_host(host), m_active(false)
		{
			if (m_host.enabled())
			{
				m_host.real_start(type);
				m_active = true;
			}
		}

		~scope()
		{
			stop();
		}

		void stop() noexcept
		{
			if (m_active)
			{
				m_host.real_stop();
				m_active = false;
			}
		}
	};

	// construction/destruction
	real_profiler_state();

	// getters
	bool enabled() const noexcept
	{
		return m_filoptr != nullptr;
	}
	const char *text(running_machine &machine);

	// enable/disable
	void enable(bool state = true) noexcept
	{
		if (state != enabled())
		{
			reset(state);
		}
	}

	// start/stop
	[[nodiscard]] auto start(profile_type type) noexcept { return scope(*this, type); }

private:
	// an entry in the FILO
	struct filo_entry
	{
		int             type;                       // type of entry
		osd_ticks_t     start;                      // start time
	};

	void reset(bool enabled) noexcept;
	void update_text(running_machine &machine);

	//-------------------------------------------------
	//  real_start - mark the beginning of a
	//  profiler entry
	//-------------------------------------------------
	ATTR_FORCE_INLINE void real_start(profile_type type) noexcept
	{
		// fail if we overflow
		if (UNEXPECTED(m_filoptr >= &m_filo[std::size(m_filo) - 1]))
		{
			std::fprintf(stderr, "Profiler FILO overflow (type = %d)\n", int(type));
			std::abort();
		}

		// get current tick count
		osd_ticks_t curticks = get_profile_ticks();

		// update previous entry
		m_data[m_filoptr->type] += curticks - m_filoptr->start;

		// move to next entry
		m_filoptr++;

		// fill in this entry
		m_filoptr->type = type;
		m_filoptr->start = curticks;
	}

	//-------------------------------------------------
	//  real_stop - mark the end of a profiler entry
	//-------------------------------------------------
	ATTR_FORCE_INLINE void real_stop() noexcept
	{
		// degenerate scenario
		if (UNEXPECTED(m_filoptr <= m_filo))
			return;

		// get current tick count
		osd_ticks_t curticks = get_profile_ticks();

		// account for the time taken
		m_data[m_filoptr->type] += curticks - m_filoptr->start;

		// move back an entry
		m_filoptr--;

		// reset previous entry start time
		m_filoptr->start = curticks;
	}

	// internal state
	filo_entry *        m_filoptr;                  // current FILO index
	std::string         m_text;                     // profiler text
	attotime            m_text_time;                // profiler text last update
	filo_entry          m_filo[32];                 // array of FILO entries
	osd_ticks_t         m_data[PROFILER_TOTAL + 1]; // array of data
};


// ======================> dummy_profiler_state

class dummy_profiler_state
{
public:
	class scope
	{
	private:
		dummy_profiler_state &m_host;

	public:
		scope(scope const &) = delete;
		scope &operator=(scope const &) = delete;
		scope(scope &&that) noexcept = default;
		scope(dummy_profiler_state &host, profile_type type) noexcept : m_host(host) { }
		~scope() { m_host.real_stop(); }
		void stop() noexcept { }
	};

	// construction/destruction
	dummy_profiler_state();

	// getters
	bool enabled() const noexcept { return false; }
	const char *text(running_machine &machine) { return ""; }

	// enable/disable
	void enable(bool state = true) noexcept { }

	// start/stop
	[[nodiscard]] auto start(profile_type type) noexcept { return scope(*this, type); }

private:
	void real_stop() noexcept { }
};


// ======================> profiler_state

#ifdef MAME_PROFILER
typedef real_profiler_state profiler_state;
#else
typedef dummy_profiler_state profiler_state;
#endif



//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

extern profiler_state g_profiler;


#endif // MAME_EMU_PROFILER_H
