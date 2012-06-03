/*
 * Copyright (C) 2007-2008 John-Paul Stanford <jp@stanwood.org.uk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef BOMBER_H
#define BOMBER_H

#include "bomberwidget.h"

#include <kxmlguiwindow.h>
#include <kgthemeprovider.h>

class KAction;
class KStatusBar;
class KToggleAction;
class KgThemeSelector;

/**
 * This class serves as the main window for Bomber.  It handles the
 * menus, toolbars, and status bars.
 *
 * @short Main window class
 */
class Bomber: public KXmlGuiWindow
{
Q_OBJECT
public:
	/**
	 * Default Constructor
	 */
	Bomber();

	void readSettings();

	/**
	 * Default Destructor
	 */
	virtual ~Bomber();

private slots:
	void closeGame();
	void newGame();
	void pauseGame();
	void showHighscore();

	void setSounds( bool val );
	void displayLevel( unsigned int level );
	void displayScore( unsigned int score );
	void displayLives( unsigned int lives );
	void gameStateChanged( BomberGameWidget::State state );

private:
	void highscore();
	void initXMLUI();
	void setupActions();

	KgThemeProvider m_provider;
	KgThemeSelector* m_selector;

	BomberGameWidget *m_gameWidget;
	KStatusBar *m_statusBar;

	KToggleAction *m_pauseAction, *m_backgroundShowAction, *m_soundAction;
	KAction *m_newAction;

};

#endif
