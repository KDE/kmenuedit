/*
 * Copyright (C) 2019  David Redondo <kde@david-redondo.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "globalaccel.h"

#include <QAction>
#include <QFile>
#include <QStandardPaths>

#include <KGlobalAccel>
#include <KLocalizedString>
#include <KService>

void GlobalAccel::changeMenuEntryShortcut(const KService::Ptr &service, const QKeySequence &shortcut)
{
    const QString desktopFile = QStringLiteral("%1.desktop").arg(service->desktopEntryName());

    if (!KGlobalAccel::isComponentActive(desktopFile)) {
        const QString destination = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation)
            + QStringLiteral("/kglobalaccel/") + desktopFile;
        QFile::copy(service->entryPath(), destination);
    }
    QAction action(i18n("Launch %1", service->name()));
    action.setProperty("componentName", desktopFile);
    action.setProperty("componentDisplayName", service->name());
    action.setObjectName(QStringLiteral("_launch"));
    //Make sure that the action is marked active
    KGlobalAccel::self()->setShortcut(&action, {shortcut});
    action.setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setShortcut(&action, {shortcut}, KGlobalAccel::NoAutoloading);

}

QKeySequence GlobalAccel::getMenuEntryShortcut(const KService::Ptr &service)
{
    const QString desktopFile = QStringLiteral("%1.desktop").arg(service->desktopEntryName());
    const QList<QKeySequence> shortcut = KGlobalAccel::self()->globalShortcut(desktopFile, QStringLiteral("_launch"));
    if (!shortcut.isEmpty()) {
        return shortcut[0];
    }
    return QKeySequence();
}


