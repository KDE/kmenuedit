/*
 * SPDX-FileCopyrightText: 2019 David Redondo <kde@david-redondo.de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <QAction>
#include <QCoreApplication>
#include <QDBusInterface>

#include <KConfig>
#include <KConfigGroup>
#include <KGlobalAccel>

#include "../globalaccel.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QDBusInterface khotkeys(QStringLiteral("org.kde.kded5"), QStringLiteral("/modules/khotkeys"), QStringLiteral("org.kde.khotkeys"));
    khotkeys.call(QStringLiteral("declareConfigOutDated"));
    KConfig khotkeysrc(QStringLiteral("khotkeysrc"), KConfig::SimpleConfig);
    const int dataCount = KConfigGroup(&khotkeysrc, QStringLiteral("Data")).readEntry("DataCount", 0);
    bool foundKmenuedit = false;
    int kmenueditIndex;
    KConfigGroup kmenueditGroup;
    for (int i = 1; i <= dataCount; ++i) {
        kmenueditGroup = KConfigGroup(&khotkeysrc, QStringLiteral("Data_%1").arg(i));
        if (kmenueditGroup.readEntry("Name", QString()) == QLatin1String("KMenuEdit")) {
            foundKmenuedit = true;
            kmenueditIndex = i;
            break;
        }
    }
    if (!foundKmenuedit) {
        return 0;
    }
    const int numShortcuts = kmenueditGroup.readEntry("DataCount", 0);
    for (int i = 1; i <= numShortcuts; ++i) {
        const QString groupName = QStringLiteral("Data_%1_%2").arg(kmenueditIndex).arg(i);
        // only migrate the launch actions for now, not the default search action
        if (KConfigGroup(&khotkeysrc, groupName).readEntry("Type") != QLatin1String("MENUENTRY_SHORTCUT_ACTION_DATA")) {
            continue;
        }
        const QString storageId = KConfigGroup(&khotkeysrc, groupName + QStringLiteral("Actions0")).readEntry("CommandURL");
        const QString id = KConfigGroup(&khotkeysrc, groupName + QStringLiteral("Triggers0")).readEntry("Uuid");
        // ask globalaccel about the current shortcut rather than parsing it ourselves
        const QList<QKeySequence> shortcut = KGlobalAccel::self()->globalShortcut(QStringLiteral("khotkeys"), id);
        QAction action;
        action.setObjectName(id);
        action.setProperty("componentName", QStringLiteral("khotkeys"));
        KGlobalAccel::self()->setShortcut(&action, {});
        KGlobalAccel::self()->removeAllShortcuts(&action);
        if (!shortcut.isEmpty() && !shortcut[0].isEmpty()) {
            GlobalAccel::changeMenuEntryShortcut(KService::serviceByStorageId(storageId), shortcut[0]);
        }
        khotkeysrc.deleteGroup(groupName);
        khotkeysrc.deleteGroup(groupName + QStringLiteral("Actions"));
        khotkeysrc.deleteGroup(groupName + QStringLiteral("Actions0"));
        khotkeysrc.deleteGroup(groupName + QStringLiteral("Conditions"));
        khotkeysrc.deleteGroup(groupName + QStringLiteral("Triggers"));
        khotkeysrc.deleteGroup(groupName + QStringLiteral("Triggers0"));
    }
    khotkeysrc.sync();
    khotkeys.call(QStringLiteral("reread_configuration"));
}
