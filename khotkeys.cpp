/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *                      Lubos Lunak    <l.lunak@email.cz>
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

#include "khotkeys.h"
#include "khotkeys_interface.h"

#include <QDebug>
#include <KLocalizedString>
#include <KMessageBox>

#include <QDebug>

static bool khotkeys_present = false;
static bool khotkeys_inited = false;
static OrgKdeKhotkeysInterface *khotkeysInterface = nullptr;


bool KHotKeys::init()
{
    khotkeys_inited = true;

    // Check if khotkeys is running
    QDBusConnection bus = QDBusConnection::sessionBus();
    khotkeysInterface = new OrgKdeKhotkeysInterface(
        QStringLiteral("org.kde.kded5"),
        QStringLiteral("/modules/khotkeys"),
        bus,
        NULL);

    if(!khotkeysInterface->isValid()) {
        QDBusError err = khotkeysInterface->lastError();
        if (err.isValid()) {
            qCritical() << err.name() << ":" << err.message();
        }
        KMessageBox::error(
            NULL,
            QStringLiteral("<qt>") + i18n("Unable to contact khotkeys. Your changes are saved, but they could not be activated.") + QStringLiteral("</qt>") );
    }

    khotkeys_present = khotkeysInterface->isValid();

    qDebug() << khotkeys_present;
    return true;
}

void KHotKeys::cleanup()
{
    if( khotkeys_inited && khotkeys_present ) {
        // CleanUp ???
    }

    khotkeys_inited = false;
}

bool KHotKeys::present()
{
    qDebug() << khotkeys_inited;

    if( !khotkeys_inited )
        init();

    qDebug() << khotkeys_present;

    return khotkeys_present;
}

QString KHotKeys::getMenuEntryShortcut( const QString& entry_P )
{
    if( !khotkeys_inited )
        init();

    if( !khotkeys_present || !khotkeysInterface->isValid())
        return QLatin1String("");
qDebug() << khotkeys_inited;
    qDebug() << khotkeys_present;
    qDebug() << entry_P;
    QDBusReply<QString> reply = khotkeysInterface->get_menuentry_shortcut(entry_P);
    if (!reply.isValid()) {
        qCritical() << reply.error();
        return QLatin1String("");

    } else {
            qDebug() << reply;
        return reply;
    }
}

QString KHotKeys::changeMenuEntryShortcut(
        const QString& entry_P,
        const QString shortcut_P )
{
    if( !khotkeys_inited )
        init();

    if( !khotkeys_present || !khotkeysInterface->isValid())
        return QLatin1String("");

    qDebug() << khotkeys_inited;
    qDebug() << khotkeys_present;
    qDebug() << entry_P;
    qDebug() << shortcut_P;

    QDBusReply<QString> reply = khotkeysInterface->register_menuentry_shortcut(
            entry_P,
            shortcut_P);

    if (!reply.isValid()) {
        qCritical() << reply.error();
        return QLatin1String("");
    } else {
        qDebug() << reply;
        return reply;
    }
}

