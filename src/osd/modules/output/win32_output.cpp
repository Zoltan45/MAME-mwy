// license:BSD-3-Clause
// copyright-holders:Aaron Giles
//============================================================
//
//  output.c - Win32 implementation of MAME output routines
//
//============================================================

#include "output_module.h"

#include "modules/osdmodule.h"

#if defined(OSD_WINDOWS)

#include "win32_output.h"

#include "winmain.h"
#include "winutil.h"

// MAME headers
#include "emu.h"

// standard windows headers
#include <windows.h>


namespace osd {

namespace {

//============================================================
//  CONSTANTS
//============================================================

// window styles
#define WINDOW_STYLE        WS_OVERLAPPEDWINDOW
#define WINDOW_STYLE_EX     0



//============================================================
//  TYPEDEFS
//============================================================

struct registered_client
{
	registered_client * next;       // next client in the list
	LPARAM              id;         // client-specified ID
	HWND                hwnd;       // client HWND
	running_machine   * machine;
};


// message IDs
static UINT                 om_mame_start;
static UINT                 om_mame_stop;
static UINT                 om_mame_update_state;
static UINT                 om_mame_register_client;
static UINT                 om_mame_unregister_client;
static UINT                 om_mame_get_id_string;

static UINT                 im_mame_message;

//============================================================
//  FUNCTION PROTOTYPES
//============================================================

static LRESULT CALLBACK output_window_proc(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam);

class output_win32 : public osd_module, public output_module
{
public:
	output_win32()
		: osd_module(OSD_OUTPUT_PROVIDER, "windows")
		, output_module()
		, m_output_hwnd(nullptr)
		, m_clientlist(nullptr)
		, m_machine(nullptr)
	{
	}
	virtual ~output_win32() { }

	virtual int init(osd_interface &osd, const osd_options &options) override;
	virtual void exit() override;

	// output_module
	virtual void notify(const char *outname, int32_t value) override;

	running_machine &machine() { return *m_machine; }

	LRESULT register_client(HWND hwnd, LPARAM id);
	LRESULT unregister_client(HWND hwnd, LPARAM id);
	LRESULT send_id_string(HWND hwnd, LPARAM id);

private:
	int create_window_class();
	// our HWND
	HWND                 m_output_hwnd;

	// client list
	registered_client *  m_clientlist;

	running_machine *    m_machine;
};


//============================================================
//  output_init
//============================================================

int output_win32::init(osd_interface &osd, const osd_options &options)
{
	m_machine = &downcast<osd_common_t &>(osd).machine();

	int result;

	// reset globals
	m_clientlist = nullptr;

	// create our window class
	result = create_window_class();
	assert(result == 0);
	(void)result; // to silence gcc 4.6

	// allocate message ids before creating the window
	// since the window proc gets called during creation
	om_mame_start = RegisterWindowMessage(OM_MAME_START);
	assert(om_mame_start != 0);
	om_mame_stop = RegisterWindowMessage(OM_MAME_STOP);
	assert(om_mame_stop != 0);
	om_mame_update_state = RegisterWindowMessage(OM_MAME_UPDATE_STATE);
	assert(om_mame_update_state != 0);

	om_mame_register_client = RegisterWindowMessage(OM_MAME_REGISTER_CLIENT);
	assert(om_mame_register_client != 0);
	om_mame_unregister_client = RegisterWindowMessage(OM_MAME_UNREGISTER_CLIENT);
	assert(om_mame_unregister_client != 0);
	om_mame_get_id_string = RegisterWindowMessage(OM_MAME_GET_ID_STRING);
	assert(om_mame_get_id_string != 0);
	im_mame_message = RegisterWindowMessage(IM_MAME_MESSAGE);
	assert(im_mame_message != 0);

	// create a window
	m_output_hwnd = CreateWindowEx(
						WINDOW_STYLE_EX,
						OUTPUT_WINDOW_CLASS,
						OUTPUT_WINDOW_NAME,
						WINDOW_STYLE,
						0, 0,
						1, 1,
						nullptr,
						nullptr,
						GetModuleHandleUni(),
						nullptr);
	assert(m_output_hwnd != nullptr);

	// set a pointer to the running machine
	SetWindowLongPtr(m_output_hwnd, GWLP_USERDATA, (LONG_PTR)this);

	// broadcast a startup message
	PostMessage(HWND_BROADCAST, om_mame_start, (WPARAM)m_output_hwnd, 0);

	return 0;
}


//============================================================
//  output_exit
//============================================================

void output_win32::exit()
{
	// free all the clients
	while (m_clientlist != nullptr)
	{
		registered_client *temp = m_clientlist;
		m_clientlist = temp->next;
		delete temp;
	}

	// broadcast a shutdown message
	PostMessage(HWND_BROADCAST, om_mame_stop, (WPARAM)m_output_hwnd, 0);
}


//============================================================
//  create_window_class
//============================================================

int output_win32::create_window_class()
{
	static bool classes_created = false;

	/* only do this once */
	if (!classes_created)
	{
		WNDCLASS wc = { 0 };

		// initialize the description of the window class
		wc.lpszClassName    = OUTPUT_WINDOW_CLASS;
		wc.hInstance        = GetModuleHandleUni();
		wc.lpfnWndProc      = output_window_proc;

		UnregisterClass(wc.lpszClassName, wc.hInstance);

		// register the class; fail if we can't
		if (!RegisterClass(&wc))
			return 1;
		classes_created = true;
	}

	return 0;
}


//============================================================
//  output_window_proc
//============================================================

static LRESULT CALLBACK output_window_proc(HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LONG_PTR ptr = GetWindowLongPtr(wnd, GWLP_USERDATA);
	output_win32 &output = *(output_win32 *)ptr;

	// register a new client
	if (message == om_mame_register_client)
		return output.register_client((HWND)wparam, lparam);

	// unregister a client
	else if (message == om_mame_unregister_client)
		return output.unregister_client((HWND)wparam, lparam);

	// get a string for an ID
	else if (message == om_mame_get_id_string)
		return output.send_id_string((HWND)wparam, lparam);

	// received a message
	else if (message == im_mame_message)
	{
		switch(wparam)
		{
		case output_module::IM_MAME_PAUSE:
			if (lparam == 1 && !output.machine().paused())
				output.machine().pause();
			else if (lparam == 0 && output.machine().paused())
				output.machine().resume();
			break;
		case output_module::IM_MAME_SAVESTATE:
			if (lparam == 0)
				output.machine().schedule_load("auto");
			else if (lparam == 1)
				output.machine().schedule_save("auto");
			break;
		}

		return 0;
	}

	else
		return DefWindowProc(wnd, message, wparam, lparam);
}


//============================================================
//  register_client
//============================================================

LRESULT output_win32::register_client(HWND hwnd, LPARAM id)
{
	registered_client **client;

	// find the end of the list; if we find ourself already registered,
	// return 1
	for (client = &m_clientlist; *client != nullptr; client = &(*client)->next)
		if ((*client)->id == id)
		{
			(*client)->hwnd = hwnd;
			machine().output().notify_all([this] (const char *outname, int32_t value) { notify(outname, value); });
			return 1;
		}

	// add us to the end
	*client = new registered_client;
	(*client)->next = nullptr;
	(*client)->id = id;
	(*client)->hwnd = hwnd;
	(*client)->machine = &machine();

	// request a notification for all outputs
	machine().output().notify_all([this] (const char *outname, int32_t value) { notify(outname, value); });
	return 0;
}


//============================================================
//  unregister_client
//============================================================

LRESULT output_win32::unregister_client(HWND hwnd, LPARAM id)
{
	registered_client **client;
	bool found = false;

	// find any matching IDs in the list and remove them
	for (client = &m_clientlist; *client != nullptr; client = &(*client)->next)
		if ((*client)->id == id)
		{
			registered_client *temp = *client;
			*client = (*client)->next;
			delete temp;
			found = true;
			break;
		}

	// return an error if not found
	return found ? 0 : 1;
}


//============================================================
//  send_id_string
//============================================================

LRESULT output_win32::send_id_string(HWND hwnd, LPARAM id)
{
	COPYDATASTRUCT copydata;
	const char *name;
	int datalen;

	// id 0 is the name of the game
	if (id == 0)
		name = machine().system().name;
	else
		name = machine().output().id_to_name(id);

	// a NULL name is an empty string
	if (name == nullptr)
		name = "";

	// allocate memory for the message
	datalen = sizeof(copydata_id_string) + strlen(name) + 1;
	std::vector<uint8_t> buffer(datalen);
	auto *temp = (copydata_id_string *)&buffer[0];
	temp->id = id;
	strcpy(temp->string, name);

	// reply by using SendMessage with WM_COPYDATA
	copydata.dwData = COPYDATA_MESSAGE_ID_STRING;
	copydata.cbData = datalen;
	copydata.lpData = temp;
	SendMessage(hwnd, WM_COPYDATA, (WPARAM)m_output_hwnd, (LPARAM)&copydata);

	return 0;
}


//============================================================
//  notifier_callback
//============================================================

void output_win32::notify(const char *outname, int32_t value)
{
	// loop over clients and notify them
	for (registered_client *client = m_clientlist; client != nullptr; client = client->next)
		PostMessage(client->hwnd, om_mame_update_state, client->machine->output().name_to_id(outname), value);
}

} // anonymous namespace

} // namespace osd


#else

namespace osd { namespace { MODULE_NOT_SUPPORTED(output_win32, OSD_OUTPUT_PROVIDER, "windows") } }

#endif

MODULE_DEFINITION(OUTPUT_WIN32, osd::output_win32)
