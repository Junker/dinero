/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * homefinances_gnome
 * Copyright (C) Dmitry Kosenkov 2011 <junker@front.ru>
 * 
 * homefinances_gnome is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * homefinances_gnome is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libgda/libgda.h>

#include "ex-connection.h"


G_DEFINE_TYPE (ExConnection, ex_connection, GDA_TYPE_CONNECTION);

static void
ex_connection_init (ExConnection *object)
{
	/* TODO: Add initialization code here */
}

static void
ex_connection_finalize (GObject *object)
{
	/* TODO: Add deinitalization code here */

	G_OBJECT_CLASS (ex_connection_parent_class)->finalize (object);
}


static void
ex_connection_class_init (ExConnectionClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GdaConnectionClass* parent_class = GDA_CONNECTION_CLASS (klass);

	object_class->finalize = ex_connection_finalize;
}

ExConnection* ex_connection_new (gchar *dsn)
{
	ExConnection *connection = EX_CONNECTION(g_object_new (EX_TYPE_CONNECTION, NULL));

	
	
	return connection;
}

