/**********************************************************************

	--- Dlgedit generated file ---

	File: IconPathDialogData.cpp
	Last generated: Tue Jul 15 17:27:58 1997

	DO NOT EDIT!!!  This file will be automatically
	regenerated by dlgedit.  All changes will be lost.

 *********************************************************************/

#include "IconPathDialogData.h"

#define Inherited QDialog

#include <qlabel.h>
#include <kapp.h>
#include <klocale.h>

IconPathDialogData::IconPathDialogData
(
	QWidget* parent,
	const char* name
)
	:
	Inherited( parent, name, TRUE )
{
  initMetaObject();
	QLabel* tmpQLabel;
	tmpQLabel = new QLabel( this, "Label_1" );
	tmpQLabel->setGeometry( 16, 16, 224, 24 );
	{
		QFont font( "helvetica", 12, 63, 0 );
		font.setStyleHint( (QFont::StyleHint)0 );
		font.setCharSet( (QFont::CharSet)0 );
		tmpQLabel->setFont( font );
	}
	tmpQLabel->setText( "IconPath is not set in your kderc file." );
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );

	tmpQLabel = new QLabel( this, "Label_2" );
	tmpQLabel->setGeometry( 16, 48, 184, 24 );
	{
		QFont font( "helvetica", 12, 63, 0 );
		font.setStyleHint( (QFont::StyleHint)0 );
		font.setCharSet( (QFont::CharSet)0 );
		tmpQLabel->setFont( font );
	}
	tmpQLabel->setText( "I suggest the following entry:" );
	tmpQLabel->setAlignment( 289 );
	tmpQLabel->setMargin( -1 );

	le_icon_path = new QLineEdit( this, "LineEdit_1" );
	le_icon_path->setGeometry( 16, 80, 360, 24 );
	le_icon_path->setText( "" );
	le_icon_path->setMaxLength( 32767 );
	le_icon_path->setEchoMode( QLineEdit::Normal );
	le_icon_path->setFrame( TRUE );

	b_ok = new QPushButton( this, "PushButton_1" );
	b_ok->setGeometry( 144, 128, 96, 24 );
	b_ok->setText( i18n("OK") );
	b_ok->setAutoRepeat( FALSE );
	b_ok->setAutoResize( FALSE );

	resize( 400, 176 );
}


IconPathDialogData::~IconPathDialogData()
{
}
