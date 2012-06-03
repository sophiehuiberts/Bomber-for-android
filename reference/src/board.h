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
#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsScene>
#include <KGameRenderer>

#include <QSize>
#include <QList>
#include <QQueue>
#include <QDir>

namespace Phonon
{
	class MediaObject;
}

class Plane;
class Building;
class Bomb;

/**
 * This class used to represent the game board. This makes sure all the game objects
 * get moved and redrawn every second. It also checks for any collisions
 */
class BomberBoard: public QGraphicsScene
{
Q_OBJECT

public:

	/**
	 * The constructor used to create the board.
	 * \param renderer The renderer used to render game objects
	 * \param view The graphics view object which this board is bound to
	 * \param parent The widget which the board is inserted into
	 */
	explicit BomberBoard(KGameRenderer *renderer, QGraphicsView* view, QObject *parent = 0);

	~BomberBoard();

	/**
	 * This is called when the game board is resized
	 * \param size The new tile size used on the game board
	 */
	void resize( QSize& size );

	/**
	 * This will redraw the game board
	 */
	void redraw();

	/**
	 * This is called to start a new level
	 * \param level The level number been started
	 */
	void newLevel( unsigned int level );

	/**
	 * This is called to pause the game.
	 * \param val True if pased, otherwise false
	 */
	void setPaused( bool val);

	/**
	 * Used to play a sound
	 * \param name The sound to play
	 */
	void playSound( const QString& name );

	/**
	 * Used to activate/deactivate sounds while playing
	 * \param val True to turn on the sounds, otherwise false
	 */
	void setSounds( bool val );

	/**
	 * This will convert the tile location to actual cords on the board
	 * \param pos The cords relative to the tile
	 * \return The cords relative to the widget
	 */
	QPoint mapPosition( const QPointF& pos ) const;

	/**
	 * This will convert the widget location to tile locations
	 * \param pos The cords relative to the widget
	 * \return The cords relative to the tile
	 */
	QPointF unmapPosition( const QPoint& pos ) const;

	/**
	 * Used to set the plane state to flying and move it to the start position
	 */
	void resetPlane();

	/**
	 * This will attempt to drop a bomb if their is not already a bomb dropping
	 */
	void dropBomb();

signals:
	/**
	 * This is emitted when a plane crashes into a building
	 */
	void onPlaneCrash();

	/**
	 * This signal is emitted when a bomb hits a building and before it's exploding
	 * animation starts
	 */
	void onBombHit();

	/**
	 * This is emitted when the level has been cleared of all buildings
	 */
	void levelCleared();

public slots:
	/**
	 * This is called when the settings change to save the settings
	 */
	void settingsChanged();

private slots:
	/** This is called once a second to update and draw all the game objects */
	void tick();

	/**
	 * This is called when a plane has finished exploding
	 */
	void planeExploded();

	/**
	 * This is called when a bomb has finished exploding
	 */
	void bombExploded();

private:
	/**
	 * This is called when a bomb hits a building
	 * \param bomb The bomb that hit
	 * \param moveBombToX The x position to move the explosion too
	 * \param moveBombToY The y position to move the explosion too
	 */
	void bombHit(Bomb *bomb,qreal moveBombToX,qreal moveBombToY);

	/**
	 * This is used to remove all the current game objects, usually called before
	 * stating a new level
	 */
	void clear();

	/**
	 * This is used to check for any collisions of the plane or bombs
	 */
	void checkCollisions();

	/**
	 * This is called when a plane crashes into a building
	 */
	void crashed();

	/** This is the renderer used to render game objects */
	KGameRenderer *m_renderer;

	/** This is the size of a tiling block */
	QSize m_tileSize;
	QTimer *m_clock;

	Phonon::MediaObject *m_audioPlayer;
	bool m_playSounds;
	QDir m_soundPath;

	/** If their is a bomb currently dropping then it is pointed to by this */
	Bomb *m_bomb;

	/** This points to the plane object used in the level */
	Plane *m_plane;

	/** This contains all the buildings in the current level */
	QList<Building *> m_buildings;

	/** This contains all the bombs that are currently exploding */
	QQueue<Bomb *> m_explodingBombs;

	/** This is the number of blocks that make up the buildings */
	unsigned int m_buildingBlocks;

	/** This is the graphics view object which this board is bound. */
	QGraphicsView *m_view;
};

#endif
