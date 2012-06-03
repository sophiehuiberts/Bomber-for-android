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
#ifndef PLANE_H
#define PLANE_H

#include "explodable.h"

/**
 * This class is used to represent the plane object in the game. It extends class
 * explodable as the plane will explode when it hits buildings. The plane
 * flys from left to right and each time it goes off the side of the screen it
 * is repositioned back at the left side, one place lower than before.
 */
class Plane: public Explodable
{

public:
	/** This is the planes size relative to the tile */
	static const qreal PLANE_RELATIVE_SIZE;

	/** This is the position before the plane goes off the screen */
	static const qreal PLANE_MAX_POSITION_X;

	/** The speed the plane will fly at */
	static const qreal DEFAULT_VELOCITY;

	Plane(KGameRenderer *renderer, BomberBoard *board);
	~Plane();

	/**
	 * Performs move calculations
	 * This method is called once per frame
	 */
	void advanceItem();

	/**
	 * Used to reset the plane to it's starting position
	 */
	void resetPosition();
};

#endif
