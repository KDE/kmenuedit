/*
 *   SPDX-FileCopyrightText: 2000 Matthias Elter <elter@kde.org>
 *   SPDX-FileCopyrightText: 2001-2002 Raffaele Sandrini <sandrini@kde.org>
 *   SPDX-FileCopyrightText: 2008 Montel Laurent <montel@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <KWindowSystem>
#include <kdbusservice.h>

#include "kmenuedit.h"
#include <QApplication>
#include <QCommandLineParser>

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
    KLocalizedString::setApplicationDomain(QByteArrayLiteral("kmenuedit"));

    KAboutData aboutData(QStringLiteral("kmenuedit"),
                         i18n("KDE Menu Editor"),
                         QStringLiteral(PROJECT_VERSION),
                         i18n("KDE menu editor"),
                         KAboutLicense::GPL,
                         i18n("(C) 2000-2003, Waldo Bastian, Raffaele Sandrini, Matthias Elter"));
    aboutData.addAuthor(i18n("Waldo Bastian"), i18n("Maintainer"), QStringLiteral("bastian@kde.org"));
    aboutData.addAuthor(i18n("Raffaele Sandrini"), i18n("Previous Maintainer"), QStringLiteral("sandrini@kde.org"));
    aboutData.addAuthor(i18n("Matthias Elter"), i18n("Original Author"), QStringLiteral("elter@kde.org"));
    aboutData.addAuthor(i18n("Montel Laurent"), QString(), QStringLiteral("montel@kde.org"));
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();
    KDBusService service(KDBusService::Unique);

    QCommandLineParser parser;
    parser.setApplicationDescription(i18n("KDE Menu Editor"));
    parser.addPositionalArgument(
        QStringLiteral("entry"),
        i18n("Entry and/or menu to select, e.g. \"org.kde.dolphin.desktop\", \"Utilties/org.kde.kwrite.desktop\", \"Education/Mathematics\""),
        QStringLiteral("[entry]"));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kmenuedit")));
    auto *menuEdit = new KMenuEdit();

    auto useArgs = [menuEdit](const QCommandLineParser &parser) {
        const QStringList args = parser.positionalArguments();
        if (!args.isEmpty()) {
            const QString &arg = args.first();

            if (!arg.endsWith(QStringLiteral(".desktop"))) {
                // No entry, so this must be a menu
                menuEdit->selectMenu(arg);
            } else {
                // Entry, might include menu
                int index = arg.lastIndexOf(QStringLiteral("/"));

                if (index != -1) {
                    // This entry has a menu, so select it
                    menuEdit->selectMenu(arg.first(index));
                } else {
                    // No menu, so select root
                    menuEdit->selectMenu(QStringLiteral("/"));
                }

                // Select the entry
                menuEdit->selectMenuEntry(arg.sliced(index + 1));
            }
        }
    };

    useArgs(parser);

    QObject::connect(&service, &KDBusService::activateRequested, menuEdit, [useArgs, &parser, menuEdit](const QStringList &args, const QString & /*workDir*/) {
        parser.parse(args);
        useArgs(parser);

        KWindowSystem::updateStartupId(menuEdit->windowHandle());
        KWindowSystem::activateWindow(menuEdit->windowHandle());
    });

    menuEdit->show();

    return app.exec();
}
