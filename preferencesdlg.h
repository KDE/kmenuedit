/*
 *   Copyright (C) 2008 Laurent Montel <montel@kde.org>
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
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef PREFERENCESDLG_h
#define PREFERENCESDLG_h

#include <KPageDialog>

class SpellCheckingPage;
class MiscPage;
class QCheckBox;

namespace Sonnet
{
class ConfigWidget;
}

class PreferencesDialog : public KPageDialog
{
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget *parent);

protected Q_SLOTS:
    void slotSave();

private:
    SpellCheckingPage *m_pageSpellChecking = nullptr;
    MiscPage *m_pageMisc = nullptr;
};

class SpellCheckingPage : public QWidget
{
    Q_OBJECT
public:
    explicit SpellCheckingPage(QWidget *);
    void saveOptions();

private:
    Sonnet::ConfigWidget *m_confPage = nullptr;
};

class MiscPage : public QWidget
{
    Q_OBJECT
public:
    explicit MiscPage(QWidget *);
    void saveOptions();

private:
    QCheckBox *m_showHiddenEntries = nullptr;
};

#endif
