// -*- C++ -*-

//
//  kmenuedit
//
//  Copyright (C) 1997 Christoph Neerfeld
//  email:  Christoph.Neerfeld@home.ivm.de or chris@kde.org
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <qdatastream.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qfont.h>
#include <qdir.h>
#include <qpixmap.h>
#include <qfile.h>

#include <kapp.h>
#include <kiconloader.h>

#include "pmenu.h"
#include "confmenu.h"

#include "pmenu.moc"

extern "C" {
#include <stdlib.h>
}

extern KIconLoader *global_pix_loader;

static bool isKdelnkFile(const char* name){
  QFile file(name);
  if (file.open(IO_ReadOnly)){
    char s[19];
    int r = file.readLine(s, 18);
    if(r > -1){
      s[r] = '\0';
      file.close();
      return (QString(s).left(17) == "# KDE Config File");
    }
    file.close();
  }
  return FALSE;
}


PMenuItem::PMenuItem() 
  : url_name(command_name), dev_name(command_name), mount_point(term_opt), fs_type(exec_path),
    dev_read_only(use_term), umount_pixmap_name(pattern), swallow_title(term_opt)
{
  initMetaObject();
  entry_type = empty; 
  sub_menu = NULL;
  cmenu = NULL;
  recv = NULL;
  memb = NULL;
  read_only = FALSE;
}

PMenuItem::PMenuItem( EntryType e, QString t, QString c, QString n, PMenu *menu,
		      QObject *receiver, char *member, CPopupMenu *cm, bool ro )
  : url_name(command_name), dev_name(command_name), mount_point(term_opt), fs_type(exec_path),
    dev_read_only(use_term), umount_pixmap_name(pattern), swallow_title(term_opt)
{
  initMetaObject();
  entry_type = e;
  text_name = t.copy();
  real_name = t.copy();
  command_name = c;
  pixmap_name = n;
  if( !pixmap_name.isEmpty() )
    {
      pixmap = global_pix_loader->loadApplicationMiniIcon( pixmap_name, 16, 16 );
    }
  else
    {
      pixmap.resize(0,0);
    }
  sub_menu = menu;
  cmenu = cm;
  recv = receiver;
  memb = member;
  read_only = ro;
}

PMenuItem::PMenuItem( PMenuItem &item )
  : url_name(command_name), dev_name(command_name), mount_point(term_opt), fs_type(exec_path),
    dev_read_only(use_term), umount_pixmap_name(pattern), swallow_title(term_opt)
{
  initMetaObject();
  text_name       = item.text_name;
  real_name       = item.real_name;
  old_name        = item.old_name;
  comment         = item.comment;
  pixmap_name     = item.pixmap_name;
  pixmap          = item.pixmap;
  entry_type      = item.entry_type; 
  command_name    = item.command_name;
  big_pixmap_name = item.big_pixmap_name;
  term_opt        = item.term_opt;
  exec_path       = item.exec_path;
  dir_path        = item.dir_path;
  use_term        = item.use_term;
  pattern         = item.pattern;
  protocols       = item.protocols;
  extensions      = item.extensions;
  if( entry_type == submenu )
    {
      sub_menu = new PMenu( *(item.sub_menu) );
      cmenu    = new CPopupMenu;
    }
  else
    {
      sub_menu = NULL;
      cmenu    = NULL;
    }
  recv         = item.cmenu;
  memb         = item.memb;
  if( item.read_only && entry_type == prog_com )
    {
      entry_type = unix_com;
      recv = NULL;
      memb = NULL;
    }
  read_only = FALSE; 
}

PMenuItem::~PMenuItem()
{
  if( cmenu ) 
    delete cmenu;
  if( sub_menu )
    {
      delete sub_menu;
    }
}

QPixmap PMenuItem::getPixmap()
{
  return pixmap;
}

QString &operator<<( QString &s, PMenuItem &item )
{
  if( item.read_only )
    {
      s = "";
      return s;
    }
  switch(item.entry_type) {
  case empty:
    s = "";
    return s;
  case submenu:
    s = "Menu ";
    break;
  case separator:
    s = "Separator ";
    break;
  case label:
    s = "Label ";
    break;
  case unix_com:
    s = "Unix_Com ";
    break;
  case swallow_com:
    s = "Swallow_Com ";
    break;
  case net_com:
    s = "Net_Com ";
    break;
  default:
    s = "";
    return s;
  };
  s += item.text_name + ", ";
  s += item.pixmap_name + ", ";
  s += item.command_name;
  return s;
}

short PMenuItem::parse( QString &s, PMenu *menu)
{
  s.simplifyWhiteSpace();
  short pos = s.find(' ');
  short npos;
  if ( pos < 0 )
    { pos = s.length(); }
  QString command = s.left( pos );
//debug ( "com = '%s'", (const char *) command);
  if ( command == "Menu" )
    {
      entry_type = submenu;
      sub_menu = menu;
      cmenu = new CPopupMenu;
    }
  else if ( command == "Separator" )
    {
      entry_type = separator;
      return 0;
    }
  else if ( command == "Label" )
    { entry_type = label; }
  else if ( command == "Unix_Com" )
    { entry_type = unix_com; }
  else if ( command == "Swallow_Com" )
    { entry_type = swallow_com; }
  else if ( command == "Net_Com" )
    { entry_type = net_com; }
  else
    { 
      entry_type = empty;
      return -1; 
    }
  if( (npos = s.find(',', pos)) < 0 )
    { entry_type = empty; return -1; }
  while( s[pos+1] == ' ' )  pos++;
  text_name = s.mid( pos+1, npos-pos-1 );
  pos = npos;
  if( (npos = s.find(',', pos+1)) < 0 )
    { entry_type = empty; return -1; }
  while( s[pos+1] == ' ' )  pos++;
  pixmap_name = s.mid( pos+1, npos-pos-1 );
  while( s[npos+1] == ' ' ) npos++;
  command_name = s.right( s.length()-npos-1 );
//debug ( "text = '%s'", (const char *) text_name);
//debug ( "pix = '%s'", (const char *) pixmap_name);
//debug ( "unix = '%s'", (const char *) command_name);
  if( !pixmap_name.isEmpty() )
    {
      pixmap = global_pix_loader->loadApplicationMiniIcon(pixmap_name, 16, 16);
    }
  return 0;
}

short PMenuItem::parse( QFileInfo *fi, PMenu *menu )
{
  QString lang = KApplication::getKApplication()->getLocale()->language();
  real_name = fi->fileName().copy();
  old_name = real_name;
  QString type_string = "";
  int pos = fi->fileName().find(".kdelnk");
  if( pos >= 0 )
    text_name = fi->fileName().left(pos);
  else
    text_name = fi->fileName();
  if( !(fi->isWritable()) )
    read_only = TRUE;
  if( menu != NULL )
    {
      QString file_name = fi->absFilePath();
      file_name += "/.directory";
      QFileInfo fi_config(file_name);
      if( fi_config.isReadable() ) 
	{
	  KConfig kconfig(file_name);
	  kconfig.setGroup("KDE Desktop Entry");
	  comment         = kconfig.readEntry("Comment");
	  text_name       = kconfig.readEntry("Name", text_name);
	  pixmap_name     = kconfig.readEntry("MiniIcon");
	  big_pixmap_name = kconfig.readEntry("Icon");
	  dir_path        = fi->dirPath(TRUE);
	}
      entry_type  = submenu;
      sub_menu    = menu;
      cmenu       = new CPopupMenu;      
    }
  else
    {
      // test if file is a kdelnk file
      QFile file(fi->absFilePath());
      if( file.open(IO_ReadOnly) )
	{
	  char s[19];
	  int r = file.readLine(s, 18);
	  if(r > -1)
	    {
	      s[r] = '\0';
	      if(QString(s).left(17) != "# KDE Config File") 
		{
		  file.close();
		  return -1;
		}
	    }
	}
      else
	{
	  file.close();
	  return -1;
	}
      // parse file
      file.at(0);
      QTextStream stream(&file);
      QString current_line, key, value;
      QString temp_swallow_title, temp_term_opt, temp_exec_path, temp_text_name;
      QString temp_use_term, temp_pattern, temp_protocols, temp_extensions, temp_url_name;
      QString temp_dev_name, temp_mount_point, temp_fs_type, temp_umount_pixmap_name;
      QString temp_dev_read_only = "0";
      temp_use_term = "0";
      bool inside_group = FALSE;
      int equal_pos;
      comment = "";
      while(!stream.eof())
	{
	  current_line = stream.readLine();
	  if( current_line[0] == '#' )
	    continue;
	  if( !inside_group ) 
	    {
	      if( current_line != "[KDE Desktop Entry]" ) 
		{
		  continue;
		}
	      else
		{
		  inside_group = TRUE;
		  continue;
		}
	    }
	  equal_pos = current_line.find('=');
	  if( equal_pos == -1 )
	    continue;
	  key = current_line.left(equal_pos).stripWhiteSpace();
	  value = current_line.right(current_line.length() - equal_pos - 1).stripWhiteSpace();
	  
	  if( key == "Exec" ) 
	    { command_name = value; continue; }
	  else if( key == "SwallowExec" )
	    { swallow_exec = value; continue; }
	  else if( key.left(7) == "Comment" )
	    { 
	      if( key == "Comment" )
		{
		  if( comment.isEmpty() )
		    comment = value;
		}
	      else 
		{
		  if( key == "Comment[" + lang + "]" )
		    comment = value;
		}
	      continue; 
	    }
	  else if( key.left(4) == "Name" )
	    { 
	      if( key == "Name" )
		{
		  if( temp_text_name.isEmpty() )
		    temp_text_name = value;
		}
	      else 
		{
		  if( key == "Name[" + lang + "]" )
		    temp_text_name = value;
		}
	      continue; 
	    }
	  else if( key == "MiniIcon" )
	    { pixmap_name = value; continue; }
	  else if( key == "Icon" )
	    { big_pixmap_name = value; continue; }
	  else if( key == "Type" )
	    { type_string = value; continue; }
	  else if( key == "SwallowTitle" )
	    { temp_swallow_title = value; continue; }
	  else if( key == "TerminalOptions" )
	    { temp_term_opt = value; continue; }
	  else if( key == "Path" )
	    { temp_exec_path = value; continue; }
	  else if( key == "Terminal" )
	    { temp_use_term = value; continue; }
	  else if( key == "BinaryPattern" )
	    { temp_pattern = value; continue; }
	  else if( key == "Protocols" )
	    { temp_protocols = value; continue; }
	  else if( key == "MimeType" )
	    { temp_extensions = value; continue; }
	  else if( key == "URL" )
	    { temp_url_name = value; continue; }
	  else if( key == "Dev" )
	    { temp_dev_name = value; continue; }
	  else if( key == "MountPoint" )
	    { temp_mount_point = value; continue; }
	  else if( key == "FSType" )
	    { temp_fs_type = value; continue; }
	  else if( key == "UnmountIcon" )
	    { temp_umount_pixmap_name = value; continue; }
	  else if( key == "ReadOnly" )
	    { temp_dev_read_only = value; continue; }
	}
      file.close();
      if( type_string.isEmpty() )
	return -1;
      if( !temp_text_name.isEmpty() )
	text_name = temp_text_name;
      if( type_string == "Application" ) 
	{
	  if( !swallow_exec.isEmpty() )
	    {
	      entry_type = swallow_com;
	      swallow_title = temp_swallow_title;
	    }
	  else
	    {
	      entry_type = unix_com;
	      term_opt   = temp_term_opt;
	    }
	  exec_path  = temp_exec_path;
	  dir_path   = fi->dirPath(TRUE);
	  if( temp_use_term == "0" )
	    use_term = 0;
	  else 
	    use_term = 1;
	  pattern    = temp_pattern;
	  protocols  = temp_protocols;
	  extensions = temp_extensions;
	}
      else if( type_string == "Link" )
	{
	  entry_type = url;
	  url_name   = temp_url_name;
	}
      else if( type_string == "FSDevice" )
	{
	  entry_type  = device;
	  dev_name    = temp_dev_name;
	  mount_point = temp_mount_point;
	  fs_type     = temp_fs_type;
	  umount_pixmap_name = temp_umount_pixmap_name;
	  if( temp_dev_read_only == "0" )
	    dev_read_only = 0;
	  else
	    dev_read_only = 1;
	}

//    if( !fi->isReadable() ) 
// 	return -1;
//       KConfig kconfig(fi->absFilePath());
//       kconfig.setGroup("KDE Desktop Entry");
//       command_name    = kconfig.readEntry("Exec");
//       swallow_exec    = kconfig.readEntry("SwallowExec");
//       comment         = kconfig.readEntry("Comment");
//       text_name       = kconfig.readEntry("Name", text_name);
//       pixmap_name     = kconfig.readEntry("MiniIcon");
//       big_pixmap_name = kconfig.readEntry("Icon");
//       type_string     = kconfig.readEntry("Type");
//       if( type_string == "Application" ) 
// 	{
// 	  if( !swallow_exec.isEmpty() )
// 	    {
// 	      entry_type = swallow_com;
// 	      swallow_title = kconfig.readEntry("SwallowTitle");
// 	    }
// 	  else
// 	    {
// 	      entry_type = unix_com;
// 	      term_opt   = kconfig.readEntry("TerminalOptions");
// 	    }
// 	  exec_path       = kconfig.readEntry("Path");
// 	  dir_path        = fi->dirPath(TRUE);
// 	  use_term        = kconfig.readNumEntry("Terminal");
// 	  pattern         = kconfig.readEntry("BinaryPattern");
// 	  protocols       = kconfig.readEntry("Protocols");
// 	  extensions      = kconfig.readEntry("MimeType");
// 	}
//       else if( type_string == "Link" )
// 	{
// 	  entry_type = url;
// 	  url_name   = kconfig.readEntry("URL");
// 	}
//       else if( type_string == "FSDevice" )
// 	{
// 	  entry_type  = device;
// 	  dev_name    = kconfig.readEntry("Dev");
// 	  mount_point = kconfig.readEntry("MountPoint");
// 	  fs_type     = kconfig.readEntry("FSType");
// 	  umount_pixmap_name = kconfig.readEntry("UnmountIcon");
// 	  dev_read_only = kconfig.readNumEntry("ReadOnly");
// 	}
    }
  
  // some code from kpanel
  QPixmap tmppix;
  pixmap = tmppix;
  if( !pixmap_name.isEmpty() ){
    pixmap = global_pix_loader->loadApplicationMiniIcon(pixmap_name, 16, 16);
  }
  if (pixmap.isNull() && !big_pixmap_name.isEmpty()){
    pixmap = global_pix_loader->loadApplicationMiniIcon(big_pixmap_name, 16, 16);
  }
  if (pixmap.isNull() && getType() == unix_com){
    QString tmp = real_name.copy();
    int pos = tmp.find(".kdelnk");
    if( pos >= 0 )
      tmp = tmp.left(pos);
    tmp.append(".xpm");
    pixmap = global_pix_loader->loadApplicationMiniIcon(tmp, 16, 16);
  }
  
  if (pixmap.isNull())
    pixmap = global_pix_loader->loadApplicationMiniIcon("mini-default.xpm", 16, 16);
  
  if (comment.isEmpty())
    comment = text_name;
  if (big_pixmap_name.isEmpty()){
    QString tmp = real_name.copy();
    int pos = tmp.find(".kdelnk");
    if( pos >= 0 )
      tmp = tmp.left(pos);
    tmp.append(".xpm");
    big_pixmap_name = tmp.copy();
  }
  // end kpanel

  return 0;
}

void PMenuItem::writeConfig( QDir dir )
{
  if( read_only || entry_type == separator )
    return;
  QString file = dir.absPath();
  dir_path = file.copy();
  file += ( (QString) "/" + real_name ); //+ ".kdelnk" );
  QFile config(file);
  if( !config.open(IO_ReadWrite) ) 
    return;
  config.close();
  KConfig kconfig(file);
  kconfig.setGroup("KDE Desktop Entry");
  kconfig.writeEntry("Comment", comment, TRUE, FALSE, TRUE );
  kconfig.writeEntry("Icon", big_pixmap_name );
  kconfig.writeEntry("MiniIcon", pixmap_name );
  kconfig.writeEntry("Name", text_name, TRUE, FALSE, TRUE );
  switch( (int) entry_type ) {
  case (int) swallow_com:
    kconfig.writeEntry("SwallowExec", swallow_exec );
    kconfig.writeEntry("SwallowTitle", swallow_title );
    //break;
  case (int) unix_com:
    if( entry_type == unix_com )
      {
	kconfig.writeEntry("SwallowExec", "" );
	kconfig.writeEntry("TerminalOptions", term_opt );
      }
    kconfig.writeEntry("Exec", command_name );
    kconfig.writeEntry("Path", exec_path );
    if( use_term )
      kconfig.writeEntry("Terminal", 1 );
    else
      kconfig.writeEntry("Terminal", 0 );
    kconfig.writeEntry("BinaryPattern", pattern);
    kconfig.writeEntry("Protocols", protocols);
    kconfig.writeEntry("MimeType", extensions);
    kconfig.writeEntry("Type", "Application");
    break;
  case (int) url:
    kconfig.writeEntry("URL", url_name);
    kconfig.writeEntry("Type", "Link");
    break;
  case (int) device:
    kconfig.writeEntry("Dev", dev_name);
    kconfig.writeEntry("MountPoint", mount_point);
    kconfig.writeEntry("FSType", fs_type);
    kconfig.writeEntry("UnmountIcon", umount_pixmap_name);
    kconfig.writeEntry("ReadOnly", dev_read_only);
    kconfig.writeEntry("Type", "FSDevice");
    break;
  };
  kconfig.sync();
}

void PMenuItem::exec_system()
{
  if( command_name.isNull() )
    return;
  QString name;
  if( command_name.right(1) != "&" )
    name = command_name + " &";
  else
    name = command_name;
  system( (const char *) name );
}

void PMenuItem::exec_fvwm()
{
  // ((TaskBar *) (main_app->mainWidget()))->send_fvwm_com(command_name);
}

QPixmap PMenuItem::getBigPixmap()
{
  QPixmap pm = global_pix_loader->loadApplicationIcon( big_pixmap_name);

  if (pm.isNull()){
    if( entry_type == submenu )
      pm = global_pix_loader->loadApplicationIcon("folder.xpm");
    else if( entry_type == unix_com )
      {
	pm = global_pix_loader->loadApplicationIcon("exec.xpm");
      }
  }
  return pm;
}

//--------------------------------------------------------------------------------------

PMenu::PMenu()
{
  initMetaObject();
  menu_conf = NULL;
  list.setAutoDelete(TRUE);
}

PMenu::PMenu( PMenu &menu )
{
  initMetaObject();
  menu_conf = NULL;
  list.setAutoDelete(TRUE);
  PMenuItem *item, *new_item;
  for( item = menu.list.first(); item != 0; item = menu.list.next() )
    {
      new_item = new PMenuItem( *item );
      list.append(new_item);
    }
}

PMenu::~PMenu()
{
  hideConfig();
  list.clear();
}

short PMenu::create_cmenu( CPopupMenu *menu )
{
  QPixmap buffer;
  PMenuItem *item;
  //  int i;
  int id;
  EntryType et;
  for( item = list.first(); item != 0; item = list.next() )
    {
      et = item->entry_type;
      switch ( et ) {
      case separator:
	menu->insertSeparator();
	continue;
      case submenu:
	item->cmenu->setFont(menu->font());
	item->sub_menu->create_cmenu( item->cmenu );
	create_pixmap(buffer, item, menu);
	id = menu->insertItem(buffer, item->cmenu, -2);
	continue;
      case label:
	create_pixmap(buffer, item, menu);
	id = menu->insertItem( buffer, -2);
	continue;
      case unix_com:
	create_pixmap(buffer, item, menu);
	id = menu->insertItem( buffer, -2);
	if( !item->command_name.isEmpty() )
	  menu->connectItem( id, item, SLOT(exec_system()) );
	continue;
      case swallow_com:
	create_pixmap(buffer, item, menu);
	id = menu->insertItem( buffer, -2);
	if( !item->command_name.isEmpty() )
	  menu->connectItem( id, item, SLOT(exec_system()) );
	continue;
      case prog_com:
	create_pixmap(buffer, item, menu);
	id = menu->insertItem( buffer, -2);
	menu->connectItem(id, item->recv, item->memb);
	continue;
      case net_com:
	create_pixmap(buffer, item, menu);
	id = menu->insertItem(buffer, -2);
	menu->connectItem( id, item, SLOT(exec_system()) );
	menu->connectItem( id, item->recv, item->memb );
	continue;
      case empty:
	continue;
      default:
	return -1;
      };
    }
  return 0;
}

void PMenu::add (PMenuItem *item)
{
  list.append(item);
}

void PMenu::insert( PMenuItem *item, int index )
{
  if( index > (int) list.count() )
    list.append(item);
  else
    list.insert(index, item);
}

short PMenu::parse( QDataStream &s )
{
  QString buf;
  short pos;
  QString command;
  PMenuItem *new_item;
  PMenu *new_menu;
  char c;
  s >> c;
  while ( c != '\n' )
    {
      buf += c; 
      s >> c;
    }
  buf.simplifyWhiteSpace();
//  debug("%s", (const char *) buf);
  if( buf != "Begin_Menu" )
    return -1;
  while( !s.eof() )
    { 
      buf = "";
      s >> c;
      while ( c != '\n' && !s.eof() )
	{
	  buf += c; 
	  s >> c;
	}
      buf.simplifyWhiteSpace();
//debug ( "line = '%s'", (const char *) buf);
      if( buf == "End_Menu" )
	break;
      if( (pos = buf.find(' ')) < 0)
	{
	  if ( buf.length() == 0 )
	    continue;
	  else
	    pos = buf.length();
	}
      command = buf.left(pos);
      if( command == "Menu" )
	{
	  new_menu = new PMenu();
	  new_item = new PMenuItem;
	  new_item->parse(buf, new_menu);
	  if( new_menu->parse(s) < 0) 
	    {
	      delete new_menu;
	      delete new_item;
	      continue;
	    }
	  add(new_item);
	}
      else
	{
	  new_item = new PMenuItem;
	  new_item->parse(buf);
	  add(new_item);
	}
    }
  if ( buf != "End_Menu" )
    return -1;
  return 0;
}

short PMenu::parse( QDir d )
{
  if( !d.exists() )
    return -1;
  PMenuItem *new_item;
  PMenu *new_menu;
  QDir new_dir;
  QStrList sort_order;
  QList<PMenuItem> item_list;
  item_list.setAutoDelete(FALSE);
  int pos;
  bool read_only = FALSE;
  sort_order.setAutoDelete(TRUE);

  QString file = d.path();
  QFileInfo dir_info(file);
  if( !dir_info.isWritable() )
    read_only = TRUE;
  file += "/.directory";
  QFileInfo dir_fi(file);
  if( dir_fi.isReadable() )
    {
      KConfig kconfig(file);
      kconfig.setGroup("KDE Desktop Entry");
      QString order = kconfig.readEntry("SortOrder");
      int len = order.length();
      int j,i;
      QString temp;
      for( i = 0; i < len; )
	{
	  j = order.find(',', i);
	  if( j == -1 )
	    j = len;
	  temp = order.mid(i, j-i);
	  temp.stripWhiteSpace();
	  sort_order.append(temp);
	  i = j+1;
	}
    }
  const QFileInfoList *list = d.entryInfoList();
  QFileInfoListIterator it( *list );
  QFileInfo *fi;
  if( it.count() < 3 )
    return -1;
  while ( (fi=it.current()) )
    {
      if( fi->fileName() == "." || fi->fileName() == ".." )
	{ ++it; continue; }
      if( fi->isDir() )
	{
	  new_menu = new PMenu;
	  new_item = new PMenuItem;
          new_dir.setPath( fi->filePath() );
	  new_item->read_only = read_only;
	  if( new_menu->parse( new_dir ) < 0 || new_item->parse( fi, new_menu ) < 0 )
	    {
	      delete new_menu;
	      delete new_item;
	      ++it;
	      continue;
	    }
	  item_list.append(new_item);
	}
      else
	{
	  //if( !fi->extension().contains("kdelnk") )
	  //if( !isKdelnkFile(fi->absFilePath()))
	  //  { ++it; continue; }
	  new_item = new PMenuItem;
	  new_item->read_only = read_only;
	  if( new_item->parse(fi) < 0 )
	    delete new_item;
	  else
	    item_list.append(new_item);
	}
      ++it;
    }
  // sort items
  QString item_name;
  PMenuItem *item;
  for( item_name = sort_order.first(); !item_name.isEmpty(); item_name = sort_order.next() )
    {
      for( item = item_list.first(); item != NULL; item = item_list.next() )
	{
	  if( item->real_name == item_name )
	    {
	      add(item);
	      item_list.removeRef(item);
	      break;
	    }
	}
    }
  if( item_list.count() != 0 )
    {
      for( item = item_list.first(); item != NULL; item = item_list.next() )
	{ add(item); }
    }
  item_list.clear();
  // insert separators
  sort_order.first();
  while( (pos = sort_order.findNext((QString) "SEPARATOR")) >= 0 )
    {
      sort_order.next();
      new_item = new PMenuItem;
      new_item->entry_type = separator;
      new_item->read_only = read_only;
      insert(new_item, pos);
    }   
  return 0;
}

void PMenu::writeConfig( QTextStream &s )
{
  QString text = "";
  s << "Begin_Menu\n";
  PMenuItem *item;
  for( item = list.first(); item != 0; item = list.next() )
    {
      text << (*item);
      if( text.length() == 0 )
	continue;
      s << text << '\n';
      if( item->getType() == submenu )
	item->sub_menu->writeConfig(s);
    }
  s << "End_Menu\n";
}

void PMenu::writeConfig( QDir base_dir, PMenuItem *parent_item)
{
  if( parent_item )
    if( parent_item->read_only )
      return;
  if( !base_dir.exists() )
    {
      return;
    }

  const QStringList *temp_list = base_dir.entryList("*", QDir::Files);
  QStringList file_list;
  QStringList::ConstIterator temp_it = temp_list->begin();
  while( !temp_it->isNull() )
    {
      file_list.append(*temp_it);
      temp_it++;
    }
  temp_list = base_dir.entryList("*", QDir::Dirs);
  QStringList dir_list;
  temp_it = temp_list->begin();
  while( !temp_it->isNull() )
    {
      if (*temp_it != "." && *temp_it != "..")
	  dir_list.append(*temp_it);
      ++temp_it;
    }
  QString sort_order;
  PMenuItem *item;
  for( item = list.first(); item != 0; item = list.next() )
    {
      //      if( item->read_only )
      //	continue;
      if( item->entry_type == separator )
	sort_order += ((QString) "SEPARATOR" + ',');
      else
	sort_order += (item->real_name + ',');
      if( item->getType() == submenu )
	{
	  if( !item->read_only )
	    {
	      QDir sub_dir(base_dir);
	      if( !sub_dir.cd(item->real_name) )
		{
		  base_dir.mkdir(item->real_name);
		  if( !sub_dir.cd(item->real_name) )
		    continue;
		}
	      item->sub_menu->writeConfig( sub_dir, item );
	    }
	  dir_list.remove(item->real_name);
	}
      else
	{
	  if( item->entry_type != separator )
	    {
	      if( !item->read_only )
		{
		  if( item->real_name.isEmpty() )
		    item->real_name = uniqueFileName(item->text_name);
		  item->writeConfig(base_dir);
		}
	      file_list.remove(item->real_name); // + ".kdelnk");
	    }
	}
    }
  QStringList::Iterator temp_it2;

  // remove files not in pmenu
  for( temp_it2 = file_list.begin(); !temp_it2->isNull(); temp_it2++ )
    {
      if( isKdelnkFile(base_dir.absFilePath(*temp_it2)) )
	{
	  //debug("will remove file: %s", (const char *) name );
	  base_dir.remove(*temp_it);
	}
    }
  // remove dirs not in pmenu
  for( temp_it2 = dir_list.begin(); !temp_it2->isNull(); temp_it2++ )
    {
      //debug("will remove dir: %s", (const char *) name );
      QDir sub_dir(base_dir);
      if(sub_dir.cd(*temp_it2))
	{
	  PMenu *new_menu = new PMenu;
	  new_menu->writeConfig(sub_dir);
	  delete new_menu;
	  sub_dir.remove(".directory");	  
	}
      base_dir.rmdir(*temp_it);
    }
  sort_order.truncate(sort_order.length()-1);
  QString file = base_dir.absPath();
  file += "/.directory";
  QFile config(file);
  if( !config.open(IO_ReadWrite) ) 
    return;
  config.close();
  KConfig kconfig(file);
  kconfig.setGroup("KDE Desktop Entry");
  kconfig.writeEntry("SortOrder", sort_order);
  if( parent_item )
    {
      kconfig.writeEntry("MiniIcon", parent_item->pixmap_name );
      if( parent_item->big_pixmap_name.isEmpty() )
	parent_item->big_pixmap_name = "folder.xpm";
      kconfig.writeEntry("Icon", parent_item->big_pixmap_name );
      kconfig.writeEntry("Name", parent_item->text_name, TRUE, FALSE, TRUE );
    }
  kconfig.sync();
}

void PMenu::copyLnkFiles(QDir base_dir)
{
  PMenuItem *item;
  for( item = list.first(); item != 0; item = list.next() )
    {
      if( item->entry_type == submenu )
	{
	  QDir sub_dir(base_dir);
	  if( item->old_name.isEmpty() )
	    {
	      if( item->real_name.isEmpty() )
		item->real_name = item->text_name;
	      base_dir.mkdir(item->real_name);
	      if( !sub_dir.cd(item->real_name) )
		continue;
	    }
	  else
	    {
	      if( !base_dir.exists( item->old_name ) )
		{
		  base_dir.mkdir(item->old_name);
		  if( base_dir.exists( item->dir_path + '/' + item->old_name + "/.directory") )
		    copyFiles( item->dir_path + '/' + item->old_name + "/.directory",
			       base_dir.absPath() + '/' + item->old_name + "/.directory" );
		}
	      if( !sub_dir.cd(item->old_name) )
		continue;
	    }
	  item->sub_menu->copyLnkFiles( sub_dir );
	}
    }
  for( item = list.first(); item != 0; item = list.next() )
    {
      if( item->entry_type == separator || item->old_name.isEmpty() )
	continue;
      if( item->entry_type != submenu )
	{
	  if( base_dir.exists( item->old_name ) )
	    continue;
	  else
	    {
	      if( base_dir.exists( item->dir_path + '/' + item->old_name ) )
		copyFiles( item->dir_path + '/' + item->old_name,
			    base_dir.absPath() + '/' + item->old_name );
	    }
	}
    }  
}

void PMenu::renameLnkFiles(QDir base_dir)
{
  PMenuItem *item;
  for( item = list.first(); item != 0; item = list.next() )
    {
      if( item->old_name.isEmpty() )
	continue;
      if( item->entry_type == submenu )
	{
	  if( item->real_name != item->old_name )
	    if( base_dir.exists( item->old_name ) )
	      {
		base_dir.rename( item->old_name, item->real_name );
		item->old_name = item->real_name;
	      }
	  QDir sub_dir(base_dir);
	  if( !sub_dir.cd(item->old_name) )
	    continue;
	  item->sub_menu->renameLnkFiles( sub_dir );
	}
      if( item->real_name == item->old_name || item->entry_type == separator )
	continue;
      if( base_dir.exists( item->old_name ) )
	{
	  base_dir.rename( item->old_name, item->real_name );
	  item->old_name = item->real_name;
	}
    }  
}

void PMenu::copyFiles( QString source, QString dest )
{
  char data[1024];
  QFile in(source);
  QFile out(dest);
  if( !in.open(IO_ReadOnly) )
    return;
  if( !out.open(IO_WriteOnly) )
    return;
  int len;
  while( (len = in.readBlock(data, 1024)) > 0 )
    {
      if( out.writeBlock(data,len) < len )
	{
	  len = -1;
	  break;
	}
    }
  out.close();
  in.close();
  if( len == -1 )
    { // abort and remove destination file
      debug("KMenuedit: copy files not succeded.");
      QFileInfo fi(dest);
      fi.dir().remove(dest);
    }
}

void PMenu::move(short item_id, short new_pos)
{
  //  PMenuItem *current;
  PMenuItem *item;
  if( item_id > new_pos )
    {
      item = list.take(item_id);
      list.insert( new_pos, item);
    }
  else
    {
      item = list.take(item_id);
      list.insert( new_pos - 1, item);
    }
  //debug
  //debug("but_id = %i / new_pos = %i", item_id, new_pos);
  //int i = 0;
  //for( item = list.first(); item != 0; item = list.next(), i++ )
  //  {
  //    debug("PMenu:: name = %s / id = %i", (const char *) item->getText(), i);
  //  }
}

void PMenu::remove( short item_id )
{
  list.remove( item_id );
}

void PMenu::create_pixmap( QPixmap &buf, PMenuItem *item, QPopupMenu *menu)
{
  int w, h;
  QPainter p;
  QFontMetrics fm = menu->fontMetrics();   // size of font set for this widget

  w  = 2 + item->pixmap.width() + 4 + fm.width( item->text_name ) + 2;
  h = ( item->pixmap.height() > fm.height() ? item->pixmap.height() : fm.height() ) + 4; 
  
  buf.resize( w, h );                    // resize pixmap
  buf.fill( menu->backgroundColor() );   // clear it
  
  p.begin( &buf );
  p.drawPixmap( 2, 2, item->pixmap );              // use 2x2 border
  p.setFont( menu->font() );
  p.drawText( 2 + item->pixmap.width() + 4,        // center text in item
	      0, w, h,
	      AlignVCenter | ShowPrefix | DontClip | SingleLine,
	      item->text_name );
  p.end();
}

void PMenu::set_net_recv(QObject *receiver, char *member)
{
  PMenuItem *item;
  for( item = list.first(); item != 0; item = list.next() )
    {
      if( item->entry_type == net_com )
	{
	  item->recv = receiver;
	  item->memb = member;
	}
    }
}

void PMenu::popupConfig(QPoint p, QWidget *par_widg, bool bot)
{
  if( !menu_conf )
    {
      menu_conf = new ConfigureMenu(this, par_widg, "menu_conf");
      PMenuItem *item;
      for( item = list.first(); item != 0; item = list.next() )
	{
	  menu_conf->append(item);
	}
      if( bot )
	{
	  QPoint np( 0, menu_conf->getHeight()+3 );
	  p -= np;
	}
      QWidget *desk = QApplication::desktop();
      int dw = desk->width();
      int dh = desk->height();
      int width = menu_conf->getWidth();
      int height = menu_conf->getHeight();
      int x = p.x();
      int y = p.y();
      if( (x+width) > dw )
	x = dw - width;
      if( (y+height+2) > dh )
	y = dh - height - 2;
      menu_conf->move(x, y);
      menu_conf->show();
    }
  else
    {
      hideConfig();
    }
}

void PMenu::moveConfig(QPoint p)
{
  if( menu_conf )
    {
      QWidget *desk = QApplication::desktop();
      int dw = desk->width();
      int dh = desk->height();
      int width = menu_conf->getWidth();
      int height = menu_conf->getHeight();
      int x = p.x();
      int y = p.y();
      if( (x+width) > dw )
	x = dw - width;
      if( (y+height+2) > dh )
	y = dh - height - 2;
      menu_conf->moveRequest(x, y);
    }
}

void PMenu::hideConfig()
{
  if( menu_conf )
    {
      menu_conf->hide();
      delete menu_conf;
      menu_conf = NULL;
    }
}

QPoint PMenu::configPos()
{
  return menu_conf->pos();
}

bool PMenu::checkFilenames(QString name)
{
  QListIterator<PMenuItem> it(list);
  for( ; it.current(); ++it )
    {
      if( it.current()->real_name == name )
	return TRUE;
    }
  return FALSE;
}

QString PMenu::uniqueFileName(QString name, bool is_dir)
{
  QString file_name = name.simplifyWhiteSpace();
  while( file_name.contains('/') )
    {
      file_name.replace(file_name.find('/'), 1, "_");
    }
  QString suffix, new_name;
  int i = 2;
  if( is_dir )
    new_name = file_name;
  else
    new_name = file_name + ".kdelnk";
  while( checkFilenames(new_name) )
    {
      if( is_dir )
	new_name = file_name + suffix.setNum(i);
      else
	new_name = file_name + suffix.setNum(i) + ".kdelnk";
      i++;
    }
  return new_name;
}
