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
#include "bomber.h"

#include "bomberwidget.h"
#include "settings.h"

#include <KAction>
#include <KActionCollection>
#include <KConfigDialog>
#include <KLocale>
#include <KMessageBox>
#include <krandom.h>
#include <KToggleAction>
#include <KStatusBar>

#include <kstandardgameaction.h>
#include <KScoreDialog>
#include <kgthemeselector.h>

Bomber::Bomber()
{
	m_provider.discoverThemes(
		"appdata", QLatin1String("themes"), //theme data location
		QLatin1String("kbomber")            //default theme name
	);
	m_selector = new KgThemeSelector(&m_provider);

	m_statusBar = statusBar();
	m_statusBar->insertItem(i18nc("Used to display the current level of play to the user", "Level: %1", 0), 1, 1);
	m_statusBar->insertItem(i18nc("Used to inform the user of their current score", "Score: %1", 0), 2, 1);
	m_statusBar->insertItem(i18nc("Used to tell the user how many lives they have left", "Lives: %1", 3), 4, 1);

	m_gameWidget = new BomberGameWidget(&m_provider, this);
	connect(&m_provider, SIGNAL(currentThemeChanged(const KgTheme*)),
		m_gameWidget, SLOT(settingsChanged()));

	connect(m_gameWidget, SIGNAL(levelChanged(uint)), this, SLOT(displayLevel(uint)));
	connect(m_gameWidget, SIGNAL(scoreChanged(uint)), this, SLOT(displayScore(uint)));
	connect(m_gameWidget, SIGNAL(livesChanged(uint)), this, SLOT(displayLives(uint)));
	connect(m_gameWidget, SIGNAL(stateChanged(BomberGameWidget::State)), this, SLOT(gameStateChanged(BomberGameWidget::State)));

	setCentralWidget(m_gameWidget);

	initXMLUI();

	setFocusPolicy(Qt::StrongFocus);

	setFocus();
	setupGUI();

	// readSettings();
}

Bomber::~Bomber()
{
	delete m_selector;
}

/**
 * create the action events create the gui.
 */
void Bomber::initXMLUI()
{
	// Game
	m_newAction = KStandardGameAction::gameNew(this, SLOT(newGame()), actionCollection());
	KStandardGameAction::end(this, SLOT(closeGame()), actionCollection());
	m_pauseAction = KStandardGameAction::pause(this, SLOT(pauseGame()), actionCollection());
	KStandardGameAction::highscores(this, SLOT(showHighscore()), actionCollection());
	KStandardGameAction::quit(this, SLOT(close()), actionCollection());

	// Settings
	KStandardAction::preferences(m_selector, SLOT(showAsDialog()), actionCollection());
	m_soundAction = new KToggleAction(i18nc("Menu item used to disable or enable sound","&Play Sounds"), this);
	actionCollection()->addAction( QLatin1String( "toggle_sound" ), m_soundAction);
	connect(m_soundAction, SIGNAL(triggered(bool)), this, SLOT(setSounds(bool)));

	KAction *dropBombAction = actionCollection()->addAction( QLatin1String( "drop_bomb" ));
	dropBombAction->setText(i18nc("The name of the action used for dropping bombs","&Drop bomb"));
	dropBombAction->setToolTip(i18nc("The tool tip text for the action used to drop bombs","Drop bomb"));
	dropBombAction->setWhatsThis(i18nc("Description of the action used to drop bombs",
			"Makes the plane drop a bomb while flying"));
	dropBombAction->setShortcut(Qt::Key_Space);
	dropBombAction->setEnabled(true);
	connect(dropBombAction, SIGNAL (triggered(bool)), m_gameWidget, SLOT (onDropBomb()));
}

void Bomber::readSettings()
{
	m_soundAction->setChecked(BomberSettings::playSounds());
	m_gameWidget->settingsChanged();
}

void Bomber::newGame()
{
	// Check for running game
	closeGame();
	if (m_gameWidget->state() == BomberGameWidget::BeforeFirstGame
			|| m_gameWidget->state() == BomberGameWidget::GameOver)
	{
		m_gameWidget->newGame();
	}
}

void Bomber::pauseGame()
{
	if (m_gameWidget->state() == BomberGameWidget::Paused)
	{
		m_gameWidget->setPaused(false);
	}
	else
	{
		m_gameWidget->setPaused(true);
	}
}

void Bomber::closeGame()
{
	if (m_gameWidget->state() == BomberGameWidget::BeforeFirstGame
			|| m_gameWidget->state() == BomberGameWidget::GameOver)
	{
		return;
	}

	BomberGameWidget::State old_state = m_gameWidget->state();
	if (old_state == BomberGameWidget::Running)
		m_gameWidget->setPaused(true);
	int ret = KMessageBox::questionYesNo(this, i18nc("Message displayed when play tries to quit a game that is currently running",
			"Do you really want to close the running game?"), QString(),
			KStandardGuiItem::close(), KStandardGuiItem::cancel());
	if (ret == KMessageBox::Yes)
	{
		m_gameWidget->closeGame();
	}
	else if (old_state == BomberGameWidget::Running)
	{
		m_gameWidget->setPaused(false);
	}
}

/**
 * Bring up the standard kde high score dialog.
 */
void Bomber::showHighscore()
{
	KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Score | KScoreDialog::Level, this);
	ksdialog.exec();
}

void Bomber::highscore()
{
	KScoreDialog ksdialog(KScoreDialog::Name | KScoreDialog::Score | KScoreDialog::Level, this);

	KScoreDialog::FieldInfo info;
	info[KScoreDialog::Score].setNum(m_gameWidget->score());
	info[KScoreDialog::Level].setNum(m_gameWidget->level());
	if (ksdialog.addScore(info))
		ksdialog.exec();
}

void Bomber::setSounds(bool val)
{
	BomberSettings::setPlaySounds(val);
	m_gameWidget->settingsChanged();
}

void Bomber::displayLevel(unsigned int level)
{
	m_statusBar->changeItem(i18nc(
			"Used to display the current level of play to the user",
			"Level: %1", level), 1);
}

void Bomber::displayScore(unsigned int score)
{
	m_statusBar->changeItem(i18nc(
			"Used to inform the user of their current score", "Score: %1",
			score), 2);
}

void Bomber::displayLives(unsigned int lives)
{
	m_statusBar->changeItem(i18nc(
			"Used to tell the user how many lives they have left", "Lives: %1",
			lives), 4);
}

void Bomber::gameStateChanged(BomberGameWidget::State state)
{
	switch (state)
	{

	case BomberGameWidget::Paused:
		m_pauseAction->setChecked(true);
		m_statusBar->clearMessage();
		break;
	case BomberGameWidget::Running:
		m_pauseAction->setChecked(false);
		m_statusBar->clearMessage();
		break;
	case BomberGameWidget::GameOver:
		m_statusBar->showMessage(i18nc("Game over messaged displayed in the status bar","Game over. Press '%1' for a new game",
				m_newAction->shortcuts().first().toString(QKeySequence::NativeText)));
		highscore();
		break;
	default:
		break;
	}
}

#include "bomber.moc"
