/*
 *   Copyright (C) 2000 Matthias Elter <elter@kde.org>
 *   Copyright (C) 2001-2002 Raffaele Sandrini <sandrini@kde.org>
 *   Copyright (C) 2003 Waldo Bastian <bastian@kde.org>
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
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef __treeview_h__
#define __treeview_h__

#include <qstring.h>
#include <klistview.h>
#include <kservice.h>
#include <kservicegroup.h>

class QPopupMenu;
class KActionCollection;
class KDesktopFile;
class MenuFile;
class MenuFolderInfo;
class MenuEntryInfo;

class TreeItem : public QListViewItem
{
public:
  TreeItem(QListViewItem *parent, QListViewItem *after, const QString &menuIdn, bool __init = false);
    TreeItem(QListView *parent, QListViewItem* after, const QString &menuId, bool __init = false);

    QString menuId() const { return _menuId; }

    QString directory() const { return _directoryPath; }
    void setDirectoryPath(const QString& path) { _directoryPath = path; }

    MenuFolderInfo *folderInfo() { return m_folderInfo; }
    void setMenuFolderInfo(MenuFolderInfo *folderInfo) { m_folderInfo = folderInfo; }

    MenuEntryInfo *entryInfo() { return m_entryInfo; }
    void setMenuEntryInfo(MenuEntryInfo *entryInfo) { m_entryInfo = entryInfo; }

    QString name() const { return _name; }
    void setName(const QString &name);

    bool isDirectory() const { return m_folderInfo; }

    bool isHidden() const { return _hidden; }
    void setHidden(bool b);

    virtual void setOpen(bool o);
    void load();

private:
    void update();

    bool _hidden : 1;
    bool _init : 1;
    QString _menuId;
    QString _name;
    QString _directoryPath;
    MenuFolderInfo *m_folderInfo;
    MenuEntryInfo *m_entryInfo;
};

class TreeView : public KListView
{
    friend class TreeItem;
    Q_OBJECT
public:
    TreeView(bool controlCenter, KActionCollection *ac, QWidget *parent=0, const char *name=0);
    ~TreeView();

    void readMenuFolderInfo(MenuFolderInfo *folderInfo=0, KServiceGroup::Ptr folder=0, const QString &prefix=QString::null);
    void setViewMode(bool showHidden);
    void save();

    bool dirty();

public slots:
    void currentChanged(MenuFolderInfo *folderInfo);
    void currentChanged(MenuEntryInfo *entryInfo);
    void findServiceShortcut(const KShortcut&, KService::Ptr &);

signals:
    void entrySelected(MenuFolderInfo *folderInfo);
    void entrySelected(MenuEntryInfo *entryInfo);
    void disableAction();
protected slots:
    void itemSelected(QListViewItem *);
    void slotDropped(QDropEvent *, QListViewItem *, QListViewItem *);
    void slotRMBPressed(QListViewItem*, const QPoint&);

    void newsubmenu();
    void newitem();

    void cut();
    void copy();
    void paste();
    void del();

protected:
    TreeItem *createTreeItem(TreeItem *parent, QListViewItem *after, MenuFolderInfo *folderInfo, bool _init = false);
    TreeItem *createTreeItem(TreeItem *parent, QListViewItem *after, MenuEntryInfo *entryInfo, bool _init = false);

    void del(TreeItem *, bool deleteInfo);
    void fill();
    void fillBranch(MenuFolderInfo *folderInfo, TreeItem *parent);
    QString findName(KDesktopFile *df, bool deleted);

    // moving = src will be removed later
    void copy( bool moving );

    void cleanupClipboard();

    QStringList fileList(const QString& relativePath);
    QStringList dirList(const QString& relativePath);

    virtual bool acceptDrag(QDropEvent* event) const;
    virtual QDragObject *dragObject();
    virtual void startDrag();

private:
    KActionCollection *m_ac;
    QPopupMenu        *m_rmb;
    int                m_clipboard;
    MenuFolderInfo    *m_clipboardFolderInfo;
    MenuEntryInfo     *m_clipboardEntryInfo;
    int                m_drag;
    MenuFolderInfo    *m_dragInfo;
    TreeItem          *m_dragItem;
    bool               m_showHidden;
    bool               m_controlCenter;
    MenuFile          *m_menuFile;
    MenuFolderInfo    *m_rootFolder;
    QStringList        m_newMenuIds;
    QStringList        m_newDirectoryList;
};


#endif
