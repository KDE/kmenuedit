/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <kglobal.h>
#include <kconfig.h>
#include <klocale.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kdebug.h>

#include "menueditview.h"
#include "kmenuedit.h"
#include "kmenuedit.moc"

KMenuEdit::KMenuEdit (QWidget *, const char *name)
  : KTMainWindow (name)
{
    setCaption(i18n("Edit K Menu"));

    // restore size
    KConfig *config = KGlobal::config();
    config->setGroup("General");
    int w = config->readNumEntry("Width",640);
    int h = config->readNumEntry("Height", 480);
    resize(w, h);

    // setup GUI
    setupView();
    setupActions();
}

KMenuEdit::~KMenuEdit()
{
    // save size
    KConfig *config = KGlobal::config();
    config->setGroup("General");
    config->writeEntry("Width", width());
    config->writeEntry("Height", height());
    config->sync();

    // clean up
    delete _view;
}

void KMenuEdit::setupActions()
{
    // setup file menu
    (void)new KAction(i18n("&New Submenu"), "filenew", 0 , _view, SLOT(file_newsubmenu()),
		      actionCollection(), "newsubmenu");

    (void)new KAction(i18n("New &Item"), "filenew", 0, _view, SLOT(file_newitem()),
		      actionCollection(), "newitem");

    KStdAction::quit(this, SLOT(close()), actionCollection());

    // setup edit menu
    KStdAction::cut(_view, SLOT(edit_cut()), actionCollection());
    KStdAction::copy(_view, SLOT(edit_copy()), actionCollection());
    KStdAction::paste(_view, SLOT(edit_paste()), actionCollection());

    (void)new KAction(i18n("&Delete"), "editdelete", 0, _view, SLOT(edit_delete()),
		      actionCollection(), "delete");

    (void)new KAction(i18n("Move &Up"), "up", 0, _view, SLOT(edit_moveup()),
		      actionCollection(), "moveup");

    (void)new KAction(i18n("Move &Down"), "down", 0, _view, SLOT(edit_movedown()),
		      actionCollection(), "movedown");

    createGUI("kmenueditui.rc");

    toolBar(0)->setIconText(KToolBar::IconTextBottom);
}

void KMenuEdit::setupView()
{
    _view = new MenuEditView(this);
    setView(_view);
}
