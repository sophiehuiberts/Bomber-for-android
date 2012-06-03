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

#ifndef EXPLODABLE_H
#define EXPLODABLE_H

#include <KGameRenderedItem>

class BomberBoard;

/**
 * This is a game object that can explode.
 */
class Explodable: public KGameRenderedItem
{
public:
	/** The states that a bomb can be in */
	enum State
	{
		Moving, Exploding, Exploded
	};

	/** The width of the explostion releative to the tile */
	static const qreal EXPLOSION_RELATIVE_SIZE_W;
	/** The height of the explostion releative to the tile */
	static const qreal EXPLOSION_RELATIVE_SIZE_H;

	explicit Explodable(const QString& mainSvg, const QString& explosionSvg,
			qreal relativeWidth, qreal relativeHeight,
			KGameRenderer *renderer, BomberBoard *board);
	virtual ~Explodable();

	/**
	 * Updates bomb position and pixmap.
	 * This method is called once per frame.
	 */
	void update();

	/**
	 * Sets width and height of bomb. Using the current tile size.
	 * \param tileSize The size of the tile
	 */
	void resize(const QSize& tileSize);

	/**
	 * Sets the velocity of the explodable object
	 * \param velocity The velocity of the object
	 */
	void setVelocity(qreal velocity);

	/**
	 * Used to get the velocity of the explodable object
	 * \return The velocity
	 */
	qreal velocity() const;

	/**
	 * Rechecks the number of frames of bomb animation and sets new pixmaps.
	 * This method is useful when changing game theme.
	 */
	void resetPixmaps();

	/**
	 * Used to set the bomb's position
	 * \param xPos Set the x position of the item
	 * \param yPos Set the y position of the item
	 */
	void setPosition(qreal xPos, qreal yPos);

	/**
	 * Return the position of the bomb
	 * \return The position of the bomb
	 */
	QPointF position() const;

	/**
	 * Set the current frame to a random frame from the frame set
	 */
	void setRandomFrame();

	/**
	 * Returns bomb's bounding rect expected in next frame
	 * used by colision test
	 * \return The items next bounding rect
	 */
	QRectF nextBoundingRect() const;

	/**
	 * Get the current state of the bomb
	 * \return The current state of the bomb
	 */
	Explodable::State state() const;

	/**
	 * Set the current state of the bomb
	 * \param state The state to set the bomb too
	 */
	void setState(Explodable::State state);

protected:
	qreal m_xPos;
	qreal m_yPos;
	QRectF m_nextBoundingRect;

private:
	BomberBoard *m_board;

	qreal m_velocity;

	/**
	 * The state the bomb is currently in
	 */
	State m_state;

	/** The tile size last time the bomb was resized */
	QSize m_lastSize;

	qreal m_relativeHeight;
	qreal m_relativeWidth;
	QString m_mainSvg;
	QString m_explosionSvg;
};

#endif
