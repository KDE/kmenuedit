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

#ifndef __treeview_h__
#define __treeview_h__

#include <klistview.h>

class TreeItem : public QListViewItem
{
  
public:
  TreeItem(QListViewItem *parent, const QString& file);
  TreeItem(QListView *parent, const QString& file);

  QString file() const { return _file; };

private:
  QString _file;
};

class TreeView : public KListView
{
  Q_OBJECT;

 public:
  TreeView(QWidget *parent=0, const char *name=0);

 public slots:
  void slotCurrentChanged(); 
  void slotDeleteCurrent();

 signals:
  void entrySelected(const QString&);

 protected slots:
  void itemSelected(QListViewItem * item);

 protected:
  void fill();
  void fillBranch(const QString& relPath, TreeItem* parent);

  void deleteFile(const QString& deskfile);
  void deleteDir(const QString& dir);

  QStringList fileList(const QString& relativePath);
  QStringList dirList(const QString& relativePath);
};

#endif
