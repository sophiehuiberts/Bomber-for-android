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
#include "plane.h"

#include "board.h"
#include "explodable.h"

/** The speed the plane will fly at */
const qreal Plane::DEFAULT_VELOCITY = 0.08;

/** This is the planes size relative to the tile */
const qreal Plane::PLANE_RELATIVE_SIZE = 1;

/** This is the position before the plane goes off the screen */
const qreal Plane::PLANE_MAX_POSITION_X = 12;

Plane::Plane(KGameRenderer *renderer, BomberBoard *board) :
	Explodable(QString("plane"), QString("plane_explode"), PLANE_RELATIVE_SIZE,
			PLANE_RELATIVE_SIZE, renderer, board)
{
	setVelocity(DEFAULT_VELOCITY);
	resetPosition();
}

Plane::~Plane()
{
}

void Plane::resetPosition()
{
	m_xPos = 0, m_yPos = 0;
	m_nextBoundingRect.moveTo(m_xPos, m_yPos);
}

void Plane::advanceItem()
{
	if (state() == Moving)
	{
		m_xPos += velocity();
		if (m_xPos > PLANE_MAX_POSITION_X)
		{
			m_xPos = 0;
			m_yPos++;
		}
	}
	m_nextBoundingRect.moveTo(m_xPos + velocity(), m_yPos);
}
