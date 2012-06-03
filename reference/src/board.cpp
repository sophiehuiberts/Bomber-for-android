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
#include "board.h"

#include "plane.h"
#include "building.h"
#include "bomb.h"

#include "settings.h"

#include <KRandom>
#include <Phonon/MediaObject>
#include <KStandardDirs>

#include <QTimer>
#include <QGraphicsScene>
#include <QGraphicsView>

/** The value that the plane velocity increases by */
const qreal PLANE_INC_VELOCITY = 0.0005;
/** The value of this controls the speed of the game */
const unsigned int GAME_DELAY = 15;
/** The number of tiles vertical in the playing area */
const unsigned int TILE_NUM_H = 20;
/** The number of builds to display */
const unsigned int NUMBER_BUILDINGS = 10;
/** The number of tiles horizontally in the playing area */
const unsigned int TILE_NUM_W = ((NUMBER_BUILDINGS)+2);
/** The maximum level number before the game stops getting harder */
const unsigned int MAX_LEVEL = 11;

/** This time in milliseconds that the plane exploding animation is played for */
const unsigned int PLANE_EXPLODE_TIME = 2000;

/** This time in milliseconds that the bomb exploding animation is played for */
const unsigned int BOMB_EXPLODE_TIME = 1000;

BomberBoard::BomberBoard(KGameRenderer *renderer, QGraphicsView* view, QObject *parent) :
	QGraphicsScene(parent), m_renderer(renderer), m_view(view)
{
	m_bomb = NULL;
	m_clock = new QTimer(this);
	m_clock->setInterval(GAME_DELAY);
	connect(m_clock, SIGNAL(timeout()), this, SLOT(tick()));
	m_plane = new Plane(m_renderer, this);
	m_plane->resize(m_tileSize);
	this->addItem(m_plane);
	m_plane->show();
	resetPlane();
	clear();

	m_audioPlayer = 0;

	m_soundPath = KStandardDirs::locate("appdata", "sounds/");
}

BomberBoard::~BomberBoard()
{
	delete m_bomb;
	delete m_plane;
	qDeleteAll(m_buildings);
	qDeleteAll(m_explodingBombs);
	delete m_audioPlayer;
}

void BomberBoard::resetPlane()
{
	m_plane->setState(Explodable::Moving);
	m_plane->resetPosition();
}

void BomberBoard::resize(QSize& size)
{
	setBackgroundBrush(m_renderer->spritePixmap("background", size));

	unsigned int minTileSizeWidth = size.width() / TILE_NUM_W;
	unsigned int minTileSizeHeight = size.height() / TILE_NUM_H;

	m_tileSize = QSize(minTileSizeWidth, minTileSizeHeight);

	foreach(Building *building, m_buildings)
	{
		building->resize(m_tileSize);
	}

	m_plane->resize(m_tileSize);
	if (m_bomb!=NULL)
	{
		m_bomb->resize(m_tileSize);
	}

	redraw();

	size.setWidth(minTileSizeWidth * TILE_NUM_W);
	size.setHeight(minTileSizeHeight * TILE_NUM_H);
}

void BomberBoard::redraw()
{
	m_plane->resetPixmaps();
	if (m_bomb != NULL)
	{
		m_bomb->resetPixmaps();
	}
}

void BomberBoard::newLevel(unsigned int level)
{
	if (level > MAX_LEVEL)
	{
		level = MAX_LEVEL;
	}

	if (level == 1)
	{
		m_plane->setVelocity(Plane::DEFAULT_VELOCITY);
	}
	else if (level % 2 == 0)
	{
		m_plane->setVelocity(m_plane->velocity() + PLANE_INC_VELOCITY);
	}

	m_clock->stop();
	clear();
	m_plane->setState(Explodable::Moving);
	m_buildingBlocks = 0;
	//Create the buildings
	for (unsigned int i = 0; i < NUMBER_BUILDINGS; i++)
	{
		unsigned int min = level;
		if (min < 3)
		{
			min = 3;
		}
		unsigned int max = level + 3;
		if (max < 5)
		{
			max = 5;
		}
		unsigned int height = (KRandom::random() % (max - min)) + min;

		m_buildingBlocks += height;
		Building *building = new Building(m_renderer, this, i + 1, height);

		building->resize(m_tileSize);
		building->show();

		m_buildings.append(building);
	}
}

void BomberBoard::setPaused(bool val)
{
	if (val)
		m_clock->stop();
	else
		m_clock->start();
}

void BomberBoard::playSound(const QString& name)
{
	if (m_playSounds == true && !name.isEmpty())
	{
		QString file = m_soundPath.filePath(name);
		m_audioPlayer->setCurrentSource(file);
		m_audioPlayer->play();
	}
}

void BomberBoard::setSounds(bool val)
{
	m_playSounds = val;
	if (val == true && m_audioPlayer == 0)
	{
		m_audioPlayer = Phonon::createPlayer(Phonon::GameCategory);
	}
}

void BomberBoard::tick()
{
	checkCollisions();

	m_plane->advanceItem();

	if (m_bomb!=NULL)
	{
		m_bomb->advanceItem();
	}

	foreach(Bomb *bomb, m_explodingBombs)
	{
		bomb->advanceItem();
	}

	// Draw everything
	m_plane->update();

	if (m_bomb!=NULL)
	{
		m_bomb->update();
	}

	foreach(Bomb *bomb, m_explodingBombs)
	{
		bomb->update();
	}
}

void BomberBoard::dropBomb()
{
	if (m_bomb == NULL && m_plane->state() == Explodable::Moving)
	{
		QPointF planePos = m_plane->position();
		m_bomb = new Bomb(m_renderer, this, planePos.x(), planePos.y() + 1, m_tileSize);
		this->addItem(m_bomb);
		m_bomb->show();
	}
}

void BomberBoard::checkCollisions()
{
	foreach(Building *building, m_buildings)
	{
		if (m_plane->nextBoundingRect().intersects(building->boundingRect()) && m_plane->state()
				== Explodable::Moving)
		{
			// Plane crashed into the building
			building->destoryTop();
			m_buildingBlocks--;
			crashed();
		}

		if (m_bomb!=NULL)
		{
			if (m_bomb->nextBoundingRect().intersects(building->boundingRect()) && m_bomb->state()
					== Explodable::Moving)
			{
				// Bomb hit a building
				building->destoryTop();
				m_buildingBlocks--;
				emit onBombHit();
				bombHit(m_bomb,building->position().x(),Building::BUILD_BASE_LOCATION-(building->height()));
				m_bomb = NULL;
			}
			else if (m_bomb->position().y()>=Building::BUILD_BASE_LOCATION+1)
			{
				// Bomb hit the ground
				bombHit(m_bomb,(unsigned int)m_bomb->position().x(),Building::BUILD_BASE_LOCATION);
				m_bomb = NULL;
			}
		}

		if (m_plane->state()==Explodable::Moving && m_buildingBlocks==0)
		{
			emit levelCleared();
		}
	}
}

void BomberBoard::bombHit(Bomb *bomb, qreal moveBombToX, qreal moveBombToY)
{
	// playSound(QString("explode.ogg"));
	bomb->setPosition(moveBombToX, moveBombToY);
	bomb->setState(Bomb::Exploding);
	m_explodingBombs.enqueue(bomb);
	QTimer::singleShot(BOMB_EXPLODE_TIME, this, SLOT(bombExploded()));
}

void BomberBoard::bombExploded()
{
	Bomb *bomb = m_explodingBombs.dequeue();
	bomb->hide();
	delete bomb;
}

void BomberBoard::settingsChanged()
{
	setSounds(BomberSettings::playSounds());
	setBackgroundBrush(m_renderer->spritePixmap("background", m_view->size()));
	redraw();
}

void BomberBoard::planeExploded()
{
	m_plane->setState(Plane::Exploded);
	emit onPlaneCrash();
}

void BomberBoard::crashed()
{
	QPointF pos = m_plane->position();
	m_plane->setPosition(pos.x() + 1, pos.y());
	m_plane->setState(Plane::Exploding);
	playSound(QString("explode.ogg"));
	QTimer::singleShot(PLANE_EXPLODE_TIME, this, SLOT(planeExploded()));
}

void BomberBoard::clear()
{
	qDeleteAll(m_buildings);
	m_buildings.clear();

	delete m_bomb;
	m_bomb=NULL;

	resetPlane();
}

QPoint BomberBoard::mapPosition(const QPointF& pos) const
{
	return QPoint(static_cast<unsigned int> (m_tileSize.width() * pos.x()),
			static_cast<int> (m_tileSize.height() * pos.y()));
}

QPointF BomberBoard::unmapPosition(const QPoint& pos) const
{
	return QPointF(1.0 * pos.x() / m_tileSize.width(), 1.0 * pos.y()
			/ m_tileSize.height());
}
