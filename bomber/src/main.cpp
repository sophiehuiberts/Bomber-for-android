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
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <KDE/KLocale>

static const char description[] = I18N_NOOP("Arcade bombing game");

static const char version[] = "0.1";

int main(int argc, char **argv)
{
	KAboutData about("bomber", 0, ki18n("Bomber"), version, ki18n(description),
			KAboutData::License_GPL, ki18n("(C) 2007 John-Paul Stanford"),
			KLocalizedString(), 0, "jp@stanwood.org.uk");
	about.addAuthor(ki18n("John-Paul Stanford"), KLocalizedString(),
			"jp@stanwood.org.uk");
	about.addAuthor(ki18n("Mehmet Emre"), ki18n("Porting to QGraphicsView."),
			"maemre2@gmail.com");
	KCmdLineArgs::init(argc, argv, &about);

	KApplication app;
	KGlobal::locale()->insertCatalog( QLatin1String( "libkdegames" ));

	// see if we are starting with session management
	if (app.isSessionRestored())
	{
		RESTORE(Bomber);
	}
	else
	{
		Bomber *widget = new Bomber;
		widget->setMinimumSize(320, 200);
		widget->show();
		widget->readSettings();
	}

	return app.exec();
}
