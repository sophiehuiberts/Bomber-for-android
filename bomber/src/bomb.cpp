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

#include "bomb.h"

#include "board.h"
#include "explodable.h"

/** The speed the bomb will fall at */
const qreal DEFAULT_VELOCITY = 0.2;

const qreal Bomb::BOMB_RELATIVE_SIZE_H = 0.7;
const qreal Bomb::BOMB_RELATIVE_SIZE_W = 0.2;

Bomb::Bomb(KGameRenderer *renderer, BomberBoard *board, qreal xPos,
		qreal yPos, const QSize& tileSize) :
	Explodable(QString("bomb"), QString("bomb_explode"), BOMB_RELATIVE_SIZE_W,
			BOMB_RELATIVE_SIZE_H, renderer, board)
{
	setVelocity(DEFAULT_VELOCITY);
	setPosition(xPos, yPos);
	resize(tileSize);
}

Bomb::~Bomb()
{
}

void Bomb::advanceItem()
{
	if (state() == Moving)
	{
		m_yPos += velocity();
	}
	m_nextBoundingRect.moveTo(m_xPos, m_yPos + velocity());
}

