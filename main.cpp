/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *   Copyright (C) 2001-2002 Raffaele Sandrini <sandrini@kde.org)
 *   Copyright (C) 2008 Montel Laurent <montel@kde.org)
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

#include <KAboutData>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>
#include <kdbusservice.h>

#include "kmenuedit.h"
#include <QApplication>
#include <QCommandLineParser>

static const char description[] = I18N_NOOP("KDE menu editor");

static KMenuEdit *menuEdit = nullptr;

class KMenuApplication : public QApplication
{
public:
    KMenuApplication(int &argc, char **argv)
        : QApplication(argc, argv)
    {
        QCoreApplication::setApplicationName(QStringLiteral("kmenuedit"));
        QCoreApplication::setApplicationVersion(QStringLiteral(PROJECT_VERSION));
        QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
        QApplication::setApplicationDisplayName(i18n("KDE Menu Editor"));
    }
};

int main(int argc, char **argv)
{
    KMenuApplication app(argc, argv);
    Kdelibs4ConfigMigrator migrate(QStringLiteral("kmenuedit"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("kmenueditrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("kmenueditui.rc"));
    migrate.migrate();

    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    KLocalizedString::setApplicationDomain("kmenuedit");

    KAboutData aboutData(QStringLiteral("kmenuedit"),
                         i18n("KDE Menu Editor"),
                         QStringLiteral(PROJECT_VERSION),
                         i18n(description),
                         KAboutLicense::GPL,
                         i18n("(C) 2000-2003, Waldo Bastian, Raffaele Sandrini, Matthias Elter"));
    aboutData.addAuthor(i18n("Waldo Bastian"), i18n("Maintainer"), QStringLiteral("bastian@kde.org"));
    aboutData.addAuthor(i18n("Raffaele Sandrini"), i18n("Previous Maintainer"), QStringLiteral("sandrini@kde.org"));
    aboutData.addAuthor(i18n("Matthias Elter"), i18n("Original Author"), QStringLiteral("elter@kde.org"));
    aboutData.addAuthor(i18n("Montel Laurent"), QString(), QStringLiteral("montel@kde.org"));
    KAboutData::setApplicationData(aboutData);

    KDBusService service(KDBusService::Unique);

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("KDE Menu Editor"));
    parser.addPositionalArgument(QStringLiteral("menu"), i18n("Sub menu to pre-select"), QStringLiteral("[menu]"));
    parser.addPositionalArgument(QStringLiteral("menu-id"), i18n("Menu entry to pre-select"), QStringLiteral("[menu-id]"));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    const auto args = parser.positionalArguments();

    menuEdit = new KMenuEdit();
    if (!args.isEmpty()) {
        menuEdit->selectMenu(args.at(0));
        if (args.count() > 1) {
            menuEdit->selectMenuEntry(args.at(1));
        }
    }
    menuEdit->show();

    return app.exec();
}
