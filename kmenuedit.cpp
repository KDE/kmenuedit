/*
 *   SPDX-FileCopyrightText: 2000 Matthias Elter <elter@kde.org>
 *   SPDX-FileCopyrightText: 2001-2002 Raffaele Sandrini <sandrini@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *
 */
#include "kmenuedit.h"

#include <QFrame>
#include <QMenuBar>
#include <QSplitter>
#include <QVBoxLayout>

#include <KActionCollection>
#include <KActionMenu>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardActions>
#include <KStandardShortcut>
#include <KToolBar>
#include <KTreeWidgetSearchLine>
#include <KXMLGUIFactory>
#include <QAction>
#include <QIcon>
#include <sonnet/configdialog.h>

#include "basictab.h"
#include "configurationmanager.h"
#include "kmenueditadaptor.h"
#include "preferencesdlg.h"
#include "treeview.h"

KMenuEdit::KMenuEdit()
    : KXmlGuiWindow(nullptr)
{
    // dbus
    (void)new KmenueditAdaptor(this);
    QDBusConnection::sessionBus().registerObject(QStringLiteral("/KMenuEdit"), this);

    m_showHidden = ConfigurationManager::getInstance()->hiddenEntriesVisible();

    // setup GUI
    setupActions();
    slotChangeView();
}

KMenuEdit::~KMenuEdit()
{
    ConfigurationManager::getInstance()->setSplitterSizes(m_splitter->sizes());
}

void KMenuEdit::setupActions()
{
    // hamburger & menubar
    const bool menuBarInitiallyVisible = ConfigurationManager::getInstance()->menuBarVisible();
    menuBar()->setVisible(menuBarInitiallyVisible);

    auto showMenuBarAction = KStandardAction::showMenubar(this, nullptr, actionCollection());
    showMenuBarAction->setChecked(menuBarInitiallyVisible);
    connect(showMenuBarAction, &QAction::toggled, this, [this, &showMenuBarAction]() {
        const bool newVisibility = !menuBar()->isVisible();

        if (!newVisibility && toolBar()->isHidden()) {
            const QString accel = showMenuBarAction->shortcut().toString(QKeySequence::NativeText);
            KMessageBox::information(this,
                                     i18n("This will hide the menu bar completely."
                                          " You can show it again by typing %1.",
                                          accel),
                                     i18n("Hide menu bar"),
                                     QStringLiteral("HideMenuBarWarning"));
        }

        menuBar()->setVisible(newVisibility);
        ConfigurationManager::getInstance()->setMenuBarVisible(newVisibility);
    });

    auto hamburgerMenu = KStandardAction::hamburgerMenu(nullptr, nullptr, actionCollection());
    hamburgerMenu->setMenuBar(menuBar());
    hamburgerMenu->setMenuBarAdvertised(true);
    hamburgerMenu->setShowMenuBarAction(showMenuBarAction);

    connect(hamburgerMenu, &KHamburgerMenu::aboutToShowMenu, this, [this]() {
        auto hamburgerMenu = actionCollection()->action(QStringLiteral("hamburger_menu"));

        auto menu = hamburgerMenu->menu();
        if (!menu) {
            menu = new QMenu(this);
            hamburgerMenu->setMenu(menu);
        } else {
            menu->clear();
        }

        menu->addAction(actionCollection()->action(NEW_ACTION_NAME));
        menu->addAction(actionCollection()->action(SAVE_ACTION_NAME));
        menu->addSeparator();
        menu->addAction(actionCollection()->action(MOVE_UP_ACTION_NAME));
        menu->addAction(actionCollection()->action(MOVE_DOWN_ACTION_NAME));
        menu->addAction(actionCollection()->action(CUT_ACTION_NAME));
        menu->addAction(actionCollection()->action(COPY_ACTION_NAME));
        menu->addAction(actionCollection()->action(COPY_FILEPATH_ACTION_NAME));
        menu->addAction(actionCollection()->action(PASTE_ACTION_NAME));
        menu->addAction(actionCollection()->action(SORT_ACTION_NAME));
        menu->addAction(actionCollection()->action(OPEN_CONTAINING_FOLDER_ACTION_NAME));
        menu->addAction(actionCollection()->action(PROPERTIES_ACTION_NAME));
        menu->addSeparator();
        menu->addAction(actionCollection()->action(QStringLiteral("restore_system_menu")));
        menu->addSeparator();

        for (KToolBar *toolbar : toolBars()) {
            static_cast<KHamburgerMenu *>(hamburgerMenu)->hideActionsOf(toolbar);
        }
    });

    // File menu
    QAction *action = actionCollection()->addAction(NEW_SUBMENU_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("menu_new")));
    action->setText(i18n("&New Submenu..."));
    actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N));
    action = actionCollection()->addAction(NEW_ITEM_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    action->setText(i18n("New &Item..."));
    actionCollection()->setDefaultShortcuts(action, KStandardShortcut::openNew());
    action = actionCollection()->addAction(NEW_SEPARATOR_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("menu_new_sep")));
    action->setText(i18n("New S&eparator"));
    actionCollection()->setDefaultShortcut(action, QKeySequence(Qt::CTRL | Qt::Key_I));

    // File actions menu
    action = actionCollection()->addAction(COPY_FILEPATH_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("edit-copy-path")));
    action->setText(i18nc("@action:inmenu", "Copy Location"));
    action = actionCollection()->addAction(OPEN_CONTAINING_FOLDER_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("document-open-folder")));
    action->setText(i18nc("@action:inmenu", "&Open Containing Folder"));
    action = actionCollection()->addAction(PROPERTIES_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("document-properties")));
    action->setText(i18nc("@action:inmenu", "Properties"));
    actionCollection()->setDefaultShortcuts(action, {Qt::ALT | Qt::Key_Return, Qt::ALT | Qt::Key_Enter});

    // "new" menu
    KActionMenu *newMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("list-add-symbolic")), i18n("&New"), this);
    newMenu->setPopupMode(QToolButton::InstantPopup);
    newMenu->addAction(actionCollection()->action(NEW_ITEM_ACTION_NAME));
    newMenu->addAction(actionCollection()->action(NEW_SUBMENU_ACTION_NAME));
    newMenu->addAction(actionCollection()->action(NEW_SEPARATOR_ACTION_NAME));
    actionCollection()->addAction(NEW_ACTION_NAME, newMenu);

    // "sort selection" menu
    KActionMenu *sortMenu = new KActionMenu(QIcon::fromTheme(QStringLiteral("view-sort-ascending")), i18n("&Sort"), this);
    sortMenu->setPopupMode(QToolButton::InstantPopup);
    actionCollection()->addAction(SORT_ACTION_NAME, sortMenu);
    action = actionCollection()->addAction(SORT_BY_NAME_ACTION_NAME);
    action->setText(i18n("&Sort Selection by Name"));
    sortMenu->addAction(action);
    action = actionCollection()->addAction(SORT_BY_DESCRIPTION_ACTION_NAME);
    action->setText(i18n("&Sort Selection by Description"));
    sortMenu->addAction(action);
    sortMenu->addSeparator();
    action = actionCollection()->addAction(SORT_ALL_BY_NAME_ACTION_NAME);
    action->setText(i18n("&Sort All by Name"));
    sortMenu->addAction(action);
    action = actionCollection()->addAction(SORT_ALL_BY_DESCRIPTION_ACTION_NAME);
    action->setText(i18n("&Sort All by Description"));
    sortMenu->addAction(action);

    // move up/down
    action = actionCollection()->addAction(MOVE_UP_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("go-up")));
    action->setText(i18n("Move &Up"));
    action = actionCollection()->addAction(MOVE_DOWN_ACTION_NAME);
    action->setIcon(QIcon::fromTheme(QStringLiteral("go-down")));
    action->setText(i18n("Move &Down"));

    actionCollection()->addAction(KStandardActions::Save, this, &KMenuEdit::slotSave);
    actionCollection()->addAction(KStandardActions::Quit, this, &KMenuEdit::close);
    actionCollection()->addAction(KStandardActions::Cut);
    actionCollection()->addAction(KStandardActions::Copy);
    actionCollection()->addAction(KStandardActions::Paste);

    action = new QAction(i18n("Restore to System Menu"), this);
    action->setIcon(QIcon::fromTheme(QStringLiteral("edit-reset-symbolic")));
    actionCollection()->addAction(QStringLiteral("restore_system_menu"), action);
    connect(action, &QAction::triggered, this, &KMenuEdit::slotRestoreMenu);

    KStandardActions::preferences(this, &KMenuEdit::slotConfigure, actionCollection());
}

void KMenuEdit::slotConfigure()
{
    PreferencesDialog dialog(this);
    if (dialog.exec()) {
        bool newShowHiddenValue = ConfigurationManager::getInstance()->hiddenEntriesVisible();
        if (newShowHiddenValue != m_showHidden) {
            m_showHidden = newShowHiddenValue;
            m_tree->updateTreeView(m_showHidden);
            m_basicTab->updateHiddenEntry(m_showHidden);
        }
    }
}

void KMenuEdit::setupView()
{
    // setup search and tree view
    m_tree = new TreeView(actionCollection(), this);

    auto wrapper = new QWidget(this);
    wrapper->setContentsMargins(wrapper->style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
                                wrapper->style()->pixelMetric(QStyle::PM_LayoutTopMargin),
                                wrapper->style()->pixelMetric(QStyle::PM_LayoutRightMargin),
                                wrapper->style()->pixelMetric(QStyle::PM_LayoutBottomMargin));
    auto wrapperLayout = new QVBoxLayout(wrapper);
    wrapperLayout->setContentsMargins({});

    m_searchLine = new KTreeWidgetSearchLine(wrapper, m_tree);
    m_searchLine->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    m_searchLine->setKeepParentsVisible(true);
    m_searchLine->setPlaceholderText(i18n("Search..."));
    m_searchLine->setToolTip(i18n("Search through the list of applications below"));

    wrapperLayout->addWidget(m_searchLine);

    auto horizontalSeparator = new QFrame(this);
    horizontalSeparator->setFrameStyle(QFrame::HLine);

    QVBoxLayout *treeLayout = new QVBoxLayout;
    treeLayout->addWidget(wrapper);
    treeLayout->addWidget(horizontalSeparator);
    treeLayout->addWidget(m_tree);
    treeLayout->setSpacing(0);
    treeLayout->setContentsMargins(0, 0, 0, 0); // no padding, fixes alignment issues
    QFrame *treeFrame = new QFrame; // required to insert tree + search into splitter
    treeFrame->setLayout(treeLayout);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Horizontal);
    m_splitter->addWidget(treeFrame);

    // setup info tab view
    m_basicTab = new BasicTab;
    m_splitter->addWidget(m_basicTab);

    // add padding to splitter
    m_splitter->setContentsMargins(0, 0, 0, 0);

    // clang-format off
    connect(m_tree, SIGNAL(entrySelected(MenuFolderInfo*)), m_basicTab, SLOT(setFolderInfo(MenuFolderInfo*)));
    connect(m_tree, SIGNAL(entrySelected(MenuEntryInfo*)), m_basicTab, SLOT(setEntryInfo(MenuEntryInfo*)));
    connect(m_tree, &TreeView::disableAction, m_basicTab, &BasicTab::slotDisableAction);

    connect(m_basicTab, SIGNAL(changed(MenuFolderInfo*)), m_tree, SLOT(currentDataChanged(MenuFolderInfo*)));
    connect(m_basicTab, SIGNAL(changed(MenuEntryInfo*)), m_tree, SLOT(currentDataChanged(MenuEntryInfo*)));
    // clang-format on

    connect(m_basicTab, &BasicTab::findServiceShortcut, m_tree, &TreeView::findServiceShortcut);

    connect(m_searchLine, &KTreeWidgetSearchLine::searchUpdated, m_tree, &TreeView::searchUpdated);
    // restore splitter sizes
    QList<int> sizes = ConfigurationManager::getInstance()->getSplitterSizes();
    if (sizes.isEmpty()) {
        sizes << 1 << 3;
    }
    m_splitter->setSizes(sizes);
    m_searchLine->setFocus();

    setCentralWidget(m_splitter);
}

void KMenuEdit::selectMenu(const QString &menu)
{
    m_tree->selectMenu(menu);
}

void KMenuEdit::selectMenuEntry(const QString &menuEntry)
{
    m_tree->selectMenuEntry(menuEntry);
}

void KMenuEdit::slotChangeView()
{
    guiFactory()->removeClient(this);

    delete m_actionDelete;

    m_actionDelete = actionCollection()->addAction(DELETE_ACTION_NAME);
    m_actionDelete->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));
    m_actionDelete->setText(i18n("&Delete"));
    actionCollection()->setDefaultShortcut(m_actionDelete, QKeySequence(Qt::Key_Delete));

    if (!m_splitter) {
        setupView();
    }
    setupGUI(KXmlGuiWindow::ToolBar | Keys | Save | Create, QStringLiteral("kmenueditui.rc"));

    m_tree->setViewMode(m_showHidden);
    m_basicTab->updateHiddenEntry(m_showHidden);
}

void KMenuEdit::slotSave()
{
    m_tree->save();
}

bool KMenuEdit::queryClose()
{
    if (!m_tree->dirty()) {
        return true;
    }

    int result;
    result = KMessageBox::warningTwoActionsCancel(this,
                                                  i18n("You have made changes to the menu.\n"
                                                       "Do you want to save the changes or discard them?"),
                                                  i18n("Save Menu Changes?"),
                                                  KStandardGuiItem::save(),
                                                  KStandardGuiItem::discard());

    switch (result) {
    case KMessageBox::PrimaryAction:
        return m_tree->save();

    case KMessageBox::SecondaryAction:
        return true;

    default:
        break;
    }
    return false;
}

void KMenuEdit::slotRestoreMenu()
{
    m_tree->restoreMenuSystem();
}

void KMenuEdit::restoreSystemMenu()
{
    m_tree->restoreMenuSystem();
}
