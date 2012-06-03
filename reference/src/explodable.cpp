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

#include "explodable.h"

#include "board.h"

#include <KRandom>

/**
 * How big is the explosion in relation to the tiles height.
 * 1.0 means it's the same size as the tile.
 */
const qreal Explodable::EXPLOSION_RELATIVE_SIZE_H = 1.0;
/**
 * How big is the explosion in relation to the tiles width.
 * 1.0 means it's the same size as the tile.
 */
const qreal Explodable::EXPLOSION_RELATIVE_SIZE_W = 1.0;

Explodable::Explodable(const QString& mainSvg, const QString& explosionSvg,
		qreal relativeWidth, qreal relativeHeight, KGameRenderer *renderer,
		BomberBoard *board) :
	KGameRenderedItem(renderer, mainSvg), m_board(board), m_mainSvg(
			mainSvg), m_explosionSvg(explosionSvg)
{
	setRenderSize(QSize(32, 64));
	m_relativeWidth = relativeWidth;
	m_relativeHeight = relativeHeight;
	resetPixmaps();
	m_state = Moving;
	m_nextBoundingRect.setSize(QSizeF(m_relativeWidth, m_relativeHeight));
	setPos(m_board->mapPosition(QPointF(m_xPos, m_yPos)));
}

Explodable::~Explodable()
{
}

void Explodable::setPosition(qreal xPos, qreal yPos)
{
	m_xPos = xPos, m_yPos = yPos;
	m_nextBoundingRect.moveTo(m_xPos, m_yPos);
}

void Explodable::update()
{
	setFrame(frame() + 1);
	setPos(m_board->mapPosition(QPointF(m_xPos, m_yPos)));
}

void Explodable::resize(const QSize& tileSize)
{
	m_lastSize = tileSize;
	if (m_state == Moving)
	{
		setRenderSize(QSize(
			m_relativeWidth * tileSize.width(),
			m_relativeHeight * tileSize.height()
		));
	}
	else
	{
		setRenderSize(QSize(
			EXPLOSION_RELATIVE_SIZE_W * tileSize.width(),
			EXPLOSION_RELATIVE_SIZE_H * tileSize.height()
		));
	}
	setPos(m_board->mapPosition(QPointF(m_xPos, m_yPos)));
}

void Explodable::setVelocity(qreal vX)
{
	m_velocity = vX;
}

void Explodable::setRandomFrame()
{
	setFrame(KRandom::random());
}

/**
 * Returns bomb's bounding rect expected in next frame
 * used by colision test
 */
QRectF Explodable::nextBoundingRect() const
{
	return m_nextBoundingRect;
}

void Explodable::setState(Explodable::State state)
{
	m_state = state;
	setRandomFrame();
	if (m_state == Moving)
	{
		m_nextBoundingRect.setSize(QSizeF(m_relativeWidth, m_relativeHeight));
		setSpriteKey(m_mainSvg);
	}
	else
	{
		m_nextBoundingRect.setSize(QSizeF(EXPLOSION_RELATIVE_SIZE_W, EXPLOSION_RELATIVE_SIZE_H));
		setSpriteKey(m_explosionSvg);
	}
	resize(m_lastSize);
}

QPointF Explodable::position() const
{
	return QPointF(m_xPos, m_yPos);
}

void Explodable::resetPixmaps()
{
	setFrame(0);
}

Explodable::State Explodable::state() const
{
	return m_state;
}

qreal Explodable::velocity() const
{
	return m_velocity;
}
