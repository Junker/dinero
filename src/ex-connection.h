/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * homefinances_gnome
 * Copyright (C) Dmitry Kosenkov 2011 <junker@front.ru>
 * 
 * homefinances_gnome is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
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

#ifndef _EX_CONNECTION_H_
#define _EX_CONNECTION_H_

#include <glib-object.h>

G_BEGIN_DECLS

#define EX_TYPE_CONNECTION             (ex_connection_get_type ())
#define EX_CONNECTION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), EX_TYPE_CONNECTION, ExConnection))
#define EX_CONNECTION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), EX_TYPE_CONNECTION, ExConnectionClass))
#define EX_IS_CONNECTION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EX_TYPE_CONNECTION))
#define EX_IS_CONNECTION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), EX_TYPE_CONNECTION))
#define EX_CONNECTION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), EX_TYPE_CONNECTION, ExConnectionClass))

typedef struct _ExConnectionClass ExConnectionClass;
typedef struct _ExConnection ExConnection;

struct _ExConnectionClass
{
	GdaConnectionClass parent_class;
};

struct _ExConnection
{
	GdaConnection parent_instance;

	GdaSqlParser *parser;
};

GType ex_connection_get_type (void) G_GNUC_CONST;

G_END_DECLS

#endif /* _EX_CONNECTION_H_ */
