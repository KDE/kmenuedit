/**********************************************************************

	--- Qt Architect generated file ---

	File: MenuNameDialog.cpp
	Last generated: Fri Nov 28 12:05:18 1997

 *********************************************************************/

#include "MenuNameDialog.h"

#define Inherited MenuNameDialogData

#include "MenuNameDialog.moc"
#include "MenuNameDialogData.moc"
#include <kapp.h>
#include <klocale.h>

MenuNameDialog::MenuNameDialog(	QWidget* parent, const char* name )
  : Inherited( parent, name )
{
  setCaption( i18n("Change Menunames") );
  i_personal->setFocus();
}

MenuNameDialog::~MenuNameDialog()
{

}

#undef Inherited
