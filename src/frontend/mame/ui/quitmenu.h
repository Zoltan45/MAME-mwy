// license:BSD-3-Clause
// copyright-holders:Vas Crabb
/***************************************************************************

    ui/quitmenu.h

    Menus involved in quitting MAME.

***************************************************************************/
#ifndef MAME_FRONTEND_UI_QUITMENU_H
#define MAME_FRONTEND_UI_QUITMENU_H

#pragma once

#include "ui/menu.h"


namespace ui {

class menu_confirm_quit : public autopause_menu<>
{
public:
	menu_confirm_quit(mame_ui_manager &mui, render_container &container);
	virtual ~menu_confirm_quit();

private:
	virtual void populate() override;
	virtual bool handle(event const *ev) override;
};

} // namespace ui

#endif // MAME_FRONTEND_UI_QUITMENU_H
