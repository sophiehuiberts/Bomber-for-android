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
#ifndef BOMB_H
#define BOMB_H

#include "explodable.h"

/**
 * This is the bomb game object class. It extends Explodable as it
 * is a explodable object.
 */
class Bomb: public Explodable
{
public:
	/** The width of the bomb relative to the tile */
	static const qreal BOMB_RELATIVE_SIZE_W;
	/** The height of the bomb relative to the tile */
	static const qreal BOMB_RELATIVE_SIZE_H;

	Bomb(KGameRenderer *renderer, BomberBoard *board, qreal xPos, qreal yPos,
			const QSize& tileSize);
	~Bomb();

	/**
	 * Performs move calculations
	 * This method is called once per frame
	 */
	void advanceItem();
};

#endif
